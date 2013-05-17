

#include <iostream>
using namespace std;

#include "common.h"
#include "Sim.h"


Sim::Sim () {
	using namespace entity;
	
	// Entity component systems
	m_physics = new PhysicsSystem();
}

Sim::~Sim () {
	// Destroy entity component systems (by extension, all entities)
	delete m_render;
	delete m_physics;
}

void Sim::tick (double dt) {
	// TODO Rotate the camera
	m_physics->tick(dt);
	m_render->update();
}


