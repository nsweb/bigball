


#ifndef BB_ENTITYMANAGER_H
#define BB_ENTITYMANAGER_H

#include "basemanager.h"

namespace bigball
{

class BIGBALL_API Entity;
class BIGBALL_API Component;

typedef Component* (*ComponentCreateFunc)();
typedef Entity* (*EntityCreateFunc)();

class BIGBALL_API EntityManager : public BaseManager 
{
	STATIC_MANAGER_H(EntityManager)

public:
					EntityManager();
	virtual			~EntityManager();

	virtual void	Create();
	virtual void	Destroy();	
	virtual void	Tick( float DeltaSeconds );

	Entity*			CreateEntity( uint64 EntityId );
	void			AddEntityToWorld( Entity* pEntity );
	void			RemoveEntityFromWorld( Entity* pEntity );
	void			DestroyEntity( Entity* pEntity );

	static void		RegisterComponent( uint64 ComponentId, ComponentCreateFunc ComponentFactory );
	static void		RegisterEntity( uint64 EntityId, EntityCreateFunc EntityFactory, uint64 ComponentIds );

protected:
	Array<Entity*>		m_Entities;

	static Array<ComponentCreateFunc>	m_ComponentFactories;
	static Array<EntityCreateFunc>		m_EntityFactories;
	static Array<uint64>				m_EntityComponentPatterns;
};

} /* namespace bigball */

#endif // BB_ENTITYMANAGER_H