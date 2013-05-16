

#include <iostream>
using namespace std;

#include "../common.h"
#include "Sim.h"


Sim::Sim () {
	using namespace entity;
	
	// Entity component systems
	m_render  = new RenderSystem();
	m_physics = new PhysicsSystem();
	
	// Some ogre heads
	for (int i=0; i<1; i++) {
		Entity* e = new Entity;
		e->setComp(CT_RENDER,  m_render->create("ogrehead.mesh"));
		e->setComp(CT_PHYSICS, m_physics->create(0, 0, 0));
	}
	
	// Keep time since the start of this session
	// At some point game time should be persistent
	m_clock.setEpoch();
}

Sim::~Sim () {
	// Destroy entity component systems (by extension, all entities)
	delete m_render;
	delete m_physics;
}


void Sim::getLatestScene (CopyableScene& scene) {
	// TODO Lock a read mutex
	// Grab the latest copyable scene if it's newer
	if (m_latest.id != scene.id) scene = m_latest;
	// TODO Unlock
}

void Sim::update () {
	// (Potentially) virtual time
	double time = boost::chrono::duration<double>(m_clock.getDurationSinceEpoch()).count();
	
	// TODO Rotate the camera
}

void Sim::pause () {
	// TODO Stop the clock
}

void Sim::unpause () {
	// TODO Start the clock
}




