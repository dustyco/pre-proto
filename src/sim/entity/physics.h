#pragma once


#include <bullet/btBulletDynamicsCommon.h>

#include "base.h"


namespace entity {
	
	// Component //////////////////////////////////////////////////////////////

	class PhysicsComponent : public Component {
	public:
		 PhysicsComponent (System* s, float x, float y, float z) : Component(s), x(x), y(y), z(z) {}
		~PhysicsComponent ();
		
		float x, y, z;
	};
	
	
	// System /////////////////////////////////////////////////////////////////
	
	class PhysicsSystem : public System {
	public:
		 PhysicsSystem ();
		~PhysicsSystem ();
		 PhysicsComponent* create (float x, float y, float z);
	
	private:
		btDefaultCollisionConfiguration*     collision_configuration;
		btCollisionDispatcher*               collision_dispatcher;
		btDbvtBroadphase*                    broadphase_interface;
		btSequentialImpulseConstraintSolver* constraint_solver;
		btDiscreteDynamicsWorld*             collision_world;
	};
	
}
