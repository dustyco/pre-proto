#pragma once


#include "../util/Clock.h"
#include "entity/entity.h"


class Sim {
public:
	 Sim ();
	~Sim ();
	
	void update ();
	
	void pause ();
	void unpause ();

	bool  m_running;
	Clock m_clock;

private:
	entity::RenderSystem*   m_render;
	entity::PositionSystem* m_position;
};
