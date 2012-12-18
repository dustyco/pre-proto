#pragma once


#include <OGRE/Ogre.h>
#include <OIS/OIS.h>

#include "common.h"
#include "util/Clock.h"
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

private:
	// Recreate RTT's for the game panels if the display changed size
	void checkDisplaySize ();
	int m_width, m_height;
	
	void createGamePanels ();
	void deleteGamePanels ();
	
	Ogre::SceneNode* m_game_a_node;
	Ogre::SceneNode* m_game_b_node;
	
	Ogre::Rectangle2D* m_game_a_rect;
	Ogre::Rectangle2D* m_game_b_rect;
	
	Ogre::MaterialPtr m_game_a_mat;
	Ogre::MaterialPtr m_game_b_mat;
	
	Ogre::TexturePtr m_game_a_rtt;
	Ogre::TexturePtr m_game_b_rtt;

private:
	Clock m_clock;
	Game* m_game_a;
	Game* m_game_b;
	
	Ogre::Viewport*     m_game_normal_viewport;
	Ogre::Viewport*     m_game_test_viewport;
	
	bool running;
};
