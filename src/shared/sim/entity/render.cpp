

#include "render.h"


namespace entity {
	
	// Component //////////////////////////////////////////////////////////////

	RenderComponent::~RenderComponent () {
	}
	
	
	// System /////////////////////////////////////////////////////////////////
	
	RenderSystem::~RenderSystem () {
		// Must be called before destroying anything else
		destroyAttachedEntities();
	}
	
	RenderComponent* RenderSystem::create (std::string model) {
		RenderComponent* c = new RenderComponent(this, model);
		comps.insert(c);
		return c;
	}
	
}
