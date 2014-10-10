

#include "../bigball.h"
#include "uimanager.h"
//#include "camera.h"
//#include "engine.h"
#include "../gfx/rendercontext.h"

// 


namespace bigball
{

STATIC_MANAGER_CPP( UIManager )

UIManager::UIManager()
{
	m_pStaticInstance = this;
}

UIManager::~UIManager()
{
	m_pStaticInstance = nullptr;
}

void UIManager::Create()
{
	// Create set of camera Ctrls
	//CameraCtrl_Fly* pCBH = new CameraCtrl_Fly();
	//m_CamCtrls.push_back( pCBH );
	//m_pActiveCamCtrl = pCBH;
}
void UIManager::Destroy()
{
	//for( int32 i = 0; i < m_CamCtrls.size(); ++i )
	//	BB_DELETE( m_CamCtrls[i] );
	//m_CamCtrls.clear();
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