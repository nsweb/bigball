
#ifndef BB_THREADINGUTILS_H
#define BB_THREADINGUTILS_H

namespace bigball
{
	namespace ThreadTools
	{
		void	Sleep( uint32 _nMs );
		void	SpinLoop();
#if WIN64
		void	InterlockedExchange( void* dest, const int64 exchange );
		int64	InterlockedCompareExchange( void* dest, int64 exchange, int64 compare );
#else
		void	InterlockedExchange( void* dest, const int32 exchange );
		int32	InterlockedCompareExchange( void* dest, int32 exchange, int32 compare );
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