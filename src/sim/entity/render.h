#pragma once


#include <iostream>

#include "base.h"


namespace entity {
	
	// Component //////////////////////////////////////////////////////////////

	class RenderComponent : public Component {
	public:
		 RenderComponent (System* s, std::string model) : Component(s), model(model) {}
		~RenderComponent ();
		
		std::string model;
	};
	
	
	// System /////////////////////////////////////////////////////////////////
	
	class RenderSystem : public System {
	public:
		 RenderComponent* create (std::string model);
		~RenderSystem ();
		void report ();
	};
	
}
