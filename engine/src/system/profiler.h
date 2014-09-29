


#ifndef BB_PROFILER_H
#define BB_PROFILER_H

#include "threadingutils.h"

namespace bigball
{

namespace Profiler
{
	typedef uint32 NameHandle;
	typedef MapRH<String,NameHandle> NameMap;

	struct CallNode;

	struct ThreadState 
	{
		ThreadTools::CASLock m_ThreadLock;
		CallNode* m_pActiveNode;
	};

	struct CallNode
	{
		CallNode( const char* szName, CallNode* pParent = nullptr ) : m_Name(szName), m_pParent(pParent)	{}

		void Start()	{}
		void Stop()		{}
		void SetActive( bool bActive )	{}

		const char* m_Name;
		CallNode* m_pParent;
		Array<CallNode*> m_ChildrenArray;

		static __declspec(thread) ThreadState m_ThreadState;
	};

	struct CallRoot
	{
		CallRoot( CallNode* pRoot, ThreadState* pThreadState ) : m_pRoot(pRoot), m_pThreadState(pThreadState)	{}
		CallNode* m_pRoot;
		ThreadState* m_pThreadState;
	};

	struct ProfilerData
	{
		ProfilerData()	{}
		Array<CallRoot>			m_ThreadRoots;
		ThreadTools::CASLock	m_ThreadLock;
	};

	void ThreadEntry( const char* szName );
	void ThreadExit();

	/** Defines a scope for thread */
	class ScopeThread
	{
	public:
		ScopeThread( const char* szName )	{ ThreadEntry( szName );	}
		~ScopeThread()						{ ThreadExit();				}
	};

	/** Start profiling on all threads */
	void Start();
	void Stop();
	void Reset();


} /* namespace Profiler */

} /* namespace bigball */

#endif // BB_PROFILER_H

