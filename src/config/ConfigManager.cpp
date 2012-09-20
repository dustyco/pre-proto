

#include <string>
#include <vector>
#include <iostream>
#include <math.h>
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
		try {
			po::store(po::parse_command_line(argc, argv, desc), vm);
			po::notify(vm);
		} catch (std::exception e) { WARNING("Crashed (on duplicates)"); }
		
		// Carry out commands
		if (vm.count("help")) std::cout << desc << "\n";
		if (vm.count("set")) {
			PRINT("config param: " + vm["set"].as<std::string>());
			m_p.set(vm["set"].as<std::string>(), ETFDocument::etfnode(std::string("on")));
		}
	}
/*			CRITICAL("// REPORT: after parameters ///////////////////////////////////////");
			WARNING("parameters:");
			m_p.report();
			WARNING("directory:");
			m_d.report();
			CRITICAL("///////////////////////////////////////////////////////////////////");
	
	INFO("Loading configuration files");
	load();
	INFO("Testing section");
	set("test_set:value", 5l);
	set("test_set:value/overwrite", 6l);
	set("test_set:value", 7l);
	set("test_set:sub1/sub2/string1", std::string("foo"));
	set("test_set:sub1/sub2/string2", "bar");
	set("test_set:sub1/sub2/float", 3.14);
	set("test_set:sub1/sub2/int", 42L);
	set("test_set:sub1/sub2/bool", false);
	set(":just_colon", 13l);
	set("no_colon", 14l);
	set("sub1/sub2", 15l);
	set("sub1/sub3/sub4", 16l);
	set("sub1/sub3/sub5", 17l);
			CRITICAL("// REPORT /////////////////////////////////////////////////////////");
			WARNING("parameters:");
			m_p.report();
			WARNING("directory:");
			m_d.report();
			CRITICAL("///////////////////////////////////////////////////////////////////");
	std::cout << "getFloat(\"test_set:sub1/sub2/float\") = " << getFloat("test_set:sub1/sub2/float") << "\n";
	std::cout << "getInt(\"test_set:sub1/sub2/float\") = " << getInt("test_set:sub1/sub2/float") << "\n"; // Check conversion
	std::cout << "getBool(\"test_set:sub1/sub2/float\", true) = " << getBool("test_set:sub1/sub2/float", true) << "\n"; // Fails with the non-default version
	std::cout << "getBool(\"test_set:sub1/sub2/float\") = " << getBool("test_set:sub1/sub2/float") << "\n"; // Check that it's been applied
	try { getString("sub1"); }
	catch (std::runtime_error e) { std::cout << "test: " << e.what() << "\n"; }
	try { getString("blank/extra"); }
	catch (std::runtime_error e) { std::cout << "test: " << e.what() << "\n"; }
	try { getString("fake/entry"); }
	catch (std::runtime_error e) { std::cout << "test: " << e.what() << "\n"; }
*/	
/*			CRITICAL("// REPORT: before set ////////////////////////////////////////////");
			WARNING("parameters:");
			m_p.report();
			WARNING("directory:");
			m_d.report();
			CRITICAL("///////////////////////////////////////////////////////////////////");
	set("try/to/remove", 42l);
			CRITICAL("// REPORT: before removal /////////////////////////////////////////");
			WARNING("parameters:");
			m_p.report();
			WARNING("directory:");
			m_d.report();
			CRITICAL("///////////////////////////////////////////////////////////////////");
	remove("try/to/remove");
			CRITICAL("// REPORT: after removal //////////////////////////////////////////");
			WARNING("parameters:");
			m_p.report();
			WARNING("directory:");
			m_d.report();
			CRITICAL("///////////////////////////////////////////////////////////////////");
*/	
/*	PRINT("print");
	INFO("info");
	DEBUG("debug");
	WARNING("warning");
	ERROR("error");
	CRITICAL("critical");
*/	
	
}
ConfigManager::~ConfigManager () {
	if (m_save_on_destroy) {
		try {
			INFO("Automatically saving configuration files");
			CRITICAL("// REPORT /////////////////////////////////////////////////////////");
			WARNING("parameters:");
			m_p.report();
			WARNING("directory:");
			m_d.report();
			CRITICAL("///////////////////////////////////////////////////////////////////");
			save();
		} catch (std::runtime_error e) {
			WARNING(std::string("Failed to save configuration files: ") + e.what());
		}
	}
}



