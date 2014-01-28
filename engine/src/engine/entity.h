


#ifndef BB_ENTITY_H
#define BB_ENTITY_H

namespace bigball
{

class BIGBALL_API Component;
class BIGBALL_API EntityPattern;

BIGBALL_TEMPLATE template class BIGBALL_API Array<Component*>;

class BIGBALL_API Entity 
{
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

	virtual void	Create( EntityPattern* Pattern, class tinyxml2::XMLDocument* Proto = nullptr );
	virtual void	Destroy();	
	virtual void	AddToWorld();
	virtual void	RemoveFromWorld();

	virtual void	Tick( float DeltaSeconds );

	bool			IsInWorld()		{ return m_State == InWorld; }

protected:
	State				m_State;

};

} /* namespace bigball */

#endif // BB_ENTITY_H