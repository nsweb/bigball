


#ifndef BB_COMPONENT_H
#define BB_COMPONENT_H

namespace bigball
{
class BIGBALL_API Entity;

class BIGBALL_API Component 
{
public:
						Component();
	virtual				~Component();

	virtual void		Create( Entity* Owner, class tinyxml2::XMLDocument* Proto = nullptr );
	virtual void		Destroy();	
	virtual void		AddToWorld();
	virtual void		RemoveFromWorld();
	virtual char const*	GetComponentName() = 0;
	virtual bool		IsA( Name const& ComponentName )		{ return false;		}

protected:
	Entity*		m_Owner;
};

} /* namespace bigball */

#endif // BB_COMPONENT_H