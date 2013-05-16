#pragma once


#include <OGRE/Ogre.h>

#include "util/Clock.h"
#include "sim/Sim.h"
#include "common.h"


struct ClientGame {
	Ogre::RenderTarget*  m_rt;
	Ogre::Viewport*      m_viewport;
	Ogre::SceneManager*  m_scene;
	Ogre::Camera*        m_camera;
	Ogre::SceneNode*     m_cam_node;
	Sim*                 m_sim;
	bool  m_running;
	Clock m_clock;
	
	
	 ClientGame ();
	~ClientGame ();
	void setRenderTarget (Ogre::RenderTarget* rt);
	void update ();
	void pause ();
	void unpause ();
};


ClientGame::ClientGame () {
	REF(Ogre::Root, ogre_root);
	
	// Ogre scene and camera
	m_scene = ogre_root.createSceneManager(Ogre::ST_GENERIC);
	m_camera = m_scene->createCamera("Camera");
	m_camera->lookAt(Ogre::Vector3(0,0,0));
	m_camera->setNearClipDistance(1);
	m_cam_node = m_scene->getRootSceneNode()->createChildSceneNode();
	m_cam_node->attachObject(m_camera);
	m_cam_node->setPosition(Ogre::Vector3(0,0,1));
	m_scene->setAmbientLight(Ogre::ColourValue(0.1f, 0.1f, 0.1f));
	Ogre::Light* light = m_scene->createLight("MainLight");
	light->setPosition(20.0f, 80.0f, 50.0f);
	
	// Let there be life
	m_sim = new Sim;
	
	// Keep time since the start of this session
	m_clock.setEpoch();
}

ClientGame::~ClientGame () {
	m_running = false;
	delete m_sim;
}

void ClientGame::setRenderTarget (Ogre::RenderTarget* rt) {
	m_rt = rt;
	m_viewport = rt->addViewport(m_camera);
	m_viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f));
//	m_viewport->setCamera(m_renderable->getCamera());
}

void ClientGame::update () {
	// (Potentially) virtual time
	double time = boost::chrono::duration<double>(m_clock.getDurationSinceEpoch()).count();
	
	float aspect = float(m_rt->getWidth()) / m_rt->getHeight();
	
	// Rotate the camera
	Ogre::Camera* cam = m_camera;
	cam->setAspectRatio(aspect);
	cam->setFOVy(Ogre::Radian(sin(time/4)/2+1));
	
//	m_cam_node->setPosition( Ogre::Vector3(sin((float)time)*70, cos((float)time*3.14159f)*10, cos((float)time)*70) );
//	m_cam_node->setPosition( Ogre::Vector3(0, 0,50) );
	m_cam_node->setPosition( Ogre::Vector3(sin((float)time)*50, sin((float)time*3.14159f)*5, cos((float)time)*50) );
//	m_cam_node->setPosition( Ogre::Vector3(sin(time)*50, 0,50) );
	m_cam_node->lookAt(Ogre::Vector3(0,0,0), Ogre::Node::TS_PARENT);

}

void ClientGame::pause () {
	m_rt->setActive(false);
	m_sim->pause();
}

void ClientGame::unpause () {
	m_rt->setActive(true);
	m_sim->unpause();
}




