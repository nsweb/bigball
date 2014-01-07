// engine.cpp
//


#include "engine.h"

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

bool Engine::StaticInit( bool bCreateWindow )
{
	g_pEngine = new Engine();
	return g_pEngine->InitPrivate( bCreateWindow );
}

void Engine::StaticShutdown()
{
	g_pEngine->ShutdownPrivate();
	delete g_pEngine;
	g_pEngine = nullptr;
}

bool Engine::InitPrivate( bool bCreateWindow )
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
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
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

    //checkSDLError(__LINE__);

    /* Create our opengl context and attach it to our window */
    m_GLContext = SDL_GL_CreateContext( m_MainWindow );
    //checkSDLError(__LINE__);


    /* This makes our buffer swap syncronized with the monitor's vertical refresh */
    SDL_GL_SetSwapInterval(1);

    /* Clear our buffer with a red background */
    glClearColor( 0.1f, 0.1f, 0.15f, 1.0f );
    glClear ( GL_COLOR_BUFFER_BIT );
    /* Swap our back buffer to the front */
    SDL_GL_SwapWindow( m_MainWindow );
 
	return true;
}

void Engine::ShutdownPrivate()
{
	/* Delete our opengl context, destroy our window, and shutdown SDL */
	SDL_GL_DeleteContext( m_GLContext );
	SDL_DestroyWindow( m_MainWindow );
	SDL_Quit();
}

void Engine::MainLoop()
{
	while( 1 )
	{
		int32 LoopStatus = 0;

		glClear( GL_COLOR_BUFFER_BIT );

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