

#include "../bigball.h"
#include "component.h"

namespace bigball
{

Component::Component() : m_Owner(nullptr)
{

}

Component::~Component()
{

}

void Component::Create( Entity* Owner, class tinyxml2::XMLDocument* Proto )
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