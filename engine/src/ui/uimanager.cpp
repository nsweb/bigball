

#include "../bigball.h"
#include "uimanager.h"
#include "../engine/engine.h"
#include "../gfx/gfxmanager.h"
#include "../gfx/shader.h"
#include "../gfx/rendercontext.h"
//#include "../gfx/bufferlock.h"
#include "../system/profiler.h"

// Needed for loading png
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace bigball
{

UIManager* UIManager::m_pStaticInstance = NULL;

UIManager::UIManager() :
	m_DebugFontTexId(0),
	m_UIShader(nullptr),
	m_UI_VAO(0),
	m_bShowDebugMenu(false),
	m_bShowProfiler(false)
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

	m_UIShader = GfxManager::GetStaticInstance()->LoadShader( "ui" );

	glGenVertexArrays( 1, &m_UI_VAO);
	glBindVertexArray( m_UI_VAO);

	glGenBuffers(1, &m_UI_VBO);
	glGenBuffers(1, &m_UI_EBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_UI_VBO);
	//const uint32 MaxUIVertex = 10000;
	//m_UI_VBO.Init( MaxUIVertex, sizeof(UIVertex) );
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex) /*stride*/, (void*)0 /*offset*/	);
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex) /*stride*/, (void*)8 /*offset*/	);
	glVertexAttribPointer( 2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(UIVertex) /*stride*/, (void*)16 /*offset*/	);

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}
void UIManager::Destroy()
{
	m_UIShader = nullptr;

	//m_UI_VBO.Cleanup();
	glDeleteBuffers( 1, &m_UI_VBO );
	glDeleteBuffers( 1, &m_UI_EBO );

	glDeleteVertexArrays( 1, &m_UI_VAO );

	ImGui::Shutdown();
}

// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
static void ImImpl_RenderDrawLists(ImDrawData* draw_data)
{
	UIManager* pManager = UIManager::GetStaticInstance();
	pManager->RenderDrawLists( draw_data );
}

void UIManager::RenderDrawLists(ImDrawData* draw_data)
{
	PROFILE_SCOPE( __FUNCTION__ );

	if( draw_data->CmdListsCount == 0 )
		return;	

#if 0
	// Backup GL state
	GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
	GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
	GLint last_blend_src; glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
	GLint last_blend_dst; glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
	GLint last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
	GLint last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
	GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
	GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
#endif

	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glActiveTexture(GL_TEXTURE0);


	// Handle cases of screen coordinates != from framebuffer coordinates (e.g. retina displays)
	ImGuiIO& io = ImGui::GetIO();
	float fb_height = io.DisplaySize.y * io.DisplayFramebufferScale.y;
	draw_data->ScaleClipRects(io.DisplayFramebufferScale);

	// Setup orthographic projection matrix
	//glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
	const float width = ImGui::GetIO().DisplaySize.x;
	const float height = ImGui::GetIO().DisplaySize.y;
	mat4 UIProjMatrix = mat4::ortho( 0.f, width, height, 0.f, 0.f, 1.f );

	m_UIShader->Bind();
	ShaderUniform UniProj = m_UIShader->GetUniformLocation("proj_mat");
	ShaderUniform UniSampler0 =  m_UIShader->GetUniformLocation("textureUnit0");
	m_UIShader->SetUniform( UniSampler0, 0 );
	m_UIShader->SetUniform( UniProj, UIProjMatrix );

	glBindVertexArray( m_UI_VAO );

	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawIdx* idx_buffer_offset = 0;

		glBindBuffer(GL_ARRAY_BUFFER, m_UI_VBO);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_UI_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);

		for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
		{
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
				glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
			}
			idx_buffer_offset += pcmd->ElemCount;
		}
	}

#if 0
	// Restore modified GL state
	glUseProgram(last_program);
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
	glBindVertexArray(last_vertex_array);
	glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
	glBlendFunc(last_blend_src, last_blend_dst);
	if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
#endif

	glDisable(GL_SCISSOR_TEST);

