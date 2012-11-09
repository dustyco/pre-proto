

#include <boost/lexical_cast.hpp>

#include "../logging/logging.h"
#include "GUIManager.h"


GUIManager::GUIManager (DisplayManager* display)
{
display->lock();

	// Set up CEGUI
	m_cegui_renderer = &CEGUI::OgreRenderer::bootstrapSystem(*(display->getRenderWindow()));
	
	CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
	
	CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
	CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");
	
	m_cegui_root = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "root");
	CEGUI::System::getSingleton().setGUISheet(m_cegui_root);
/*	m_cegui_test = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", "_test");
	m_cegui_root->addChildWindow(m_cegui_test);
	m_cegui_test->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(1, -20)));
	m_cegui_test->setSize(CEGUI::UVector2(CEGUI::UDim(0,150),CEGUI::UDim(0,20)));
	m_cegui_test->setText(boost::lexical_cast<std::string>(int(42)));
*/
	m_cegui_framewindow = (CEGUI::FrameWindow*)CEGUI::WindowManager::getSingleton().createWindow( "TaharezLook/FrameWindow", "framewindow" );
	m_cegui_root->addChildWindow(m_cegui_framewindow);
	
	// position a quarter of the way in from the top-left of parent.
	m_cegui_framewindow->setPosition( CEGUI::UVector2( CEGUI::UDim( 0.25f, 0 ), CEGUI::UDim( 0.25f, 0 ) ) );
	// set size to be half the size of the parent
	m_cegui_framewindow->setSize( CEGUI::UVector2( CEGUI::UDim( 0.5f, 0 ), CEGUI::UDim( 0.5f, 0 ) ) );
	
display->unlock();
}
GUIManager::~GUIManager ()
{
	
}


