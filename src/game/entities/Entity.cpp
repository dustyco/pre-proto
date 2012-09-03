#include "Entity.h"
#include "../../logging/logging.h"
#include <cstdlib>
#include <algorithm>
using namespace Game;

DECLARE_ENTITY(Entity, "ent_base")

Entity::Entity() : parent(NULL) {
}

void Entity::baseInit() {
}

