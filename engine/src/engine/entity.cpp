

#include "../bigball.h"
#include "entity.h"
#include "entitymanager.h"
#include "component.h"
#include "componentmanager.h"

namespace bigball
{

CLASS_EQUIP_CPP(Entity);

Entity::Entity() : m_State(Empty)
{
	
}

Entity::~Entity()
{
	
}

void Entity::Create( EntityPattern* Pattern, class tinyxml2::XMLDocument* Proto )
{
	ASSERT( m_State == Empty );

	// Create associated components
	EntityManager* pEntityManager = EntityManager::GetStaticInstance();
	for( int i = 0; i < Pattern->m_Components.size(); ++i )
	{
		ComponentFactory* Factory = pEntityManager->FindComponentFactory( Pattern->m_Components[i] );
		if( !Factory )
		{
			BB_LOG( Entity, Error, "Could not find component <%s>", Pattern->m_Components[i].c_str() );
		}
		else
		{
			Component* NewComponent = Factory->m_CreateFunc();
			NewComponent->Create( this, Proto );

			m_Components.push_back( NewComponent );
		}
	}

	m_State = Created;
}
void Entity::Destroy()
{
	ASSERT( m_State == Created );

	for( int i = 0; i < m_Components.size(); ++i )
	{
		Component* pComponent = m_Components[i];
		pComponent->Destroy();
	}
	m_Components.clear();

	m_State = Empty;
}

void Entity::AddToWorld()
{
	ASSERT( m_State == Created );

	EntityManager* pEntityManager = EntityManager::GetStaticInstance();
	for( int i = 0; i < m_Components.size(); ++i )
	{
		ComponentFactory* Factory = pEntityManager->FindComponentFactory( m_Components[i]->GetClassName() );
		if( Factory && Factory->m_Manager )
		{
			Factory->m_Manager->AddComponentToWorld( m_Components[i] );
		}
	}

	m_State = InWorld;
}
void Entity::RemoveFromWorld()
{
	ASSERT( m_State == InWorld );

	EntityManager* pEntityManager = EntityManager::GetStaticInstance();
	for( int i = 0; i < m_Components.size(); ++i )
	{
		ComponentFactory* Factory = pEntityManager->FindComponentFactory( m_Components[i]->GetClassName() );
		if( Factory && Factory->m_Manager )
		{
			Factory->m_Manager->RemoveComponentFromWorld( m_Components[i] );
		}
	}

	m_State = Created;
}

void Entity::Tick( float DeltaSeconds )
{

}

} /* namespace bigball */