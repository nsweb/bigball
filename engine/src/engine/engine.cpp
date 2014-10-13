// engine.cpp
//

#include "../bigball.h"
#include "engine.h"
#include "controller.h"
#include "entitymanager.h"
#include "../gfx/gfxmanager.h"
#include "../gfx/rendercontext.h"
#include "../system/workerthreadmanager.h"
#include "../system/profiler.h"
#include "../ui/uimanager.h"
#include "coposition.h"
#include "camera.h"
#include "tickcontext.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

// HACK
#include "../system/file.h"

namespace bigball
{

Engine* g_pEngine = nullptr;

static CameraView g_SavedFrustumView;

Engine::Engine() :
	m_MainWindow(nullptr),
	m_FrameCount(0),
	m_RenderMode(RenderContext::eRM_Lit),
	m_bShowCulling(false),
	m_DebugFontTexId(0)
{
	
}

Engine::~Engine()
{

}

bool Engine::Init( bool bCreateWindow )
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) /* Initialize SDL's Video subsystem */
	{
        //sdldie("Unable to initialize SDL"); /* Or die on error */
		return false;
	}

    /* Request opengl 3.2 context.
     * SDL doesn't have the ability to choose which profile at this time of writing,
     * but it should default to the core profile */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);//SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);//SDL_GL_CONTEXT_PROFILE_CORE);

    /* Turn on double buffering with a 24bit Z buffer.
     * You may need to change this to 16 or 32 for your system */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    /* Create our window centered at 512x512 resolution */
    m_MainWindow = SDL_CreateWindow("GL Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  800/*1280*/, 600/*720*/, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if( !m_MainWindow ) /* Die if creation failed */
	{
      //  sdldie("Unable to create window");
		return false;
	}

    /* Create our opengl context and attach it to our window */
    m_GLContext = SDL_GL_CreateContext( m_MainWindow );
    //checkSDLError(__LINE__);

	// Init GLEW
	glewExperimental = GL_TRUE; 
	glewInit();

	// Show version info
	const char* RendererId = (const char*)glGetString (GL_RENDERER);	// get renderer string
	const char* VersionId = (const char*)glGetString (GL_VERSION);		// version as a string
	BB_LOG( EngineInit, Log, "Renderer: %s", RendererId );
	BB_LOG( EngineInit, Log, ("OpenGL version supported %s", VersionId) );

    /* This makes our buffer swap syncronized with the monitor's vertical refresh */
    SDL_GL_SetSwapInterval(1);

    /* Clear our buffer with a red background */
    glClearColor( 0.1f, 0.1f, 0.15f, 1.0f );
    glClear ( GL_COLOR_BUFFER_BIT );
    /* Swap our back buffer to the front */
    SDL_GL_SwapWindow( m_MainWindow );

	SDL_GetWindowDisplayMode( m_MainWindow, &m_DisplayMode );

	// Allow FPS style mouse movement
	SDL_SetRelativeMouseMode(SDL_FALSE);
	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_SetWindowGrab( m_MainWindow, SDL_TRUE );

	// Prepare ImGui
	InitImGui();

	// Ready to init our managers
	InitManagers();

	// Create UI manager
	UIManager* pUIManager = new UIManager();
	pUIManager->Create();

	// Declare available components and entities
	DeclareComponentsAndEntities();

	// Setup available cameractrl
	CreateGameCameras();
	
	return true;
}

void Engine::Shutdown()
{
	UIManager::GetStaticInstance()->Destroy();
	delete UIManager::GetStaticInstance();

	DestroyManagers();

	/* Delete our opengl context, destroy our window, and shutdown SDL */
	SDL_GL_DeleteContext( m_GLContext );
	SDL_DestroyWindow( m_MainWindow );
	SDL_Quit();
}

//////////////////////////////////////////////////////////////////////////

