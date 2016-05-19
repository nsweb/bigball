
#ifndef BB_ENTITY_H
#define BB_ENTITY_H

namespace bigball
{

class BIGBALL_API Component;
class BIGBALL_API EntityPattern;
class Archive;

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

	Array<Component*>	m_components;

					Entity();
	virtual			~Entity();

	static Entity*	NewEntity()		{ return new Entity();	}

	virtual void	Create( EntityPattern* pattern, class json::Object* proto = nullptr, Name in_name = Name() );
	virtual void	Destroy();	
	virtual void	AddToWorld();
	virtual void	RemoveFromWorld();
	virtual void	Tick( float DeltaSeconds );
    virtual void    Serialize(Archive& file);

	bool			IsInWorld()		{ return m_state == InWorld; }
	Component*		GetComponent( Name const& component_name );
	Component*		GetCompatibleComponent( Name const& component_name );
	Name const&		GetName()		{ return m_name;			}

protected:
	EntityPattern*		m_pattern;
	Name				m_name;
	State				m_state;

};

} /* namespace bigball */

#endif // BB_ENTITY_H