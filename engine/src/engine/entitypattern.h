


#ifndef BB_ENTITYPATTERN_H
#define BB_ENTITYPATTERN_H

namespace bigball
{

class BIGBALL_API Entity; 
class BIGBALL_API Component; 
class BIGBALL_API ComponentManager;

typedef Entity* (*EntityCreateFunc)();
typedef Component* (*ComponentCreateFunc)();

//////////////////////////////////////////////////////////////////////////
class BIGBALL_API ComponentFactory
{
public:

	ComponentFactory()		{}
	~ComponentFactory()		{}

	bool			operator== ( Name const& OtherName ) const		{ return m_Name == OtherName;	}

public:
	Name				m_Name;
	ComponentCreateFunc	m_CreateFunc;
	ComponentManager*	m_Manager;
};

//////////////////////////////////////////////////////////////////////////
class BIGBALL_API EntityPattern 
{
public:

	EntityPattern()		{}
	~EntityPattern()	{}

	void			SetComponentList( int N, ... );
	void			SetOptionalComponentList( int N, ... );

	bool			operator== ( Name const& OtherName ) const		{ return m_Name == OtherName;	}

public:
	Name				m_Name;
	EntityCreateFunc	m_CreateFunc;
	Array<Name>			m_Components;
	Array<Name>			m_OptionalComponents;
};

} /* namespace bigball */

#endif // BB_ENTITYPATTERN_H