

#ifndef BB_COREMEMORY_H
#define BB_COREMEMORY_H

namespace bigball
{
	
#define DEFAULT_ALIGNMENT	8

struct BIGBALL_API Memory
{
	static inline void* Memmove( void* Dest, const void* Src, SIZE_T Count )
	{
		return memmove( Dest, Src, Count );
	}

	static inline int32 Memcmp( const void* Buf1, const void* Buf2, SIZE_T Count )
	{
		return memcmp( Buf1, Buf2, Count );
	}

	static inline void* Memset(void* Dest, uint8 Char, SIZE_T Count)
	{
		return memset( Dest, Char, Count );
	}

	static inline void* Memzero(void* Dest, SIZE_T Count)
	{
		return memset( Dest, 0, Count );
	}

	static inline void* Memcpy(void* Dest, const void* Src, SIZE_T Count)
	{
		return memcpy( Dest, Src, Count );
	}

	/** On some platforms memcpy optimized for big blocks is available */
	static inline void* BigBlockMemcpy(void* Dest, const void* Src, SIZE_T Count)
	{
		return memcpy( Dest, Src, Count );
	}

	/** On some platforms memcpy optimized for big blocks that avoid L2 cache pollution are available */
	static inline void* StreamingMemcpy(void* Dest, const void* Src, SIZE_T Count)
	{
		return memcpy( Dest, Src, Count );
	}

	//static inline void Memswap( void* Ptr1, void* Ptr2, SIZE_T Size )
	//{
	//	void* Temp = FMemory_Alloca(Size);
	//	Memory::Memcpy( Temp, Ptr1, Size );
	//	Memory::Memcpy( Ptr1, Ptr2, Size );
	//	Memory::Memcpy( Ptr2, Temp, Size );
	//}

	/** Templated version of Memory::Memset. */
	template< class T > static inline void MemSet( T& Src, uint8 I )
	{
		Memory::Memset( &Src, I, sizeof( T ) );
	}

	/** Templated version of Memory::Memzero. */
	template< class T > static inline void MemZero( T& Src )
	{
		Memory::Memset( &Src, 0, sizeof( T ) );
	}

	/** Templated version of Memory::Memcpy. */
	template< class T > static inline void MemCopy( T& Dest, const T& Src )
	{
		Memory::Memcpy( &Dest, &Src, sizeof( T ) );
	}

	//
	// C style memory allocation stubs that fall back to C runtime
	//
	static inline void* SystemMalloc(SIZE_T Size)
	{
		return ::malloc(Size);
	}

	static inline void SystemFree(void* Ptr)
	{
		::free(Ptr);
	}

	//
	// C style memory allocation stubs.
	//


	static void* Malloc( size_t Count, uint32 Alignment=DEFAULT_ALIGNMENT );
	static void* Realloc( void* Original, size_t Count, uint32 Alignment=DEFAULT_ALIGNMENT );
	static void Free( void* Original );

	//static void* PhysicalAlloc( SIZE_T Count, ECacheBehaviour CacheBehaviour = CACHE_WriteCombine );
	//static void PhysicalFree( void* Original );

	static size_t GetAllocSize( void* Original );
};


} /* namespace bigball */


// Operator new / delete overload

inline void* operator new( size_t Size )
{
	return bigball::Memory::Malloc( Size );
}
inline void operator delete( void* Ptr )
{
	bigball::Memory::Free( Ptr );
}

inline void* operator new[]( size_t Size )
{
	return bigball::Memory::Malloc( Size );
}
inline void operator delete[]( void* Ptr )
{
	bigball::Memory::Free( Ptr );
}

#endif // BB_COREMEMORY_H

