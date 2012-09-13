#pragma once
#include <list>
#include <string>
#include <utility>
#include <stdarg.h>

#include "EntityRegistry.h"

#define MAX_ENTID 10000000

// Entity base classes
namespace Game {
	class GameWorld;
	typedef unsigned long EntID;

	struct EntityInfo {
		bool solid;			// Whether the entity is solid (collides with other entities)
	};

	class Entity {
	public:
		// Entity constructors take their params as a va_list
		Entity();
		~Entity() {};

		// Serialization functions
		std::string serialize();
		static Entity* deserialize(std::string data); // Deserialize the given string and return a pointer to a new entity

		// Important functions
		virtual void init(va_list l) {}; // Anything that's not appropriate for the constructor
		virtual void think() {}; // Called every frame
		virtual void render(bool inEditor) {}; // Called before a render so the entity can set up its render state

		// Member info functions
		virtual std::string getName() { return "ent_base"; };
		
		GameWorld* parent;
		EntID id;
	protected:
		// Serialization utility functions

		// Subclass overrides. Note that first thing in these should be calling your superclass's
		// version of these functions unless you have a VERY good reason not to do so.
		virtual void serializeSubclass() {}; // Serialize a subclass using the serialization utility functions
		virtual void deserializeSubclass() {}; // Deserialize a subclass using the serialization utility functions

		// This must be called from the init() method first
		void baseInit();
	private:
		// Serialization support
		std::string buffer;


		ENTITY_MEMBER(Entity);
	};
};
