/*
 *  InputManager.h
 *  TimeStream
 *
 *  Created by Noah Zentzis on 4/9/10.
 *  Copyright 2010 Noah Zentzis. All rights reserved.
 *
 */

#pragma once

#include <OIS/OIS.h>
#include <OGRE/Ogre.h>

class InputManager : public Ogre::FrameListener, public OIS::KeyListener, public OIS::MouseListener {
public:
	InputManager(Ogre::RenderWindow* win, Ogre::Root* r);
	~InputManager();
	
	Ogre::Vector2 mousePosition();
	OIS::MouseState getMouseState();
	bool isMousePressed(OIS::MouseButtonID btn);
	bool isKeyPressed(OIS::KeyCode k);
	
	void registerKeyListener(OIS::KeyListener* l);
	void registerMouseListener(OIS::MouseListener* l);
	
	bool keyPressed(const OIS::KeyEvent& evt);
	bool keyReleased(const OIS::KeyEvent& evt);
	
	bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID bid);
	bool mouseMoved(const OIS::MouseEvent& evt);
	bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID bid);
	
	bool frameRenderingQueued(const Ogre::FrameEvent &evt);
private:
	std::list<OIS::KeyListener*> m_keyListeners;
	std::list<OIS::MouseListener*> m_mouseListeners;
	
	OIS::MouseState m_mouseState;
	Ogre::Vector2 m_mousePos;
	
	OIS::Keyboard* m_keybd;
	OIS::Mouse* m_mouse;
	OIS::InputManager* m_inMgr;
};
