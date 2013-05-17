#pragma once


#include <string>
#include "base.h"


namespace entity {
	
	// Component //////////////////////////////////////////////////////////////

	class RenderComponent : public Component {
	public:
		 RenderComponent (System* s, std::string model) : Component(s), model(model) {}
		~RenderComponent ();
		virtual void update () {}
		
		std::string model;
	};
	
	
	// System /////////////////////////////////////////////////////////////////
	
	class RenderSystem : public System {
	public:
		~RenderSystem ();
		virtual RenderComponent* create (std::string model);
		virtual void update () {}
	};
	
}
