

#include <fstream>
#include <sstream>
#include <stack>

#include <boost/property_tree/info_parser.hpp>
using boost::property_tree::ptree;
#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include "../logging/logging.h"
#include "ConfigManager.h"




void ConfigManager::Tree::remove (std::string key) {
/*	try {
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
*/
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
				read_info( (*it).path().native(), m_trees[fs::basename(*it)] );
			} catch (std::exception) {}
	} catch (std::exception e) { WARNING(e.what()); }
}
// Save all ETFDocuments to separate files in path 
void ConfigManager::Tree::save (std::string path) {
	if (!fs::is_directory(path)) throw std::runtime_error("Folder doesn't exist");
	for (treemap::iterator it = m_trees.begin(); it!=m_trees.end(); it++)
		try {
			std::string filename = path + "/" + (*it).first + CONFIG_FILE_EXT;
			
			// TODO Delete the file if it's empty
/*			if (boost::get<std::map<std::string, ETFDocument::etfnode> >((*it).second.getRoot().value).size() == 0) {
				// It's empty, delete the file
				if (fs::is_regular_file(filename)) fs::remove(filename);
				continue;
			}
*/			write_info(filename, it->second);
		} catch (std::exception) {}

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
void ConfigManager::Tree::_doKey (std::string key, std::string& file, std::string& path) {
	typedef std::list<std::string> strings;
	strings subs = _split(key, ":");
	if (subs.size()==2 && subs.back().size()>0) {
		if (subs.front().size()>0) file = subs.front();
		else file = CONFIG_FILE_DEFAULT;
		path = subs.back();
	} else if (subs.size()==1 && subs.front().size()>0) {
		file = CONFIG_FILE_DEFAULT;
		path = subs.front();
	} else throw std::runtime_error("Invalid key string: \"" + key + "\"");
}


