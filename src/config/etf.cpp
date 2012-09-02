#include "etf.h"

#include <stdio.h>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/karma.hpp>

//#include <boost/lambda/lambda.hpp>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

//#define PARSER_DEBUG

using namespace boost;
namespace qi = spirit::qi;
namespace karma = spirit::karma;
namespace ascii = spirit::ascii;
namespace ph = phoenix;

typedef ETFDocument::etfnode etfnode;

BOOST_FUSION_ADAPT_STRUCT(
	ETFDocument::etfnode,
	(ETFDocument::DataType, type)
	(ETFDocument::etfvalue, value)
)

template<typename Iterator>
struct etf_parser : qi::grammar<Iterator, std::map<std::string, etfnode>(), ascii::space_type> {
	etf_parser() : etf_parser::base_type(start) {
		using qi::long_;
		using qi::ulong_;
		using qi::lit;
		using qi::bool_;
		using qi::lexeme;
		using ascii::char_;

		using qi::debug;

		quoted_string %= lexeme[lit('"') >> +(char_ - '"') >> lit('"')];

		// The order is important here. The | operator works on a first-match basis, so
		// the double parser has to appear before long_ so long_ doesn't consume the first
		// part of the double
		dataVal %= (quoted_string
				| dblParser
				| long_
				| bool_
				| listObj
				| pairObj
				| mapObj);

		pairObj %= (lit('<') >> dataVal >> lit(',') >> dataVal >> lit('>'));

		listObj %= lit('{') >> -(dataVal % ',') >> lit('}');
		
		mapKey %= +qi::char_("a-zA-Z_-0-9.");
		mapPair %= mapKey >> lit('=') >> dataVal;
		mapObj %= lit('(') >> -(mapPair % ',') >> lit(')');
		start %= mapPair % ';';

#ifdef PARSER_DEBUG
		quoted_string.name("qstr");
		dataVal.name("dataval");
		pairObj.name("pair");
		listObj.name("list");
		mapKey.name("mkey");
		mapPair.name("mpair");
		mapObj.name("map");
		start.name("etf");
		debug(quoted_string);
		debug(dataVal);
		debug(pairObj);
		debug(listObj);
		debug(mapKey);
		debug(mapPair);
		debug(mapObj);
		debug(start);
#endif
	}

	// Primitive type parsing
	qi::rule<Iterator, std::string(), ascii::space_type> quoted_string;

	// We use this to parse doubles - strict_real_policies requires a decimal point
	qi::real_parser<double, qi::strict_real_policies<double> > dblParser;

	// Data value parsers
	qi::rule<Iterator, etfnode(), ascii::space_type> dataVal;
	qi::rule<Iterator, std::vector<etfnode>(), ascii::space_type> listObj;
	qi::rule<Iterator, std::pair<etfnode, etfnode>(), ascii::space_type> pairObj;
	qi::rule<Iterator, std::map<std::string, etfnode>(), ascii::space_type> mapObj;

	qi::rule<Iterator, std::pair<std::string, etfnode>(), ascii::space_type> mapPair;
	qi::rule<Iterator, std::string(), ascii::space_type> mapKey;

	qi::rule<Iterator, std::map<std::string, etfnode>(), ascii::space_type> start;
};

// Generates one-line minimal representations of ETF trees
template <typename OutputIterator>
struct etf_grammar : karma::grammar<OutputIterator, std::map<std::string, etfnode>()> {
	etf_grammar() : etf_grammar::base_type(etf) {
		using karma::long_;
		using karma::eol;
		using karma::double_;
		using karma::bool_;
		using karma::lit;
		using karma::eps;
		using karma::string;
		using karma::omit;
		using karma::auto_;
		
		quotedString = lit('"') << string << lit('"');

		dtSyms.add
			(ETFDocument::DT_INT, "int")
			(ETFDocument::DT_STRING, "string")
			(ETFDocument::DT_FLOAT, "float")
			(ETFDocument::DT_BOOL, "bool")
			(ETFDocument::DT_LIST, "list")
			(ETFDocument::DT_PAIR, "pair")
			(ETFDocument::DT_MAP, "map");

		etfNode = omit[dtSyms] << (long_
			| double_
			| bool_
			| listNode
			| pairNode
			| mapNode
			| quotedString);
		
		listNode = '{' << -(etfNode % ',') << '}';
		pairNode = '<' << etfNode << ',' << etfNode << '>';
		mapNode = '(' << -(mapping % ',') << ')';
		mapping = string << '=' << etfNode;

		etf = mapping % ';';
	}

	karma::symbols<ETFDocument::DataType, std::string> dtSyms;

	karma::rule<OutputIterator, std::map<std::string, etfnode>()> etf;
	karma::rule<OutputIterator, std::vector<etfnode>()> listNode;
	karma::rule<OutputIterator, std::pair<etfnode, etfnode>()> pairNode;
	karma::rule<OutputIterator, std::map<std::string, etfnode>()> mapNode;
	karma::rule<OutputIterator, std::pair<std::string, etfnode>()> mapping;
	karma::rule<OutputIterator, std::string()> quotedString;
	karma::rule<OutputIterator, etfnode()> etfNode;
};

