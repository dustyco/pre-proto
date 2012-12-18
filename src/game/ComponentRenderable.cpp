

#include "../common.h"
#include "ComponentRenderable.h"


Renderable* ComponentRenderable::add (EntID id, std::string model) {
	Renderable* r = new Renderable(m_scene, model);
	std::map<EntID, Renderable*>::iterator it = m_by_id.find(id);
	if (it == m_by_id.end()) m_by_id.insert(std::pair<EntID, Renderable*>(id, r));
	else {
		delete it->second;
		it->second = r;
	}
	return r;
}

ComponentRenderable::ComponentRenderable () {
	REF(Ogre::Root, ogre_root);
	
	// Set up scene and camera
	m_scene = ogre_root.createSceneManager(Ogre::ST_GENERIC);
	m_camera = m_scene->createCamera("Camera");
	m_camera->lookAt(Ogre::Vector3(0,0,0));
	m_camera->setNearClipDistance(1);
	m_camNode = m_scene->getRootSceneNode()->createChildSceneNode();
	m_camNode->attachObject(m_camera);
	m_camNode->setPosition(Ogre::Vector3(10,10,50));
	m_camNode->setPosition(Ogre::Vector3(0,0,1));
	
	m_scene->setAmbientLight(Ogre::ColourValue(0.1f, 0.1f, 0.1f));
	Ogre::Light* light = m_scene->createLight("MainLight");
	light->setPosition(20.0f, 80.0f, 50.0f);
}

ComponentRenderable::~ComponentRenderable () {
	// Delete stuff
}
