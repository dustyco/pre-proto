#pragma once

#include <string>
#include <map>
#include <vector>
#include <utility>
#include <ostream>

#include <boost/variant.hpp>
#include <boost/variant/recursive_variant.hpp>

class ETFDocument {
public:
	enum DataType {
		DT_INT,
		DT_STRING,
		DT_FLOAT,
		DT_BOOL,
		DT_LIST,
		DT_PAIR,
		DT_MAP
	};

	struct etfnode;

	typedef boost::variant<
		long,
		std::string,
		double,
		bool,
		boost::recursive_wrapper<std::vector<etfnode> >,
		boost::recursive_wrapper<std::pair<etfnode, etfnode> >,
		boost::recursive_wrapper<std::map<std::string, etfnode> > > etfvalue;

	struct etfnode {
		DataType type;
		etfvalue value;

		etfnode(const std::string& s);
		etfnode(const long i);
		etfnode(const double d);
		etfnode(const bool b);
		etfnode(const std::vector<etfnode>& n);
		etfnode(const std::pair<etfnode, etfnode>& p);
		etfnode(const std::map<std::string, etfnode>& p);
		etfnode();
	};
public:
	ETFDocument();
	ETFDocument(std::string content);
	~ETFDocument();

	etfnode& getRoot();

	void dump(std::ostream &s, bool formatted=false);

private:
	etfnode m_root;
};
