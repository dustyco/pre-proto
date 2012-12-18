

#include <sstream>
#include <math.h>

#include "config/config.h"
#include "video/video.h"
#include "input/input.h"

#include "Application.h"


void Application::init (int argc, char **argv) {
	Logging::LogManager::getInstance()->addLogger(new Logging::ConsoleLogger(), Logging::LL_DEBUG);

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
	m_width  = -1;
	m_height = -1;

	// Initialize input and add a listener
	set<InputManager>().registerKeyListener(this);
	
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
	
	m_game_a = new Game;
	m_game_b = new Game;
	
	// Set up the RTTs each game will render into
	// They are recreated when the display resizes: checkDisplaySize()
	createGamePanels();
	
	// Keep time since the start of this session
	m_clock.setEpoch();
	
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
	
	// Real time
	double time = 1e-6*m_clock.getDurationSinceEpoch().total_microseconds();
	
	// See if we should stop
	if (display.isClosing()) { INFO("Display is closing"); running = false; }
	if (!running) return false;

	display.lock_shared();
	
	// See if we need to resize game panel RTT's
	checkDisplaySize();
	
	float aspect = (float)(display.getRenderWindow()->getWidth()) / display.getRenderWindow()->getHeight();
	float pos = cos(time)*0.5f;
	
	m_game_normal_viewport->setDimensions(pos-0.5, 0, 1, 1);
	m_game_test_viewport->setDimensions(pos+0.5, 0, 1, 1);
	
	// Play with game speed
//	m_game_b->m_clock.warp(pow(1.2, time_r));
	m_game_b->m_clock.warp(sin(time)*sin(time));
//	m_game_b->m_clock.unwarp();

	m_game_a->update();
	m_game_b->update();

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

void Application::checkDisplaySize () {
	Ogre::RenderWindow& window = *(ref<DisplayManager>().getRenderWindow());
	if (m_width != window.getWidth() || m_height != window.getHeight()) {
		// It changed
		m_width  = window.getWidth();
		m_height = window.getHeight();
//		INFO("Resolution changed");
		// Recreate game RTT's
		deleteGamePanels();
		createGamePanels();
	}
}

void Application::deleteGamePanels () {
	using namespace Ogre;
	
	delete m_game_a_rect;
	delete m_game_b_rect;
	
	TextureManager::getSingleton().remove(m_game_a_rtt->getName());
	TextureManager::getSingleton().remove(m_game_b_rtt->getName());
	
	MaterialManager::getSingleton().remove(m_game_a_mat->getName());
	MaterialManager::getSingleton().remove(m_game_b_mat->getName());
	
	m_game_a_node->getParentSceneNode()->removeAndDestroyChild(m_game_a_node->getName());
	m_game_b_node->getParentSceneNode()->removeAndDestroyChild(m_game_b_node->getName());
	
	m_game_a_rtt.setNull();
	m_game_b_rtt.setNull();
	
	m_game_a_mat.setNull();
	m_game_b_mat.setNull();
}
void Application::createGamePanels () {
	using namespace Ogre;
	
	// Textures
	m_game_a_rtt = Ogre::TextureManager::getSingleton().createManual(
		"app_game_a_rtt",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D,
		ref<DisplayManager>().getRenderWindow()->getWidth(),
		ref<DisplayManager>().getRenderWindow()->getHeight(),
		0,
		Ogre::PF_R8G8B8,
		Ogre::TU_RENDERTARGET
	);
	m_game_b_rtt = Ogre::TextureManager::getSingleton().createManual(
		"app_game_b_rtt",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D,
		ref<DisplayManager>().getRenderWindow()->getWidth(),
		ref<DisplayManager>().getRenderWindow()->getHeight(),
		0,
		Ogre::PF_R8G8B8,
		Ogre::TU_RENDERTARGET
	);
	
	// Materials
	m_game_a_mat = MaterialManager::getSingleton().create("app_game_a_mat", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	m_game_b_mat = MaterialManager::getSingleton().create("app_game_b_mat", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	{
		Pass* pass = m_game_a_mat->getTechnique(0)->getPass(0);
		pass->setLightingEnabled(false);
		TextureUnitState* tus = pass->createTextureUnitState();
		tus->setTextureName(m_game_a_rtt->getName());
		tus->setNumMipmaps(0);
	} {
		Pass* pass = m_game_b_mat->getTechnique(0)->getPass(0);
		pass->setLightingEnabled(false);
		TextureUnitState* tus = pass->createTextureUnitState();
		tus->setTextureName(m_game_b_rtt->getName());
		tus->setNumMipmaps(0);
	}
	
	// Rectangles
	m_game_a_rect = new Ogre::Rectangle2D(true);
	m_game_b_rect = new Ogre::Rectangle2D(true);
	m_game_a_rect->setCorners(-2.0f, 1.0f, 0.0f, -1.0f);
	m_game_b_rect->setCorners( 0.0f, 1.0f, 2.0f, -1.0f);
//	m_game_a_rect->setCorners(-1.0f, 1.0f, 1.0f, -1.0f);
//	m_game_b_rect->setCorners(-1.0f, 1.0f, 0.0f, -1.0f);
	m_game_a_rect->setBoundingBox(Ogre::AxisAlignedBox(-100000.0f * Ogre::Vector3::UNIT_SCALE, 100000.0f * Ogre::Vector3::UNIT_SCALE));
	m_game_b_rect->setBoundingBox(Ogre::AxisAlignedBox(-100000.0f * Ogre::Vector3::UNIT_SCALE, 100000.0f * Ogre::Vector3::UNIT_SCALE));
	m_game_a_rect->setMaterial(m_game_a_mat->getName());
	m_game_b_rect->setMaterial(m_game_b_mat->getName());
	m_game_a_node = m_app_sceneMgr->getRootSceneNode()->createChildSceneNode("app_game_a_node");
	m_game_b_node = m_app_sceneMgr->getRootSceneNode()->createChildSceneNode("app_game_b_node");
	m_game_a_node->attachObject(m_game_a_rect);
	m_game_b_node->attachObject(m_game_b_rect);
	
	// Point the games at them
	m_game_a->setRenderTarget(m_game_a_rtt->getBuffer()->getRenderTarget());
	m_game_b->setRenderTarget(m_game_b_rtt->getBuffer()->getRenderTarget());
}