void Engine::InitManagers()
{
	//tinyxml2::XMLDocument TestDoc;
	////TestDoc.LoadFile("../../data/test.xml");
	//tinyxml2::XMLError err = TestDoc.LoadFile("../data/test.xml");
	//tinyxml2::XMLElement* FirstElt = TestDoc.FirstChildElement();
	//tinyxml2::XMLNode* FirstChild = TestDoc.FirstChild();

	WorkerThreadManager* pWorkerThreadManager = new WorkerThreadManager();
	pWorkerThreadManager->Create();
	m_Managers.push_back( pWorkerThreadManager );

	Controller* pController = new Controller();
	pController->Create();
	m_Managers.push_back( pController );

	EntityManager* pEntityManager = new EntityManager();
	pEntityManager->Create();
	m_Managers.push_back( pEntityManager );

	GfxManager* pGfxManager = new GfxManager();
	pGfxManager->Create();
	m_Managers.push_back( pGfxManager );
}

void Engine::DestroyManagers()
{
	for( int32 i = m_Managers.size() - 1; i >=0 ; --i )
	{
		m_Managers[i]->Destroy();
		BB_DELETE( m_Managers[i] );
	}
	m_Managers.clear();
}

void Engine::DeclareComponentsAndEntities()
{
	DECLARE_COMPONENT( CoPosition );
	DECLARE_ENTITYPATTERN( Camera, Camera, (0), (0) );
}

void Engine::CreateGameCameras()
{

}

