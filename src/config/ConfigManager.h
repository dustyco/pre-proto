#pragma once


#include <stdexcept>
#include <string>
#include <list>
#include <map>

#include "etf.h"


class ConfigManager {
public:
	ConfigManager (int argc, char** argv, bool save_on_destroy = true);
	~ConfigManager ();
	void        set  (std::string key, std::string value);
	void        set  (std::string key, double value);
	void        set  (std::string key, long value);
	void        set  (std::string key, bool value);
	std::string get  (std::string key);
	void        load ();
	void        save ();
	
private:
	struct Tree {
		void        set    (std::string key, ETFDocument::etfvalue value, ETFDocument::DataType type);
		std::string get    (std::string key);
		void        remove (std::string key, ETFDocument::DataType type);
		void        load   (std::string path);
		void        save   (std::string path);
		void        report ();
		void        _recurse (ETFDocument::etfnode& node, std::string path);
		std::list<std::string> _split (std::string key);
		
		std::map<std::string, ETFDocument> m_docs;
	};
	
	std::string _getHomeFolder ();
	void        _set (std::string key, ETFDocument::etfvalue value, ETFDocument::DataType type);
	
	Tree        m_p;
	Tree        m_d;
	bool        m_save_on_destroy;
	bool        m_home_is_good;
	std::string m_home;
};


