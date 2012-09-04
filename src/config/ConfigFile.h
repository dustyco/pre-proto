#pragma once


#include <stdexcept>


class ConfigFile {
public:
	ConfigFile (std::string filename = NULL, bool save_on_destruct = false);
	~ConfigFile ();
	
	bool save ();
	
	std::string get (std::string key);

private:
	bool load (std::string filename);
	bool save (std::string filename);
	
	std::string m_filename;
	bool        m_save_on_destruct;
	ETFDocument m_etfdoc;
};
