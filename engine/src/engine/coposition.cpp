

#include "../bigball.h"
#include "coposition.h"

namespace bigball
{

CoPosition::CoPosition() : m_Position(0.f), m_Rotation(1.0f)
{

}

CoPosition::~CoPosition()
{

}

bool CoPosition::IsA( Name const& ComponentName )
{
	if( ComponentName == GetComponentName() )
		return true;
	return Super::IsA( ComponentName );
}


} /* namespace bigball */