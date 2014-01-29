


#ifndef BB_ENTITYMANAGER_H
#define BB_ENTITYMANAGER_H

#include "basemanager.h"
#include "entitypattern.h"

namespace bigball
{

class BIGBALL_API Entity;
class BIGBALL_API Component;

#define DECLARE_COMPONENT( Klass ) \
{ \
	ComponentFactory Factory; \
	Factory.m_Name = Klass::StaticClass(); \
	Factory.m_CreateFunc = Klass::NewComponent; \
	Factory.m_Manager = nullptr; \
	EntityManager::GetStaticInstance()->RegisterFactory( Factory ); \
}

#define DECLARE_COMPONENT_MGR( Klass, Manager ) \
{ \
	ComponentFactory Factory; \
	Factory.m_Name = Klass::StaticClass(); \
	Factory.m_CreateFunc = Klass::NewComponent; \
	Factory.m_Manager = Manager::GetStaticInstance(); \
	EntityManager::GetStaticInstance()->RegisterFactory( Factory ); \
}

#define DECLARE_ENTITYPATTERN( Name, Klass, CompList, OptionalCompList ) \
{ \
	EntityPattern Pattern; \
	Pattern.m_Name = #Name; \
	Pattern.m_CreateFunc = Klass::NewEntity; \
	Pattern.SetComponentList CompList; \
	Pattern.SetOptionalComponentList OptionalCompList; \
	EntityManager::GetStaticInstance()->RegisterPattern( Pattern ); \
}

//BIGBALL_TEMPLATE template class BIGBALL_API Array<Entity*>;
//BIGBALL_TEMPLATE template class BIGBALL_API Array<ComponentFactory>;
//BIGBALL_TEMPLATE template class BIGBALL_API Array<EntityPattern>;

class BIGBALL_API EntityManager : public BaseManager 
{
	STATIC_MANAGER_H(EntityManager)

public:
						EntityManager();
	virtual				~EntityManager();

	virtual void		Create();
	virtual void		Destroy();	
	virtual void		Tick( float DeltaSeconds );

	Entity*				CreateEntity( char const* PatternName, class tinyxml2::XMLDocument* Proto = nullptr );
	Entity*				CreateEntityFromXML( char const* XMLPath );

	void				AddEntityToWorld( Entity* pEntity );
	void				RemoveEntityFromWorld( Entity* pEntity );
	void				DestroyEntity( Entity* pEntity );

	void				RegisterFactory( ComponentFactory& Factory );
	void				RegisterPattern( EntityPattern const& Pattern );
	ComponentFactory*	FindComponentFactory( Name const& ComponentName );

protected:
	Array<Entity*>		m_Entities;

	Array<ComponentFactory>		m_ComponentFactories;
	Array<EntityPattern>		m_EntityPatterns;
};

} /* namespace bigball */

#endif // BB_ENTITYMANAGER_H