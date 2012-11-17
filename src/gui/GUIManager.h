#pragma once


#include <stdexcept>

#include <CEGUI.h>
#include <CEGUIOgreRenderer.h>
#include <CEGUIDefaultResourceProvider.h>


class GUIManager {
public:
	GUIManager  ();
	~GUIManager ();
	
private:
	CEGUI::OgreRenderer* m_cegui_renderer;
	CEGUI::Window*       m_cegui_root;
	CEGUI::Window*       m_cegui_test;
	CEGUI::FrameWindow*  m_cegui_framewindow;
};


