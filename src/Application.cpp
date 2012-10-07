

#include <sstream>
#include <math.h>

#include "Application.h"


int Application::init (int argc, char **argv) {
	m_log = Logging::LogManager::getInstance();
	m_log->addLogger(new Logging::ConsoleLogger(), Logging::DEBUG);

	// In order to avoid Ogre's default logging behavior we have to create
	// the Ogre::LogManager before initializing the Root object.
	Ogre::LogManager* ogreLogMgr = new Ogre::LogManager();
	Logging::OgreLogAdapter* logAdapter = new Logging::OgreLogAdapter();
	Ogre::LogManager::getSingleton().createLog("ogre.log", true, true, true)->addListener(logAdapter);
	ogreLogMgr->setLogDetail(Ogre::LL_LOW);
	
	// Initialize the configuration system
	m_config = new ConfigManager(argc, argv);

	// Initialize the Ogre root
	m_root = new Ogre::Root("", "", "");
/*	if ((!m_root->restoreConfig()) && (!m_root->showConfigDialog())) {
		CRITICAL("Cannot restore or generate OGRE configuration");
		return -1;
	}
*/
	// Load the render system
	try {
		// Where to look for plugins at runtime
		#ifndef OGRE_PLUGIN_DIR
		#define OGRE_PLUGIN_DIR "."
		#endif
		#ifdef _WIN32
			m_root->loadPlugin(OGRE_PLUGIN_DIR "/RenderSystem_Direct3D9");
			Ogre::RenderSystem* rs = m_root->getRenderSystemByName("Direct3D9 Rendering Subsystem");
		#else
			m_root->loadPlugin(OGRE_PLUGIN_DIR "/RenderSystem_GL");
			Ogre::RenderSystem* rs = m_root->getRenderSystemByName("OpenGL Rendering Subsystem");
		#endif
		m_root->setRenderSystem(rs);
	} catch (std::exception e) {
		CRITICAL("Failed to start render system");
		return -1;
	}
	
	// Initialize the display manager
	m_display = new DisplayManager(m_config, m_root);
	m_root->addFrameListener(this);
	
	// Set resource search paths
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media",                    "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/materials/scripts",  "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/materials/textures", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/models",             "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	// Set up scene and camera
	m_sceneMgr = m_root->createSceneManager(Ogre::ST_GENERIC, "SceneMgr");
	m_camera = m_sceneMgr->createCamera("Camera");
	m_camNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode();
	m_camNode->attachObject(m_camera);
	m_camNode->setPosition(Ogre::Vector3(10,10,50));
	m_camNode->setPosition(Ogre::Vector3(0,0,1));
	m_camera->lookAt(Ogre::Vector3(0,0,0));
	m_camera->setNearClipDistance(1);
	
	// This will have to be reinitialized if the window is recreated for FSAA changes (it's not right now)
	m_viewport = m_display->getRenderWindow()->addViewport(m_camera);
	m_viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f));
	m_viewport->setCamera(m_camera);
	
	// PLACEHOLDER SCENE //////////////////////////////////////////////////////
	Ogre::Entity* ogreHead = m_sceneMgr->createEntity("Head", "ogrehead.mesh");
	Ogre::SceneNode* headNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode("HeadNode");
	headNode->attachObject(ogreHead);
	m_sceneMgr->setAmbientLight(Ogre::ColourValue(0.1f, 0.1f, 0.1f));
	Ogre::Light* light = m_sceneMgr->createLight("MainLight");
	light->setPosition(20.0f, 80.0f, 50.0f);

	// Set up input handling
	m_input = new InputManager(m_display->getRenderWindow(), m_root);
	m_input->registerKeyListener(this);

	INFO("Proto initialized");
	return 0;
}

int Application::run () {
	INFO("Rendering");
	running = true;
	m_root->startRendering();

	return 0;
}

void Application::shutdown () {
	m_input->unregisterKeyListener(this);
	m_root->shutdown();
	delete m_config;
	
	LOG(Logging::TELETYPE, "GET ME THOSE CHINESE LANGUAGE FILES I ASKED FOR");
	LOG(Logging::TELETYPE, "END OF LINE");
}

// Early start on cpu-based updates
// Input here will be up to 1 extra frame older than frameStarted
bool Application::frameRenderingQueued (const Ogre::FrameEvent& evt)
{
	
	return true;
}
// Normal frame start
bool Application::frameStarted (const Ogre::FrameEvent& evt)
{
	double time = (double)timer.getMicroseconds()/1e6;
	
	// See if we should stop
	if (m_display->isClosing()) running = false;
	if (!running) return false;

	
	m_display->lock_shared();
	
	float aspect = (float)(m_display->getRenderWindow()->getWidth()) / m_display->getRenderWindow()->getHeight();
	
	// Rotate the camera
	m_camNode->setPosition( Ogre::Vector3(sin(time)*70, cos(time*3.14159)*10, cos(time)*70) );
	m_camNode->setPosition( Ogre::Vector3(0, 0,50) );
	m_camNode->setPosition( Ogre::Vector3(sin(time)*50, sin(time*3.14)*5, cos(time)*50) );
//	m_camNode->setPosition( Ogre::Vector3(sin(time)*50, 0,50) );
	m_camera->lookAt(Ogre::Vector3(0,0,0));
	m_camera->setAspectRatio(aspect);
	m_camera->setFOVy(Ogre::Radian(sin(time/4)/2+1));
	
	m_display->unlock_shared();
	
	return true;
}

bool Application::keyPressed (const OIS::KeyEvent& evt) {
	switch (evt.key) {
		case OIS::KC_ESCAPE: running = false; break;
		case OIS::KC_BACKSLASH:
			// Toggle fullscreen/window
			if (m_display->getRenderWindow()->isFullScreen()) m_config->set("video:display/mode", "window");
			else m_config->set("video:display/mode", "fullscreen");
			m_display->applySettings();
			break;
	} return true;
}
bool Application::keyReleased(const OIS::KeyEvent& evt) { return true; }


