

#include <sstream>
#include <math.h>
#include <OGRE/Ogre.h>
#include <OIS/OIS.h>
#include "video/video.h"
#include "input/input.h"
#include "common.h"
#include "config/config.h"
#include "util/Clock.h"
#include "clientgame/ClientGame.h"

#ifndef OGRE_PLUGIN_DIR
#define OGRE_PLUGIN_DIR "."
#endif


struct Client : public Ogre::FrameListener {
	InputManager::Subscription* m_sub;
	bool                running;
	Clock               m_clock;
	Ogre::SceneManager* m_app_sceneMgr;
	Ogre::Camera*       m_app_camera;
	Ogre::Viewport*     m_app_viewport;
	ClientGame*         m_game_a;
	ClientGame*         m_game_b;
	Ogre::SceneNode*    m_game_a_node;
	Ogre::SceneNode*    m_game_b_node;
	Ogre::Rectangle2D*  m_game_a_rect;
	Ogre::Rectangle2D*  m_game_b_rect;
	Ogre::MaterialPtr   m_game_a_mat;
	Ogre::MaterialPtr   m_game_b_mat;
	Ogre::TexturePtr    m_game_a_rtt;
	Ogre::TexturePtr    m_game_b_rtt;
	float               m_game_shift; // 1=a, -1=b
	int                 m_width, m_height;


	void init (int argc, char** argv);
	void run ();
	void shutdown ();

	// Recreate RTT's for the game panels if the display changed size
	void checkDisplaySize ();
	void createGamePanels ();
	void deleteGamePanels ();
	
	// Frame events
	bool frameRenderingQueued (const Ogre::FrameEvent& evt);
	bool frameStarted         (const Ogre::FrameEvent& evt);
};


void Client::init (int argc, char **argv) {
	Logging::LogManager::getInstance()->addLogger(new Logging::ConsoleLogger(), Logging::LL_DEBUG);

	// In order to avoid Ogre's default logging behavior we have to create
	// the Ogre::LogManager before initializing the Root object.
	Ogre::LogManager* ogreLogMgr = new Ogre::LogManager();
	Logging::OgreLogAdapter* logAdapter = new Logging::OgreLogAdapter();
	Ogre::LogManager::getSingleton().createLog("ogre.log", true, true, true)->addListener(logAdapter);
	ogreLogMgr->setLogDetail(Ogre::LL_LOW);
	
	// Initialize the configuration system
	set<ConfigManager>(new ConfigManager(argc, argv));

	// Initialize the Ogre root
	set<Ogre::Root>(new Ogre::Root("", "", ""));
	REF(Ogre::Root, ogre_root);
	
	// Load Ogre plugins
	#ifndef OGRE_PLUGIN_DIR
	#define OGRE_PLUGIN_DIR "."
	#endif
	#ifdef _WIN32
		ogre_root.loadPlugin(OGRE_PLUGIN_DIR "/RenderSystem_Direct3D9");
		Ogre::RenderSystem* rs = ogre_root.getRenderSystemByName("Direct3D9 Rendering Subsystem");
	#else
		ogre_root.loadPlugin(OGRE_PLUGIN_DIR "/RenderSystem_GL");
		Ogre::RenderSystem* rs = ogre_root.getRenderSystemByName("OpenGL Rendering Subsystem");
	#endif
	ogre_root.setRenderSystem(rs);
	ogre_root.addFrameListener(this);
	
	// Initialize the display manager
	set<DisplayManager>();
	m_width  = -1;
	m_height = -1;

	// Initialize input and add a listener
//	set<InputManager>().registerKeyListener(this);
	m_sub = set<InputManager>().subscribe();
	
	// Set resource search paths
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media",                    "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/materials/scripts",  "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/materials/textures", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/models",             "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	
	// Scene //////////////////////////////////////////////////////////////////
	m_app_sceneMgr = ogre_root.createSceneManager(Ogre::ST_GENERIC, "app_SceneManager");
	m_app_camera = m_app_sceneMgr->createCamera("app_Camera");
	m_app_viewport = ref<DisplayManager>().getRenderWindow()->addViewport(m_app_camera);
	m_app_viewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f));
	m_app_viewport->setCamera(m_app_camera);
	
	m_game_a = new ClientGame;
	m_game_b = new ClientGame;
	
	// Look at a only
	m_game_shift = 1;
	
	// Set up the RTTs each game will render into
	// They are recreated when the display resizes: checkDisplaySize()
	createGamePanels();
	
	// Keep time since the start of this session
	m_clock.setEpoch();
	
	INFO("Proto initialized");
}

