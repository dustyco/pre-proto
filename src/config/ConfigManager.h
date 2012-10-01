#pragma once


#include <stdexcept>
#include <string>
#include <list>
#include <map>

#include "etf.h"


#ifdef _WIN32
	#define CONFIG_FOLDER "proto"
	#define USER_BEFORE_GAME false
#else
	#define CONFIG_FOLDER ".proto"
	#define USER_BEFORE_GAME true
#endif
#define CONFIG_FILE_DEFAULT "proto"
#define CONFIG_FILE_EXT ".cfg"
#define SAVE_ON_DESTROY true


class ConfigManager {
public:
	ConfigManager (int argc, char** argv);
	~ConfigManager ();
	
	// Throws std::runtime_error if:
	//  -key string is invalid
	void        set (std::string key, std::string value);
	void        set (std::string key, const char* value);
	void        set (std::string key, double value);
	void        set (std::string key, long value); // Literals must be explicit: (long)42, 42l, 42L
	void        set (std::string key, bool value);
	
	// Recommended for normal use:
	//  -sets and returns default_value if the value isn't there or is the wrong type
	// Throws std::runtime_error if:
	//  -key string is invalid
	std::string getString (std::string key, std::string default_value);
	double      getFloat  (std::string key, double default_value);
	long        getInt    (std::string key, long default_value);
	bool        getBool   (std::string key, bool default_value);
	
	// Throws std::runtime_error if:
	//  -key string is invalid
	//  -the value isn't there
	//  -it's the wrong type
	std::string getString (std::string key);
	double      getFloat  (std::string key);
	long        getInt    (std::string key);
	bool        getBool   (std::string key);
	
	// Throws std::runtime_error if:
	//  -key string is invalid
	// Will not throw if the element already doesn't exist
	void        remove (std::string key);
	
	void        load ();
	void        save ();
	void        report ();
	
private:
	struct Tree {
		void                   set      (std::string key, ETFDocument::etfnode value);
		ETFDocument::etfnode   get      (std::string key);
		void                   remove   (std::string key);
		void                   load     (std::string path);
		void                   save     (std::string path);
		void                   report   ();
		void                   _clean       ();
		bool                   _clean_node  (ETFDocument::etfnode& node);
		void                   _report_node (ETFDocument::etfnode& node, std::string path);
		std::list<std::string> _split       (std::string key, std::string delimiter);
		void                   _doKey       (std::string key, std::string& file, std::list<std::string>& subs);
		
		std::map<std::string, ETFDocument> m_docs;
	};
	
	std::string          _getUserFolder ();
	void                 _prepFolder (std::string path);
	void                 _set (std::string key, ETFDocument::etfnode value);
	ETFDocument::etfnode _get (std::string key);
	
	Tree        m_p;
	Tree        m_d;
	bool        m_save_on_destroy;
	bool        m_user_before_game;
};


