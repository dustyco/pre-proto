/*
 *  InputManager.cpp
 *  TimeStream
 *
 *  Created by Noah Zentzis on 4/9/10.
 *  Copyright 2010 Noah Zentzis. All rights reserved.
 *
 */

#include "InputManager.h"
#include <sstream>
#include <iostream>

using namespace std;

InputManager::InputManager(Ogre::RenderWindow* win, Ogre::Root* rt) {
	OIS::ParamList pl;
	ostringstream ss;
	size_t winHandle = 0;
	win->getCustomAttribute("WINDOW", &winHandle);
	ss << winHandle;
	pl.insert(make_pair(string("WINDOW"), ss.str()));
	m_inMgr = OIS::InputManager::createInputSystem(pl);
	m_keybd = static_cast<OIS::Keyboard*> (m_inMgr->createInputObject(OIS::OISKeyboard, true));
	m_mouse = static_cast<OIS::Mouse*> (m_inMgr->createInputObject(OIS::OISMouse, true));
	m_keybd->setEventCallback(this);
	m_mouse->setEventCallback(this);
	
	// Add a listener
	rt->addFrameListener(this);
}

InputManager::~InputManager() {
	m_inMgr->destroyInputObject(m_keybd);
	m_inMgr->destroyInputObject(m_mouse);
	OIS::InputManager::destroyInputSystem(m_inMgr);
}

void InputManager::registerKeyListener(OIS::KeyListener* l) {
	m_keyListeners.push_front(l);
	m_keyListeners.unique();
}

void InputManager::registerMouseListener(OIS::MouseListener* l) {
	m_mouseListeners.push_front(l);
	m_mouseListeners.unique();
}

Ogre::Vector2 InputManager::mousePosition() {
	return m_mousePos;
}

OIS::MouseState InputManager::getMouseState() {
	return m_mouseState;
}

bool InputManager::isMousePressed(OIS::MouseButtonID btn) {
	return m_mouseState.buttonDown(btn);
}

bool InputManager::isKeyPressed(OIS::KeyCode k) {
	return m_keybd->isKeyDown(k);
}

// Key listener
bool InputManager::frameRenderingQueued(const Ogre::FrameEvent &evt) {
	// Update stuffs
	m_mouse->capture();
	m_keybd->capture();
	m_mouseState = m_mouse->getMouseState();
	
	return true;
}

bool InputManager::keyPressed(const OIS::KeyEvent& evt) {
	list<OIS::KeyListener*>::iterator i;
	for(i=m_keyListeners.begin();i != m_keyListeners.end();i++) {
		if(!(*i)->keyPressed(evt)) {
			m_keyListeners.erase(i);
		}
	}
	
	return true;
}

bool InputManager::keyReleased(const OIS::KeyEvent& evt) {
	list<OIS::KeyListener*>::iterator i;
	for(i=m_keyListeners.begin();i != m_keyListeners.end();i++) {
		if(!(*i)->keyReleased(evt)) {
			m_keyListeners.erase(i);
		}
	}
	
	return true;
}

bool InputManager::mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID bid) {
	m_mouseState = evt.state;
	
	list<OIS::MouseListener*>::iterator i;
	for(i=m_mouseListeners.begin();i != m_mouseListeners.end();i++) {
		if(!(*i)->mousePressed(evt, bid)) {
			m_mouseListeners.erase(i);
		}
	}
	
	return true;
}

bool InputManager::mouseMoved(const OIS::MouseEvent& evt) {
	m_mouseState = evt.state;
	m_mousePos += Ogre::Vector2(evt.state.X.rel, evt.state.Y.rel);
	
	list<OIS::MouseListener*>::iterator i;
	for(i=m_mouseListeners.begin();i != m_mouseListeners.end();i++) {
		if(!(*i)->mouseMoved(evt)) {
			m_mouseListeners.erase(i);
		}
	}
	
	return true;
}

bool InputManager::mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID bid) {
	m_mouseState = evt.state;
	
	list<OIS::MouseListener*>::iterator i;
	for(i=m_mouseListeners.begin();i != m_mouseListeners.end();i++) {
		if(!(*i)->mouseReleased(evt, bid)) {
			m_mouseListeners.erase(i);
		}
	}
	
	return true;
}
