#include "GameWorld.h"
#include "../logging/logging.h"

using namespace Game;

GameWorld::GameWorld () {
	m_maxEntID = 1;
	m_usedIDs.insert(0);

	m_timer.resetTime();
}

GameWorld::~GameWorld () { }

void GameWorld::update () {
	update(m_timer.read());
}

void GameWorld::update (float dt) {
	// Call entity think methods
	std::map<EntID, Entity*>::iterator i;
	for (i=m_ents.begin();i != m_ents.end();i++) {
		i->second->think();
	}
}

EntID GameWorld::createEntity (std::string name) {
	// Create the entity
	Entity* ent = EntityFactory::buildEntity(name);
	ent->parent = this;
	ent->id = newEntityID();

	// Validate it
	if (ent->id == 0) { // Returned when no ID could be found
		delete ent;
		return 0;
	}
	ent->init();

	// Add to data structures and return
	m_ents[ent->id] = ent;
	return ent->id;
}

void GameWorld::deleteEntity (Entity* ent) {
	if (ent->parent != this) {
		ERROR("Tried to delete ent #%d from a GameWorld that does not own it!", ent->id);
		return;
	}
	deleteEntity(ent->id);
}

void GameWorld::deleteEntity(EntID id) {
	if (m_ents.find(id) == m_ents.end()) {
		WARNING("Tried to delete non-existant EntID (%d)", id);
		return;
	}
	Entity* ent = m_ents[id];
	ent->id = 0;
	ent->parent = NULL;
	m_ents.erase(id);
	freeEntityID(id);
}

Entity* GameWorld::getEntity (EntID id) {
	if (m_ents.find(id) == m_ents.end()) return NULL;
	return m_ents[id];
}

EntID GameWorld::newEntityID () {
	// Check whether we've reached the EntID limit
	if (m_maxEntID < MAX_ENTID) {
		// Just assign the next one
		EntID res = m_maxEntID++;
		m_usedIDs.insert(res);
		return res;
	} else {
		// Search through the list for an unused one
		EntID last = 0;
		std::set<EntID>::iterator i;
		for (i=m_usedIDs.begin();i != m_usedIDs.end();i++) {
			EntID id = (*i);
			if (last < (id-1)) {
				m_usedIDs.insert(last+1);
				return last+1;
			}
		}
		return 0;
	}
}

void GameWorld::freeEntityID (EntID id) {
	if (id == m_maxEntID) {
		m_maxEntID--;
	}
	m_usedIDs.erase(id);
	m_ents.erase(id);
}

