#pragma once


#include <string>
#include <stdexcept>


class ConfigManager {
public:
	ConfigManager (int argc, char** argv);
	
private:
	std::string getHomeFolder ();
	
	bool        m_home_is_good;
	std::string m_home;
};


