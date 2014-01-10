


#ifndef BB_ENTITY_H
#define BB_ENTITY_H

namespace bigball
{

class BIGBALL_API Component;

class Entity 
{
public:
					Entity();
	virtual			~Entity();

	virtual void	Create();
	virtual void	Destroy();	
	virtual void	AddToWorld();
	virtual void	RemoveFromWorld();

protected:
	Array<Component*> Components;
};

} /* namespace bigball */

#endif // BB_ENTITY_H