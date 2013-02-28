#pragma once


#include <boost/thread/recursive_mutex.hpp>
#include <OIS/OIS.h>
#include <OGRE/Ogre.h>


// Query a buffered list of events since the last check
// Multiple subscribers register so that each gets their own buffer
// Thread-safe
class InputManager :
	public OIS::KeyListener,
	public OIS::MouseListener,
	public boost::recursive_mutex
{
public:
	struct Event {
		enum EventType { MOUSE, KEY, JOY };
		EventType type;
		OIS::MouseState state;
		OIS::KeyCode key;
		bool press;
		
		Event () {}
		Event (EventType type) : type(type) {}
	};
	typedef std::list<Event> EventList;
	
	struct Subscription { EventList::iterator next; };
	typedef std::list<Subscription*> SubList;
	
public:
	 InputManager ();
	~InputManager ();
	
	Subscription* subscribe   ();
	void          unsubscribe (Subscription* sub);
	bool          nextEvent   (Subscription* sub, Event& event);
	
	void _connect ();
	void _disconnect ();
	
	// TRUE:  Visible, absolute movement
	// FALSE: Hidden, relative movement
	bool getMouseFreedom ();
	void setMouseFreedom (bool);
	
	// QUERIES ////////////////////////////////////////////////////////////////
	Ogre::Vector2   mousePosition ();
	OIS::MouseState getMouseState ();
	
	bool isMousePressed (OIS::MouseButtonID btn);
	bool isKeyPressed   (OIS::KeyCode k);
	
	// CALLBACKS //////////////////////////////////////////////////////////////
	bool keyPressed  (const OIS::KeyEvent& evt);
	bool keyReleased (const OIS::KeyEvent& evt);
	
	bool mousePressed  (const OIS::MouseEvent& evt, OIS::MouseButtonID bid);
	bool mouseReleased (const OIS::MouseEvent& evt, OIS::MouseButtonID bid);
	bool mouseMoved    (const OIS::MouseEvent& evt);

private:
	void pushEvent (Event& event);
	
private:
	EventList          m_events;
	SubList            m_subs;
	
	bool               m_mouse_freedom;
	bool               m_reconnect;
	
	OIS::MouseState    m_mouseState;
	Ogre::Vector2      m_mousePos;
	
	OIS::Keyboard*     m_keybd;
	OIS::Mouse*        m_mouse;
	OIS::InputManager* m_inMgr;
};
