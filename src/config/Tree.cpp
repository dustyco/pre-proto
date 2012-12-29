

#include <fstream>
#include <sstream>
#include <stack>
#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include "../logging/logging.h"
#include "ConfigManager.h"


ConfigManager::Tree::Tree () { INFO("Tree"); }
ConfigManager::Tree::~Tree () { INFO("~Tree"); }
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
		for (strings::iterator s=subs.begin(); s!=subs.end(); s++) {
			if (node->type != ETFDocument::DT_MAP) {
				// Node isn't a map - convert it
				node->value = etfmap();
				node->type = ETFDocument::DT_MAP;
			}
			node = &(boost::get<etfmap>(node->value)[*s]);
		}
		node->value = value.value;
		node->type = value.type;
	} catch (std::runtime_error e) { throw std::runtime_error(std::string("Failed to set config entry: ") + e.what()); }
}


ETFDocument::etfnode ConfigManager::Tree::get (std::string key)
{
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
		if (node->type != ETFDocument::DT_MAP) throw std::runtime_error("Entry doesn't exist: \"" + key + "\"");
		etfmap::iterator it = (boost::get<etfmap>(node->value)).find(*s);
		if (it == boost::get<etfmap>(node->value).end()) throw std::runtime_error("Entry doesn't exist: \"" + key + "\"");
		node = &((*it).second);
	}
	return *node;
}


void ConfigManager::Tree::remove (std::string key) {
	try {
		// Parse the key string
		std::string file;
		typedef std::list<std::string> strings;
		strings subs;
		_doKey(key, file, subs);
		if (subs.size()<1) return;
		std::string leaf = subs.back(); subs.pop_back();
	
		// Select the appropriate ETFDocument
		typedef std::map<std::string, ETFDocument::etfnode> etfmap;
		std::map<std::string, ETFDocument>::iterator doc = m_docs.find(file);
		if (doc == m_docs.end()) return;
		ETFDocument::etfnode* node = &((*doc).second.getRoot());
	
		// Step down the nodes to find the map just before the leaf
		etfmap::iterator it;
		for (strings::iterator s=subs.begin(),end = subs.end(); s!=end; s++) {
			if (node->type != ETFDocument::DT_MAP) return;
			etfmap& map = boost::get<etfmap>(node->value);
			it = map.find(*s);
			if (it == map.end()) return;
			node = &((*it).second);
		}
		if (node->type != ETFDocument::DT_MAP) return;
		etfmap& map = boost::get<etfmap>(node->value);
		it = map.find(leaf);
		if (it == map.end()) return;
		map.erase(it);
		_clean();
	} catch (std::runtime_error e) { throw std::runtime_error(std::string("Failed to remove config entry: ") + e.what()); }
}

// Remove empty maps from each file
void ConfigManager::Tree::_clean () {
	typedef std::map<std::string, ETFDocument> docmap;
	for (docmap::iterator doc = m_docs.begin(); doc!=m_docs.end(); doc++)
		_clean_node((*doc).second.getRoot());
}
// Returns true if the node is empty and can be removed
bool ConfigManager::Tree::_clean_node (ETFDocument::etfnode& node) {
	if (node.type == ETFDocument::DT_MAP) {
		typedef std::map<std::string, ETFDocument::etfnode> etfmap;
		etfmap& map = boost::get<etfmap>(node.value);
		if (map.empty()) return true;
		etfmap::iterator it = map.begin();
		while (it != map.end()) {
			if (_clean_node((*it).second)) map.erase(it++);
			else it++;
			if (map.empty()) return true;
		}
	}
	return false;
}


// Load all config files from path into separate ETFDocuments
void ConfigManager::Tree::load (std::string path)
{
	try {
	if (!fs::is_directory(path)) throw std::runtime_error("Folder doesn't exist");
		for (fs::directory_iterator it(path),end; it!=end; it++)
			try {
				if (!fs::is_regular_file(*it)) continue;
				if (fs::extension(*it).compare(CONFIG_FILE_EXT) != 0) continue;
				std::string basename = fs::basename(*it);
//				std::cout << basename << ": " << *it << "\n";
				std::ifstream file((*it).path().native().c_str());
				std::stringstream buffer;
				buffer << file.rdbuf();
				file.close();
//				WARNING("input data:");
//				std::cout << buffer.str();
				m_docs[basename] = ETFDocument(buffer.str());
			} catch (std::exception) {}
	} catch (std::exception e) { WARNING(e.what()); }
	_clean();
}
// Save all ETFDocuments to separate files in path 
void ConfigManager::Tree::save (std::string path) {
	if (!fs::is_directory(path)) throw std::runtime_error("Folder doesn't exist");
	_clean();
	typedef std::map<std::string, ETFDocument> docmap;
	for (docmap::iterator it = m_docs.begin(); it!=m_docs.end(); it++)
		try {
			std::string filename = path + "/" + (*it).first + CONFIG_FILE_EXT;
			if (boost::get<std::map<std::string, ETFDocument::etfnode> >((*it).second.getRoot().value).size() == 0) {
				// It's empty, delete the file
				if (fs::is_regular_file(filename)) fs::remove(filename);
				continue;
			}
//			PRINT("Writing: \"" + filename + "\"");
			std::ofstream out(filename.c_str());
			if (out.good()) { (*it).second.dump(out, true); out << "\n"; }
			else WARNING("Failed to write configuration file: \"" + filename + "\"");
		} catch (std::exception) {}

}

void ConfigManager::Tree::report () {
	typedef std::map<std::string, ETFDocument> docmap;
	for (docmap::iterator doc = m_docs.begin(); doc!=m_docs.end(); doc++) {
		std::cout << (*doc).first + "\n";
		_report_node((*doc).second.getRoot(), std::string(""));
	}
}
void ConfigManager::Tree::_report_node (ETFDocument::etfnode& node, std::string path) {
	if (node.type == ETFDocument::DT_MAP) {
		typedef std::map<std::string, ETFDocument::etfnode> etfmap;
		etfmap& map = boost::get<etfmap>(node.value);
		if (map.empty()) std::cout << "\t" << path << "/\n";
		else for (etfmap::iterator it = map.begin(); it!=map.end(); it++)
			_report_node((*it).second, path + "/" + (*it).first);
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
			default: break;
		}
	}
}


// INTERNAL FUNCTIONS /////////////////////////////////////////////////////////

// Splits a string on a given delimiter, includes blank tokens
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


