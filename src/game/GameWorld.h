#pragma once
#include "entities/entities.h"
#include "../util/util.h"

#include <set>
#include <list>

namespace Game {
	/* GameWorld manages the simulation context for a game world, separately from
	 * the graphics, audio, and other such detritus. This should be entirely
	 * decoupled from the graphics engine, without any dependencies on OGRE. */
	class GameWorld {
	public:
		GameWorld ();
		~GameWorld ();

		void update ();
		void update (float dt);

		// Entity functions
		EntID createEntity (std::string name);
		// Note that these don't actually deallocate the entity, they just
		// remove it from the GameWorld and set its ID and parent to 0.
		void deleteEntity (Entity* ent);
		void deleteEntity (EntID id);
		
		Entity* getEntity (EntID id);
	private:
		// Utility functions
		EntID newEntityID ();
		void freeEntityID (EntID id);

		// Entity state
		std::map<EntID, Entity*> m_ents;
		std::set<EntID> m_usedIDs;
		EntID m_maxEntID;

		// World state
		timer m_timer;
	};
};
