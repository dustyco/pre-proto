

#include <sstream>
#include <iostream>
using namespace std;

#include "InputManager.h"
#include "../logging/logging.h"


InputManager::InputManager (Ogre::Root* root, DisplayManager* display) {
	lock();
	
	m_root = root;
	m_display = display;
	m_mouse_freedom = true;
	m_reconnect = false;
	_connect();
	
	unlock();
}

InputManager::~InputManager () {
	lock();
	_disconnect();
	unlock();
}
	
void InputManager::_connect () {
	size_t winHandle = 0;
	m_display->getRenderWindow()->getCustomAttribute("WINDOW", &winHandle);
	ostringstream ss;
	ss << winHandle;
	
	OIS::ParamList pl;
	pl.insert(std::make_pair("WINDOW", ss.str()));
	
	if (m_mouse_freedom) {
		#if defined OIS_WIN32_PLATFORM
			pl.insert(std::make_pair("w32_mouse",    "DISCL_FOREGROUND"));
			pl.insert(std::make_pair("w32_mouse",    "DISCL_NONEXCLUSIVE"));
			pl.insert(std::make_pair("w32_keyboard", "DISCL_FOREGROUND"));
			pl.insert(std::make_pair("w32_keyboard", "DISCL_NONEXCLUSIVE"));
		#elif defined OIS_LINUX_PLATFORM
			pl.insert(std::make_pair("x11_mouse_grab",    "false"));
			pl.insert(std::make_pair("x11_mouse_hide",    "false"));
			pl.insert(std::make_pair("x11_keyboard_grab", "false"));
			pl.insert(std::make_pair("XAutoRepeatOn",     "true"));
		#endif
	}
	
	m_inMgr = OIS::InputManager::createInputSystem(pl);
	m_keybd = static_cast<OIS::Keyboard*> (m_inMgr->createInputObject(OIS::OISKeyboard, true));
	m_mouse = static_cast<OIS::Mouse*> (m_inMgr->createInputObject(OIS::OISMouse, true));
	m_keybd->setEventCallback(this);
	m_mouse->setEventCallback(this);
	m_root->addFrameListener(this);
}
void InputManager::_disconnect () {
	m_keybd->setEventCallback(0);
	m_mouse->setEventCallback(0);
	m_inMgr->destroyInputObject(m_keybd);
	m_inMgr->destroyInputObject(m_mouse);
	OIS::InputManager::destroyInputSystem(m_inMgr);
	m_keybd = 0;
	m_mouse = 0;
	m_root->removeFrameListener(this);
}
	
bool InputManager::getMouseFreedom () {
	lock();
	bool mouse_freedom = m_mouse_freedom;
	unlock();
	return mouse_freedom;
}
void InputManager::setMouseFreedom (bool mouse_freedom) {
	lock();
	if (m_mouse_freedom != mouse_freedom) {
		m_mouse_freedom = mouse_freedom;
		m_reconnect = true;
	}
	unlock();
}

void InputManager::registerKeyListener(OIS::KeyListener* l) {
	m_keyListeners.push_front(l);
	m_keyListeners.unique();
}
void InputManager::registerMouseListener(OIS::MouseListener* l) {
	m_mouseListeners.push_front(l);
	m_mouseListeners.unique();
}

void InputManager::unregisterKeyListener(OIS::KeyListener* l) {
	m_keyListeners.remove(l);
}
void InputManager::unregisterMouseListener(OIS::MouseListener* l) {
	m_mouseListeners.remove(l);
}


// QUERIES ////////////////////////////////////////////////////////////////////
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


// CALLBACKS //////////////////////////////////////////////////////////////////
// FRAME LISTENER /////////////////////////////////////////////////////////////
bool InputManager::frameRenderingQueued(const Ogre::FrameEvent &evt) {
	return true;
}

bool InputManager::frameStarted(const Ogre::FrameEvent &evt) {
	lock();
	m_mouse->capture();
	m_keybd->capture();
	m_mouseState = m_mouse->getMouseState();
	if (m_reconnect) {
		m_reconnect = false;
		_disconnect();
		_connect();
	}
	unlock();
	return true;
}

// KEY LISTENER ///////////////////////////////////////////////////////////////
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
