#pragma once


#ifndef PCH
	#include <OGRE/Ogre.h>
#endif

#include "../util/Clock.h"
#include "../sim/Sim.h"


// 

class ClientGame {
public:
	 ClientGame ();
	~ClientGame ();
	
	void setRenderTarget (Ogre::RenderTarget* rt);
	void update ();
	
	void pause ();
	void unpause ();

	bool  m_running;
	Clock m_clock;
	
private:
	Ogre::RenderTarget*  m_rt;
	Ogre::Viewport*      m_viewport;
	
	Ogre::SceneManager*  m_scene;
	Ogre::Camera*        m_camera;
	Ogre::SceneNode*     m_cam_node;
	
	Sim*                 m_sim;
};
