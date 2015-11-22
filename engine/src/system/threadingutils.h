
#ifndef BB_THREADINGUTILS_H
#define BB_THREADINGUTILS_H

namespace bigball
{
	namespace ThreadTools
	{
		void	Sleep( uint32 _nMs );
		void	SpinLoop();
#if _WIN32
        void	InterlockedExchange( void* dest, const int32 exchange );
        int32	InterlockedCompareExchange( void* dest, int32 exchange, int32 compare );
#else
        void	InterlockedExchange( void* dest, const int64 exchange );
        int64	InterlockedCompareExchange( void* dest, int64 exchange, int64 compare );
#endif
		//int32	InterlockedCompareExchange2( void *dest, const int32 exchange1, const int32 exchange2, const int32 compare1, const int32 compare2 );
		void	CompilerBarrier();
		void	CPUBarrierReadAcquire();
		void	CPUBarrierWriteRelease();
	

		/** Helper for critical sections */
		class CriticalSection
		{
		public:
			CriticalSection();
			~CriticalSection();
			void Lock();
			void Unlock();

		private:
			SDL_mutex* m_Mutex;
		};

		class CASLock 
		{
		public:

            void Lock() 
			{ 
				while( 0 != InterlockedCompareExchange( (void*)&m_SyncObject, 1, 0 ) )
					ThreadTools::SpinLoop();
			}
            void Unlock() 
			{ 
				while( 1 != InterlockedCompareExchange( (void*)&m_SyncObject, 0, 1 ) )
					ThreadTools::SpinLoop();
			}
            bool TryLock()		{ return 0 != InterlockedCompareExchange( (void*)&m_SyncObject, 1, 0 ); }
            bool TryUnlock()	{ return 1 != InterlockedCompareExchange( (void*)&m_SyncObject, 0, 1 ); }

#if _WIN32
            volatile uint32 m_SyncObject;
#else
			volatile uint64 m_SyncObject;
#endif
        };

		/** Defines a scope for the critical section */
		class ScopeLock
		{
		public:
			ScopeLock( CriticalSection* SyncObject );
			~ScopeLock();

		private:

			/** Hide other constructors, copy and assignment operator */
			ScopeLock();
			ScopeLock( ScopeLock* InScopeLock );
			ScopeLock& operator = ( ScopeLock& InScopeLock );

			CriticalSection* m_SyncObject;
		};

	} /*namespace ThreadTools*/

} /*namespace bigball*/

#endif // BB_THREADINGUTILS_H