


#include "../bigball.h"
#include "name.h"

namespace bigball
{
	
NameHandle NameManager::FindOrAddHandle( String const& str )
{
	const NameMap::Pair* NameEntry = m_StringTable.Find( str );
	if( !NameEntry )
	{
		NameEntry = m_StringTable.Add( str, ++m_GlobalHandle );
	}

	return NameEntry->Value;
}

} /* namespace bigball */

