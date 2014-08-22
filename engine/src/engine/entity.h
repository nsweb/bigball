


#ifndef BB_ENTITY_H
#define BB_ENTITY_H

namespace bigball
{

class BIGBALL_API Component;
class BIGBALL_API EntityPattern;

//BIGBALL_TEMPLATE template class BIGBALL_API Array<Component*>;

namespace json
{
	class Object;
};

class BIGBALL_API Entity 
{
	CLASS_EQUIP_BASE_H(Entity)

public:

	enum State
	{
		Empty,
		Created,
		InWorld,
	};

	Array<Component*>	m_Components;

					Entity();
	virtual			~Entity();

	static Entity*	NewEntity()		{ return new Entity();	}

	virtual void	Create( EntityPattern* Pattern, class json::Object* Proto = nullptr, Name InName = Name() );
	virtual void	Destroy();	
	virtual void	AddToWorld();
	virtual void	RemoveFromWorld();

	virtual void	Tick( float DeltaSeconds );

	bool			IsInWorld()		{ return m_State == InWorld; }
	Component*		GetComponent( Name const& ComponentName );
	Component*		GetCompatibleComponent( Name const& ComponentName );
	Name const&		GetName()		{ return m_Name;			}

protected:
	EntityPattern*		m_pPattern;
	Name				m_Name;
	State				m_State;

};

} /* namespace bigball */

#endif // BB_ENTITY_H