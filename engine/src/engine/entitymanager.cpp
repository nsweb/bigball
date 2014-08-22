

#include "../bigball.h"
#include "entity.h"
#include "component.h"
#include "entitymanager.h"
#include "tickcontext.h"
#include "../core/json.h"

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

void EntityManager::Tick( TickContext& TickCtxt )
{
	for( int i = 0; i < m_Entities.size(); ++i )
	{
		m_Entities[i]->Tick( TickCtxt.m_DeltaSeconds );
	}
}

Entity* EntityManager::CreateEntity( char const* PatternName, json::Object* Proto, Name InName )
{
	int PatternIdx = m_EntityPatterns.FindByKey( Name(PatternName) );
	if( PatternIdx == INDEX_NONE )
	{
		BB_LOG( Entity, Error, "Unknown entity pattern name <%s>", PatternName );
		return nullptr;
	}

	EntityPattern& Pattern = m_EntityPatterns[PatternIdx];
	Entity* NewEntity = Pattern.m_CreateFunc();
	NewEntity->Create( &Pattern, Proto, InName );

	m_Entities.push_back( NewEntity );
	
	return NewEntity;
}

Entity* EntityManager::CreateEntityFromJson( char const* JsonPath, Name InName )
{
	json::Object jsnObj;
	if( jsnObj.ParseFile( JsonPath ) )
	{
		json::TokenIdx EntTok = jsnObj.GetToken( "entity", json::OBJECT );
		json::TokenIdx PatTok = jsnObj.GetToken( "pattern", json::STRING, EntTok );
		String strPattern;
		jsnObj.GetStringValue( PatTok, strPattern );
		return CreateEntity( strPattern.c_str(), &jsnObj, InName );
	}

	//tinyxml2::XMLDocument XMLProto;
	//tinyxml2::XMLError err = XMLProto.LoadFile( XMLPath );
	//tinyxml2::XMLElement* RootElt = XMLProto.FirstChildElement();
	//String RootName( RootElt->Name() );
	//if( RootName == "entity" )
	//{
	//	const char* PatternName = RootElt->Attribute( "pattern" );
	//	return CreateEntity( PatternName, &XMLProto );
	//}

	BB_LOG( Entity, Error, "JSON file <%s> is not a valid entity prototype", JsonPath );

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
	BB_ASSERT( -1 == m_EntityPatterns.FindByKey( Pattern.m_Name ) );

	m_EntityPatterns.push_back( Pattern );
}

void EntityManager::RegisterFactory( ComponentFactory& Factory )
{
	BB_ASSERT( -1 == m_ComponentFactories.FindByKey( Factory.m_Name ) );

	m_ComponentFactories.push_back( Factory );
}

ComponentFactory* EntityManager::FindComponentFactory( Name const& ComponentName )
{
	int FactoryIdx = m_ComponentFactories.FindByKey( ComponentName );
	if( FactoryIdx != INDEX_NONE )
		return &m_ComponentFactories[FactoryIdx];

	return nullptr;
}

Entity* EntityManager::FindEntityByName( Name InName )
{
	for( int i = 0; i < m_Entities.size(); ++i )
	{
		Entity* pEntity = m_Entities[i];
		if( pEntity->GetName() == InName )
			return pEntity;
	}
	return nullptr;
}


} /*namespace bigball*/