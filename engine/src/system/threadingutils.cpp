
#include "../bigball.h"
#include "threadingutils.h"

// see http://www.1024cores.net/home/lock-free-algorithms/your-arsenal
// http://preshing.com/20120913/acquire-and-release-semantics/
// and SDL atomics

namespace bigball
{

namespace ThreadTools
{


void SpinLoop()
{
	//__asm { pause };
	Sleep( 0 );
}

void Sleep( uint32 _nMs )
{
    SDL_Delay(_nMs);
}

#if _WIN32
    void InterlockedExchange( void *_dest, const int32 _exchange )
    {
        ::InterlockedExchange( (volatile LONG *) _dest, (LONG) _exchange );
    }
    
    int32 InterlockedCompareExchange( void *_dest, int32 _exchange, int32 _compare )
    {
        return ::InterlockedCompareExchange( (volatile LONG *) _dest, (LONG) _exchange, (LONG) _compare );
    }
#else // 64 bit architectures
    void InterlockedExchange( void *_dest, const int64 _exchange )
    {
#if !defined _MSC_VER
        __sync_lock_test_and_set( (int64*)_dest, _exchange );
        __sync_synchronize();
#else
        ::InterlockedExchange64( (int64*)_dest, _exchange );
#endif
        
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
#if !defined _MSC_VER
        //bool b = std::atomic_compare_exchange_strong (A* obj, T* expected, T val);
        return __sync_val_compare_and_swap( (int64*)_dest, _compare, _exchange );
#else
        return ::InterlockedCompareExchange64( (int64*)_dest, _exchange, _compare );
#endif
        
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
#endif // _WIN32
    

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

void CompilerBarrier()
{
	SDL_CompilerBarrier();		// prevents compiler reordering	
}
void CPUBarrierReadAcquire()
{
	//::MemoryBarrier();			// prevents cpu reordering
	SDL_MemoryBarrierAcquire();
}
void CPUBarrierWriteRelease()
{
	SDL_MemoryBarrierRelease();
}

CriticalSection::CriticalSection()
{
	m_Mutex = SDL_CreateMutex();
	BB_ASSERT_LOG( m_Mutex != nullptr, "Failed to create a critical section mutex" );
}

CriticalSection::~CriticalSection()
{
	if( m_Mutex != nullptr )
	{
		SDL_DestroyMutex(m_Mutex);
	}
}

void CriticalSection::Lock()
{
	if( SDL_mutexP(m_Mutex) != 0 )
	{
		BB_LOG( Threading, Error, "Failed to enter a critical section" );
	}
}

void CriticalSection::Unlock()
{
	if( SDL_mutexV(m_Mutex) != 0 )
	{
		BB_LOG( Threading, Error, "Failed to leave a critical section" );
	}
}

ScopeLock::ScopeLock( CriticalSection* SyncObject ) : m_SyncObject(SyncObject)
{
	BB_ASSERT( SyncObject );
	SyncObject->Lock();
}

ScopeLock::~ScopeLock()
{
	BB_ASSERT( m_SyncObject );
	m_SyncObject->Unlock();
}

ScopeLock& ScopeLock::operator=( ScopeLock& InScopeLock )
{
	return *this;
}


} /*namespace ThreadTools*/

} /*namespace bigball*/


