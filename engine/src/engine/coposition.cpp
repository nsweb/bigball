
#include "../bigball.h"
#include "coposition.h"
#include "../system/file.h"

namespace bigball
{
CLASS_EQUIP_CPP(CoPosition);

CoPosition::CoPosition() : m_transform( vec3(1.f) )
{

}

CoPosition::~CoPosition()
{

}

void CoPosition::SetPosition( vec3 p )
{
	m_transform.SetTranslation( p );
}

void CoPosition::SetRotation( quat r )
{
	m_transform.SetRotation( r );
}
    
void CoPosition::SetScale( float s )
{
    return m_transform.SetScale( s );
}
    
void CoPosition::SetTransform( transform const& t )
{
    m_transform = t;
}
void CoPosition::Serialize(Archive& file)
{
    file.SerializeRaw(m_transform);
}

} /* namespace bigball */