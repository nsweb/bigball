


#ifndef BB_COMPONENT_H
#define BB_COMPONENT_H

namespace bigball
{

class BIGBALL_API Component 
{
public:
					Component();
	virtual			~Component();

	virtual void	Create();
	virtual void	Destroy();	
	virtual void	AddToWorld();
	virtual void	RemoveFromWorld();

protected:

};

} /* namespace bigball */

#endif // BB_COMPONENT_H