#pragma once


#include <boost/thread.hpp>
#include <OGRE/Ogre.h>


class Game
{
public:
	Game ();
	~Game ();
	
	Ogre::Camera* getCamera ();
	
	void update ();
	
private:
	void physics ();

private:
	boost::thread* m_physics;
	Ogre::Timer    timer;
	bool           m_running;
	
	Ogre::SceneManager* m_sceneMgr;
	Ogre::Camera*       m_camera;
	Ogre::SceneNode*    m_camNode;
};
