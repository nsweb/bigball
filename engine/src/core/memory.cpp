


#include "../bigball.h"

namespace bigball
{

void* Memory::Malloc( size_t Count, uint32 Alignment )
{
	return ::malloc( Count );
}
void* Memory::Realloc( void* Original, size_t Count, uint32 Alignment )
{
	return ::realloc( Original, Count );
}
void Memory::Free( void* Original )
{
	return ::free( Original );
}

} /* namespace bigball */

