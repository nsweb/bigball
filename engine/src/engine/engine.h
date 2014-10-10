

#ifndef BB_ENGINE_H
#define BB_ENGINE_H

namespace bigball
{

class BIGBALL_API BaseManager;
//BIGBALL_TEMPLATE template class BIGBALL_API Array< BaseManager* >;

struct BIGBALL_API CommandLine
{
	void Parse( int argc, char* argv[] );
	bool IsCommand( String& CmdType );

	Array<String> Tokens;
	Array<String> Switches;
};

//////////////////////////////////////////////////////////////////////////
class BIGBALL_API Engine
{
public:
					Engine();
	virtual			~Engine();

	virtual bool	Init( bool bCreateWindow );
	virtual void	Shutdown();
	virtual void	MainLoop();
	virtual bool	RunCommand( String const& CmdType, Array<String> const& Switches, Array<String> const& Tokens );

	SDL_DisplayMode const& GetDisplayMode()							{ return m_DisplayMode;		}
	CommandLine&	GetCommandLine()								{ return m_CmdLine;			}

protected:
	SDL_Window*				m_MainWindow; 
	SDL_DisplayMode			m_DisplayMode;
	SDL_GLContext			m_GLContext; 
	CommandLine				m_CmdLine;

	Array<BaseManager*>		m_Managers;

public:
	uint32					m_FrameCount;
	uint32					m_RenderMode;
	bool					m_bShowCulling;

	GLuint					m_DebugFontTexId;

protected:
	virtual void	InitManagers();
	virtual void	DestroyManagers();
	virtual void	DeclareComponentsAndEntities();
	virtual void	CreateGameCameras() = 0;
	void			InitImGui();
};

extern BIGBALL_API Engine* g_pEngine;

} /* namespace bigball */

#endif  // BB_ENGINE_H