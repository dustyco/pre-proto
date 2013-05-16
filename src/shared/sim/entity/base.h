#pragma once


#include <cstdlib>
#include <stdint.h>
#include <map>
#include <set>


namespace entity {
	
	// Component type masks
	typedef uint8_t ct_mask;
	ct_mask static  CT_I = 0;
	const static ct_mask CT_RENDER  = 1<<CT_I++;
	const static ct_mask CT_PHYSICS = 1<<CT_I++;
	
	
	struct Entity;
	struct Component;
	struct System;
	
	
	// Entity /////////////////////////////////////////////////////////////////
	
	struct Entity {
		 Entity () : mask(0) {}
		~Entity ();
	
		Component* setComp  (ct_mask type, Component* comp);
		Component* getComp  (ct_mask type);
		void       delComp  (ct_mask type);
		bool       hasComps (ct_mask types);
	
		ct_mask                      mask;
		std::map<ct_mask,Component*> comps;
	};
	
	
	// Component //////////////////////////////////////////////////////////////
	
	struct Component {
		Component (System* s) : system(s) {}
		virtual ~Component ();
		
		Entity* entity;
		System* system;
	};
	
	
	// System /////////////////////////////////////////////////////////////////
	
	class System {
	public:
		virtual ~System ();
		void remove (Component* comp);
		
	protected:
		// This MUST be called before a derived system is destroyed
		// Each attached entity will destroy each of its components
		// which will in turn remove themselves from this and other systems
		// Recommended: Call it FIRST in the derived destructor
		void destroyAttachedEntities ();
		
		std::set<Component*> comps;
	};



}
