


#ifndef BB_CONTROLLER_H
#define BB_CONTROLLER_H

#include "basemanager.h"


namespace bigball
{

class BIGBALL_API Camera;
class BIGBALL_API CameraBehavior_Base;

BIGBALL_TEMPLATE template class BIGBALL_API Array<Camera*>;
BIGBALL_TEMPLATE template class BIGBALL_API Array<CameraBehavior_Base*>;

class BIGBALL_API Controller : public BaseManager 
{
	STATIC_MANAGER_H(Controller)

public:
						Controller();
	virtual				~Controller();

	virtual void		Create();
	virtual void		Destroy();	
	virtual void		Tick( float DeltaSeconds );

	void				AddCamera( Camera* pCamera );
	void				RemoveCamera( Camera* pCamera );

protected:

	Array<Camera*>				m_Cameras;
	Array<CameraBehavior_Base*>	m_CamBehaviors;
};

} /* namespace bigball */

#endif // BB_CONTROLLER_H