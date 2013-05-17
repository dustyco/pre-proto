#pragma once


#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>
#include "sim/entity/entity.h"


namespace entity {
	
	// Component //////////////////////////////////////////////////////////////

	class OgreComponent : public RenderComponent {
	public:
		 OgreComponent (System* s, std::string model) : RenderComponent(s, model) {}
		~OgreComponent ();
		virtual void update ();
		
		Ogre::Entity*     ogre_entity;
		Ogre::SceneNode*  ogre_node;
	};
	
	
	// System /////////////////////////////////////////////////////////////////
	
	class OgreSystem : public RenderSystem {
	public:
		 OgreSystem (Ogre::SceneManager* ogre_scene) : ogre_scene(ogre_scene) {}
		~OgreSystem ();
		virtual RenderComponent* create (std::string model);
		virtual void update ();
		
		Ogre::SceneManager*  ogre_scene;
	};
	
}
