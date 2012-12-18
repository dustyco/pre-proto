

#include "Renderable.h"


Renderable::Renderable (Ogre::SceneManager* ogre_scene, std::string& model) {
	m_ogre_scene = ogre_scene;
	m_ogre_entity = ogre_scene->createEntity(model);
	m_ogre_scene_node = m_ogre_scene->getRootSceneNode()->createChildSceneNode();
	m_ogre_scene_node->attachObject(m_ogre_entity);
}
Renderable::~Renderable () {
	m_ogre_scene->destroyEntity(m_ogre_entity);
}
void Renderable::setPosition (Ogre::Vector3 v) {
	m_ogre_scene_node->setPosition(v);
}

