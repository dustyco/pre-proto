#pragma once


#include <boost/thread/shared_mutex.hpp>
#include <OGRE/Ogre.h>
#include "../config/config.h"

#define WINDOW_TITLE "Proto"
#define DEFAULT_FULLSCREEN false
#define DEFAULT_VSYNC true
#define DEFAULT_GAMMA true


// In charge of the ogre window according to settings in the ConfigManager
// Outside systems are to use the applySettings function
class DisplayManager : public boost::shared_mutex {
public:
	DisplayManager (ConfigManager* config, Ogre::Root* root);
	~DisplayManager ();
	
	// Returns true when the window has been recreated and needs a new viewport
	bool needsNewViewport ();
	
	// Uses settings from the ConfigManager
	// Tries to disrupt things only as much as necessary
	// (Only FSAA requires the window to be recreated)
	void applySettings ();
	
	// So outside systems can manage the camera and viewport
	// Hopefully this pointer will never become invalid :S
	Ogre::RenderWindow* getRenderWindow () {return m_renderWindow;}
	
private:
	void _initWindow ();
	// Fills fields with valid settings from ConfigManager or auto-generated defaults
	void _getSettings (int& width, int& height, bool& fullscreen, Ogre::NameValuePairList& misc);
	// Scans the rendersystem's possible resolutions for the largest
	void _getBestRes (int& width, int& height);
	// Checks if it matches an available mode in the rendersystem
	bool _isValidRes (int width, int height);
	bool _isValidRes (std::string res);
	// Just converts a resolution string to numbers
	void _parseRes (std::string res, int& width, int& height);
	
	ConfigManager*               m_config;
	Ogre::Root*                  m_root;
	Ogre::RenderWindow*          m_renderWindow;
	Ogre::RenderWindow*          m_dummyWindow;
	bool                         m_needs_new_viewport;
};


