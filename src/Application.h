#pragma once
#include <OGRE/Ogre.h>

#include "logging/logging.h"
#include "config/config.h"
#include "input/input.h"

class Application {
public:
	int init (int argc, char **argv);
	int run ();
	void shutdown ();

private:
	Logging::LogManager* m_log;
	ConfigManager*       m_config;
	InputManager*        m_input;

	Ogre::Root*         m_root;
	Ogre::RenderWindow* m_renderWindow;
	Ogre::SceneManager* m_sceneMgr;
	Ogre::Camera*       m_camera;
	Ogre::SceneNode*    m_camNode;
	Ogre::Viewport*     m_viewport;
};
