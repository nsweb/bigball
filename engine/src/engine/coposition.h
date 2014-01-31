


#ifndef BB_COPOSITION_H
#define BB_COPOSITION_H

#include "component.h"

namespace bigball
{

//BIGBALL_TEMPLATE class vec3;

class BIGBALL_API CoPosition : public Component 
{
	CLASS_EQUIP_H(CoPosition, Component);

public:
						CoPosition();
	virtual				~CoPosition();

	static Component*	NewComponent()		{ return new CoPosition();	}

	//virtual void	Create();
	//virtual void	Destroy();	
	//virtual void	AddToWorld();
	//virtual void	RemoveFromWorld();

protected:
	dvec3	m_Position;
	quat	m_Rotation;
};

} /* namespace bigball */

#endif // BB_COMPONENT_H