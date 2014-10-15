

#include "../bigball.h"
#include "uimanager.h"
#include "../gfx/rendercontext.h"
#include "../gfx/bufferlock.h"

// Needed for loading png
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

//
//struct UIId
//{
//	uint32 Owner;
//	uint32 Item;
//	uint32 Index;
//};
//
//struct UIContext
//{
//	UIId Hovered;		// about to be interacting (mouse hovering, highlight etc.)
//	UIId Active;	// actually interacting
//
//	//ActiveContext;	// specific data for widget when active
//};
//
///** Page, menu etc. (important for draw order) */
//class UILayer
//{
//
//};
//
///** Reurns true if the button was clicked */
//bool DoButton( UIContext& Ctx, UIId& ButtonID, const char* Text );
//{
//	if( Ctx.Active == ButtonID )
//	{
//		if( mousewentup )
//		{
//			if( hot)
//				result=true;
//			Setnotactive;
//		}
//	}
//	else if( Ctx.Hovered == ButtonID )
//	{
//		if( mousewentdown )
//		{
//			Setactive;
//		}
//	}
//
//	if( Inside )
//		Sethot;
//
//	// Display the button
//}
//
//void MainUpdateLoop()
//{
//	// Affichage en lua par ex
//	for( menus )
//	{
//		for( items )
//			DoItems;
//	}
//	for( popups )
//
//	for( 3delt )
//}


namespace bigball
{

UIManager* UIManager::m_pStaticInstance = NULL;

UIManager::UIManager() :
	m_DebugFontTexId(0),
	m_UIShader(nullptr),
	m_UI_VAO(0)
{
	m_pStaticInstance = this;
}

UIManager::~UIManager()
{
	m_pStaticInstance = nullptr;
}

void UIManager::Create()
{
	InitImGui();

	m_UIShader = GfxManager::GetStaticInstance()->LoadShader( "ui" );//"test" );//"block" );

	glGenVertexArrays( 1, &m_UI_VAO);
	glBindVertexArray( m_UI_VAO);

	const uint32 MaxUIVertex = 10000;
	m_UI_VBO.Init( MaxUIVertex, sizeof(UIVertex) );

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex) /*stride*/, (void*)0 /*offset*/	);
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex) /*stride*/, (void*)8 /*offset*/	);
	glVertexAttribPointer( 2, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(UIVertex) /*stride*/, (void*)16 /*offset*/	);

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}
void UIManager::Destroy()
{
	m_UIShader = nullptr;

	m_UI_VBO.Cleanup();
	glDeleteVertexArrays( 1, &m_UI_VAO );

	ImGui::Shutdown();
}

//static GLuint fontTex;
//static bool mousePressed[2] = { false, false };
//static ImVec2 mousePosScale(1.0f, 1.0f);

// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
// If text or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
// - try adjusting ImGui::GetIO().PixelCenterOffset to 0.5f or 0.375f
static void ImImpl_RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count)
{
	UIManager* pManager = UIManager::GetStaticInstance();
	pManager->RenderDrawLists( cmd_lists, cmd_lists_count );
}

