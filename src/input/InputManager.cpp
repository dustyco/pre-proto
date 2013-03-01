

#include <sstream>
#include <iostream>
using namespace std;

#include <boost/thread/locks.hpp>

#include "../logging/logging.h"
#include "../util/singleton.h"
#include "../video/DisplayManager.h"

#include "InputManager.h"


InputManager::InputManager () {
	boost::lock_guard<boost::recursive_mutex>(*this);
	m_mouse_freedom = true;
	m_reconnect = false;
	connect();
}
InputManager::~InputManager () {
	boost::lock_guard<boost::recursive_mutex>(*this);
	disconnect();
}


InputManager::Subscription* InputManager::subscribe () {
	boost::lock_guard<boost::recursive_mutex>(*this);
	Subscription* sub = new Subscription;
	sub->next = m_events.begin();
	m_subs.push_back(sub);
	return sub;
}
void InputManager::unsubscribe (InputManager::Subscription* sub) {
	boost::lock_guard<boost::recursive_mutex>(*this);
	m_subs.remove(sub);
	// Pop any old events that were waiting for it
	bool old = true;
	while (old) {
		for (SubList::iterator it=m_subs.begin(); it!=m_subs.end(); it++) {
			if ((*it)->next==m_events.begin()) {
				old = false;
				break;
			}
		}
		if (old) m_events.pop_front();
	}
}
bool InputManager::nextEvent (InputManager::Subscription* sub, InputManager::Event& event) {
	boost::lock_guard<boost::recursive_mutex>(*this);

	// Add any new events
	m_mouse->capture();
	m_keybd->capture();
	m_mouseState = m_mouse->getMouseState();
	if (m_reconnect) {
		m_reconnect = false;
		disconnect();
		connect();
	}

	// Are there events waiting for this subscriber?
	bool r = true;
	if (sub->next!=m_events.end()) {
		// Copy the next event and advance subscriber's pointer
		event = *(sub->next++);
	
		// Pop the oldest event if all subscribers are past it
		bool pop = true;
		for (SubList::iterator it=m_subs.begin(); it!=m_subs.end(); it++) {
			if ((*it)->next==m_events.begin()) {
				pop = false;
				break;
			}
		}
		if (pop) m_events.pop_front();
	} else r = false;
	return r;
}

// QUERIES ////////////////////////////////////////////////////////////////////
bool InputManager::getMouseFreedom () {
	boost::lock_guard<boost::recursive_mutex>(*this);
	return m_mouse_freedom;
}
void InputManager::setMouseFreedom (bool mouse_freedom) {
	boost::lock_guard<boost::recursive_mutex>(*this);
	if (m_mouse_freedom != mouse_freedom) {
		m_mouse_freedom = mouse_freedom;
		m_reconnect = true;
	}
}
vec<2,int> InputManager::mousePosition() {
	boost::lock_guard<boost::recursive_mutex>(*this);
	return m_mousePos;
}
OIS::MouseState InputManager::getMouseState() {
	boost::lock_guard<boost::recursive_mutex>(*this);
	return m_mouseState;
}
bool InputManager::isMousePressed(OIS::MouseButtonID btn) {
	boost::lock_guard<boost::recursive_mutex>(*this);
	return m_mouseState.buttonDown(btn);
}
bool InputManager::isKeyPressed(OIS::KeyCode k) {
	boost::lock_guard<boost::recursive_mutex>(*this);
	return m_keybd->isKeyDown(k);
}


void InputManager::connect () {
	size_t winHandle = 0;
	ref<DisplayManager>().getRenderWindow()->getCustomAttribute("WINDOW", &winHandle);
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
}
void InputManager::disconnect () {
	m_keybd->setEventCallback(0);
	m_mouse->setEventCallback(0);
	m_inMgr->destroyInputObject(m_keybd);
	m_inMgr->destroyInputObject(m_mouse);
	OIS::InputManager::destroyInputSystem(m_inMgr);
	m_keybd = 0;
	m_mouse = 0;
}
void InputManager::pushEvent (Event& event) {
	m_events.push_back(event);
	for (SubList::iterator it=m_subs.begin(); it!=m_subs.end(); it++) {
		if ((*it)->next==m_events.end()) {
			((*it)->next)--;
		}
	}
}


// CALLBACKS //////////////////////////////////////////////////////////////////
bool InputManager::keyPressed(const OIS::KeyEvent& evt) {
	Event event(Event::KEY);
	event.press = true;
	event.key = evt.key;
	pushEvent(event);
	return true;
}
bool InputManager::keyReleased(const OIS::KeyEvent& evt) {
	Event event(Event::KEY);
	event.press = false;
	event.key = evt.key;
	pushEvent(event);
	return true;
}
bool InputManager::mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID bid) {
	m_mouseState = evt.state;
	Event event(Event::MOUSE);
	event.state = evt.state;
	pushEvent(event);
	return true;
}
bool InputManager::mouseMoved(const OIS::MouseEvent& evt) {
	m_mouseState = evt.state;
	m_mousePos += vec<2,int>(evt.state.X.rel, evt.state.Y.rel);
	Event event(Event::MOUSE);
	event.state = evt.state;
	pushEvent(event);
	return true;
}
bool InputManager::mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID bid) {
	m_mouseState = evt.state;
	Event event(Event::MOUSE);
	event.state = evt.state;
	pushEvent(event);
	return true;
}
