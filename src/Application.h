#pragma once


#include <OGRE/Ogre.h>
#include <OIS/OIS.h>

#include "logging/logging.h"
#include "game/Game.h"

#ifndef OGRE_PLUGIN_DIR
#define OGRE_PLUGIN_DIR "."
#endif


class Application :
	public Ogre::FrameListener,
	public OIS::KeyListener
{
public:
	void init (int argc, char** argv);
	void run ();
	void shutdown ();
	
	// Ogre::FrameListener
	bool frameStarted         (const Ogre::FrameEvent& evt);
	bool frameRenderingQueued (const Ogre::FrameEvent& evt);
	
	// OIS::KeyListener
	bool keyPressed  (const OIS::KeyEvent& evt);
	bool keyReleased (const OIS::KeyEvent& evt);
	
	Ogre::RenderTarget* makePanel (float x, float y);

private:
	class Panel {
	public:
		Panel ();
	};
	
	Logging::LogManager* m_log;
	
	Game* m_game_normal;
	Game* m_game_test;
	
	Ogre::Viewport*     m_game_normal_viewport;
	Ogre::Viewport*     m_game_test_viewport;
	
	Ogre::Timer timer;
	bool running;
};
