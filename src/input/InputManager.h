#pragma once


#include <OIS/OIS.h>
#include <OGRE/Ogre.h>


class InputManager :
	public Ogre::FrameListener,
	public OIS::KeyListener,
	public OIS::MouseListener
{
public:
	InputManager (Ogre::RenderWindow* win, Ogre::Root* r);
	~InputManager ();
	
	// EXTERNAL LISTENERS /////////////////////////////////////////////////////
	void registerKeyListener   (OIS::KeyListener* l);
	void registerMouseListener (OIS::MouseListener* l);
	
	void unregisterKeyListener   (OIS::KeyListener* l);
	void unregisterMouseListener (OIS::MouseListener* l);
	
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
	
	bool frameRenderingQueued (const Ogre::FrameEvent& evt);
	bool frameStarted         (const Ogre::FrameEvent& evt);
	
private:
	std::list<OIS::KeyListener*>   m_keyListeners;
	std::list<OIS::MouseListener*> m_mouseListeners;
	
	OIS::MouseState    m_mouseState;
	Ogre::Vector2      m_mousePos;
	
	OIS::Keyboard*     m_keybd;
	OIS::Mouse*        m_mouse;
	OIS::InputManager* m_inMgr;
};
