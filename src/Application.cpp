

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
	
	// Scene //////////////////////////////////////////////////////////////////
	m_app_sceneMgr = ogre_root.createSceneManager(Ogre::ST_GENERIC, "app_SceneManager");
	m_app_camera = m_app_sceneMgr->createCamera("app_Camera");
	m_app_viewport = ref<DisplayManager>().getRenderWindow()->addViewport(m_app_camera);
	m_app_viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f));
	m_app_viewport->setCamera(m_app_camera);
	
	{
		// RTT Normal /////////////////////////////////////////////////////////////
		Ogre::TexturePtr rtt_texture = Ogre::TextureManager::getSingleton().createManual(
			"app_game_normal_rtt",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D,
			ref<DisplayManager>().getRenderWindow()->getWidth(),
			ref<DisplayManager>().getRenderWindow()->getHeight(),
			0,
			Ogre::PF_R8G8B8,
			Ogre::TU_RENDERTARGET
		);
		Ogre::MaterialPtr renderMaterial = Ogre::MaterialManager::getSingleton().create("app_game_normal_mat", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		renderMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(false);
		renderMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("app_game_normal_rtt");
		Ogre::Rectangle2D *mMiniScreen = new Ogre::Rectangle2D(true);
		mMiniScreen->setCorners(0.0f, 1.0f, 2.0f, -1.0f);
		mMiniScreen->setBoundingBox(Ogre::AxisAlignedBox(-100000.0f * Ogre::Vector3::UNIT_SCALE, 100000.0f * Ogre::Vector3::UNIT_SCALE));
		mMiniScreen->setMaterial("app_game_normal_mat");
		Ogre::SceneNode* app_game_normal_node = m_app_sceneMgr->getRootSceneNode()->createChildSceneNode("app_game_normal_node");
		app_game_normal_node->attachObject(mMiniScreen);
	
		m_game_normal = new Game(rtt_texture->getBuffer()->getRenderTarget());
	}
	{
		// RTT Normal /////////////////////////////////////////////////////////////
		Ogre::TexturePtr rtt_texture = Ogre::TextureManager::getSingleton().createManual(
			"app_game_test_rtt",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D,
			ref<DisplayManager>().getRenderWindow()->getWidth(),
			ref<DisplayManager>().getRenderWindow()->getHeight(),
			0,
			Ogre::PF_R8G8B8,
			Ogre::TU_RENDERTARGET
		);
		Ogre::MaterialPtr renderMaterial = Ogre::MaterialManager::getSingleton().create("app_game_test_mat", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		renderMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(false);
		renderMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("app_game_test_rtt");
		Ogre::Rectangle2D *mMiniScreen = new Ogre::Rectangle2D(true);
		mMiniScreen->setCorners(-2.0f, 1.0f, 0.0f, -1.0f);
		mMiniScreen->setBoundingBox(Ogre::AxisAlignedBox(-100000.0f * Ogre::Vector3::UNIT_SCALE, 100000.0f * Ogre::Vector3::UNIT_SCALE));
		mMiniScreen->setMaterial("app_game_test_mat");
		Ogre::SceneNode* app_game_normal_node = m_app_sceneMgr->getRootSceneNode()->createChildSceneNode("app_game_test_node");
		app_game_normal_node->attachObject(mMiniScreen);
	
		m_game_test = new Game(rtt_texture->getBuffer()->getRenderTarget());
	}
	
	
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
	} return true;
}
bool Application::keyReleased(const OIS::KeyEvent& evt) { return true; }


