


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

	virtual void		Create( Entity* owner, class json::Object* proto = nullptr );
	virtual void		Destroy();	
	virtual void		AddToWorld();
	virtual void		RemoveFromWorld();
	inline Entity*		GetEntity()				{ return m_owner;		}
    Component*          GetEntityComponent( Name const& component_name );

private:
	Entity*		m_owner;
};

} /* namespace bigball */

#endif // BB_COMPONENT_H