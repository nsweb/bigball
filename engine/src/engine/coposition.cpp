

#include "../bigball.h"
#include "coposition.h"

namespace bigball
{
CLASS_EQUIP_CPP(CoPosition);

CoPosition::CoPosition() : m_transform( vec3(1.f) )
{

}

CoPosition::~CoPosition()
{

}

void CoPosition::SetPosition( vec3 Position )
{
	m_transform.SetTranslation( Position );
}

void CoPosition::SetRotation( quat Rotation )
{
	m_transform.SetRotation( Rotation );
}


} /* namespace bigball */