#pragma once
#include <map>
#include <string>

namespace Game {
	class Entity; // This way the Entity class can register itself
	
	template<class T> Entity* createEntity() { return new T(); };
	
	class EntityFactory {
		typedef std::map<std::string, Entity*(*)()> map_type;
	
	public:
		static Entity* buildEntity(std::string const& name);
	
	protected:
		static map_type* getMap();
	
	private:
		static map_type* map;
	};
	
	template<typename T>
	class EntityRegister : EntityFactory {
	public:
		EntityRegister(std::string const& name) {
			getMap()->insert(std::make_pair(name, &createEntity<T>));
		}
	};
};

// Registration macros
// This must be a private member of the class declaration
#define ENTITY_MEMBER(cls) \
	static Game::EntityRegister<cls> m_entityRegistryMember
// And this must be in a .cpp file with access to the class
#define DECLARE_ENTITY(cls, name) \
	Game::EntityRegister<cls> cls::m_entityRegistryMember = Game::EntityRegister<cls>(name);