void Client::run () {
	INFO("Rendering");
	running = true;
	ref<Ogre::Root>().startRendering();
}

void Client::shutdown () {
	ref<Ogre::Root>().shutdown();
	delete ptr<ConfigManager>();
	
	LOG(Logging::LL_TELETYPE, "GET ME THOSE CHINESE LANGUAGE FILES I ASKED FOR");
	LOG(Logging::LL_TELETYPE, "END OF LINE");
}

// Early start on cpu-based updates
// Input here will be up to 1 extra frame older than frameStarted
bool Client::frameRenderingQueued (const Ogre::FrameEvent& evt)
{
	return true;
}
// Normal frame start
bool Client::frameStarted (const Ogre::FrameEvent& evt)
{
	REF(DisplayManager, display);
	REF(InputManager, input);
	REF(ConfigManager, config);
	
	double time = boost::chrono::duration<double>(m_clock.getDurationSinceEpoch()).count();
	
	// Input
	InputManager::Event event;
	while (input.nextEvent(m_sub, event))
	switch (event.type) {
		case InputManager::Event::KEY:
			if (event.press)
			switch (event.key) {
				case OIS::KC_ESCAPE: running = false; break;
				case OIS::KC_BACKSLASH:
					// Toggle fullscreen/window
					if (display.getRenderWindow()->isFullScreen()) config.set("video:display.mode", "window");
					else                                           config.set("video:display.mode", "fullscreen");
					display.applySettings();
					break;
				case OIS::KC_F1:
					// Toggle mouse mode
					if (input.getMouseFreedom()) input.setMouseFreedom(false);
					else input.setMouseFreedom(true);
					break;
			}
			break;
		
		case InputManager::Event::MOUSE:
			break;
			
		case InputManager::Event::JOY:
			break;
	}
	
	// See if we should stop
	if (display.isClosing()) { INFO("Display is closing"); running = false; }
	if (!running) return false;
	
	// See if we need to resize game panel RTT's
	checkDisplaySize();
	
	float aspect = (float)(display.getRenderWindow()->getWidth()) / display.getRenderWindow()->getHeight();
	
	// Play with game speed
//	m_game_b->m_clock.warp(pow(1.2, time_r));
	m_game_b->m_clock.warp(sin(time)*sin(time));
//	m_game_b->m_clock.unwarp();

	m_game_a_rect->setCorners(m_game_shift-2.0f, 1.0f, m_game_shift+0.0f, -1.0f, false);
	m_game_b_rect->setCorners(m_game_shift+0.0f, 1.0f, m_game_shift+2.0f, -1.0f, false);
	
	m_game_a->update();
//	m_game_b->update();
	
	return true;
}

void Client::checkDisplaySize () {
	Ogre::RenderWindow& window = *(ref<DisplayManager>().getRenderWindow());
	if (m_width != window.getWidth() || m_height != window.getHeight()) {
		// It changed
		m_width  = window.getWidth();
		m_height = window.getHeight();
//		INFO("Resolution changed");
		// Recreate game RTT's
		deleteGamePanels();
		createGamePanels();
	}
}

