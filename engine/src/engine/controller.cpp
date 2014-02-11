

#include "../bigball.h"
#include "controller.h"
#include "camera.h"
#include "engine.h"
#include "tickcontext.h"

namespace bigball
{

STATIC_MANAGER_CPP( Controller )

Controller::Controller()
{
	m_pStaticInstance = this;
	m_pActiveCamCtrl = nullptr;
	m_pActiveCam = nullptr;
}

Controller::~Controller()
{
	m_pStaticInstance = nullptr;
}

void Controller::Create()
{
	// Create set of camera Ctrls
	CameraCtrl_Fly* pCBH = new CameraCtrl_Fly();
	m_CamCtrls.push_back( pCBH );
	m_pActiveCamCtrl = pCBH;
}
void Controller::Destroy()
{
	for( int32 i = 0; i < m_CamCtrls.size(); ++i )
		BB_DELETE( m_CamCtrls[i] );
	m_CamCtrls.clear();
}

void Controller::Tick( TickContext& TickCtxt )
{
	// Manage inputs
	if( m_pActiveCamCtrl && m_pActiveCam )
	{
		for( int32 i = 0; i < m_FrameInputs.size(); ++i )
		{
			m_pActiveCamCtrl->OnControllerInput( m_pActiveCam, m_FrameInputs[i] );
		}
	}
	m_FrameInputs.clear();


	UpdateRenderCamera( TickCtxt.m_DeltaSeconds );
}

void Controller::AddCamera( Camera* pCamera )
{
	if( INDEX_NONE == m_Cameras.find( pCamera ) )
		m_Cameras.push_back( pCamera );
}

void Controller::RemoveCamera( Camera* pCamera )
{
	int CameraIdx = m_Cameras.find( pCamera );
	if( INDEX_NONE != CameraIdx )
		m_Cameras.remove( pCamera );
}

void Controller::UpdateRenderCamera( float DeltaSeconds )
{
	if( !m_pActiveCam && m_Cameras.size() )
	{
		m_pActiveCam = m_Cameras[0];
		//m_pActiveCamCtrl->InitFromView( m_pActiveCam->GetView() );	
	}

	// TEMP
	if( m_pActiveCam )
		m_RenderView = m_pActiveCam->GetView();

	m_pActiveCamCtrl->UpdateView( m_RenderView, DeltaSeconds );

	// Compute proj matrix
	SDL_DisplayMode DisplayMode = g_pEngine->GetDisplayMode();
	m_RenderView.m_fParameters[eCP_ASPECTRATIO] = (float)DisplayMode.w / (float)DisplayMode.h;
	m_RenderProjMatrix = mat4::perspective_fov( m_RenderView.m_fParameters[eCP_FOV], (float)DisplayMode.w, (float)DisplayMode.h, m_RenderView.m_fParameters[eCP_NEARPLANE], m_RenderView.m_fParameters[eCP_FARPLANE] );
}

void Controller::OnInputX( uint32 ModifierFlags, float Delta )
{
	OnInputXYZ( ModifierFlags, vec3( Delta, 0.f, 0.f ) );
}

void Controller::OnInputY( uint32 ModifierFlags, float Delta )
{
	OnInputXYZ( ModifierFlags, vec3( 0.f, Delta, 0.f ) );
}

void Controller::OnInputZ( uint32 ModifierFlags, float Delta )
{
	OnInputXYZ( ModifierFlags, vec3( 0.f, 0.f, Delta ) );
}

void Controller::OnInputXYZ( uint32 ModifierFlags, vec3 Delta )
{
	eControllerInputType InputType = (ModifierFlags & eIM_Ctrl ? eCIT_KeyCtrl : eCIT_Key);
	int Idx = m_FrameInputs.FindByKey( InputType );
	if( Idx == INDEX_NONE )
	{
		ControllerInput Input;
		Input.m_Delta = Delta;
		Input.m_Type = InputType;
		m_FrameInputs.push_back( Input );
	}
	else
	{
		ControllerInput& Input = m_FrameInputs[Idx];
		Input.m_Delta += Delta;
	}
}

void Controller::OnMouseMove( uint32 ModifierFlags, vec3 Delta )
{
	eControllerInputType InputType = (ModifierFlags & eIM_Ctrl ? eCIT_MouseCtrl : eCIT_Mouse);
	//BB_LOG( Inputs, Log, "----MouseDelta x=%f y=%f CTRL=%d", Delta.x, Delta.y, ModifierFlags & KMOD_CTRL ? 1 : 0 );
	int Idx = m_FrameInputs.FindByKey( InputType );
	if( Idx == INDEX_NONE )
	{
		ControllerInput Input;
		Input.m_Delta = Delta;
		Input.m_Type = InputType;
		m_FrameInputs.push_back( Input );
	}
	else
	{
		ControllerInput& Input = m_FrameInputs[Idx];
		Input.m_Delta += Delta;
	}
}

}