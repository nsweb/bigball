


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

	void				SetPosition( vec3 p );
	vec3				GetPosition()		{ return m_transform.GetTranslation(); }
	void				SetRotation( quat r );
	quat				GetRotation()		{ return m_transform.GetRotation(); }
    void                SetScale( float s );
    float               GetScale()          { return m_transform.GetScale(); }
    void                SetTransform( transform const& t );
	const transform&	GetTransform()		{ return m_transform;	}

protected:
	transform	m_transform;
};

} /* namespace bigball */

#endif // BB_COMPONENT_H