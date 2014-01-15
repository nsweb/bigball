

#include "../bigball.h"
#include "entity.h"

namespace bigball
{

Entity::Entity() : m_State(Empty)
{
	
}

Entity::~Entity()
{
	
}

void Entity::Create()
{
	ASSERT( m_State == Empty );
	m_State = Created;
}
void Entity::Destroy()
{
	ASSERT( m_State == Created );
	m_State = Empty;
}

void Entity::AddToWorld()
{
	ASSERT( m_State == Created );
	m_State = InWorld;
}
void Entity::RemoveFromWorld()
{
	ASSERT( m_State == InWorld );
	m_State = Created;
}

void Entity::Tick( float DeltaSeconds )
{

}

} /* namespace bigball */