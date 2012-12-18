


#include <iostream>
using namespace std;

#include <boost/bind.hpp>
using namespace boost;

#include "../common.h"
#include "Game.h"


Game::Game () {
//	m_physics = new thread(physics);
	REF(Ogre::Root, ogre_root);
	
	// Set up scene and camera
	m_sceneMgr = ogre_root.createSceneManager(Ogre::ST_GENERIC);
	m_camera = m_sceneMgr->createCamera("Camera");
	m_camNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode();
	m_camNode->attachObject(m_camera);
	m_camNode->setPosition(Ogre::Vector3(10,10,50));
	m_camNode->setPosition(Ogre::Vector3(0,0,1));
	m_camera->lookAt(Ogre::Vector3(0,0,0));
	m_camera->setNearClipDistance(1);
	
	// PLACEHOLDER SCENE //////////////////////////////////////////////////////
	Ogre::Entity* ogreHead = m_sceneMgr->createEntity("Head", "ogrehead.mesh");
	Ogre::SceneNode* headNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode("HeadNode");
	headNode->attachObject(ogreHead);
	m_sceneMgr->setAmbientLight(Ogre::ColourValue(0.1f, 0.1f, 0.1f));
	Ogre::Light* light = m_sceneMgr->createLight("MainLight");
	light->setPosition(20.0f, 80.0f, 50.0f);
	
	// Keep time since the start of this session
	m_clock.setEpoch();
	
	// To test waiting on the clock
//	thread* t = new thread(bind(&Game::waitLoop, this));
}
Game::~Game () {
	m_running = false;
//	physics.join();
}
void Game::setRenderTarget (Ogre::RenderTarget* rt) {
//	INFO("setRenderTarget");
	m_rt = rt;
	m_viewport = rt->addViewport(m_camera);
	m_viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f));
	m_viewport->setCamera(m_camera);
}

/*
// To test waiting on the clock
void Game::waitLoop () {
	boost::posix_time::time_duration duration = boost::posix_time::milliseconds(10);
	cout << "duration = " << duration << endl;
	while (m_running) {
		boost::posix_time::ptime before = m_clock.getTime();
		m_clock.waitFor(duration);
		cout << "wait " << frame_count << ": " << m_clock.getTime() - before << endl;
		frame_count++;
	}
}
*/

void Game::update () {
//	double time = double(timer.getMicroseconds())/1e6;
	
	// (Potentially) virtual time
	double time = 1e-6*m_clock.getDurationSinceEpoch().total_microseconds();
	
	float aspect = float(m_rt->getWidth()) / m_rt->getHeight();
	
	// Rotate the camera
	m_camNode->setPosition( Ogre::Vector3(sin((float)time)*70, cos((float)time*3.14159f)*10, cos((float)time)*70) );
	m_camNode->setPosition( Ogre::Vector3(0, 0,50) );
	m_camNode->setPosition( Ogre::Vector3(sin((float)time)*50, sin((float)time*3.14159f)*5, cos((float)time)*50) );
//	m_camNode->setPosition( Ogre::Vector3(sin(time)*50, 0,50) );
	m_camera->lookAt(Ogre::Vector3(0,0,0));
	m_camera->setAspectRatio(aspect);
	m_camera->setFOVy(Ogre::Radian(sin(time/4)/2+1));
}

/*
void Game::physics () {
	while (m_running) {
		
	}
}
*/

void Game::pause () {
	m_rt->setActive(false);
}
void Game::unpause () {
	m_rt->setActive(true);
}




