#pragma once


#include "entity/entity.h"


struct Sim {
	entity::RenderSystem*  m_render;
	entity::PhysicsSystem* m_physics;
	bool                   m_running;
	
	
	 Sim ();
	~Sim ();
	void tick (double dt);
};
