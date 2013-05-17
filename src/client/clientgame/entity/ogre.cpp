

#include "ogre.h"
#include "util/convert_ogre_vector.h"


namespace entity {
	
	// Component //////////////////////////////////////////////////////////////

	OgreComponent::~OgreComponent () {
	}
	
	void OgreComponent::update () {
		if (entity->mask && CT_PHYSICS) {
			PhysicsComponent* comp_physics = (PhysicsComponent*)entity->comps[CT_PHYSICS];
			ogre_node->setPosition(convert(comp_physics->pos));
		}
	}
	
	
	// System /////////////////////////////////////////////////////////////////
	
	OgreSystem::~OgreSystem () {
		// Must be called before destroying anything else
		destroyAttachedEntities();
	}
	
	RenderComponent* OgreSystem::create (std::string model) {
		OgreComponent* c = new OgreComponent(this, model);
		
		c->ogre_entity = ogre_scene->createEntity(model+".mesh");
		c->ogre_node = ogre_scene->getRootSceneNode()->createChildSceneNode();
		c->ogre_node->attachObject(c->ogre_entity);
		
		comps.insert(c);
		return c;
	}
	
	void OgreSystem::update () {
		for (std::set<Component*>::iterator it=comps.begin(); it!=comps.end(); ++it) {
			OgreComponent& comp = *((OgreComponent*)*it);
			comp.update();
		}
	}
	
}
