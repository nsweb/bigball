

#include "../bigball.h"
#include "entity.h"
#include "entitymanager.h"


//DECLARE_ENTITY( PlanetId, PlanetClass, (COPLANET | CORENDER) )


namespace bigball
{

STATIC_MANAGER_CPP( EntityManager )

Array<ComponentCreateFunc>	EntityManager::m_ComponentFactories;
Array<EntityCreateFunc>		EntityManager::m_EntityFactories;
Array<uint64>				EntityManager::m_EntityComponentPatterns;

EntityManager::EntityManager()
{
	m_pStaticInstance = this;
}

EntityManager::~EntityManager()
{
	m_pStaticInstance = nullptr;
}

void EntityManager::Create()
{

}
void EntityManager::Destroy()
{

}

void EntityManager::Tick( float DeltaSeconds )
{

}

Entity* EntityManager::CreateEntity( uint64 EntityId )
{
	Entity* pEntity = nullptr;
	uint64 ComponentPattern = 0;

	// Create entity
	for( int i = 0; i < m_EntityFactories.size(); ++i )
	{
		if( EntityId & ((uint64)1 << i) )
		{
			pEntity = m_EntityFactories[i]();
			ComponentPattern = m_EntityComponentPatterns[i];
		}
	}

	ASSERT( pEntity );

	// Create components
	for( int i = 0; i < m_ComponentFactories.size(); ++i )
	{
		if( ComponentPattern & ((uint64)1 << i) )
		{
			Component* pComponent = m_ComponentFactories[i]();
			pEntity->Components;
		}
	}


	//Entity* pEntity = new Entity();
	pEntity->Create();

	m_Entities.push_back( pEntity );

	return pEntity;
}

void EntityManager::AddEntityToWorld( Entity* pEntity )
{

}

void EntityManager::RemoveEntityFromWorld( Entity* pEntity )
{

}

void EntityManager::DestroyEntity( Entity* pEntity )
{
	pEntity->Destroy();
	m_Entities.remove( pEntity );

	delete pEntity;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void EntityManager::RegisterComponent( uint64 ComponentId, ComponentCreateFunc ComponentFactory )
{
	ASSERT( ComponentId > 0 );

	for( int32 i = 0; i < 64; ++i )
	{
		if( ComponentId & ((uint64)1 << i) )
		{
			ASSERT( m_ComponentFactories[i] == nullptr );
			m_ComponentFactories[i] = ComponentFactory;
			return;
		}
	}

	ASSERT( 0 );	// could not register component
}
void EntityManager::RegisterEntity( uint64 EntityId, EntityCreateFunc EntityFactory, uint64 ComponentIds )
{
	ASSERT( EntityId > 0 );

	for( int32 i = 0; i < 64; ++i )
	{
		if( EntityId & ((uint64)1 << i) )
		{
			ASSERT( m_EntityFactories[i] == nullptr );
			m_EntityFactories[i] = EntityFactory;
			m_EntityComponentPatterns[i] = ComponentIds;
			return;
		}
	}

	ASSERT( 0 );	// could not register component
}


} /*namespace bigball*/