
#include "../bigball.h"
#include "threadingutils.h"

// see http://www.1024cores.net/home/lock-free-algorithms/your-arsenal
// and SDL atomics

namespace bigball
{

namespace ThreadTools
{

void SpinLoop()
{
	//__asm { pause };
	::Sleep( 0 );
}

void Sleep( uint32 _nMs )
{
	::Sleep( _nMs );
}

#if WIN64
void InterlockedExchange( void *_dest, const int64 _exchange )
{
	::InterlockedExchange64( (int64*)_dest, _exchange );

//	__asm
//	{
//		mov      ebx, dword ptr [_exchange]
//		mov      ecx, dword ptr [_exchange + 4]
//		mov      edi, _dest
//			mov      eax, dword ptr [edi]
//		mov      edx, dword ptr [edi + 4]
//		jmp      start
//retry:
//		pause
//start:
//		lock cmpxchg8b [edi]
//		jnz      retry
//	};
//
}

int64 InterlockedCompareExchange( void* _dest, int64 _exchange, int64 _compare )
{
	return ::InterlockedCompareExchange64( (int64*)_dest, _exchange, _compare );

	//char _ret;
	////
	//__asm
	//{
	//	mov      edx, [_dest]
	//	mov      eax, [_compare]
	//	mov      ecx, [_exchange]

	//	lock cmpxchg [edx], ecx

	//		setz    al
	//		mov     byte ptr [_ret], al
	//}
	////
	//return _ret;

}
#else // 32 bit architectures

void InterlockedExchange( void *_dest, const int32 _exchange )
{
	::InterlockedExchange( _dest, _exchange );
}

int32 InterlockedCompareExchange( void *_dest, int32 _exchange, int32 _compare )
{
	return ::InterlockedCompareExchange( _dest, _exchange, _compare );
);

#endif // WIN64

#if 0
int32 InterlockedCompareExchange2( void *_dest, const int32 _exchange1, const int32 _exchange2, const int32 _compare1, const int32 _compare2 )
{

	char _ret;
	//
	__asm
	{
		mov     ebx, [_exchange1]
		mov     ecx, [_exchange2]
		mov     edi, [_dest]
		mov     eax, [_compare1]
		mov     edx, [_compare2]
		lock	cmpxchg8b [edi]
		setz    al
		mov     byte ptr [_ret], al
	}
	//
	return _ret;
}
#endif

void ReadWriteBarrier()
{
	::_ReadWriteBarrier();	// compiler reordering
	
}
void MemBarrier()
{
	::MemoryBarrier();		// cpu reordering
}

} /*namespace ThreadTools*/

} /*namespace bigball*/