void Client::deleteGamePanels () {
	using namespace Ogre;
	
	delete m_game_a_rect;
	delete m_game_b_rect;
	
	TextureManager::getSingleton().remove(m_game_a_rtt->getName());
	TextureManager::getSingleton().remove(m_game_b_rtt->getName());
	
	MaterialManager::getSingleton().remove(m_game_a_mat->getName());
	MaterialManager::getSingleton().remove(m_game_b_mat->getName());
	
	m_game_a_node->getParentSceneNode()->removeAndDestroyChild(m_game_a_node->getName());
	m_game_b_node->getParentSceneNode()->removeAndDestroyChild(m_game_b_node->getName());
	
	m_game_a_rtt.setNull();
	m_game_b_rtt.setNull();
	
	m_game_a_mat.setNull();
	m_game_b_mat.setNull();
}
void Client::createGamePanels () {
	using namespace Ogre;
	
	// Textures
	m_game_a_rtt = Ogre::TextureManager::getSingleton().createManual(
		"app_game_a_rtt",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D,
		ref<DisplayManager>().getRenderWindow()->getWidth(),
		ref<DisplayManager>().getRenderWindow()->getHeight(),
		0,
		Ogre::PF_R8G8B8,
		Ogre::TU_RENDERTARGET
	);
	m_game_b_rtt = Ogre::TextureManager::getSingleton().createManual(
		"app_game_b_rtt",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D,
		ref<DisplayManager>().getRenderWindow()->getWidth(),
		ref<DisplayManager>().getRenderWindow()->getHeight(),
		0,
		Ogre::PF_R8G8B8,
		Ogre::TU_RENDERTARGET
	);
	
	// Materials
	m_game_a_mat = MaterialManager::getSingleton().create("app_game_a_mat", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	m_game_b_mat = MaterialManager::getSingleton().create("app_game_b_mat", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	{
		Pass* pass = m_game_a_mat->getTechnique(0)->getPass(0);
		pass->setLightingEnabled(false);
		TextureUnitState* tus = pass->createTextureUnitState();
		tus->setTextureName(m_game_a_rtt->getName());
		tus->setNumMipmaps(0);
		tus->setTextureFiltering(TFO_NONE);
	} {
		Pass* pass = m_game_b_mat->getTechnique(0)->getPass(0);
		pass->setLightingEnabled(false);
		TextureUnitState* tus = pass->createTextureUnitState();
		tus->setTextureName(m_game_b_rtt->getName());
		tus->setNumMipmaps(0);
		tus->setTextureFiltering(TFO_NONE);
	}
	
	// Rectangles
	m_game_a_rect = new Ogre::Rectangle2D(true);
	m_game_b_rect = new Ogre::Rectangle2D(true);
	m_game_a_rect->setCorners(-2.0f, 1.0f, 0.0f, -1.0f);
	m_game_b_rect->setCorners( 0.0f, 1.0f, 2.0f, -1.0f);
//	m_game_a_rect->setCorners(-1.0f, 1.0f, 1.0f, -1.0f);
//	m_game_b_rect->setCorners(-1.0f, 1.0f, 0.0f, -1.0f);
	m_game_a_rect->setBoundingBox(Ogre::AxisAlignedBox(-100000.0f * Ogre::Vector3::UNIT_SCALE, 100000.0f * Ogre::Vector3::UNIT_SCALE));
	m_game_b_rect->setBoundingBox(Ogre::AxisAlignedBox(-100000.0f * Ogre::Vector3::UNIT_SCALE, 100000.0f * Ogre::Vector3::UNIT_SCALE));
	m_game_a_rect->setMaterial(m_game_a_mat->getName());
	m_game_b_rect->setMaterial(m_game_b_mat->getName());
	m_game_a_node = m_app_sceneMgr->getRootSceneNode()->createChildSceneNode("app_game_a_node");
	m_game_b_node = m_app_sceneMgr->getRootSceneNode()->createChildSceneNode("app_game_b_node");
	m_game_a_node->attachObject(m_game_a_rect);
	m_game_b_node->attachObject(m_game_b_rect);
	
	// Point the games at them
	m_game_a->setRenderTarget(m_game_a_rtt->getBuffer()->getRenderTarget());
	m_game_b->setRenderTarget(m_game_b_rtt->getBuffer()->getRenderTarget());
}






#include <stdexcept>
#include <iostream>
using namespace std;


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
//	INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
int main (int argc, char** argv)
#else
int main (int argc, char** argv)
#endif
{
	try {
		Client app;
		app.init(argc, argv);
		app.run();
		app.shutdown();
		return 0;
	} catch (Ogre::Exception& e) {
//		cerr << "Uncaught Ogre Exception: " << e.getFullDescription().c_str() << endl;
		#ifdef WIN32
			MessageBox(NULL, e.getFullDescription().c_str(), "Uncaught Ogre Exception!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		#endif
	} catch (runtime_error& e) {
		cerr << "Uncaught Runtime Error: " << e.what() << endl;
		#ifdef WIN32
			MessageBox(NULL, e.what(), "Uncaught Runtime Error!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		#endif
	} catch (exception& e) {
		cerr << "Uncaught Exception: " << e.what() << endl;
		#ifdef WIN32
			MessageBox(NULL, e.what(), "Uncaught Exception!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		#endif
	}

	return 1;
}



