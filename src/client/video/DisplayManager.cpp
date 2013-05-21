

#include <sstream>
#include <string>
using namespace std;

#include "DisplayManager.h"
#include "logging/logging.h"
#include "util/singleton.h"

const string WINDOW_TITLE     = "Proto";
const bool DEFAULT_FULLSCREEN = false;
const bool DEFAULT_VSYNC      = true;
const bool DEFAULT_GAMMA      = true;


DisplayManager::DisplayManager ()
{
	m_closing = false;

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
	ref<Ogre::Root>().initialise(false);
	
	// Create a dummy window so ogre doesn't shit itself when the
	// main window gets recreated
	Ogre::NameValuePairList misc_dummy;
	misc_dummy["vsync"] = "true";
	misc_dummy["hidden"] = "true";
	m_dummyWindow = ref<Ogre::Root>().createRenderWindow("PROTO DUMMY", 1, 1, false, &misc_dummy);
	
	// Create main window
	_initWindow();
	
	// Listen for window events
	m_moved = m_resized = false;
	Ogre::WindowEventUtilities::addWindowEventListener(m_renderWindow, this);
}
DisplayManager::~DisplayManager () {
	Ogre::WindowEventUtilities::removeWindowEventListener(m_renderWindow, this);
	delete m_dummyWindow;
}

void DisplayManager::applySettings ()
{
	m_moved = m_resized = false;
	
	// Get intended settings
	int new_w, new_h;
	bool new_fullscreen;
	Ogre::NameValuePairList new_misc;
	_getSettings(new_w, new_h, new_fullscreen, new_misc);
	
	// Decide to recreate the window or just use set()'s
	// NOTE: WINDOW RECREATION DOESN'T WORK YET SO ALWAYS USE SETS FOR NOW
//	getFSAA()
	if (false) {
		// FSAA setting is different - gotta reinit the window
		_reinitWindow();
	} else {
		// Resolution/mode (ogre will do nothing if there's no change)
		m_renderWindow->setFullscreen(new_fullscreen, new_w, new_h);
		
		// Vsync
		if (new_misc["vsync"].compare((m_renderWindow->isVSyncEnabled())?"true":"false") != 0) 
			m_renderWindow->setVSyncEnabled(!m_renderWindow->isVSyncEnabled());
	}
	cout << "vsync: " << (m_renderWindow->isVSyncEnabled()?"true":"false") << endl;
	cout << "fps: " << m_renderWindow->getAverageFPS() << endl;
}

void DisplayManager::reinitWindow () {
	ref<Ogre::Root>().destroyRenderTarget(m_renderWindow);
	_initWindow();
}

bool DisplayManager::windowIsNew () {
	if (m_window_is_new) {
		m_window_is_new = false;
		return true;
	}
	return false;
}

bool DisplayManager::isClosing () {
	if (m_closing) {
		m_closing = false;
		return true;
	}
	return false;
}


// WindowEventListener CALLBACKS //////////////////////////////////////////////
void DisplayManager::windowMoved (Ogre::RenderWindow* rw) {
	if (!rw->isFullScreen()) {
		if (!m_moved) { m_moved = true; return; }
	
		unsigned int d; int x, y;
		rw->getMetrics(d, d, d, x, y);
		stringstream pos; pos << x << 'x' << y;
//		INFO("windowMoved() %s", pos.str().c_str());
		ref<ConfigManager>().set("video:display.window_pos", pos.str());
	}
}
void DisplayManager::windowResized (Ogre::RenderWindow* rw) {
	REF(ConfigManager, config);
	if (!m_resized) { m_resized = true; return; }
	stringstream res; res << rw->getWidth() << 'x' << rw->getHeight();
//	INFO("windowResized() %s", res.str().c_str());
	if (rw->isFullScreen()) config.set("video:display.fullscreen_res", res.str());
	else {
		unsigned int d; int x, y;
		rw->getMetrics(d, d, d, x, y);
		stringstream pos; pos << x << 'x' << y;
		config.set("video:display.window_pos", pos.str());
		config.set("video:display.window_res", res.str());
	}
}
void DisplayManager::windowFocusChange (Ogre::RenderWindow* rw) {
//	INFO("windowFocusChange()");
}
bool DisplayManager::windowClosing (Ogre::RenderWindow* rw) {
	m_closing = true;
	return false;
}
void DisplayManager::windowClosed (Ogre::RenderWindow* rw) {
	// Note: This probably won't get called because of the above interception
	
	// The window has not actually close yet when this event triggers. It's only
	// closed after all windowClosed events are triggered. This allows apps to deinitialise
	// properly if they have services that needs the window to exist when deinitialising.
}



