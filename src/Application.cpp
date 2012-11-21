

#include <sstream>
#include <math.h>

#include "logging/logging.h"
#include "util/singleton.h"
#include "config/config.h"
#include "video/video.h"
#include "input/input.h"
#include "gui/gui.h"

#include "Application.h"


void Application::init (int argc, char **argv) {
	m_log = Logging::LogManager::getInstance();
	m_log->addLogger(new Logging::ConsoleLogger(), Logging::LL_DEBUG);

	// In order to avoid Ogre's default logging behavior we have to create
	// the Ogre::LogManager before initializing the Root object.
	Ogre::LogManager* ogreLogMgr = new Ogre::LogManager();
	Logging::OgreLogAdapter* logAdapter = new Logging::OgreLogAdapter();
	Ogre::LogManager::getSingleton().createLog("ogre.log", true, true, true)->addListener(logAdapter);
	ogreLogMgr->setLogDetail(Ogre::LL_LOW);
	
	// Initialize the configuration system
	set<ConfigManager>(new ConfigManager(argc, argv));

	// Initialize the Ogre root
	set<Ogre::Root>(new Ogre::Root("", "", ""));
	REF(Ogre::Root, ogre_root);
	
	// Load Ogre plugins
	#ifndef OGRE_PLUGIN_DIR
	#define OGRE_PLUGIN_DIR "."
	#endif
	#ifdef _WIN32
		ogre_root.loadPlugin(OGRE_PLUGIN_DIR "/RenderSystem_Direct3D9");
		Ogre::RenderSystem* rs = ogre_root.getRenderSystemByName("Direct3D9 Rendering Subsystem");
	#else
		ogre_root.loadPlugin(OGRE_PLUGIN_DIR "/RenderSystem_GL");
		Ogre::RenderSystem* rs = ogre_root.getRenderSystemByName("OpenGL Rendering Subsystem");
	#endif
	ogre_root.setRenderSystem(rs);
	ogre_root.addFrameListener(this);
	
	// Initialize the display manager
	set<DisplayManager>();

	// Initialize input and add a listener
	set<InputManager>().registerKeyListener(this);
	
	// Create CEGUI resource groups and initialize the gui
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/cegui/schemes",      "FileSystem", "Schemes");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/cegui/imagesets",    "FileSystem", "Imagesets");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/cegui/fonts",        "FileSystem", "Fonts");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/cegui/looknfeel",    "FileSystem", "LookNFeel");
	set<GUIManager>();
	
	// Set resource search paths
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media",                    "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/materials/scripts",  "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/materials/textures", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/models",             "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	
	// Two Viewports //////////////////////////////////////////////////////////
	m_game_normal = new Game;
	m_game_normal_viewport = ref<DisplayManager>().getRenderWindow()->addViewport(m_game_normal->getCamera(), 0, -0.5f);
	m_game_normal_viewport->setBackgroundColour(Ogre::ColourValue(0.2f, 0.0f, 0.0f, 1.0f));
	
	m_game_test = new Game;
	m_game_test_viewport = ref<DisplayManager>().getRenderWindow()->addViewport(m_game_test->getCamera(), 1, 0.5f);
	m_game_test_viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.2f, 0.0f, 1.0f));
	
	INFO("Proto initialized");
}

void Application::run () {
	INFO("Rendering");
	running = true;
	ref<Ogre::Root>().startRendering();
}

void Application::shutdown () {
	ref<InputManager>().unregisterKeyListener(this);
	ref<Ogre::Root>().shutdown();
	delete ptr<ConfigManager>();
	
	LOG(Logging::LL_TELETYPE, "GET ME THOSE CHINESE LANGUAGE FILES I ASKED FOR");
	LOG(Logging::LL_TELETYPE, "END OF LINE");
}

// Early start on cpu-based updates
// Input here will be up to 1 extra frame older than frameStarted
bool Application::frameRenderingQueued (const Ogre::FrameEvent& evt) {
	return true;
}
// Normal frame start
bool Application::frameStarted (const Ogre::FrameEvent& evt) {
	REF(DisplayManager, display);
	
	double time = (double)timer.getMicroseconds()/1e6;
	
	// See if we should stop
	if (display.isClosing()) { INFO("Display is closing"); running = false; }
	if (!running) return false;

	
	display.lock_shared();
	
//	INFO("got lock, starting to render");
	
	
	float aspect = (float)(display.getRenderWindow()->getWidth()) / display.getRenderWindow()->getHeight();
	float pos = cos(time)*0.5f;
	
	m_game_normal_viewport->setDimensions(pos-0.5, 0, 1, 1);
	m_game_test_viewport->setDimensions(pos+0.5, 0, 1, 1);
	
	m_game_normal->update();
	m_game_test->update();
	
	display.unlock_shared();
	
	return true;
}
bool Application::keyPressed (const OIS::KeyEvent& evt) {
	REF(InputManager, input);
	switch (evt.key) {
		case OIS::KC_ESCAPE: running = false; break;
		case OIS::KC_BACKSLASH:
			// Toggle fullscreen/window
			if (ref<DisplayManager>().getRenderWindow()->isFullScreen()) ref<ConfigManager>().set("video:display/mode", "window");
			else ref<ConfigManager>().set("video:display/mode", "fullscreen");
			ref<DisplayManager>().applySettings();
			break;
		case OIS::KC_F1:
			// Toggle mouse mode
			if (input.getMouseFreedom()) input.setMouseFreedom(false);
			else input.setMouseFreedom(true);
			break;
		case OIS::KC_F9:
			// Take a screenshot
			ref<DisplayManager>().getRenderWindow()->writeContentsToTimestampedFile("screenshot-", ".png");
			break;
	} return true;
}
bool Application::keyReleased(const OIS::KeyEvent& evt) { return true; }


