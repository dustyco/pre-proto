

#include <iostream>
using namespace std;

#include "../common.h"
#include "Sim.h"


Sim::Sim () {
	using namespace entity;
	
	// Entity component systems
	m_render   = new RenderSystem();
	m_position = new PositionSystem();
	
	// Some ogre heads
	for (int i=0; i<1; i++) {
		Entity* e = new Entity;
		e->setComp(CT_RENDER,   m_render->create("ogrehead.mesh"));
		e->setComp(CT_POSITION, m_position->create(0, 0, 0));
	}
	
	// Keep time since the start of this session
	// At some point game time should be persistent
	m_clock.setEpoch();
}

Sim::~Sim () {
	// Destroy entity component systems (by extension, all entities)
	delete m_render;
	delete m_position;
}

void Sim::update () {
	// (Potentially) virtual time
	double time = 1e-6*m_clock.getDurationSinceEpoch().total_microseconds();
	
	// TODO Rotate the camera
}

void Sim::pause () {
	// TODO Stop the clock
}

void Sim::unpause () {
	// TODO Start the clock
}




