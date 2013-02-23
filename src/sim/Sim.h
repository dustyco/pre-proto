#pragma once


#include "../util/Clock.h"
#include "entity/entity.h"
#include "CopyableScene.h"


class Sim {
public:
	 Sim ();
	~Sim ();
	
	void getLatestScene (CopyableScene& scene);
	
	void update ();
	
	void pause ();
	void unpause ();

	bool  m_running;
	Clock m_clock;

private:
	CopyableScene          m_latest;
	
	entity::RenderSystem*  m_render;
	entity::PhysicsSystem* m_physics;
};
