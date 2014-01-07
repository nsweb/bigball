
// engine.h

#include "../bigball.h"

#ifndef BB_ENGINE_H
#define BB_ENGINE_H 1

namespace bigball
{

class BIGBALL_API Engine
{
public:

	static bool StaticInit( bool bCreateWindow );
	static void StaticShutdown();

	void		MainLoop();

protected:
	SDL_Window*		m_MainWindow; 
	SDL_GLContext	m_GLContext; 

				Engine();
				~Engine();
	bool		InitPrivate( bool bCreateWindow );
	void		ShutdownPrivate();
};

extern BIGBALL_API Engine* g_pEngine;

};

#endif  // BB_ENGINE_H