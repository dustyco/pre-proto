

#include "../util/singleton.h"

#include "Game.h"

using namespace boost;


Game::Game (Ogre::RenderTarget* rt) {
//	m_physics = new thread(physics);
	m_rt = rt;
	REF(Ogre::Root, ogre_root);

/*	// Spinning ogre head /////////////////////////////////////////////////////
	// Set up scene and camera
	m_sceneMgr = ogre_root.createSceneManager(Ogre::ST_GENERIC, "SceneMgr");
	m_camera = m_sceneMgr->createCamera("Camera");
	m_camNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode();
	m_camNode->attachObject(m_camera);
	m_camNode->setPosition(Ogre::Vector3(10,10,50));
	m_camNode->setPosition(Ogre::Vector3(0,0,1));
	m_camera->lookAt(Ogre::Vector3(0,0,0));
	m_camera->setNearClipDistance(1);
	
	// This will have to be reinitialized if the window is recreated for FSAA changes (it's not right now)
//	m_viewport = rt->addViewport(m_camera);
//	m_viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f));
//	m_viewport->setCamera(m_camera);
	
	// PLACEHOLDER SCENE //////////////////////////////////////////////////////
	Ogre::Entity* ogreHead = m_sceneMgr->createEntity("Head", "ogrehead.mesh");
	Ogre::SceneNode* headNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode("HeadNode");
	headNode->attachObject(ogreHead);
	m_sceneMgr->setAmbientLight(Ogre::ColourValue(0.1f, 0.1f, 0.1f));
	Ogre::Light* light = m_sceneMgr->createLight("MainLight");
	light->setPosition(20.0f, 80.0f, 50.0f);
	
	m_camNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode();
	m_camNode->attachObject(m_camera);
	m_camNode->setPosition(Ogre::Vector3(10,10,50));
	m_camNode->setPosition(Ogre::Vector3(0,0,1));
	m_camera->lookAt(Ogre::Vector3(0,0,0));
	m_camera->setNearClipDistance(1);
	
	rt->addViewport(m_camera);
	rt->getViewport(0)->setClearEveryFrame(true);
	rt->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Black);
	rt->getViewport(0)->setOverlaysEnabled(false);
*/	
	
	
	
	
	
	
	// Set up scene and camera
	m_sceneMgr = ogre_root.createSceneManager(Ogre::ST_GENERIC);
	m_camera = m_sceneMgr->createCamera("Camera");
	m_camNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode();
	m_camNode->attachObject(m_camera);
	m_camNode->setPosition(Ogre::Vector3(10,10,50));
	m_camNode->setPosition(Ogre::Vector3(0,0,1));
	m_camera->lookAt(Ogre::Vector3(0,0,0));
	m_camera->setNearClipDistance(1);
	
	// This will have to be reinitialized if the window is recreated for FSAA changes (it's not right now)
	m_viewport = rt->addViewport(m_camera);
	m_viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f));
	m_viewport->setCamera(m_camera);
	
	// PLACEHOLDER SCENE //////////////////////////////////////////////////////
	Ogre::Entity* ogreHead = m_sceneMgr->createEntity("Head", "ogrehead.mesh");
	Ogre::SceneNode* headNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode("HeadNode");
	headNode->attachObject(ogreHead);
	m_sceneMgr->setAmbientLight(Ogre::ColourValue(0.1f, 0.1f, 0.1f));
	Ogre::Light* light = m_sceneMgr->createLight("MainLight");
	light->setPosition(20.0f, 80.0f, 50.0f);
	
	
	
	
}
Game::~Game () {
	m_running = false;
//	physics.join();
}

void Game::setRenderTarget (Ogre::RenderTarget* rt) {
	
}
void Game::update () {
	double time = double(timer.getMicroseconds())/1e6;
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
	
void Game::physics () {
	while (m_running) {
		
	}
}
