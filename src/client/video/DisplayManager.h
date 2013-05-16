#pragma once


#include <OGRE/Ogre.h>
#include "config/config.h"

#define WINDOW_TITLE "Proto"
#define DEFAULT_FULLSCREEN false
#define DEFAULT_VSYNC true
#define DEFAULT_GAMMA true


// In charge of the ogre window according to settings in the ConfigManager
// Outside systems are to use the applySettings function
class DisplayManager : public Ogre::WindowEventListener {
public:
	DisplayManager ();
	~DisplayManager ();
	
	// Uses settings from the ConfigManager
	// Tries to disrupt things only as much as necessary
	// (Only FSAA requires the window to be recreated)
	void applySettings ();
	
	// So outside systems can manage the camera and viewport
	// Hopefully this pointer will never be invalid :S
	Ogre::RenderWindow* getRenderWindow () {return m_renderWindow;}
	
	// DOES NOT WORK
	void reinitWindow ();
	
	// Returns true when the window has been created/recreated and external systems need to be aware
	// Resets on being called
	bool windowIsNew ();
	
	// True if the close button was pressed
	// Resets on being called
	bool isClosing ();
	
	// WindowEventListener
	void windowMoved (Ogre::RenderWindow* rw);
	void windowResized (Ogre::RenderWindow* rw);
	void windowFocusChange (Ogre::RenderWindow* rw);
	void windowClosed (Ogre::RenderWindow* rw);
	bool windowClosing (Ogre::RenderWindow* rw);
	
private:
	void _initWindow ();
	void _reinitWindow ();
	
	// Fills fields with valid settings from ConfigManager or auto-generated defaults
	void _getSettings (int& width, int& height, bool& fullscreen, Ogre::NameValuePairList& misc);
	std::string _getValidFSAA ();
	
	// Checks if it matches an available mode in the rendersystem
	bool _isValidRes (int width, int height);
	bool _isValidRes (std::string res);
	// Just converts a resolution string to numbers
	void _parseRes (std::string res, int& width, int& height);
	// Scans the rendersystem's possible resolutions for the largest
	void _getBestRes (int& width, int& height);
	
	Ogre::RenderWindow*          m_renderWindow;
	Ogre::RenderWindow*          m_dummyWindow;
	bool                         m_window_is_new, m_closing;
	bool                         m_moved, m_resized;
};


