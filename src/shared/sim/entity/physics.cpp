

#include "physics.h"

#include <iostream>
using namespace std;


namespace entity {
	
	// Component //////////////////////////////////////////////////////////////
	
	PhysicsComponent::~PhysicsComponent () {
	}
	
	
	// System /////////////////////////////////////////////////////////////////
	
	PhysicsSystem::PhysicsSystem () {
		// Collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
		collision_configuration = new btDefaultCollisionConfiguration;

		// Use the default collision collision_dispatcher. For parallel processing you can use a diffent collision_dispatcher (see Extras/BulletMultiThreaded)
		collision_dispatcher = new btCollisionDispatcher(collision_configuration);

		// btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
		broadphase_interface = new btDbvtBroadphase;

		// The default constraint constraint_solver. For parallel processing you can use a different constraint_solver (see Extras/BulletMultiThreaded)
		constraint_solver = new btSequentialImpulseConstraintSolver;

		collision_world = new btDiscreteDynamicsWorld(collision_dispatcher, broadphase_interface, constraint_solver, collision_configuration);
		collision_world->setGravity(btVector3(0,-10,0));
	}
	PhysicsSystem::~PhysicsSystem () {
		// Must be called before destroying anything else
		destroyAttachedEntities();
	}
	PhysicsComponent* PhysicsSystem::create (Vec3 pos) {
		PhysicsComponent* c = new PhysicsComponent(this, pos);
		comps.insert(c);
		return c;
	}
	
	void PhysicsSystem::tick (double dt) {
		
	}
	
}
