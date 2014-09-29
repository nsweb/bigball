


#ifndef BB_PROFILER_H
#define BB_PROFILER_H


namespace bigball
{

namespace Profiler
{
	typedef uint32 NameHandle;
	typedef MapRH<String,NameHandle> NameMap;

	class CallNode
	{
		CallNode* m_pParent;

		// per thread state
		struct ThreadState 
		{
			Lock ThreadLock;
			CallNode *pActiveCaller;
		};

		static __declspec(thread) ThreadState m_ThreadState;
	};

	class CallRoot
	{
		CallNode* m_pRoot;
		ThreadState* m_pThreadState;
	};

	/** Start profiling on all threads */
	void Start();
	void Stop();
	void Reset();

	void ThreadEntry( const char* szName );
	void ThreadExit( const char* szName );


} /* namespace Profiler */

} /* namespace bigball */

#endif // BB_PROFILER_H

