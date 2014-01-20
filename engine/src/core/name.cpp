


#include "../bigball.h"
#include "name.h"

namespace bigball
{

NameManager::NameManager()
{
	m_pStaticInstance = this;
}

NameManager::~NameManager()
{
	m_pStaticInstance = nullptr;
}

NameHandle NameManager::FindOrAddHandle( String const& str )
{
	const NameMap::Pair* NameEntry = m_StringTable.Find( str );
	if( !NameEntry )
	{
		NameEntry = m_StringTable.Add( str, m_StringTable.m_NbActivePairs );
	}

	return NameEntry->Value;
}

const String& NameManager::GetString( NameHandle Handle )
{
	if( Handle < m_StringTable.m_NbActivePairs )
	{
		return m_StringTable.m_Pairs[Handle].Key;
	}

	return m_StringTable.m_Pairs[0].Key;
}

void NameManager::StaticInit()
{
	m_StringTable.reserve( 4096 );

	FindOrAddHandle( "None" );
}
NameManager* NameManager::GetStaticInstance() 
{ 
	if( !m_pStaticInstance )
		StaticInit();
	return m_pStaticInstance; 
}

} /* namespace bigball */

