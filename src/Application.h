#pragma once


#include <OGRE/Ogre.h>

#include "logging/logging.h"
#include "config/config.h"
#include "video/video.h"
#include "input/input.h"


class Application :
	public Ogre::FrameListener,
	public OIS::KeyListener
{
public:
	int init (int argc, char** argv);
	int run ();
	void shutdown ();
	
	// Ogre::FrameListener
	bool frameStarted         (const Ogre::FrameEvent& evt);
	bool frameRenderingQueued (const Ogre::FrameEvent& evt);
	
	// OIS::KeyListener
	bool keyPressed  (const OIS::KeyEvent& evt);
	bool keyReleased  (const OIS::KeyEvent& evt);

private:
	Logging::LogManager* m_log;
	ConfigManager*       m_config;
	DisplayManager*      m_display;
	InputManager*        m_input;

	Ogre::Root*         m_root;
	Ogre::SceneManager* m_sceneMgr;
	Ogre::Camera*       m_camera;
	Ogre::SceneNode*    m_camNode;
	Ogre::Viewport*     m_viewport;
	
	Ogre::Timer timer;
	bool running;
};
