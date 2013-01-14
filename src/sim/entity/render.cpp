

#include "render.h"

#include <iostream>
using namespace std;


namespace entity {
	
	// Component //////////////////////////////////////////////////////////////

	RenderComponent::~RenderComponent () {
	}
	
	RenderSystem::~RenderSystem () {
		// Must be called before destroying anything else
		destroyAttachedEntities();
	}
	
	
	// System /////////////////////////////////////////////////////////////////
	
	RenderComponent* RenderSystem::create (std::string model) {
		RenderComponent* c = new RenderComponent(this, model);
		comps.insert(c);
		return c;
	}
	
}
