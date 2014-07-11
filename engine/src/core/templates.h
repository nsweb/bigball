

#ifndef BB_TEMPLATES_H
#define BB_TEMPLATES_H

namespace bigball
{

#define UNUSED(...)
#define BB_ASSERT(...)

#define BB_FREE(p)			{ if(p) Memory::Free(p); (p)=nullptr; }
#define BB_DELETE(p)		{ if(p) delete(p); (p)=nullptr; }
#define BB_DELETE_ARRAY(p)	{ if(p) delete[](p); (p)=nullptr; }

#define COUNT_OF(_Array)	(sizeof(_Array) / sizeof(_Array[0]))
#define CACHE_LINE			32
#define CACHE_ALIGN			__declspec(align(CACHE_LINE))

template< class T > 
inline void swap( T& A, T& B )
{
	const T Tmp = A;
	A = B;
	B = Tmp;
}

#define CLASS_EQUIP_BASE_H( klass ) \
private:\
	static Name m_ClassName;\
public:\
	static	Name		StaticClass()		{ return m_ClassName;		}\
	virtual Name		GetClassName()		{ return m_ClassName;		}\
	virtual bool		IsA( Name const& ClassName )	{ if( ClassName == GetClassName() ) return true; return false; }

#define CLASS_EQUIP_H( klass, klassParent ) \
private:\
	typedef klassParent Super;\
	static Name m_ClassName;\
public:\
	static	Name		StaticClass()		{ return m_ClassName;		}\
	virtual Name		GetClassName()		{ return m_ClassName;		}\
	virtual bool		IsA( Name const& ClassName )	{ if( ClassName == GetClassName() ) return true; return Super::IsA( ClassName ); }

#define CLASS_EQUIP_CPP( klass ) \
Name klass::m_ClassName( #klass )

#undef BB_MATH_CONSTANT

} /* namespace bigball */

#endif // BB_TEMPLATES_H

