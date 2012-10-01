

#include <string>
using namespace std;

#include "DisplayManager.h"
#include "../logging/logging.h"


DisplayManager::DisplayManager (ConfigManager* config, Ogre::Root* root)
{
	lock();
	m_config = config;
	m_root = root;

	
/*	// List rendersystem options
	Ogre::RenderSystem* rs = m_root->getRenderSystem();
	Ogre::ConfigOptionMap& ops = rs->getConfigOptions();
	for (Ogre::ConfigOptionMap::iterator it = ops.begin(); it != ops.end(); it++) {
		std::cout << (*it).first << " = " << (*it).second.currentValue << "\n";
		for (Ogre::StringVector::iterator jt = (*it).second.possibleValues.begin(); jt != (*it).second.possibleValues.end(); jt++)
			std::cout << "\t" << (*jt) << "\n";
	}
*/	
	// Don't create the window automatically - use the method below
	m_root->initialise(false);
	
	// Create a dummy window so ogre doesn't shit itself when the
	// main window gets recreated
	Ogre::NameValuePairList misc_dummy;
	misc_dummy["vsync"] = "true";
	misc_dummy["hidden"] = "true";
	m_dummyWindow = m_root->createRenderWindow("PROTO DUMMY", 1, 1, false, &misc_dummy);
//	m_dummyWindow->setHidden(true);
	
	// Create main window
	_initWindow();
	
	unlock();
}
DisplayManager::~DisplayManager () {
	delete m_dummyWindow;
}

bool DisplayManager::needsNewViewport () {
	lock_shared();
	if (m_needs_new_viewport) {
		m_needs_new_viewport = false;
		unlock_shared();
		return true;
	}
	unlock_shared();
	return false;
}

void DisplayManager::applySettings ()
{
	lock();
	// Get intended settings
	int new_w, new_h;
	bool new_fullscreen;
	Ogre::NameValuePairList new_misc;
	_getSettings(new_w, new_h, new_fullscreen, new_misc);
	
	// See what needs applying
//	getFSAA()
	if (true) {
		WARNING("destroying the window");
		// FSAA setting is different - gotta recreate the window
//		m_renderWindow->destroy();
		m_root->destroyRenderTarget(m_renderWindow);
//		m_renderWindow->create(WINDOW_TITLE, new_w, new_h, new_fullscreen, &new_misc);
		_initWindow();
		WARNING("done");
	} else {
		// FSAA is the same - sets will suffice
		// Resolution/mode (ogre will automatically ignore if there's no change)
		m_renderWindow->setFullscreen(new_fullscreen, new_w, new_h);
		// Vsync
		if (new_misc["vsync"].compare((m_renderWindow->isVSyncEnabled())?"true":"false") != 0) {
//			INFO("vsync = %s", (!m_renderWindow->isVSyncEnabled())?"true":"false");
			m_renderWindow->setVSyncEnabled(!m_renderWindow->isVSyncEnabled());
		}
	}
	unlock();
}

// INTERNAL FUNCTIONS /////////////////////////////////////////////////////////
void DisplayManager::_initWindow ()
{
	WARNING("creating new window");
	
	// This allows us to submit all parameters at once -
	int width, height;
	bool fullscreen;
	Ogre::NameValuePairList misc;
	_getSettings(width, height, fullscreen, misc);
	m_renderWindow = m_root->createRenderWindow(
		WINDOW_TITLE,
		width,
		height,
		fullscreen,
		&misc
		);
	m_needs_new_viewport = true;
	
	INFO("created");
		
		
}
void DisplayManager::_getSettings (int& width, int& height, bool& fullscreen, Ogre::NameValuePairList& misc)
{
	fullscreen = DEFAULT_FULLSCREEN;
	string mode = m_config->getString("video:display/mode", "auto");
	     if (mode.compare("fullscreen") == 0) fullscreen = true;
	else if (mode.compare("window")     == 0) fullscreen = false;
	else if (mode.compare("auto")       != 0) m_config->set("video:display/mode", "auto");
	
	if (fullscreen) {
		// Get the resolution string from configmanager
		_parseRes(m_config->getString("video:display/fullscreen_res", "auto"), width, height);
		// See if it's sane
		if (!_isValidRes(width, height)) {
			// It's not
			m_config->set("video:display/fullscreen_res", "auto");
			_getBestRes(width, height);
		}
	} else {
		// Get the resolution string from configmanager
		_parseRes(m_config->getString("video:display/window_res", "auto"), width, height);
		// See if it's sane
		int max_w, max_h; _getBestRes(max_w, max_h);
		if (!(width>0 && height>0 && width<=max_w && height<=max_h)) {
			// It's not
			m_config->set("video:display/window_res", "auto");
			width = max_w;
			height = max_h;
		}
		// TODO Window position
	}
	
	// FSAA
	// TODO Compare it to the available ones
	misc["FSAA"] = m_config->getString("video:display/fsaa", "0");
	
	// vsync
	misc["vsync"] = ((m_config->getBool("video:display/vsync", DEFAULT_VSYNC))?"true":"false");
	// TODO vsyncInterval
//	misc["vsyncInterval"] = "2";
	
	// gamma
	// TODO Find out what the deal is with this
	misc["gamma"] = (DEFAULT_GAMMA?"true":"false");
}
void DisplayManager::_getBestRes (int& width, int& height) {
	int best = 0;
	Ogre::StringVector possible = m_root->getRenderSystem()->getConfigOptions()["Video Mode"].possibleValues;
	for (Ogre::StringVector::iterator it = possible.begin(); it != possible.end(); it++) {
		int w, h; _parseRes(*it, w, h);
		if (w*h >= best) {
			width = w;
			height = h;
			best = w*h;
		}
	}
}
// See if these dimensions match any of those in the rendersystem's valid resolutions list
bool DisplayManager::_isValidRes (int width, int height) {
//	INFO("_isValidRes(%d, %d)", width, height);
	Ogre::StringVector possible = m_root->getRenderSystem()->getConfigOptions()["Video Mode"].possibleValues;
	for (Ogre::StringVector::iterator it = possible.begin(); it != possible.end(); it++) {
		int w, h; _parseRes(*it, w, h);
//		INFO("Compare: %d?=%d x %d?=%d", w, width, h, height);
		if ((w == width) && (h == height)) return true;
	}
	return false;
}
// Parse numbers from a string and uses the above function
bool DisplayManager::_isValidRes (string res) {
	try {
		int w, h; _parseRes(res, w, h);
		return _isValidRes(w, h);
	} catch (exception) { return false; }
}
void DisplayManager::_parseRes (std::string res, int& width, int& height) {
	try {
		res[res.find('x')] = ' ';
		stringstream ss(res);
		ss >> width >> height;
	} catch (exception) {
		width = 0;
		height = 0;
	}
}


