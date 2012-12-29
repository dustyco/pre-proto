#pragma once


#include <OGRE/Ogre.h>

#include "../util/Clock.h"
#include "ComponentRenderable.h"


class Game {
public:
	 Game ();
	~Game ();
	
	void setRenderTarget (Ogre::RenderTarget* rt);
	void update ();
	
	void pause ();
	void unpause ();

	bool  m_running;
	Clock m_clock;
	
	Ogre::RenderTarget*  m_rt;
	Ogre::Viewport*      m_viewport;

private:
	ComponentRenderable* m_renderable;
};
