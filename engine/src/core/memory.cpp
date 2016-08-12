


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

// Operator new / delete overload

void* operator new( size_t Size ) throw(std::bad_alloc)
{
    return bigball::Memory::Malloc( Size );
}
void operator delete( void* Ptr ) throw()
{
    bigball::Memory::Free( Ptr );
}

void* operator new[]( size_t Size ) throw(std::bad_alloc)
{
    return bigball::Memory::Malloc( Size );
}
void operator delete[]( void* Ptr ) throw()
{
    bigball::Memory::Free( Ptr );
}

