

#include "ConfigManager.h"
#include "../logging/logging.h"

#include <string>
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


ConfigManager::ConfigManager (int argc, char** argv)
{
	// PARSE COMMAND LINE ARGUMENTS ///////////////////////////////////////////
	if (argc>1) {
		// EXPAND ALIASES
		// PARSE
		po::options_description desc("Allowed options");
		desc.add_options()
			("help,h", "produce help message")
			(",s", po::value<std::string>(), "override a setting")
		;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
			std::cout << desc << "\n";
		}

		if (vm.count("set")) {
			std::cout << "set: " << vm["set"].as<std::string>() << ".\n";
		}
	}
	
	// Set the user's home folder
	m_home_is_good = false;
	try {
		m_home = getHomeFolder();
		if (fs::is_directory(m_home)) m_home_is_good = true;
		else throw std::runtime_error("Home folder doesn't exist: \"" + m_home + "\"");
	} catch (std::runtime_error& e) { WARNING(e.what()); }
}

std::string ConfigManager::getHomeFolder ()
{
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


