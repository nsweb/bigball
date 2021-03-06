

#ifndef BB_ENGINE_H
#define BB_ENGINE_H

namespace bigball
{

class BIGBALL_API BaseManager;
//BIGBALL_TEMPLATE template class BIGBALL_API Array< BaseManager* >;

struct BIGBALL_API CommandLine
{
	void Parse( int argc, char* argv[] );
	bool IsCommand( String& cmd_type );
	bool GetTokenValue( String const& key, String& value );

	String			cmd_exec;
	Array<String>	tokens;
	Array<String>	switches;
};

struct BIGBALL_API EngineInitParams
{
	bool create_window;
	bool resizable_window;
	bool mouse_capture;
	int32 default_res_x;
	int32 default_res_y;

	EngineInitParams() :
		create_window(true),
		resizable_window(false),
		mouse_capture(true),
		default_res_x(800),
		default_res_y(600)
	{}
};

//////////////////////////////////////////////////////////////////////////
class BIGBALL_API Engine
{
public:
					Engine();
	virtual			~Engine();

	virtual bool	Init(EngineInitParams const& init_params);
	virtual void	Shutdown();
	virtual void	MainLoop();
	virtual bool	RunCommand( String const& cmd_type, Array<String> const& switches, Array<String> const& tokens );
    virtual int     HandleEvents(float delta_seconds);

	SDL_DisplayMode const&	GetDisplayMode()							{ return m_display_mode;		}
	SDL_Window*				GetDisplayWindow()							{ return m_main_window;		}
	CommandLine&			GetCommandLine()							{ return m_cmd_line;			}

protected:
	SDL_Window*				m_main_window; 
	SDL_DisplayMode			m_display_mode;
	SDL_GLContext			m_gl_context; 
	CommandLine				m_cmd_line;
	EngineInitParams		m_init_params;

	Array<BaseManager*>		m_managers;

public:
	uint32					m_frame_count;
	uint32					m_render_mode;
	bool					m_show_culling;

protected:
	virtual void	InitManagers();
	virtual void	DestroyManagers();
	virtual void	DeclareComponentsAndEntities();
	virtual void	CreateGameCameras() = 0;
	void			InitImGui();
	virtual void	ResizeWindow(int w, int h);
    virtual void    PreTickManagers( struct TickContext& tick_ctxt );
};

extern BIGBALL_API Engine* g_pEngine;

} /* namespace bigball */

#endif  // BB_ENGINE_H
