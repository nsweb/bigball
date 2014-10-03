


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
void FrameReset()
{
	g_ProfilerData.m_ThreadLock.Lock();

	for( int32 RootIdx = 0; RootIdx < g_ProfilerData.m_ThreadRoots.size(); ++RootIdx )
	{
		CallRoot& TRoot = g_ProfilerData.m_ThreadRoots[RootIdx];
		//if( TRoot.m_pThreadState->m_pActiveNode )
		{
			TRoot.m_pRoot->FrameReset();
		}
	}
	g_ProfilerData.m_ThreadLock.Unlock();	
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
	float fTimeSpent = (StopTime - m_StartTime) / (float)SDL_GetPerformanceFrequency();
	m_fTimeSpent += fTimeSpent;
	m_fMaxTimeSpent = bigball::max( m_fTimeSpent, m_fMaxTimeSpent );

	BB_LOG( Profiler, Log, "[STAT] %s = %f ms - Max = %f ms", m_Name, m_fTimeSpent*100.0f, m_fMaxTimeSpent*100.0f );
}

void CallNode::SetActive( bool bActive )
{

}

void CallNode::FrameReset()
{
	m_StartTime = 0; 
	m_CallCount = 0; 
	m_fTimeSpent = 0;

	// Recursively call each child node
	for( int32 ChildIdx = 0; ChildIdx < m_ChildrenArray.size(); ++ChildIdx )
	{
		m_ChildrenArray[ChildIdx]->FrameReset();
	}
}

void CallNode::HardReset()
{
	m_StartTime = 0; 
	m_CallCount = 0; 
	m_fTimeSpent = 0; 
	m_fMaxTimeSpent = 0;
}






} /* namespace Profiler */

} /* namespace bigball */

