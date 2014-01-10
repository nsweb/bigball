


#ifndef BB_CONTROLLER_H
#define BB_CONTROLLER_H

#include "basemanager.h"


namespace bigball
{

class BIGBALL_API Controller : public BaseManager 
{
	STATIC_MANAGER_H(Controller)

public:
					Controller();
	virtual			~Controller();

	virtual void	Create();
	virtual void	Destroy();	
	virtual void	Tick( float DeltaSeconds );
};

} /* namespace bigball */

#endif // BB_CONTROLLER_H