ETFDocument::etfnode::etfnode(const std::string& s) {
	type = DT_STRING;
	value = s;
}

ETFDocument::etfnode::etfnode(const long i) {
	type = DT_INT;
	value = i;
}

ETFDocument::etfnode::etfnode(const double d) {
	type = DT_FLOAT;
	value = d;
}

ETFDocument::etfnode::etfnode(const bool b) {
	type = DT_BOOL;
	value = b;
}

ETFDocument::etfnode::etfnode(const std::vector<etfnode>& n) {
	type = DT_LIST;
	value = n;
}

ETFDocument::etfnode::etfnode(const std::pair<etfnode, etfnode>& p) {
	type = DT_PAIR;
	value = p;
}

ETFDocument::etfnode::etfnode(const std::map<std::string, etfnode>& p) {
	type = DT_MAP;
	value = p;
}

ETFDocument::etfnode::etfnode() {
}

ETFDocument::ETFDocument() {
	m_root = etfnode(std::map<std::string, etfnode>());
}

ETFDocument::ETFDocument(std::string content) {
	etf_parser<std::string::iterator> parser;
	std::map<std::string, etfnode> results;
	std::string::iterator begin = content.begin();
	std::string::iterator end = content.end();
	bool result = qi::phrase_parse(begin, end, parser, ascii::space, results);
	if(!result) throw std::runtime_error("Cannot parse ETF document");

	m_root = etfnode(results);
}

ETFDocument::~ETFDocument() {
}

void dumpMap(std::ostream &s, etfnode& n, unsigned int indent);
void dumpList(std::ostream &s, etfnode& n, unsigned int indent);
void dumpPair(std::ostream &s, etfnode& n, unsigned int indent);

void dumpNode(std::ostream &s, etfnode& n, unsigned int indent) {
	switch(n.type) {
		case ETFDocument::DT_INT:
			s << boost::get<long>(n.value); break;
		case ETFDocument::DT_STRING:
			s << boost::get<std::string>(n.value); break;
		case ETFDocument::DT_FLOAT:
			s << boost::get<double>(n.value); break;
		case ETFDocument::DT_BOOL:
			s << (boost::get<bool>(n.value)) ? ("true") : ("false"); break;
		case ETFDocument::DT_LIST:
			dumpList(s, n, indent); break;
		case ETFDocument::DT_PAIR:
			dumpPair(s, n, indent); break;
		case ETFDocument::DT_MAP:
			dumpMap(s, n, indent); break;
	}
}

void dumpMap(std::ostream &s, etfnode& n, unsigned int indent) {
	char ibuf[indent+1];
	char ibuf_in[indent+2];
	memset(ibuf, 0, indent+1);
	memset(ibuf_in, 0, indent+2);
	memset(ibuf, '\t', indent);
	memset(ibuf_in, '\t', indent+1);

	std::map<std::string, etfnode> &m = boost::get<std::map<std::string, etfnode> >(n.value);
	std::map<std::string, etfnode>::iterator i;
	s << "(\n";
	for(i=m.begin();i != m.end();i++) {
		s << ibuf_in << i->first << " = ";
		dumpNode(s, i->second, indent+1);
		s << ",\n";
	}
	s << ibuf << ")";
}

void dumpList(std::ostream &s, etfnode& n, unsigned int indent) {
	char ibuf[indent+1];
	char ibuf_in[indent+2];
	memset(ibuf, 0, indent+1);
	memset(ibuf_in, 0, indent+2);
	memset(ibuf, '\t', indent);
	memset(ibuf_in, '\t', indent+1);

	s << "{\n";
	std::vector<etfnode> &r = boost::get<std::vector<etfnode> >(n.value);
	std::vector<etfnode>::iterator i;
	for(i=r.begin();i != r.end();i++) {
		s << ibuf_in;
		dumpNode(s, *i, indent+1);
		s << ",\n";
	}
	s << ibuf << '}';
}

void dumpPair(std::ostream &s, etfnode& n, unsigned int indent) {
	s << '<';
	std::pair<etfnode, etfnode> &r = boost::get<std::pair<etfnode, etfnode> >(n.value);
	dumpNode(s, r.first, indent+1);
	s << ", ";
	dumpNode(s, r.second, indent+1);
	s << '>';
}

void ETFDocument::dump(std::ostream &s, bool formatted) {
	if(!formatted) {
		etf_grammar<std::back_insert_iterator<std::string> > g;
		std::string gen;
		std::back_insert_iterator<std::string> sink(gen);
		karma::generate(sink, g, boost::get<std::map<std::string, etfnode> >(m_root.value));
		s << gen;
	} else {
		dumpNode(s, m_root, 0);
	}
}

etfnode& ETFDocument::getRoot() {
	return m_root;
}

