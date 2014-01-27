

#include "../bigball.h"
#include "entity.h"
#include "component.h"
#include "entitymanager.h"


//DECLARE_ENTITY( PlanetId, PlanetClass, (COPLANET | CORENDER) )


namespace bigball
{

STATIC_MANAGER_CPP( EntityManager )


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
	// Destroy remaining entities (no remove from world)
	for( int i = 0; i < m_Entities.size(); ++i )
	{
		Entity* pEntity = m_Entities[i];
		pEntity->Destroy();
		delete pEntity;
	}
	m_Entities.clear();
}

void EntityManager::Tick( float DeltaSeconds )
{
	for( int i = 0; i < m_Entities.size(); ++i )
	{
		m_Entities[i]->Tick( DeltaSeconds );
	}
}

Entity* EntityManager::CreateEntity( char const* PatternName, class tinyxml2::XMLDocument* Proto )
{
	int PatternIdx = m_EntityPatterns.FindByKey( Name(PatternName) );
	if( PatternIdx == INDEX_NONE )
	{
		BB_LOG( Entity, Error, "Unknown entity pattern name <%s>", PatternName );
		return nullptr;
	}

	EntityPattern& Pattern = m_EntityPatterns[PatternIdx];
	Entity* NewEntity = Pattern.m_CreateFunc();
	NewEntity->Create( &Pattern, Proto );

	m_Entities.push_back( NewEntity );
	
	return NewEntity;
}

Entity* EntityManager::CreateEntityFromXML( char const* XMLPath )
{
	tinyxml2::XMLDocument XMLProto;
	tinyxml2::XMLError err = XMLProto.LoadFile( XMLPath );
	tinyxml2::XMLElement* RootElt = XMLProto.FirstChildElement();
	String RootName( RootElt->Name() );
	if( RootName == "entity" )
	{
		const char* PatternName = RootElt->Attribute( "pattern" );
		return CreateEntity( PatternName, &XMLProto );
	}

	BB_LOG( Entity, Error, "XML <%s> is not a valid entity prototype", XMLPath );

	return nullptr;
}

void EntityManager::AddEntityToWorld( Entity* pEntity )
{
	if( !pEntity->IsInWorld() )
	{
		pEntity->AddToWorld();
	}
}

void EntityManager::RemoveEntityFromWorld( Entity* pEntity )
{
	if( pEntity->IsInWorld() )
	{
		pEntity->RemoveFromWorld();
	}
}

void EntityManager::DestroyEntity( Entity* pEntity )
{
	pEntity->Destroy();
	m_Entities.remove( pEntity );

	delete pEntity;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void EntityManager::RegisterPattern( EntityPattern const& Pattern )
{
	ASSERT( -1 == m_EntityPatterns.FindByKey( Pattern.m_Name ) );

	m_EntityPatterns.push_back( Pattern );
}

void EntityManager::RegisterFactory( ComponentFactory& Factory )
{
	ASSERT( -1 == m_ComponentFactories.FindByKey( Factory.m_Name ) );

	m_ComponentFactories.push_back( Factory );
}

ComponentFactory* EntityManager::FindComponentFactory( Name const& ComponentName )
{
	int FactoryIdx = m_ComponentFactories.FindByKey( ComponentName );
	if( FactoryIdx != INDEX_NONE )
		return &m_ComponentFactories[FactoryIdx];

	return nullptr;
}


} /*namespace bigball*/