

#include <vector>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#ifdef unix
	#include <unistd.h>
	#include <pwd.h>
#endif

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;
//#include <boost/program_options.hpp>
//namespace po = boost::program_options;

#include "../logging/logging.h"
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


template <class T>
void ConfigManager::set (std::string key, T value) {
	m_p.remove(key);
	m_d.set(key, value);
}
template <class T>
T ConfigManager::get (std::string key, T default_value) {
	try {
		return m_p.get<T>(key);
		return m_d.get<T>(key);
	} catch (std::runtime_error) { set(key, default_value); return default_value; }
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


