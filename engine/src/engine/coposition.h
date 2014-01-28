


#ifndef BB_COPOSITION_H
#define BB_COPOSITION_H

#include "component.h"

namespace bigball
{

//BIGBALL_TEMPLATE class vec3;

class BIGBALL_API CoPosition : public Component 
{
private:
	typedef Component Super;

public:
						CoPosition();
	virtual				~CoPosition();

	static Component*	NewComponent()		{ return new CoPosition();	}
	static char const*	GetStaticName()		{ return "Position";		}
	virtual char const*	GetComponentName()	{ return GetStaticName();	}
	virtual bool		IsA( Name const& ComponentName );
	//virtual void	Create();
	//virtual void	Destroy();	
	//virtual void	AddToWorld();
	//virtual void	RemoveFromWorld();

protected:
	vec3	m_Position;
	quat	m_Rotation;
};

} /* namespace bigball */

#endif // BB_COMPONENT_H