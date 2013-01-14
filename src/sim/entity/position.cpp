

#include "position.h"

#include <iostream>
using namespace std;


namespace entity {
	
	// Component //////////////////////////////////////////////////////////////
	
	PositionComponent::~PositionComponent () {
	}
	
	
	// System /////////////////////////////////////////////////////////////////
	
	PositionSystem::~PositionSystem () {
		// Must be called before destroying anything else
		destroyAttachedEntities();
	}
	PositionComponent* PositionSystem::create (float x, float y, float z) {
		PositionComponent* c = new PositionComponent(this, x, y, z);
		comps.insert(c);
		return c;
	}
	
}
