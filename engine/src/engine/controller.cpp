

#include "../bigball.h"
#include "controller.h"
#include "camera.h"
#include "engine.h"

namespace bigball
{

STATIC_MANAGER_CPP( Controller )

Controller::Controller()
{
	m_pStaticInstance = this;
	m_pActiveCamCtrl = nullptr;
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

void Controller::Tick( float DeltaSeconds )
{
	UpdateRenderCamera( DeltaSeconds );
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
	m_pActiveCamCtrl->UpdateView( m_RenderView, DeltaSeconds );

	// Compute proj matrix
	SDL_DisplayMode DisplayMode = g_pEngine->GetDisplayMode();
	m_RenderProjMatrix = mat4::perspective_fov( m_RenderView.m_fParameters[eCP_FOV], (float)DisplayMode.w, (float)DisplayMode.h, m_RenderView.m_fParameters[eCP_NEARPLANE], m_RenderView.m_fParameters[eCP_FARPLANE] );
}

}