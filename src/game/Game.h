#pragma once


//#include <boost/thread.hpp>
#include <OGRE/Ogre.h>

#include "../util/Clock.h"


class Game {
public:
	Game ();
	~Game ();
	
	void setRenderTarget (Ogre::RenderTarget* rt);
	void update ();
	
	void pause ();
	void unpause ();
	
//	void physics ();
//	void waitLoop ();

//	boost::thread* m_physics;
	bool           m_running;
	Clock          m_clock;
	
	Ogre::RenderTarget* m_rt;
	Ogre::SceneManager* m_sceneMgr;
	Ogre::Camera*       m_camera;
	Ogre::SceneNode*    m_camNode;
	Ogre::Viewport*     m_viewport;
};
