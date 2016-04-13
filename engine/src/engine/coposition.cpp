

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

void CoPosition::SetPosition( vec3 position )
{
	m_transform.SetTranslation( position );
}

void CoPosition::SetRotation( quat rotation )
{
	m_transform.SetRotation( rotation );
}


} /* namespace bigball */