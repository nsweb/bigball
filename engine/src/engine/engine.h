

#ifndef BB_ENGINE_H
#define BB_ENGINE_H

namespace bigball
{

class BIGBALL_API BaseManager;
//BIGBALL_TEMPLATE template class BIGBALL_API Array< BaseManager* >;

class BIGBALL_API Engine
{
public:
					Engine();
	virtual			~Engine();

	virtual bool	Init( bool bCreateWindow );
	virtual void	Shutdown();
	virtual void	MainLoop();
	SDL_DisplayMode const& GetDisplayMode()		{ return m_DisplayMode;		}

protected:
	SDL_Window*				m_MainWindow; 
	SDL_DisplayMode			m_DisplayMode;
	SDL_GLContext			m_GLContext; 

	Array<BaseManager*>		m_Managers;

	virtual void	InitManagers();
	virtual void	DestroyManagers();
	virtual void	DeclareComponentsAndEntities();
};

extern BIGBALL_API Engine* g_pEngine;

} /* namespace bigball */

#endif  // BB_ENGINE_H