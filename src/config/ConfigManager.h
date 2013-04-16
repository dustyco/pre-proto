#pragma once


#include <stdexcept>
#include <string>
#include <map>
#include <list>

#ifndef PCH
	#include <boost/property_tree/ptree.hpp>
#endif
using boost::property_tree::ptree;

#include "../logging/logging.h"


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
	
	// Required because c++ string literals are still const char*
	// and the parser can't stream extract to it
	std::string get (std::string key, const char* default_value) { return get<std::string>(key, default_value); }
	
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
		void                    _doKey   (std::string key, std::string& file, std::string& path);
		
		treemap                 m_trees;
	};
	
	std::string          _getUserFolder ();
	void                 _prepFolder (std::string path);
	
	Tree        m_p;
	Tree        m_d;
	bool        m_save_on_destroy;
	bool        m_user_before_game;
};


// These 4 functions need to be in the header because templates

template <class T>
void ConfigManager::set (std::string key, T value) {
	m_p.remove(key);
	m_d.set(key, value);
}
template <class T>
T ConfigManager::get (std::string key, T default_value) {
	try {
		try { return m_p.get<T>(key); }
		catch (std::runtime_error) {}
		return m_d.get<T>(key);
	} catch (std::runtime_error) { set(key, default_value); return default_value; }
}

template <class T>
void ConfigManager::Tree::set (std::string key, T value)
{
	try {
		// Parse the key string
		std::string file, path;
		_doKey(key, file, path);
	
		// Select the appropriate file (ptree) and apply
		m_trees[file].put<T>(path, value);
		
	} catch (std::runtime_error e) { throw std::runtime_error(std::string("Failed to set config entry: ") + e.what()); }
}

template <class T>
T ConfigManager::Tree::get (std::string key)
{
	// Parse the key string
	std::string file, path;
	_doKey(key, file, path);
	
	// Select the appropriate file (ptree)
	std::map<std::string, ptree>::iterator tree = m_trees.find(file);
	if (tree == m_trees.end()) throw std::runtime_error("File doesn't exist: \"" + file + "\"\n");
	
	// Get the entry
	try { return tree->second.get<T>(path); }
	catch (boost::property_tree::ptree_error) { std::runtime_error("Entry doesn't exist/wrong type"); }
	
	return T();
}

