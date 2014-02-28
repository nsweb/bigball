


#ifndef BB_CONTROLLER_H
#define BB_CONTROLLER_H

#include "basemanager.h"
#include "camera.h"

namespace bigball
{

//class BIGBALL_API Camera;
//class BIGBALL_API CameraCtrl_Base;

//BIGBALL_TEMPLATE template class BIGBALL_API Array<Camera*>;
//BIGBALL_TEMPLATE template class BIGBALL_API Array<CameraCtrl_Base*>;

enum eControllerInputModifier
{
	eIM_Ctrl = 0x00000001,
	eIM_Alt = 0x00000002,
	eIM_Shift = 0x00000004,
};

enum eControllerInputType
{
	eCIT_Key = 0,
	eCIT_KeyCtrl,
	eCIT_Mouse,
	eCIT_MouseCtrl,
};

struct ControllerInput
{
	vec3				m_Delta;
	eControllerInputType			m_Type;

	bool operator == ( eControllerInputType OtherType ) const		{ return m_Type == OtherType; }
};


//////////////////////////////////////////////////////////////////////////
class BIGBALL_API Controller : public BaseManager 
{
	STATIC_MANAGER_H(Controller)

public:

						Controller();
	virtual				~Controller();

	virtual void		Create();
	virtual void		Destroy();	
	virtual void		Tick( struct TickContext& TickCtxt );

	void				RegisterCameraCtrl( CameraCtrl_Base* pCamCtrl );
	void				SetActiveCameraCtrl( Name const& CamCtrlName );
	CameraCtrl_Base*	GetActiveCameraCtrl()			{ return m_pActiveCamCtrl;			}
	CameraCtrl_Base*	GetCameraCtrl( Name const& CamCtrlName );
	void				AddCamera( Camera* pCamera );
	void				RemoveCamera( Camera* pCamera );
	void				UpdateRenderCamera( float DeltaSeconds );

	void				OnInputX( uint32 ModifierFlags, float Delta );
	void				OnInputY( uint32 ModifierFlags, float Delta );
	void				OnInputZ( uint32 ModifierFlags, float Delta );
	void				OnMouseMove( uint32 ModifierFlags, vec3 Delta );
	CameraView const&	GetRenderView()					{ return m_RenderView;				}
	mat4 const&			GetRenderProjMatrix()			{ return m_RenderProjMatrix;		}

protected:
	Array<ControllerInput>		m_FrameInputs;
	Array<Camera*>				m_Cameras;
	Array<CameraCtrl_Base*>		m_CamCtrls;
	Camera*						m_pActiveCam;
	CameraCtrl_Base*			m_pActiveCamCtrl;
	CameraView					m_RenderView;
	mat4						m_RenderProjMatrix;

	void				OnInputXYZ( uint32 ModifierFlags, vec3 Delta );

};

} /* namespace bigball */

#endif // BB_CONTROLLER_H