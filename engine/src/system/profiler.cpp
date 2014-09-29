


#include "../bigball.h"
#include "profiler.h"
#include "threadingutils.h"

namespace bigball
{

namespace Profiler
{

static ProfilerData g_ProfilerData = ProfilerData();
__declspec(thread) CallNode* g_ThreadCallRoot = nullptr;
__declspec(thread) ThreadState CallNode::m_ThreadState = { {0}, nullptr };


/** Start profiling on all threads */
void Start()
{

}
void Stop()
{

}
void Reset()
{

}

void ThreadEntry( const char* szName )
{
	CallNode* newRoot = new CallNode( szName );

	g_ProfilerData.m_ThreadLock.Lock();
	g_ProfilerData.m_ThreadRoots.push_back( CallRoot(newRoot, &CallNode::m_ThreadState ) );
	
	CallNode::m_ThreadState.m_ThreadLock.Lock();
	CallNode::m_ThreadState.m_pActiveNode = newRoot;
	newRoot->Start();
	newRoot->SetActive( true );
	g_ThreadCallRoot = newRoot;
	CallNode::m_ThreadState.m_ThreadLock.Unlock();

	g_ProfilerData.m_ThreadLock.Unlock();	
}

void ThreadExit()
{
	g_ProfilerData.m_ThreadLock.Lock();

    CallNode::m_ThreadState.m_ThreadLock.Lock();
    g_ThreadCallRoot->Stop();
    g_ThreadCallRoot->SetActive( false );
	CallNode::m_ThreadState.m_pActiveNode = nullptr;
    CallNode::m_ThreadState.m_ThreadLock.Unlock();

    g_ProfilerData.m_ThreadLock.Unlock();	
}

} /* namespace Profiler */

} /* namespace bigball */

