

#include <fstream>
#include <sstream>
#include <stack>
#include <vector>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#ifdef unix
	#include <unistd.h>
	#include <pwd.h>
#endif

#include <boost/property_tree/info_parser.hpp>
#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;
using boost::property_tree::ptree;
//#include <boost/program_options.hpp>
//namespace po = boost::program_options;

#include "logging/logging.h"
#include "ConfigManager.h"


ConfigManager::ConfigManager (int argc, char** argv)
{
	m_save_on_destroy = SAVE_ON_DESTROY;
	m_user_before_game = USER_BEFORE_GAME;
	
	// PARSE COMMAND LINE ARGUMENTS ///////////////////////////////////////////
/*	if (argc>1) {
		// TODO EXPAND ALIASES
		// PARSE
		// TODO Fix duplicate flag crash
		po::options_description desc("Allowed options");
		desc.add_options()
			("help,h", "produce help message")
			("set,s", po::value<std::string>(), "override a configuration value")
		;

		INFO("Parsing command line parameters (may crash on duplicates)");
		po::variables_map vm;
		try {
			po::store(po::parse_command_line(argc, argv, desc), vm);
			po::notify(vm);
		} catch (std::exception e) { WARNING("Crashed (on duplicates)"); }
		
		// Carry out commands
		if (vm.count("help")) std::cout << desc << "\n";
		if (vm.count("set")) {
			PRINT("config param: " + vm["set"].as<std::string>());
			// m_p is the parameter tree - it overrides m_d but won't be saved and will be written through with set()'s
			m_p.set(vm["set"].as<std::string>(), ETFDocument::etfnode(std::string("on")));
		}
	}
*/
	load();
}
ConfigManager::~ConfigManager ()
{
	if (m_save_on_destroy) {
		try {
			save();
		} catch (std::runtime_error e) {
			WARNING(std::string("Failed to save configuration: ") + e.what());
		}
	}
}


void ConfigManager::remove (std::string key) {
	m_p.remove(key);
	m_d.remove(key);
}


void ConfigManager::load ()
{
	// Clear any existing settings
	m_p = Tree();
	m_d = Tree();
	
	std::string dir(".");
	if (m_user_before_game) {
		try {
			INFO("Loading configuration from user folder");
			// Try user folder first
			dir = _getUserFolder() + "/" + CONFIG_FOLDER;
			_prepFolder(dir);
			m_d.load(dir);
		} catch (std::exception e) {
			WARNING(std::string("Failed to load configuration from user folder: ") + e.what());
			WARNING("Using the game folder instead");
			try {
				// Fallback to game folder
				dir = ".";
				_prepFolder(dir);
				m_d.load(dir);
			} catch (std::exception e) {
				// Fail fail fail fail fail
				WARNING(std::string("Failed to load configuration from game folder: ") + e.what());
			}
		}
	} else {
		try {
			INFO("Loading configuration from game folder");
			// Try game folder first
			dir = ".";
			_prepFolder(dir);
			m_d.load(dir);
		} catch (std::exception e) {
			WARNING(std::string("Failed to load configuration from game folder: ") + e.what());
			WARNING("Using the user folder instead");
			try {
				// Fallback to user folder
				dir = _getUserFolder() + "/" + CONFIG_FOLDER;
				_prepFolder(dir);
				m_d.load(dir);
			} catch (std::exception e) {
				// Fail fail fail fail fail
				WARNING(std::string("Failed to load configuration from user folder: ") + e.what());
			}
		}
	}
}
void ConfigManager::save ()
{
	std::string dir(".");
	if (m_user_before_game) {
		try {
			INFO("Saving configuration to user folder");
			// Try user folder first
			dir = _getUserFolder() + "/" + CONFIG_FOLDER;
			_prepFolder(dir);
			m_d.save(dir);
		} catch (std::exception e) {
			WARNING(std::string("Failed to save configuration to user folder: ") + e.what());
			WARNING("Using the game folder instead");
			try {
				// Fallback to game folder
				dir = ".";
				_prepFolder(dir);
				m_d.load(dir);
			} catch (std::exception e) {
				// Fail fail fail fail fail
				WARNING(std::string("Failed to save configuration to game folder: ") + e.what());
			}
		}
	} else {
		try {
			INFO("Saving configuration to game folder");
			// Try game folder first
			dir = ".";
			_prepFolder(dir);
			m_d.load(dir);
		} catch (std::exception e) {
			WARNING(std::string("Failed to save configuration to game folder: ") + e.what());
			WARNING("Using the user folder instead");
			try {
				// Fallback to user folder
				dir = _getUserFolder() + "/" + CONFIG_FOLDER;
				_prepFolder(dir);
				m_d.load(dir);
			} catch (std::exception e) {
				// Fail fail fail fail fail
				WARNING(std::string("Failed to save configuration to user folder: ") + e.what());
			}
		}
	}
}


// INTERNAL FUNCTIONS /////////////////////////////////////////////////////////
std::string ConfigManager::_getUserFolder () {
#ifdef _WIN32
	// Use %APPDATA% first
	const char* str = getenv("APPDATA");
	if (str != NULL && strlen(str) > 0)
		return std::string(str);
	
	// Try the home folder instead
	const char* str1 = getenv("HOMEDRIVE");
	const char* str2 = getenv("HOMEPATH");
	if (str1 != NULL && strlen(str1) > 0 && str2 != NULL && strlen(str2) > 0)
		return std::string(str1) + std::string(str2);
#else
	// Check the HOME environment variable
	const char* str = getenv("HOME");
	if (str != NULL && strlen(str) > 0)
		return std::string(str);
	
	// Check the user settings
	str = (getpwuid(getuid()))->pw_dir;
	if (str != NULL && strlen(str) > 0)
		return std::string(str);
#endif
	// Failed
	throw std::runtime_error("Couldn't determine user folder");
}
void ConfigManager::_prepFolder (std::string path) {
	try {
		if (!fs::exists(path)) fs::create_directories(path);
		if (!fs::is_directory(path)) throw std::runtime_error("Can't create folder");
	} catch (std::exception e) { throw std::runtime_error(std::string("Couldn't prepare folder: ") + e.what()); }
}


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
#ifdef _WIN32
				read_info( (const wchar*)((*it).path().native().c_str()), m_trees[fs::basename(*it)] );
#else
				read_info( (*it).path().native(), m_trees[fs::basename(*it)] );
#endif
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


