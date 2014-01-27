// engine.cpp
//

#include "../bigball.h"
#include "engine.h"
#include "controller.h"
#include "entitymanager.h"
#include "../gfx/gfxmanager.h"
#include "coposition.h"
#include "camera.h"

namespace bigball
{

Engine* g_pEngine = nullptr;

Engine::Engine()
{
	m_MainWindow = nullptr;
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
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    /* Turn on double buffering with a 24bit Z buffer.
     * You may need to change this to 16 or 32 for your system */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    /* Create our window centered at 512x512 resolution */
    m_MainWindow = SDL_CreateWindow("GL Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
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

	// Ready to init our managers
	InitManagers();

	// Declare available components and entities
	DeclareComponentsAndEntities();
 
	return true;
}

void Engine::Shutdown()
{
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
		delete m_Managers[i];
	}
	m_Managers.clear();
}

void Engine::DeclareComponentsAndEntities()
{
	DECLARE_COMPONENT( CoPosition );
	DECLARE_ENTITYPATTERN( Camera, Camera, (0), (0) );
}

//////////////////////////////////////////////////////////////////////////
void Engine::MainLoop()
{
	uint32 OldTime, CurrentTime = SDL_GetTicks();

	while( 1 )
	{
		int32 LoopStatus = 0;
		OldTime = CurrentTime;
		CurrentTime = SDL_GetTicks();
		float DeltaSeconds = (CurrentTime - OldTime) / 1000.0f;


		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		// tell GL to only draw onto a pixel if the shape is closer to the viewer
		glEnable( GL_DEPTH_TEST ); // enable depth-testing
		glDepthFunc( GL_LESS ); // depth-testing interprets a smaller value as "closer"

		for( int32 i = 0; i < m_Managers.size(); ++i )
		{
			m_Managers[i]->Tick( DeltaSeconds );
		}

		for( int32 i = 0; i < m_Managers.size(); ++i )
		{
			m_Managers[i]->_Render( DeltaSeconds );
		}

		//glUseProgram (shader_programme);
		//glBindVertexArray (vao);
		//// draw points 0-3 from the currently bound VAO with current in-use shader
		//glDrawArrays (GL_TRIANGLES, 0, 3);

		SDL_GL_SwapWindow( m_MainWindow );

		SDL_Event Event;

		while( SDL_PollEvent( &Event ) )
		{
			switch( Event.type )
			{
			case SDL_KEYDOWN:
				break;
			case SDL_KEYUP:
				// if escape is pressed, quit
				if( Event.key.keysym.sym == SDLK_ESCAPE )
					LoopStatus = 1; // set status to 1 to exit main loop
				break;
			case SDL_QUIT:
				LoopStatus = 1;
				break;
			}
		}

		if( LoopStatus == 1 ) // if received instruction to quit
			break;
	}
}

}