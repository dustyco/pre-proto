#pragma once


#include "base.h"


namespace entity {
	
	// Component //////////////////////////////////////////////////////////////

	class PositionComponent : public Component {
	public:
		 PositionComponent (System* s, float x, float y, float z) : Component(s), x(x), y(y), z(z) {}
		~PositionComponent ();
		
		float x, y, z;
	};
	
	
	// System /////////////////////////////////////////////////////////////////
	
	class PositionSystem : public System {
	public:
		 PositionComponent* create (float x, float y, float z);
		~PositionSystem ();
	};
	
}
