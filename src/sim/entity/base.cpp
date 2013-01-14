

#include "base.h"


namespace entity {

	
	// Entity /////////////////////////////////////////////////////////////////
	
	// Delete all components before being unallocated
	Entity::~Entity () {
		for (std::map<ct_mask,Component*>::iterator it=comps.begin(); it!=comps.end(); it++) {
			delete (*it).second;
		}
	}
	Component* Entity::setComp (ct_mask type, Component* comp) {
		comp->entity = this;
		comps[type] = comp;
		mask = mask | type;
		return comp;
	}
	Component* Entity::getComp (ct_mask type) {
		if (mask & type) return comps[type];
		else return 0;
	}
	void Entity::delComp (ct_mask type) {
		if (mask & type) {
			delete comps[type];
			mask = (mask & ~type);
		}
	}
	bool Entity::hasComps (ct_mask types) {
		return (mask & types) == types;
	}
	
	
	// Component //////////////////////////////////////////////////////////////
	
	Component::~Component () {
		system->remove(this);
	}
	
	
	// System /////////////////////////////////////////////////////////////////
	
	System::~System () {
	}
	void System::destroyAttachedEntities () {
		std::set<Component*>::iterator it=comps.begin(), temp;
		while (it!=comps.end()) {
			temp = it++;
			delete (*temp)->entity;
		}
	}
	void System::remove (Component* comp) {
		comps.erase(comp);
	}
	

}
