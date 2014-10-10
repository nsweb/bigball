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
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

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

	// HACK
	UIManager* pUIManager = new UIManager();
	pUIManager->Create();
	m_Managers.push_back( pUIManager );

	// Declare available components and entities
	DeclareComponentsAndEntities();

	// Setup available cameractrl
	CreateGameCameras();
	
	return true;
}

void Engine::Shutdown()
{
	ImGui::Shutdown();

	DestroyManagers();

	/* Delete our opengl context, destroy our window, and shutdown SDL */
	SDL_GL_DeleteContext( m_GLContext );
	SDL_DestroyWindow( m_MainWindow );
	SDL_Quit();
}

//////////////////////////////////////////////////////////////////////////

	//static GLuint fontTex;
	//static bool mousePressed[2] = { false, false };
	//static ImVec2 mousePosScale(1.0f, 1.0f);

	// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
	// If text or lines are blurry when integrating ImGui in your engine:
	// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
	// - try adjusting ImGui::GetIO().PixelCenterOffset to 0.5f or 0.375f
	static void ImImpl_RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count)
	{
		if (cmd_lists_count == 0)
			return;

		// We are using the OpenGL fixed pipeline to make the example code simpler to read!
		// A probable faster way to render would be to collate all vertices from all cmd_lists into a single vertex buffer.
		// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, vertex/texcoord/color pointers.
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_SCISSOR_TEST);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		// Setup texture
		glBindTexture(GL_TEXTURE_2D, g_pEngine->m_DebugFontTexId);
		glEnable(GL_TEXTURE_2D);

		// Setup orthographic projection matrix
		const float width = ImGui::GetIO().DisplaySize.x;
		const float height = ImGui::GetIO().DisplaySize.y;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0f, width, height, 0.0f, -1.0f, +1.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Render command lists
		for (int n = 0; n < cmd_lists_count; n++)
		{
			const ImDrawList* cmd_list = cmd_lists[n];
			const unsigned char* vtx_buffer = (const unsigned char*)cmd_list->vtx_buffer.begin();
			glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer));
			glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer+8));
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)(vtx_buffer+16));

			int vtx_offset = 0;
			const ImDrawCmd* pcmd_end = cmd_list->commands.end();
			for (const ImDrawCmd* pcmd = cmd_list->commands.begin(); pcmd != pcmd_end; pcmd++)
			{
				glScissor((int)pcmd->clip_rect.x, (int)(height - pcmd->clip_rect.w), (int)(pcmd->clip_rect.z - pcmd->clip_rect.x), (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
				glDrawArrays(GL_TRIANGLES, vtx_offset, pcmd->vtx_count);
				vtx_offset += pcmd->vtx_count;
			}
		}
		glDisable(GL_SCISSOR_TEST);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	// NB: ImGui already provide OS clipboard support for Windows so this isn't needed if you are using Windows only.
	static const char* ImImpl_GetClipboardTextFn()
	{
		return "";//glfwGetClipboardString(window);
	}

	static void ImImpl_SetClipboardTextFn(const char* text)
	{
		//glfwSetClipboardString(window, text);
	}

	#ifdef _MSC_VER
	// Notify OS Input Method Editor of text input position (e.g. when using Japanese/Chinese inputs, otherwise this isn't needed)
	static void ImImpl_ImeSetInputScreenPosFn(int x, int y)
	{
		//HWND hwnd = glfwGetWin32Window(window);
		//if (HIMC himc = ImmGetContext(hwnd))
		//{
		//	COMPOSITIONFORM cf;
		//	cf.ptCurrentPos.x = x;
		//	cf.ptCurrentPos.y = y;
		//	cf.dwStyle = CFS_FORCE_POSITION;
		//	ImmSetCompositionWindow(himc, &cf);
		//}
	}
	#endif

void Engine::InitImGui()
{
	int wx, wy;
	SDL_GetWindowSize( m_MainWindow, &wx, &wy );
	float mousePosScalex = (float)m_DisplayMode.w / wx;                  // Some screens e.g. Retina display have framebuffer size != from window size, and mouse inputs are given in window/screen coordinates.
	float mousePosScaley = (float)m_DisplayMode.h / wy;

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)m_DisplayMode.w, (float)m_DisplayMode.h);  // Display size, in pixels. For clamping windows positions.
	io.DeltaTime = 1.0f/60.0f;                          // Time elapsed since last frame, in seconds (in this sample app we'll override this every frame because our timestep is variable)
	io.PixelCenterOffset = 0.0f;                        // Align OpenGL texels
	io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;// GLFW_KEY_TAB;             // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
	io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;// GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;// GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;// GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;// GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;// GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;//GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;//GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;//GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;//GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;//GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;//GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;//GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;//GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;//GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;//GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;//GLFW_KEY_Z;

	io.RenderDrawListsFn = ImImpl_RenderDrawLists;
	io.SetClipboardTextFn = ImImpl_SetClipboardTextFn;
	io.GetClipboardTextFn = ImImpl_GetClipboardTextFn;
#ifdef _MSC_VER
	io.ImeSetInputScreenPosFn = ImImpl_ImeSetInputScreenPosFn;
#endif

	// Load debug font texture

	//void* tex_data;
	//unsigned int png_size;
	////ImGui::GetDefaultFontData(NULL, NULL, &png_data, &png_size);
	//int tex_x, tex_y;//, tex_comp;

	//File FontFile;
	//if( FontFile.Open("../data/texture/courier_new_18.tga", File::Read) )
	//{
	//	TgaFileHeader TgaHead;
	//	Memory::Memzero( &TgaHead, sizeof(TgaHead) );
	//	FontFile.Serialize( &TgaHead, sizeof(TgaHead) );
	//	
	//	tex_x = TgaHead.width;
	//	tex_y = TgaHead.height;

	//}




	// Load font texture
	glGenTextures(1, &m_DebugFontTexId);
	glBindTexture(GL_TEXTURE_2D, m_DebugFontTexId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

#if 1
	// Default font (embedded in code)
	const void* png_data;
	uint32 png_size;
	ImGui::GetDefaultFontData(NULL, NULL, &png_data, &png_size);
	int32 tex_x, tex_y, tex_comp;
	void* tex_data = stbi_load_from_memory((const unsigned char*)png_data, (int)png_size, &tex_x, &tex_y, &tex_comp, 0);
	BB_ASSERT(tex_data != NULL);
#else
	// Custom font from filesystem
	io.Font = new ImBitmapFont();
	io.Font->LoadFromFile("../../extra_fonts/mplus-2m-medium_18.fnt");
	IM_ASSERT(io.Font->IsLoaded());

	int tex_x, tex_y, tex_comp;
	void* tex_data = stbi_load("../../extra_fonts/mplus-2m-medium_18.png", &tex_x, &tex_y, &tex_comp, 0);
	IM_ASSERT(tex_data != NULL);

	// Automatically find white pixel from the texture we just loaded
	// (io.FontTexUvForWhite needs to contains UV coordinates pointing to a white pixel in order to render solid objects)
	for (int tex_data_off = 0; tex_data_off < tex_x*tex_y; tex_data_off++)
		if (((unsigned int*)tex_data)[tex_data_off] == 0xffffffff)
		{
			io.FontTexUvForWhite = ImVec2((float)(tex_data_off % tex_x)/(tex_x), (float)(tex_data_off / tex_x)/(tex_y));
			break;
		}
#endif

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_x, tex_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
		stbi_image_free(tex_data);
}



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
		delete m_Managers[i];
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