#if 0

	// We are using the OpenGL fixed pipeline to make the example code simpler to read!
	// A probable faster way to render would be to collate all vertices from all cmd_lists into a single vertex buffer.
	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, vertex/texcoord/color pointers.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	//glEnable(GL_TEXTURE_2D);

	// Setup orthographic projection matrix
	const float width = ImGui::GetIO().DisplaySize.x;
	const float height = ImGui::GetIO().DisplaySize.y;
	mat4 UIProjMatrix = mat4::ortho( 0.f, width, height, 0.f, 0.f, 1.f );

	m_UIShader->Bind();

	ShaderUniform UniProj = m_UIShader->GetUniformLocation("proj_mat");
	ShaderUniform UniSampler0 =  m_UIShader->GetUniformLocation("textureUnit0");
	m_UIShader->SetUniform( UniSampler0, 0 );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_DebugFontTexId);

	m_UIShader->SetUniform( UniProj, UIProjMatrix );

	glBindVertexArray( glBindVertexArray( m_UI_VAO ); );
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);


	if(1)
	{
		// Count the size we need to lock
		uint32 sum_vtx_count = 0;
		for (int n = 0; n < data->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = data->CmdLists[n];

			const ImDrawCmd* pcmd_end = cmd_list->CmdBuffer.end();
			for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != pcmd_end; pcmd++)
			{
				sum_vtx_count += pcmd->ElemCount;
			}
		}

		uint32 StartIndex = m_UI_VBO.m_DestHead / sizeof(UIVertex);

		glBindBuffer( GL_ARRAY_BUFFER, m_UI_VBO.m_VB_ID );

		//--m_UI_VBO.m_LockManager.WaitForLockedRange( m_UI_VBO.m_DestHead, sum_vtx_count*sizeof(UIVertex) ); 

		uint32 vbo_byte_offset = 0;
		uint32 vbo_vtx_offset = 0;
		for (int n = 0; n < data->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = data->CmdLists[n];
			const uint8* vtx_buffer = (const uint8*)cmd_list->VtxBuffer.begin();
			uint32 byte_offset = 0;
			uint32 vtx_offset = 0;

			const ImDrawCmd* pcmd_end = cmd_list->CmdBuffer.end();
			for( const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != pcmd_end; pcmd++ )
			{
				void* Dst = glMapBufferRange( GL_ARRAY_BUFFER, vbo_byte_offset, pcmd->ElemCount * sizeof(UIVertex), GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_WRITE_BIT );
				Memory::Memcpy( Dst, vtx_buffer + byte_offset, pcmd->ElemCount * sizeof(UIVertex) );
				glUnmapBuffer( GL_ARRAY_BUFFER );

				//--Memory::Memcpy( m_UI_VBO.m_VertexDataPtr + vbo_byte_offset, vtx_buffer + byte_offset, pcmd->vtx_count * sizeof(UIVertex) );
				//glScissor((int)pcmd->clip_rect.x, (int)(height - pcmd->clip_rect.w), (int)(pcmd->clip_rect.z - pcmd->clip_rect.x), (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
				glDrawArrays( GL_TRIANGLES, StartIndex + vbo_vtx_offset, pcmd->ElemCount );

				vtx_offset += pcmd->ElemCount;
				vbo_vtx_offset += pcmd->ElemCount;
				byte_offset += pcmd->ElemCount * sizeof(UIVertex); 
				vbo_byte_offset += pcmd->ElemCount * sizeof(UIVertex); 
			}
		}
	}


	glBindVertexArray(0);
