#include "EntityRegistry.h"
using namespace Game;

EntityFactory::map_type* EntityFactory::map = NULL;

EntityFactory::map_type* EntityFactory::getMap() {
	// Never delete this, because the memory will be reclaimed at the
	// end anyway, and we can't guarantee it won't get deleted before
	// we need it.
	if(!map) map = new map_type;
	return map;
}

Entity* EntityFactory::buildEntity(std::string const& name) {
	map_type::iterator i = getMap()->find(name);
	if(i == getMap()->end()) return NULL;
	return i->second();
}
