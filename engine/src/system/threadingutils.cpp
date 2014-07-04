
#include "../bigball.h"
#include "threadingutils.h"

namespace bigball
{

namespace ThreadTools
{

void SpinLoop()
{
	__asm { pause };
}

void Sleep( uint32 _nMs )
{
	::Sleep( _nMs );
}

void InterlockedExchange( void *_dest, const int64 _exchange )
{

	__asm
	{
		mov      ebx, dword ptr [_exchange]
		mov      ecx, dword ptr [_exchange + 4]
		mov      edi, _dest
			mov      eax, dword ptr [edi]
		mov      edx, dword ptr [edi + 4]
		jmp      start
retry:
		pause
start:
		lock cmpxchg8b [edi]
		jnz      retry
	};

}

int32 InterlockedCompareExchange( void *_dest, int32 _exchange, int32 _compare )
{
	char _ret;
	//
	__asm
	{
		mov      edx, [_dest]
		mov      eax, [_compare]
		mov      ecx, [_exchange]

		lock cmpxchg [edx], ecx

			setz    al
			mov     byte ptr [_ret], al
	}
	//
	return _ret;

}

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

void	ReadWriteBarrier()
{
	_ReadWriteBarrier();	// compiler reordering
	
}
void	MemoryBarrier()
{
	MemoryBarrier();		// cpu reordering
}

} /*namespace ThreadTools*/

} /*namespace bigball*/