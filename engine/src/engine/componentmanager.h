


#ifndef BB_COMPONENTMANAGER_H
#define BB_COMPONENTMANAGER_H

#include "basemanager.h"

namespace bigball
{

class BIGBALL_API Component;

class BIGBALL_API ComponentManager : public BaseManager 
{

public:
					ComponentManager();
	virtual			~ComponentManager();

	virtual void	AddComponentToWorld( Component* component );
	virtual void	RemoveComponentFromWorld( Component* component );


protected:

};

} /* namespace bigball */

#endif // BB_COMPONENTMANAGER_H