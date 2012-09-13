

#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>
#ifdef unix
	#include <string.h>
	#include <unistd.h>
	#include <pwd.h>
#endif

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "../logging/logging.h"
#include "ConfigManager.h"

// Constructor
ConfigManager::ConfigManager (int argc, char** argv, bool save_on_destroy)
{
	m_save_on_destroy = save_on_destroy;
	
	// PARSE COMMAND LINE ARGUMENTS ///////////////////////////////////////////
	if (argc>1) {
		// TODO EXPAND ALIASES
		// PARSE
		po::options_description desc("Allowed options");
		desc.add_options()
			("help,h", "produce help message")
			("set,s", po::value<std::string>(), "override a configuration value")
		;

		INFO("Parsing command line parameters (may crash on duplicates)");
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
		
		// Carry out commands
		if (vm.count("help")) std::cout << desc << "\n";
		if (vm.count("set")) {
			PRINT("config param: " + vm["set"].as<std::string>());
			set(vm["set"].as<std::string>(), std::string("on"));
		}
	}
	
	INFO("Loading configuration files");
	load();
	set("test:string","foo");
	set("test:float", 3.14);
	set("test:int", (long)42);
	set("test:bool", true);
	
	m_p.report();
	m_d.report();
}
// Destructor
ConfigManager::~ConfigManager () {
	if (m_save_on_destroy) {
		try {
			INFO("Automatically saving configuration files");
			save();
		} catch (std::runtime_error e) {
			WARNING(std::string("Error saving configuration files: ") + e.what());
		}
	}
}



void ConfigManager::set (std::string key, std::string value) {
	_set(key, ETFDocument::etfvalue(value), ETFDocument::DT_STRING);
}
void ConfigManager::set (std::string key, double value) {
	_set(key, ETFDocument::etfvalue(value), ETFDocument::DT_FLOAT);
}
void ConfigManager::set (std::string key, long value) {
	_set(key, ETFDocument::etfvalue(value), ETFDocument::DT_INT);
}
void ConfigManager::set (std::string key, bool value) {
	_set(key, ETFDocument::etfvalue(value), ETFDocument::DT_BOOL);
}



std::string ConfigManager::get (std::string key) {
	try { return m_p.get(key); }
	catch (std::runtime_error) { return m_d.get(key); }
}
void ConfigManager::load ()
{
	// TODO Determine the correct folder to load settings from
	
	// Set the user's home folder
	m_home_is_good = false;
	try {
		m_home = _getHomeFolder();
		if (fs::is_directory(m_home)) m_home_is_good = true;
		else throw std::runtime_error("Home folder doesn't exist: \"" + m_home + "\"");
	} catch (std::runtime_error& e) { WARNING(e.what()); }
	m_d.load(".");
}
void ConfigManager::save ()
{
	// TODO Determine the correct folder to save settings to and write them
	
	// Set the user's home folder
	m_home_is_good = false;
	try {
		m_home = _getHomeFolder();
		if (fs::is_directory(m_home)) m_home_is_good = true;
		else throw std::runtime_error("Home folder doesn't exist: \"" + m_home + "\"");
	} catch (std::runtime_error& e) { WARNING(e.what()); }
	m_d.save(".");
}


// INTERNAL FUNCTIONS /////////////////////////////////////////////////////////

void ConfigManager::_set (std::string key, ETFDocument::etfvalue value, ETFDocument::DataType type) {
	m_p.remove(key, type);
	m_d.set(key, value, type);
}
std::string ConfigManager::_getHomeFolder () {
#ifdef _WIN32
	// Combine environment variables
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
	throw std::runtime_error("Couldn't determine home folder");
}


