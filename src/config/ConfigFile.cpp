

#include "etf.h"
#include "ConfigFile.h"
#include "../logging/logging.h"


ConfigFile::ConfigFile (std::string filename = NULL, bool save_on_destruct = true) {
	m_save_on_destruct = save_on_destruct;
	if (filename) {
		m_filename = filename;
		load (filename);
	}
	else throw std::runtime_error("No filename");
}
ConfigFile::~ConfigFile () {
	if (m_save_on_destruct) {
		try save(m_filename);
		catch (std::runtime_exception e) ERROR(e.what());
	}
}

bool ConfigFile::load (std::string filename)
{
	return false;
}
bool ConfigFile::save (std::string filename)
{
	return false;
}

std::string ConfigFile::get (std::string key)
{
	
}
