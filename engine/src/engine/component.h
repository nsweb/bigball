


#ifndef BB_COMPONENT_H
#define BB_COMPONENT_H

namespace bigball
{
namespace json
{
	class Object;
};

class BIGBALL_API Entity;

class BIGBALL_API Component 
{
	CLASS_EQUIP_BASE_H(Component)

public:
						Component();
	virtual				~Component();

	virtual void		Create( Entity* Owner, class json::Object* Proto = nullptr );
	virtual void		Destroy();	
	virtual void		AddToWorld();
	virtual void		RemoveFromWorld();
	inline Entity*		GetEntity()				{ return m_Owner;		}

private:
	Entity*		m_Owner;
};

} /* namespace bigball */

#endif // BB_COMPONENT_H