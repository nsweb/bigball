


#ifndef BB_COPOSITION_H
#define BB_COPOSITION_H

#include "component.h"

namespace bigball
{

class BIGBALL_API CoPosition : public Component 
{
private:
	typedef Component Super;

public:
						CoPosition();
	virtual				~CoPosition();

	static Component*	NewComponent()		{ return new CoPosition();	}
	static char const*	GetComponentName()	{ return "Position";		}

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