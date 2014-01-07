
// engine.h

#include "../bigball.h"

#ifndef BB_ENGINE_H
#define BB_ENGINE_H

namespace bigball
{

class BIGBALL_API Engine
{
public:
					Engine();
	virtual			~Engine();

	virtual bool	Init( bool bCreateWindow );
	virtual void	Shutdown();
	virtual void	MainLoop();

protected:
	SDL_Window*		m_MainWindow; 
	SDL_GLContext	m_GLContext; 


};

extern BIGBALL_API Engine* g_pEngine;

};

#endif  // BB_ENGINE_H