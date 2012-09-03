#include "Application.h"

int Application::init (int argc, char **argv) {
	m_log = Logging::LogManager::getInstance();
	m_log->addLogger(new Logging::ConsoleLogger());

	// Initialize the Ogre root
	m_root = new Ogre::Root();
	if ((!m_root->restoreConfig()) && (!m_root->showConfigDialog())) {
		ERROR("Cannot restore or generate OGRE configuration");
		return -1;
	}
	m_renderWindow = m_root->initialise(true, "Proto");

	// Init resources
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("dist/media", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	// Set up scene and camera
	m_sceneMgr = m_root->createSceneManager(Ogre::ST_GENERIC, "SceneMgr");
	// Those fucking English, man...
	m_sceneMgr->setAmbientLight(Ogre::ColourValue(0.3f, 0.3f, 0.3f));

	m_camera = m_sceneMgr->createCamera("Camera");
	m_camNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode();
	m_camNode->attachObject(m_camera);
	m_camNode->setPosition(Ogre::Vector3(10,10,10));
	m_camera->lookAt(Ogre::Vector3(0,0,0));
	m_camera->setNearClipDistance(1);

	// Initialize the viewport
	m_viewport = m_renderWindow->addViewport(m_camera);
	m_viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f));
	m_camera->setAspectRatio(Ogre::Real(m_viewport->getActualWidth()) / Ogre::Real(m_viewport->getActualHeight()));
	m_viewport->setCamera(m_camera);

	// Set up input handling
	m_input = new InputManager(m_renderWindow, m_root);

	INFO("Proto initialized");
	return 0;
}

int Application::run () {
	bool running = true;
	while (m_root->renderOneFrame() && running) {
		Ogre::WindowEventUtilities::messagePump();
		if (m_input->isKeyPressed(OIS::KC_ESCAPE)) running = false;
	}
	return 0;
}

void Application::shutdown () {
	m_root->shutdown();
}