

#include "../bigball.h"
#include "component.h"

namespace bigball
{
CLASS_EQUIP_CPP(Component);

Component::Component() : m_Owner(nullptr)
{

}

Component::~Component()
{

}

void Component::Create( Entity* Owner, class json::Object* Proto )
{
	m_Owner = Owner;
}
void Component::Destroy()
{

}

void Component::AddToWorld()
{

}
void Component::RemoveFromWorld()
{

}

} /* namespace bigball */