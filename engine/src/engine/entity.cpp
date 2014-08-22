

#include "../bigball.h"
#include "entity.h"
#include "entitymanager.h"
#include "component.h"
#include "componentmanager.h"

namespace bigball
{

CLASS_EQUIP_CPP(Entity);

Entity::Entity() : m_State(Empty), m_pPattern(nullptr)
{
	
}

Entity::~Entity()
{
	
}

void Entity::Create( EntityPattern* Pattern, class json::Object* Proto, Name InName )
{
	BB_ASSERT( m_State == Empty );

	m_pPattern = Pattern;
	m_Name = InName;

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
	BB_ASSERT( m_State == Created );

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
	BB_ASSERT( m_State == Created );

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
	BB_ASSERT( m_State == InWorld );

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

Component* Entity::GetComponent( Name const& ComponentName )
{
	for( int i = 0; i < m_pPattern->m_Components.size(); ++i )
	{
		if( m_pPattern->m_Components[i] == ComponentName )
		{
			BB_ASSERT( m_Components[i]->IsA( ComponentName) );
			return m_Components[i];
		}
	}
	return nullptr;
}

Component* Entity::GetCompatibleComponent( Name const& ComponentName )
{
	for( int i = 0; i < m_Components.size(); ++i )
	{
		if( m_Components[i]->IsA( ComponentName ) )
		{
			return m_Components[i];
		}
	}
	return nullptr;
}

} /* namespace bigball */