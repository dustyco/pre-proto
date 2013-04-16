#pragma once


#ifndef PCH
	#include <OIS/OIS.h>
	#include <boost/thread/recursive_mutex.hpp>
#endif

#include "../util/linear_algebra.h"
using namespace linear_algebra;


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
		enum Type { MOUSE, KEY, JOY };
		Type            type;
		OIS::MouseState state;
		OIS::KeyCode    key;
		bool            press;
		
		Event () {}
		Event (Type type) : type(type) {}
	};
	typedef std::list<Event> EventList;
	
	struct Subscription { EventList::iterator next; };
	typedef std::list<Subscription*> SubList;
	
public:
	 InputManager ();
	~InputManager ();
	
	// Event subscription
	Subscription* subscribe   ();
	void          unsubscribe (Subscription* sub);
	bool          nextEvent   (Subscription* sub, Event& event);
	
	// Queries
	bool            getMouseFreedom ();
	void            setMouseFreedom (bool);
	bool            isMousePressed  (OIS::MouseButtonID btn);
	bool            isKeyPressed    (OIS::KeyCode k);
	OIS::MouseState getMouseState   ();
	vec<2,int>      mousePosition   ();
	
	// Non-API callbacks
	bool keyPressed    (const OIS::KeyEvent& evt);
	bool keyReleased   (const OIS::KeyEvent& evt);
	bool mousePressed  (const OIS::MouseEvent& evt, OIS::MouseButtonID bid);
	bool mouseReleased (const OIS::MouseEvent& evt, OIS::MouseButtonID bid);
	bool mouseMoved    (const OIS::MouseEvent& evt);

private:
	void connect    ();
	void disconnect ();
	void pushEvent  (Event& event);
	
private:
	EventList          m_events;
	SubList            m_subs;
	
	bool               m_mouse_freedom;
	bool               m_reconnect;
	
	OIS::MouseState    m_mouseState;
	vec<2,int>         m_mousePos;
	
	OIS::Keyboard*     m_keybd;
	OIS::Mouse*        m_mouse;
	OIS::InputManager* m_inMgr;
};
