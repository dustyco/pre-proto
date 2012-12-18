#pragma once


#include <OGRE/Ogre.h>


class Renderable {
public:
	Renderable (Ogre::SceneManager* ogre_scene, std::string& model);
	~Renderable ();
	
public:
	void setPosition (Ogre::Vector3 v);
	
private:
	Ogre::SceneManager* m_ogre_scene;
	Ogre::Entity*       m_ogre_entity;
	Ogre::SceneNode*    m_ogre_scene_node;
};
