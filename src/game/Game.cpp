

#include <iostream>
using namespace std;

#include "../common.h"
#include "Game.h"


Game::Game () {
	m_renderable = new ComponentRenderable;
	m_renderable->add(1, "ogrehead.mesh");
	m_renderable->add(2, "ogrehead.mesh")->setPosition(Ogre::Vector3(50,0,0));
	
	// 400 ogres just for fun
	int id = 1;
	for (int x=-10; x<10; x++)
	for (int y=-10; y<10; y++) {
		m_renderable->add(id, "ogrehead.mesh")->setPosition(Ogre::Vector3(50*x,50*y,0));
		id++;
	}
	
	// Keep time since the start of this session
	m_clock.setEpoch();
	
	// To test waiting on the clock
//	thread* t = new thread(bind(&Game::waitLoop, this));
}
Game::~Game () {
	m_running = false;
}
void Game::setRenderTarget (Ogre::RenderTarget* rt) {
	m_rt = rt;
	m_viewport = rt->addViewport(m_renderable->getCamera());
	m_viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f));
//	m_viewport->setCamera(m_renderable->getCamera());
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
	Ogre::Camera* cam = m_renderable->getCamera();
	cam->setAspectRatio(aspect);
	cam->setFOVy(Ogre::Radian(sin(time/4)/2+1));
	
	Ogre::SceneNode* cam_node = m_renderable->getCameraNode();
//	cam_node->setPosition( Ogre::Vector3(sin((float)time)*70, cos((float)time*3.14159f)*10, cos((float)time)*70) );
//	cam_node->setPosition( Ogre::Vector3(0, 0,50) );
	cam_node->setPosition( Ogre::Vector3(sin((float)time)*50, sin((float)time*3.14159f)*5, cos((float)time)*50) );
//	cam_node->setPosition( Ogre::Vector3(sin(time)*50, 0,50) );
	cam_node->lookAt(Ogre::Vector3(0,0,0), Ogre::Node::TS_PARENT);

}

void Game::pause () {
	m_rt->setActive(false);
}
void Game::unpause () {
	m_rt->setActive(true);
}