void ConfigManager::set (std::string key, std::string value) {
	_set(key, ETFDocument::etfnode(value));
}
void ConfigManager::set (std::string key, const char* value) {
	_set(key, ETFDocument::etfnode(std::string(value)));
}
void ConfigManager::set (std::string key, double value) {
	_set(key, ETFDocument::etfnode(value));
}
void ConfigManager::set (std::string key, long value) {
	_set(key, ETFDocument::etfnode(value));
}
void ConfigManager::set (std::string key, bool value) {
	_set(key, ETFDocument::etfnode(value));
}


std::string ConfigManager::getString (std::string key, std::string default_value) {
	try { return getString(key); }
	catch (std::runtime_error) { set(key, default_value); return default_value; }
}
double ConfigManager::getFloat (std::string key, double default_value) {
	try { return getFloat(key); }
	catch (std::runtime_error) { set(key, default_value); return default_value; }
}
long ConfigManager::getInt (std::string key, long default_value) {
	try { return getInt(key); }
	catch (std::runtime_error) { set(key, default_value); return default_value; }
}
bool ConfigManager::getBool (std::string key, bool default_value) {
	try { return getBool(key); }
	catch (std::runtime_error) { set(key, default_value); return default_value; }
}


std::string ConfigManager::getString (std::string key) {
	try {
		ETFDocument::etfnode node = _get(key);
		switch (node.type) {
			case ETFDocument::DT_STRING:
				return boost::get<std::string>(node.value); break;
			default:
				throw std::runtime_error("Incorrect type"); break;
		}
	} catch (std::runtime_error e) { throw std::runtime_error(std::string("Failed to get string: ") + e.what()); }
}
double ConfigManager::getFloat (std::string key) {
	try {
		ETFDocument::etfnode node = _get(key);
		switch (node.type) {
			case ETFDocument::DT_FLOAT:
				return boost::get<double>(node.value); break;
			case ETFDocument::DT_INT:
				return boost::get<long>(node.value); break;
			default:
				throw std::runtime_error("Incorrect type"); break;
		}
	} catch (std::runtime_error e) { throw std::runtime_error(std::string("Failed to get float: ") + e.what()); }
}
long ConfigManager::getInt (std::string key) {
	try {
		ETFDocument::etfnode node = _get(key);
		switch (node.type) {
			case ETFDocument::DT_INT:
				return boost::get<long>(node.value); break;
			case ETFDocument::DT_FLOAT:
				return floor(boost::get<double>(node.value) + 0.5); break;
			default:
				throw std::runtime_error("Incorrect type"); break;
		}
	} catch (std::runtime_error e) { throw std::runtime_error(std::string("Failed to get int: ") + e.what()); }
}
bool ConfigManager::getBool (std::string key) {
	try {
		ETFDocument::etfnode node = _get(key);
		switch (node.type) {
			case ETFDocument::DT_BOOL:
				return boost::get<bool>(node.value); break;
			default:
				throw std::runtime_error("Incorrect type"); break;
		}
	} catch (std::runtime_error e) { throw std::runtime_error(std::string("Failed to get bool: ") + e.what()); }
}


void ConfigManager::remove (std::string key) {
	m_p.remove(key);
	m_d.remove(key);
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

void ConfigManager::_set (std::string key, ETFDocument::etfnode value) {
	m_p.remove(key);
	m_d.set(key, value);
}
ETFDocument::etfnode ConfigManager::_get (std::string key) {
	try { return m_p.get(key); }
	catch (std::runtime_error) { return m_d.get(key); }
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


