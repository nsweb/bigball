


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

	void			SetPosition( vec3 Position );
	vec3			GetPosition()		{ return m_Transform.GetTranslation(); }
	void			SetRotation( quat Rotation );
	quat			GetRotation()		{ return m_Transform.GetRotation(); }

protected:
	transform	m_Transform;
};

} /* namespace bigball */

#endif // BB_COMPONENT_H