//////////////////////////////////////////////////////////////////////////
void Engine::MainLoop()
{
//int32 OldTime, CurrentTime = SDL_GetTicks();
	uint64 OldTime, CurrentTime = SDL_GetPerformanceCounter();

	while( 1 )
	{
		int32 LoopStatus = 0;
		OldTime = CurrentTime;
		CurrentTime = SDL_GetPerformanceCounter();

		float DeltaSeconds = (CurrentTime - OldTime) / (float)SDL_GetPerformanceFrequency(); // / 1000.0f;


		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		// tell GL to only draw onto a pixel if the shape is closer to the viewer
		glEnable( GL_DEPTH_TEST ); // enable depth-testing
		glDepthFunc( GL_LESS ); // depth-testing interprets a smaller value as "closer"
		glEnable( GL_CULL_FACE );
		glCullFace( GL_FRONT );

		if( m_RenderMode == RenderContext::eRM_Wireframe )
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		TickContext TickCtxt( DeltaSeconds, m_FrameCount );
		for( int32 i = 0; i < m_Managers.size(); ++i )
		{
			m_Managers[i]->Tick( TickCtxt );
		}

		// Prepare rendering
		RenderContext RenderCtxt;
		RenderCtxt.m_View = Controller::GetStaticInstance()->GetRenderView();
		RenderCtxt.m_pFrustumView = (m_bShowCulling ? &g_SavedFrustumView : nullptr);
		RenderCtxt.m_ProjMat = Controller::GetStaticInstance()->GetRenderProjMatrix();
		RenderCtxt.m_DeltaSeconds = DeltaSeconds;
		RenderCtxt.m_FrameIdx = m_FrameCount++;
		RenderCtxt.m_RenderMode = m_RenderMode;
		for( int32 i = 0; i < m_Managers.size(); ++i )
		{
			m_Managers[i]->_Render( RenderCtxt );
		}

		UIManager::GetStaticInstance()->_Render(RenderCtxt);

		SDL_GL_SwapWindow( m_MainWindow );

		// Handle SDL events & inputs
		SDL_Event Event;
		const uint8* Keys = SDL_GetKeyboardState( nullptr );
		uint32 Modifiers = 0;
		if( Keys[SDL_SCANCODE_LCTRL] || Keys[SDL_SCANCODE_RCTRL] )
			Modifiers |= eIM_Ctrl;
		if( Keys[SDL_SCANCODE_LSHIFT] || Keys[SDL_SCANCODE_RSHIFT] )
			Modifiers |= eIM_Shift;
		if( Keys[SDL_SCANCODE_LALT] || Keys[SDL_SCANCODE_RALT] )
			Modifiers |= eIM_Alt;

		//Event.key.keysym.mod
		if( Keys[SDL_SCANCODE_LEFT] )
			Controller::GetStaticInstance()->OnInputX( Modifiers, -DeltaSeconds );
		if( Keys[SDL_SCANCODE_RIGHT] )
			Controller::GetStaticInstance()->OnInputX( Modifiers, DeltaSeconds );
		if( Keys[SDL_SCANCODE_UP] )
			Controller::GetStaticInstance()->OnInputY( Modifiers, DeltaSeconds );
		if( Keys[SDL_SCANCODE_DOWN] )
			Controller::GetStaticInstance()->OnInputY( Modifiers, -DeltaSeconds );
		if( Keys[SDL_SCANCODE_PAGEUP] )
			Controller::GetStaticInstance()->OnInputZ( Modifiers, DeltaSeconds );
		if( Keys[SDL_SCANCODE_PAGEDOWN] )
			Controller::GetStaticInstance()->OnInputZ( Modifiers, -DeltaSeconds );
		

		while( SDL_PollEvent( &Event ) )
		{
			switch( Event.type )
			{
			case SDL_KEYDOWN:
				{
					SDL_Keycode keyPressed = Event.key.keysym.sym;
					//if( keyPressed == SDLK_LEFT )
					//	Controller::GetStaticInstance()->OnInputX( Event.key.keysym.mod, -DeltaSeconds );
				}
				break;
			case SDL_KEYUP:
				{
					// if escape is pressed, quit
					if( Event.key.keysym.sym == SDLK_ESCAPE )
						LoopStatus = 1; // set status to 1 to exit main loop
					else if( Event.key.keysym.sym >= SDLK_F1 && Event.key.keysym.sym <= SDLK_F4 )
					{
						m_RenderMode = Event.key.keysym.sym - SDLK_F1;
					}
					else if( Event.key.keysym.sym == SDLK_F9 || Event.key.keysym.sym == SDLK_F10 )
					{
						// HACK
						bool bWrite = Event.key.keysym.sym == SDLK_F9 ? true : false;

						File outFile;
						if( outFile.Open("../save/camera_debug.bin", bWrite ? File::Write : File::Read) )
						{
							Camera* pCurrentCam = Controller::GetStaticInstance()->GetActiveCamera();
							if( pCurrentCam )
							{
								CameraView& CamView = pCurrentCam->GetView();
								outFile.Serialize( &CamView.m_Transform, sizeof(CamView.m_Transform) );
								outFile.Serialize( &CamView.m_fParameters, sizeof(CamView.m_fParameters) );
							}
						}
					}
					else if( Event.key.keysym.sym == SDLK_c )
					{
						m_bShowCulling = !m_bShowCulling;
						if( m_bShowCulling )
							g_SavedFrustumView = Controller::GetStaticInstance()->GetRenderView();
					}
				}
				break;
			case SDL_MOUSEMOTION:
				{
					vec3 MouseDelta(	-(float)Event.motion.xrel / (float)m_DisplayMode.w, 
										-(float)Event.motion.yrel / (float)m_DisplayMode.h,
										0.f );
					//BB_LOG( Inputs, Log, "MouseDelta x=%f y=%f Mouse x=%d y=%d xrel=%d yrel=%d mod=%d", MouseDelta.x, MouseDelta.y, Event.motion.x, Event.motion.y, Event.motion.xrel, Event.motion.yrel, Event.key.keysym.mod );
					Controller::GetStaticInstance()->OnMouseMove( Modifiers, MouseDelta * DeltaSeconds );
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				break;
			case SDL_MOUSEBUTTONUP:
				break;
			case SDL_QUIT:
				LoopStatus = 1;
				break;
			}
		}

		// Reset profiler data for next frame
		PROFILE_THREAD_FRAMERESET()

		if( LoopStatus == 1 ) // if received instruction to quit
			break;
	}
}

bool Engine::RunCommand( String const& CmdType, Array<String> const& Switches, Array<String> const& Tokens )
{
	return false;
}

//////////////////////////////////////////////////////////////////////////
void CommandLine::Parse( int argc, char* argv[] )
{
	String strArg;
	for( int32 ArgIdx = 1; ArgIdx < argc; ++ArgIdx )
	{
		strArg = argv[ArgIdx];
		if( strArg[0] == '-' )
		{
			// switch
			Switches.push_back( strArg.c_str() + 1 );
		}
		else
		{
			// token
			Tokens.push_back( strArg );
		}
	}
}

bool CommandLine::IsCommand( String& CmdType )
{
	if( Tokens.size() > 0 && Tokens[0].StartsWith( "cmd=" ) )
	{
		CmdType = Tokens[0].Sub( 4, Tokens[0].Len() - 4 );
		return true;
	}
	return false;
}

}