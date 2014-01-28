

#include "../bigball.h"
#include "controller.h"
#include "camera.h"

namespace bigball
{

STATIC_MANAGER_CPP( Controller )

Controller::Controller()
{
	m_pStaticInstance = this;
}

Controller::~Controller()
{
	m_pStaticInstance = nullptr;
}

void Controller::Create()
{
	// Create set of camera behaviors
	CameraBehavior_Fly* pCBH = new CameraBehavior_Fly();
	m_CamBehaviors.push_back( pCBH );
}
void Controller::Destroy()
{
	for( int32 i = 0; i < m_CamBehaviors.size(); ++i )
		BB_DELETE( m_CamBehaviors[i] );
	m_CamBehaviors.clear();
}

void Controller::Tick( float DeltaSeconds )
{

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

}