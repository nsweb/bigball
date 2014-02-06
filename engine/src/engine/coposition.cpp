

#include "../bigball.h"
#include "coposition.h"

namespace bigball
{
CLASS_EQUIP_CPP(CoPosition);

CoPosition::CoPosition() : m_Transform( vec3(1.f) )
{

}

CoPosition::~CoPosition()
{

}

void CoPosition::SetPosition( vec3 Position )
{
	m_Transform.SetTranslation( Position );
}

void CoPosition::SetRotation( quat Rotation )
{
	m_Transform.SetRotation( Rotation );
}


} /* namespace bigball */