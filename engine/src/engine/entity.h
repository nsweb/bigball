


#ifndef BB_ENTITY_H
#define BB_ENTITY_H

namespace bigball
{

class BIGBALL_API Component;


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

	virtual void	Create();
	virtual void	Destroy();	
	virtual void	AddToWorld();
	virtual void	RemoveFromWorld();

	virtual void	Tick( float DeltaSeconds );

protected:
	State				m_State;

};

} /* namespace bigball */

#endif // BB_ENTITY_H