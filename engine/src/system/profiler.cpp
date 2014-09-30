


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
	
	CallNode::m_ThreadState.m_ThreadNodeLock.Lock();
	CallNode::m_ThreadState.m_pActiveNode = newRoot;
	newRoot->Start();
	newRoot->SetActive( true );
	g_ThreadCallRoot = newRoot;
	CallNode::m_ThreadState.m_ThreadNodeLock.Unlock();

	g_ProfilerData.m_ThreadLock.Unlock();	
}

void ThreadExit()
{
	g_ProfilerData.m_ThreadLock.Lock();

    CallNode::m_ThreadState.m_ThreadNodeLock.Lock();
    g_ThreadCallRoot->Stop();
    g_ThreadCallRoot->SetActive( false );
	CallNode::m_ThreadState.m_pActiveNode = nullptr;
    CallNode::m_ThreadState.m_ThreadNodeLock.Unlock();

    g_ProfilerData.m_ThreadLock.Unlock();	
}

void NodeEntry( const char* szName ) 
{
	CallNode* pParent = CallNode::m_ThreadState.m_pActiveNode;
	//if ( !parent )
	//	return;

	CallNode* pActive = pParent->FindOrCreateChildNode( szName );
	pActive->Start();
	CallNode::m_ThreadState.m_pActiveNode = pActive;
}

void NodeExit() 
{
	CallNode* pActive = CallNode::m_ThreadState.m_pActiveNode;
	//if ( !active )
	//	return;

	pActive->Stop();
	CallNode::m_ThreadState.m_pActiveNode = pActive->m_pParent;
}

CallNode* CallNode::FindOrCreateChildNode( const char* szName ) 
{
	for( int32 i = 0; i < m_ChildrenArray.size(); ++i )
	{
		if( m_ChildrenArray[i]->m_Name == szName )
			return m_ChildrenArray[i];
	}

	// didn't find the caller, lock this thread and mutate
	CallNode::m_ThreadState.m_ThreadNodeLock.Lock();
	CallNode* newChild = new CallNode( szName, this );
	m_ChildrenArray.push_back( newChild );
	CallNode::m_ThreadState.m_ThreadNodeLock.Unlock();

	return newChild;
}

void CallNode::Start()
{
	m_StartTime = SDL_GetPerformanceCounter();
	m_CallCount++;
}

void CallNode::Stop()
{
	uint64 StopTime = SDL_GetPerformanceCounter();
	m_fTimeSpent = (StopTime - m_StartTime) / (float)SDL_GetPerformanceFrequency();

	BB_LOG( Profiler, Log, "Sum time spent in %s = %f seconds", m_Name, m_fTimeSpent );
}

void CallNode::SetActive( bool bActive )
{

}




} /* namespace Profiler */

} /* namespace bigball */