void UIManager::RenderDrawLists(struct ImDrawList** const cmd_lists, int cmd_lists_count)
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
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);

	// Setup texture
	glBindTexture(GL_TEXTURE_2D, m_DebugFontTexId);
	glEnable(GL_TEXTURE_2D);

	// Setup orthographic projection matrix
	//const float width = ImGui::GetIO().DisplaySize.x;
	//const float height = ImGui::GetIO().DisplaySize.y;
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glOrtho(0.0f, width, height, 0.0f, -1.0f, +1.0f);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();


	glBindVertexArray( m_UI_VAO );



	// Render command lists
	//for (int n = 0; n < cmd_lists_count; n++)
	//{
	//	const ImDrawList* cmd_list = cmd_lists[n];
	//	const unsigned char* vtx_buffer = (const unsigned char*)cmd_list->vtx_buffer.begin();
	//	glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer));
	//	glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer+8));
	//	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)(vtx_buffer+16));

	//	int vtx_offset = 0;
	//	const ImDrawCmd* pcmd_end = cmd_list->commands.end();
	//	for (const ImDrawCmd* pcmd = cmd_list->commands.begin(); pcmd != pcmd_end; pcmd++)
	//	{
	//		glScissor((int)pcmd->clip_rect.x, (int)(height - pcmd->clip_rect.w), (int)(pcmd->clip_rect.z - pcmd->clip_rect.x), (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
	//		glDrawArrays(GL_TRIANGLES, vtx_offset, pcmd->vtx_count);
	//		vtx_offset += pcmd->vtx_count;
	//	}
	//}

	// Count the size we need to lock
	uint32 sum_vtx_count = 0;
	for (int n = 0; n < cmd_lists_count; n++)
	{
		const ImDrawList* cmd_list = cmd_lists[n];
		
		const ImDrawCmd* pcmd_end = cmd_list->commands.end();
		for (const ImDrawCmd* pcmd = cmd_list->commands.begin(); pcmd != pcmd_end; pcmd++)
		{
			sum_vtx_count += pcmd->vtx_count;
		}
	}

	BufferLockManager::GetStaticInstance()->WaitForLockedRange( m_UI_VBO.m_DestHead, sum_vtx_count*sizeof(UIVertex) ); 
	for (int i = 0; i < particleCount; ++i) 
	{ 
		const int vertexOffset = i * kVertsPerParticle; 
		const int thisDstOffset = m_UI_VBO.m_DestHead + (i * kParticleSizeBytes); 
		void* dst = (unsigned char*) m_UI_VBO.m_VertexDataPtr + thisDstOffset; 
		Memory::Memcpy(dst, &_vertices[vertexOffset], kParticleSizeBytes); 
		//DrawArrays(TRIANGLES, kStartIndex + vertexOffset, kVertsPerParticle); 

		glScissor((int)pcmd->clip_rect.x, (int)(height - pcmd->clip_rect.w), (int)(pcmd->clip_rect.z - pcmd->clip_rect.x), (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
		glDrawArrays( GL_TRIANGLES, kStartIndex + vertexOffset, kVertsPerParticle );
	} 
	BufferLockManager::GetStaticInstance()->LockRange(m_UI_VBO.m_DestHead, vertSizeBytes); 
	m_UI_VBO.m_DestHead = (m_UI_VBO.m_DestHead + vertSizeBytes) % m_UI_VBO.m_BufferSize;



	glBindVertexArray(0);

	// glFenceSync() / glClientWaitSync() ?
	//WriteGeometry( data, ... );
	//data += dataSize;

	glDisable(GL_SCISSOR_TEST);
	//glDisableClientState(GL_COLOR_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//glDisableClientState(GL_VERTEX_ARRAY);
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

void UIManager::InitImGui()
{
	int wx, wy;
	SDL_GetWindowSize( g_pEngine->GetDisplayWindow(), &wx, &wy );
	float mousePosScalex = (float)g_pEngine->GetDisplayMode().w / wx;                  // Some screens e.g. Retina display have framebuffer size != from window size, and mouse inputs are given in window/screen coordinates.
	float mousePosScaley = (float)g_pEngine->GetDisplayMode().h / wy;

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


void UIManager::Tick( TickContext& TickCtxt )
{
	// Manage inputs
	//if( m_pActiveCamCtrl && m_pActiveCam )
	//{
	//	for( int32 i = 0; i < m_FrameInputs.size(); ++i )
	//	{
	//		m_pActiveCamCtrl->OnControllerInput( m_pActiveCam, m_FrameInputs[i] );
	//	}
	//}
	//m_FrameInputs.clear();


	//UpdateRenderCamera( TickCtxt.m_DeltaSeconds );
}


//void UpdateImGui()
//{
//	ImGuiIO& io = ImGui::GetIO();
//
//	// Setup timestep
//	static double time = 0.0f;
//	const double current_time =  glfwGetTime();
//	io.DeltaTime = (float)(current_time - time);
//	time = current_time;
//
//	// Setup inputs
//	// (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
//	double mouse_x, mouse_y;
//	glfwGetCursorPos(window, &mouse_x, &mouse_y);
//	io.MousePos = ImVec2((float)mouse_x * mousePosScale.x, (float)mouse_y * mousePosScale.y);      // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
//	io.MouseDown[0] = mousePressed[0] || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != 0;  // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
//	io.MouseDown[1] = mousePressed[1] || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != 0;
//
//	// Start the frame
//	ImGui::NewFrame();
//}

void UIManager::_Render( struct RenderContext& RenderCtxt )
{
	ImGuiIO& io = ImGui::GetIO();
	//mousePressed[0] = mousePressed[1] = false;
	io.MouseWheel = 0;
	//glfwPollEvents();
	//UpdateImGui();

	io.DeltaTime = bigball::max(0.000001f, RenderCtxt.m_DeltaSeconds);


	// Setup inputs
	// (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
	//double mouse_x, mouse_y;
	//glfwGetCursorPos(window, &mouse_x, &mouse_y);
	io.MousePos = ImVec2(0,0);//(float)mouse_x * mousePosScale.x, (float)mouse_y * mousePosScale.y);      // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
	io.MouseDown[0] = 0;//mousePressed[0] || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != 0;  // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
	io.MouseDown[1] = 0;//mousePressed[1] || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != 0;

	// Start the frame
	ImGui::NewFrame();


	static bool show_test_window = true;
	static bool show_another_window = false;

	// 1. Show a simple window
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
	{
		static float f;
		ImGui::Text("Hello, world!");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		show_test_window ^= ImGui::Button("Test Window");
		show_another_window ^= ImGui::Button("Another Window");

		// Calculate and show framerate
		static float ms_per_frame[120] = { 0 };
		static int ms_per_frame_idx = 0;
		static float ms_per_frame_accum = 0.0f;
		ms_per_frame_accum -= ms_per_frame[ms_per_frame_idx];
		ms_per_frame[ms_per_frame_idx] = ImGui::GetIO().DeltaTime * 1000.0f;
		ms_per_frame_accum += ms_per_frame[ms_per_frame_idx];
		ms_per_frame_idx = (ms_per_frame_idx + 1) % 120;
		const float ms_per_frame_avg = ms_per_frame_accum / 120;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", ms_per_frame_avg, 1000.0f / ms_per_frame_avg);
	}

	// 2. Show another simple window, this time using an explicit Begin/End pair
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window, ImVec2(200,100));
		ImGui::Text("Hello");
		ImGui::End();
	}

	// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if (show_test_window)
	{
		ImGui::SetNewWindowDefaultPos(ImVec2(650, 20));        // Normally user code doesn't need/want to call this, because positions are saved in .ini file. Here we just want to make the demo initial state a bit more friendly!
		ImGui::ShowTestWindow(&show_test_window);
	}

	ImGui::Render();
}




}