


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


class BIGBALL_API Controller : public BaseManager 
{
	STATIC_MANAGER_H(Controller)

public:

	enum eInputModifier
	{
		eIM_Ctrl = 0x00000001,
		eIM_Alt = 0x00000002,
		eIM_Shift = 0x00000004,
	};

						Controller();
	virtual				~Controller();

	virtual void		Create();
	virtual void		Destroy();	
	virtual void		Tick( float DeltaSeconds );

	void				AddCamera( Camera* pCamera );
	void				RemoveCamera( Camera* pCamera );
	void				UpdateRenderCamera( float DeltaSeconds );

	void				OnInputX( uint32 ModifierFlags, float Delta );
	void				OnInputY( uint32 ModifierFlags, float Delta );
	void				OnMouseMove( uint32 ModifierFlags, vec2 Delta );
	CameraView const&	GetRenderView()					{ return m_RenderView;				}
	mat4 const&			GetRenderProjMatrix()			{ return m_RenderProjMatrix;		}

protected:

	Array<Camera*>				m_Cameras;
	Array<CameraCtrl_Base*>		m_CamCtrls;
	CameraCtrl_Base*			m_pActiveCamCtrl;
	CameraView					m_RenderView;
	mat4						m_RenderProjMatrix;

};

} /* namespace bigball */

#endif // BB_CONTROLLER_H