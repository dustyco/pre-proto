#pragma once


#include <boost/thread.hpp>
#include <OGRE/Ogre.h>


class Game
{
public:
	Game (Ogre::RenderTarget* rt);
	~Game ();
	
	void setRenderTarget (Ogre::RenderTarget* rt);
	void update ();
	
private:
	void physics ();

private:
	boost::thread* m_physics;
	Ogre::Timer    timer;
	bool           m_running;
	
	Ogre::RenderTarget* m_rt;
	Ogre::SceneManager* m_sceneMgr;
	Ogre::Camera*       m_camera;
	Ogre::SceneNode*    m_camNode;
	Ogre::Viewport*     m_viewport;
};
