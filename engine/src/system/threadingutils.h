
#ifndef BB_THREADINGUTILS_H
#define BB_THREADINGUTILS_H

namespace bigball
{
	namespace ThreadTools
	{
		void	Sleep( uint32 _nMs );
		void	SpinLoop();
		void	InterlockedExchange( void *dest, const int64 exchange );
		int32	InterlockedCompareExchange( void *dest, int32 exchange, int32 compare );
		int32	InterlockedCompareExchange2( void *dest, const int32 exchange1, const int32 exchange2, const int32 compare1, const int32 compare2 );
		void	ReadWriteBarrier();
		void	MemoryBarrier();
	} /*namespace ThreadTools*/

} /*namespace bigball*/

#endif // BB_THREADINGUTILS_H