// INTERNAL FUNCTIONS /////////////////////////////////////////////////////////
void DisplayManager::_initWindow ()
{
	// This allows us to submit all parameters at once
	int width, height;
	bool fullscreen;
	Ogre::NameValuePairList misc;
	_getSettings(width, height, fullscreen, misc);
	
/*	cout << width << "x" << height << endl;
	cout << "fullscreen: " << (fullscreen?"true":"false") << endl;
	for (Ogre::NameValuePairList::iterator it = misc.begin(); it!=misc.end(); it++)
		cout << (*it).first << " = " << (*it).second << endl;
*/	
	m_renderWindow = ref<Ogre::Root>().createRenderWindow(
		WINDOW_TITLE,
		width,
		height,
		fullscreen,
		&misc
		);
	m_window_is_new = true;
}
void DisplayManager::_reinitWindow () {
	CRITICAL("DisplayManager::_reinitWindow(): THIS SHOULDN'T BE USED - IT DOESN'T WORK");
	
//	m_renderWindow->destroy();
	ref<Ogre::Root>().destroyRenderTarget(m_renderWindow);
	_initWindow();
}
void DisplayManager::_getSettings (int& width, int& height, bool& fullscreen, Ogre::NameValuePairList& misc)
{
	REF(ConfigManager, config);
	
	fullscreen = DEFAULT_FULLSCREEN;
	string mode = config.get("video:display.mode", "auto");
	     if (mode.compare("fullscreen") == 0) fullscreen = true;
	else if (mode.compare("window")     == 0) fullscreen = false;
	else if (mode.compare("auto")       != 0) config.set("video:display.mode", "auto");
	
	if (fullscreen) {
		// Get the resolution string from configmanager
		_parseRes(config.get("video:display.fullscreen_res", "auto"), width, height);
		// See if it's sane
		if (!_isValidRes(width, height)) {
			// It's not
			config.set("video:display.fullscreen_res", "auto");
			_getBestRes(width, height);
		}
	} else {
		// Get the resolution string from configmanager
		_parseRes(config.get("video:display.window_res", "auto"), width, height);
		// See if it's sane
		int max_w, max_h; _getBestRes(max_w, max_h);
		if (!(width>0 && height>0 && width<=max_w && height<=max_h)) {
			// It's not
			config.set("video:display.window_res", "auto");
			width = max_w;
			height = max_h;
		}
		// TODO Window position
	}
	
	// FSAA
	misc["FSAA"] = _getValidFSAA();
	
	// vsync
	string vsync = config.get("video:display.vsync", "");
	if (vsync.compare("true")==0 || vsync.compare("false")==0) misc["vsync"] = vsync;
	else {
		config.set("video:display.vsync", "auto");
		misc["vsync"] = (DEFAULT_VSYNC?"true":"false");
	}
	// TODO vsyncInterval
//	misc["vsyncInterval"] = "2";
	
	// gamma
	// TODO Find out what the deal is with this
	misc["gamma"] = (DEFAULT_GAMMA?"true":"false");
}
void DisplayManager::_getBestRes (int& width, int& height) {
	int best = 0;
	Ogre::StringVector possible = ref<Ogre::Root>().getRenderSystem()->getConfigOptions()["Video Mode"].possibleValues;
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
	Ogre::StringVector possible = ref<Ogre::Root>().getRenderSystem()->getConfigOptions()["Video Mode"].possibleValues;
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

string DisplayManager::_getValidFSAA () {
	string fsaa = ref<ConfigManager>().get("video:graphics.fsaa", "auto");
	if (fsaa.compare("auto") == 0) fsaa = "4";
	
	Ogre::StringVector possible = ref<Ogre::Root>().getRenderSystem()->getConfigOptions()["FSAA"].possibleValues;
	for (Ogre::StringVector::iterator it = possible.begin(); it != possible.end(); it++)
		if ((*it) == fsaa) return fsaa;

	return string("0");
}



