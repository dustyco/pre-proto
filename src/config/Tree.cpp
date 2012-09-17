

#include <fstream>
#include <stack>
#include <boost/tokenizer.hpp>

#include "../logging/logging.h"
#include "ConfigManager.h"


// Access the appropriate ETFDocument and apply it
void ConfigManager::Tree::set (std::string key, ETFDocument::etfnode value)
{
	try {
		// Parse the key string
		std::string file;
		typedef std::list<std::string> strings;
		strings subs;
		_doKey(key, file, subs);
	
		// Select the appropriate ETFDocument
		ETFDocument::etfnode* node = &(m_docs[file].getRoot());
	
		// Step down the nodes to find the leaf
		typedef std::map<std::string, ETFDocument::etfnode> etfmap;
//		std::cout << &node << ": " << file << "\n";
		for (strings::iterator s=subs.begin(); s!=subs.end(); s++) {
			if (node->type != ETFDocument::DT_MAP) {
				// Node isn't a map - convert it
				node->value = etfmap();
				node->type = ETFDocument::DT_MAP;
			}
			node = &(boost::get<etfmap>(node->value)[*s]);
//			std::cout << node << ": " << *s << "\n";
		}
		node->value = value.value;
		node->type = value.type;
	} catch (std::runtime_error e) { throw std::runtime_error(std::string("Failed to set config entry: ") + e.what()); }
}


ETFDocument::etfnode ConfigManager::Tree::get (std::string key)
{
	try {
		// Parse the key string
		std::string file;
		typedef std::list<std::string> strings;
		strings subs;
		_doKey(key, file, subs);
	
		// Select the appropriate ETFDocument
		typedef std::map<std::string, ETFDocument::etfnode> etfmap;
		std::map<std::string, ETFDocument>::iterator doc = m_docs.find(file);
		if (doc == m_docs.end()) throw std::runtime_error("File doesn't exist: \"" + file + "\"\n");
		ETFDocument::etfnode* node = &((*doc).second.getRoot());
	
		// Step down the nodes to find the leaf
		for (strings::iterator s=subs.begin(); s!=subs.end(); s++) {
			etfmap::iterator it = (boost::get<etfmap>(node->value)).find(*s);
			if (it == boost::get<etfmap>(node->value).end()) throw std::runtime_error("Entry doesn't exist: \"" + key + "\"");
			node = &((*it).second);
		}
		return *node;
	} catch (std::runtime_error e) { throw std::runtime_error(std::string("Failed to get config entry: ") + e.what()); }
}


void ConfigManager::Tree::remove (std::string key) {
	try {
		// Parse the key string
		std::string file;
		typedef std::list<std::string> strings;
		strings subs;
		_doKey(key, file, subs);
	
		// Select the appropriate ETFDocument
		typedef std::map<std::string, ETFDocument::etfnode> etfmap;
		std::map<std::string, ETFDocument>::iterator doc = m_docs.find(file);
		if (doc == m_docs.end()) return;
		ETFDocument::etfnode* node = &((*doc).second.getRoot());
	
		// Step down the nodes to find the leaf
		for (strings::iterator s=subs.begin(); s!=subs.end(); s++) {
			etfmap::iterator it = (boost::get<etfmap>(node->value)).find(*s);
			if (it == boost::get<etfmap>(node->value).end()) throw std::runtime_error("Entry doesn't exist: \"" + key + "\"");
			node = &((*it).second);
		}
	} catch (std::runtime_error e) { throw std::runtime_error(std::string("Failed to get config entry: ") + e.what()); }
}


void ConfigManager::Tree::load (std::string path)
{
	try {
		// TODO Load all config files from this folder into separate ETFDocuments
//		typedef std::map<std::string, ETFDocument> docmap;
//		m_docs["test_parse"] = ETFDocument("hello=1;test=\"test2\"");
	} catch (std::runtime_error e) {
		WARNING(e.what());
	}
}
void ConfigManager::Tree::save (std::string path) {
	typedef std::map<std::string, ETFDocument> docmap;
	for (docmap::iterator it = m_docs.begin(); it!=m_docs.end(); it++) {
		std::string filename = path + "/" + (*it).first + "." + CONFIG_FILE_EXT;
		PRINT("writing: \"" + filename + "\"");
		std::ofstream out(filename.c_str());
//		if (out.good()) (*it).second.dump(std::cout);
		if (out.good()) { (*it).second.dump(out, true); out << "\n"; }
		else ERROR("failed to write to \"" + filename + "\"");
	}
}

void ConfigManager::Tree::report () {
	typedef std::map<std::string, ETFDocument> docmap;
	for (docmap::iterator doc = m_docs.begin(); doc!=m_docs.end(); doc++) {
		std::cout << (*doc).first + "\n";
		_recurse((*doc).second.getRoot(), std::string(""));
	}
}
void ConfigManager::Tree::_recurse (ETFDocument::etfnode& node, std::string path) {
	if (node.type == ETFDocument::DT_MAP) {
		typedef std::map<std::string, ETFDocument::etfnode> nodemap;
		for (nodemap::iterator it = boost::get<nodemap>(node.value).begin(); it!=boost::get<nodemap>(node.value).end(); it++)
			_recurse((*it).second, path + "/" + (*it).first);
	} else {
		switch (node.type) {
			case ETFDocument::DT_STRING:
				std::cout << "\t" << path << " = \"" << boost::get<std::string>(node.value) << "\"\n";
				break;
			case ETFDocument::DT_FLOAT:
				std::cout << "\t" << path << " = " << boost::get<double>(node.value) << "\n";
				break;
			case ETFDocument::DT_INT:
				std::cout << "\t" << path << " = " << boost::get<long>(node.value) << "\n";
				break;
			case ETFDocument::DT_BOOL:
				std::cout << "\t" << path << " = " << (boost::get<bool>(node.value)?"true":"false") << "\n";
				break;
		}
	}
}


// INTERNAL FUNCTIONS /////////////////////////////////////////////////////////

// Break a config key up into tokens for parsing
std::list<std::string> ConfigManager::Tree::_split (std::string key, std::string delimiter) {
	typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
	boost::char_separator<char> delim(delimiter.c_str(), "", boost::keep_empty_tokens);
	tokenizer toks(key, delim);
	std::list<std::string> fields;
	for (tokenizer::iterator tok=toks.begin(); tok!=toks.end(); tok++)
		fields.push_back(*tok);
	return fields;
}

// Parse a key string, saving it to parameters 2 and 3
void ConfigManager::Tree::_doKey (std::string key, std::string& file, std::list<std::string>& subs) {
	std::string temp_key = key;
	typedef std::list<std::string> strings;
	subs = _split(key, ":");
	if (subs.size()==2 && subs.back().size()>0) {
		if (subs.front().size()>0) file = subs.front();
		else file = CONFIG_FILE_DEFAULT;
		temp_key = subs.back();
	} else if (subs.size()==1 && subs.front().size()>0) {
		file = CONFIG_FILE_DEFAULT;
		temp_key = subs.front();
	} else throw std::runtime_error("Invalid key string: \"" + key + "\"");
	
	subs = _split(temp_key, "/\\");
	if (subs.size()<1) throw std::runtime_error("Invalid key string: \"" + key + "\"");
}