#endif

	// glFenceSync() / glClientWaitSync() ?
	//WriteGeometry( data, ... );
	//data += dataSize;


	//glDisableClientState(GL_COLOR_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//glDisableClientState(GL_VERTEX_ARRAY);

	m_UIShader->Unbind();
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
	//float mousePosScalex = (float)g_pEngine->GetDisplayMode().w / wx;                  // Some screens e.g. Retina display have framebuffer size != from window size, and mouse inputs are given in window/screen coordinates.
	//float mousePosScaley = (float)g_pEngine->GetDisplayMode().h / wy;

	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = "../data/imgui.ini";
	io.LogFilename = "../data/log/imgui.log";
	io.DisplaySize = ImVec2((float)g_pEngine->GetDisplayMode().w, (float)g_pEngine->GetDisplayMode().h);  // Display size, in pixels. For clamping windows positions.
	io.DeltaTime = 1.0f/60.0f;                          // Time elapsed since last frame, in seconds (in this sample app we'll override this every frame because our timestep is variable)
	//io.PixelCenterOffset = 0.0f;                        // Align OpenGL texels
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

	// Load font texture
	glGenTextures(1, &m_DebugFontTexId);
	glBindTexture(GL_TEXTURE_2D, m_DebugFontTexId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Build texture atlas
	uint8* pixels;
	int width, height, bytes_per_pixel;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);
	//const int data_size = width * bytes_per_pixel * height;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Store our identifier
	io.Fonts->TexID = (void *)(intptr_t)m_DebugFontTexId;

	// Cleanup (don't clear the input data if you want to append new fonts later)
	io.Fonts->ClearInputData();
	io.Fonts->ClearTexData();
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
	int MouseX, MouseY;
	uint32 MouseState = SDL_GetMouseState( &MouseX, &MouseY );

	io.MousePos = ImVec2((float)MouseX,(float)MouseY);//(float)mouse_x * mousePosScale.x, (float)mouse_y * mousePosScale.y);      // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
	io.MouseDown[0] = MouseState & SDL_BUTTON_LMASK ? true : false;//mousePressed[0] || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != 0;  // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
	io.MouseDown[1] = MouseState & SDL_BUTTON_RMASK ? true : false;//mousePressed[1] || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != 0;

	// Start the frame
	ImGui::NewFrame();

	if( m_bShowDebugMenu )
		DrawDebugMenu();

	if( m_bShowProfiler )
		DrawProfiler();
	//static bool show_test_window = false;//true;
	//static bool show_another_window = true;

	//// 1. Show a simple window
	//// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
	//{
	//	static float f;
	//	ImGui::Text("Hello, world!");
	//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
	//	show_test_window ^= ImGui::Button("Test Window");
	//	show_another_window ^= ImGui::Button("Another Window");

	//	// Calculate and show framerate
	//	static float ms_per_frame[120] = { 0 };
	//	static int ms_per_frame_idx = 0;
	//	static float ms_per_frame_accum = 0.0f;
	//	ms_per_frame_accum -= ms_per_frame[ms_per_frame_idx];
	//	ms_per_frame[ms_per_frame_idx] = ImGui::GetIO().DeltaTime * 1000.0f;
	//	ms_per_frame_accum += ms_per_frame[ms_per_frame_idx];
	//	ms_per_frame_idx = (ms_per_frame_idx + 1) % 120;
	//	const float ms_per_frame_avg = ms_per_frame_accum / 120;
	//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", ms_per_frame_avg, 1000.0f / ms_per_frame_avg);
	//}

	//// 2. Show another simple window, this time using an explicit Begin/End pair
	//if (show_another_window)
	//{
	//	ImGui::Begin("Another Window", &show_another_window, ImVec2(200,100));
	//	ImGui::Text("Hello");
	//	ImGui::End();
	//}

	//// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	//if (show_test_window)
	//{
	//	ImGui::SetNewWindowDefaultPos(ImVec2(650, 20));        // Normally user code doesn't need/want to call this, because positions are saved in .ini file. Here we just want to make the demo initial state a bit more friendly!
	//	ImGui::ShowTestWindow(&show_test_window);
	//}

	ImGui::Render();
}

void UIManager::DrawDebugMenu()
{
	bool bShowDebugMenu = m_bShowDebugMenu;

	ImGui::Begin("Debug menu", &bShowDebugMenu, ImVec2(10,10));
	ImGui::Text("Hello");
	if (ImGui::CollapsingHeader("Profiling"))
	{
		ImGui::Checkbox("enable profiling", &m_bShowProfiler);
	}
	if (ImGui::CollapsingHeader("Style Editor"))
	{
		ImGui::ShowStyleEditor();
	}

	if (ImGui::CollapsingHeader("Logging"))
	{
		ImGui::LogButtons();
	}
	ImGui::End();

	// Release mouse if window was closed
	if( bShowDebugMenu != m_bShowDebugMenu )
		ToggleDebugMenu();
}

void UIManager::DrawProfiler()
{
	ImGui::Begin("Profiler", &m_bShowDebugMenu, ImVec2(200,400), -1.f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar );
	Profiler::BuildGui();	
	ImGui::End();
}

void UIManager::ToggleDebugMenu()
{
	bool bNewShowDebugMenu = !m_bShowDebugMenu;

	if( bNewShowDebugMenu )
	{
		// Allow menu interaction
		SDL_SetRelativeMouseMode(SDL_FALSE);
		SDL_SetWindowGrab( g_pEngine->GetDisplayWindow(), SDL_FALSE );
	}
	else
	{
		// Allow FPS style mouse movement
		SDL_SetRelativeMouseMode(SDL_TRUE);
		SDL_SetWindowGrab( g_pEngine->GetDisplayWindow(), SDL_TRUE );
	}

	m_bShowDebugMenu = bNewShowDebugMenu;
}

} /* namespace bigball */