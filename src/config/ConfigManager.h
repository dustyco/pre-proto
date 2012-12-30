#pragma once


#include <stdexcept>
#include <string>
#include <map>
#include <list>

#include <boost/property_tree/ptree.hpp>
using boost::property_tree::ptree;


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
	template <class T>
	void set (std::string key, T value);
	
	// Sets and returns default_value if the value isn't there or is the wrong type
	// Throws std::runtime_error if:
	//  -key string is invalid
	template <class T>
	T    get (std::string key, T default_value);
	
	// Throws std::runtime_error if:
	//  -key string is invalid
	// Will not throw if the element already doesn't exist
	void remove (std::string key);
	
	void load ();
	void save ();
	
private:
	typedef std::map<std::string, ptree> treemap;
	
	struct Tree {
		template <class T> void set      (std::string key, T value);
		template <class T> T    get      (std::string key);
		void                    remove   (std::string key);
		void                    load     (std::string path);
		void                    save     (std::string path);
		std::list<std::string>  _split   (std::string key, std::string delimiter);
		void                    _doKey   (std::string key, std::string& file, std::list<std::string>& subs);
		
		treemap                 m_trees;
	};
	
	std::string          _getUserFolder ();
	void                 _prepFolder (std::string path);
	
	Tree        m_p;
	Tree        m_d;
	bool        m_save_on_destroy;
	bool        m_user_before_game;
};


