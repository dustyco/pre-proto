#pragma once


#include <map>

#include <OGRE/Ogre.h>

#include "EntID.h"
#include "Renderable.h"


class ComponentRenderable {
public:
	Renderable* add (EntID id, std::string model);

public:
	ComponentRenderable ();
	~ComponentRenderable ();
	Ogre::Camera*    getCamera ()     { return m_camera; }
	Ogre::SceneNode* getCameraNode () { return m_camNode; }

private:
	std::map<EntID, Renderable*> m_by_id;
	
	Ogre::SceneManager*  m_scene;
	Ogre::Camera*        m_camera;
	Ogre::SceneNode*     m_camNode;
};
