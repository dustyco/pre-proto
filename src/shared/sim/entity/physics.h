#pragma once


#include <bullet/btBulletDynamicsCommon.h>
#include "common.h"
#include "base.h"


namespace entity {
	
	// Component //////////////////////////////////////////////////////////////

	class PhysicsComponent : public Component {
	public:
		 PhysicsComponent (System* s, Vec3 pos) : Component(s), pos(pos) {}
		~PhysicsComponent ();
		
		Vec3 pos;
	};
	
	
	// System /////////////////////////////////////////////////////////////////
	
	class PhysicsSystem : public System {
	public:
		 PhysicsSystem ();
		~PhysicsSystem ();
		 PhysicsComponent* create (Vec3 pos);
		 void tick (double dt);
	
	private:
		btDefaultCollisionConfiguration*     collision_configuration;
		btCollisionDispatcher*               collision_dispatcher;
		btDbvtBroadphase*                    broadphase_interface;
		btSequentialImpulseConstraintSolver* constraint_solver;
		btDiscreteDynamicsWorld*             collision_world;
	};
	
}
