
#include "../bigball.h"

#include "workerthreadmanager.h"
#include "thread.h"
#include "threadingutils.h"


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

namespace bigball
{

STATIC_MANAGER_CPP( WorkerThreadManager )

#define WORKER_THREAD_POOL		4

WorkerThreadManager::WorkerThreadManager() :
	m_NbAsyncTasks(0)
{
	m_pStaticInstance = this;
}

WorkerThreadManager::~WorkerThreadManager(void)
{
	m_pStaticInstance = NULL;
}

void WorkerThreadManager::Create()
{
	//m_pDxCamera = new peDxCamera;
	//m_pCamTarget = new peCameraTarget_Ed;

	m_vWorkerThreadPool.resize( WORKER_THREAD_POOL );
	uint32 i, nPool = m_vWorkerThreadPool.size();
	for( i = 0; i < nPool; i++ )
	{
		WorkerThread* pWorker = new WorkerThread;
		pWorker->Create();
		m_vWorkerThreadPool[i] = pWorker;
	}

	m_RemainingTaskCounts.resize( WORKER_THREAD_POOL, 0 );

}

void WorkerThreadManager::Destroy()
{
	//SAFE_DELETE( m_pCamTarget );
	//SAFE_DELETE( m_pDxCamera );

	WaitThreadFinish();

	uint32 i, nPool = m_vWorkerThreadPool.size();
	for( i = 0; i < nPool; i++ )
	{
		WorkerThread* pWorker = m_vWorkerThreadPool[i];
		pWorker->Destroy();
		BB_DELETE( pWorker );
	}
	m_vWorkerThreadPool.clear();
	m_RemainingTaskCounts.clear();
}

void WorkerThreadManager::Tick( struct TickContext& TickCtxt )
{
	// Update async tasks once in a frame
	UpdateAsyncTasks();
}

void WorkerThreadManager::UpdateAsyncTasks()
{
	// Check end of async
	while( Task* pTask = m_vWorkerThreadPool.Last()->PopFinishedTask() )
	{
		BB_ASSERT( pTask->IsAsync() );
		pTask->OnFinished();
		m_NbAsyncTasks--;
	}
}

void WorkerThreadManager::UpdateTasks( int32& nFinishedTasks )
{
	// Check for tasks finished, and push new tasks to worker thread
	int32 i, nPool = m_vWorkerThreadPool.size();
	for( i = 0; i < nPool - 1 /* last is for async */; i++ )
	{
		while( Task* pTask = m_vWorkerThreadPool[i]->PopFinishedTask() )
		{
			BB_ASSERT( !pTask->IsAsync() );
			//pTask->OnFinished();
			nFinishedTasks++;
		}
	}

	// Push new tasks
	// Don't push more than (nPool - 1) * 2 tasks at the same time
	int32 SumRemainingTasks = 0;
	for( i = 0; i < nPool - 1 /* last is for async */; i++ )
	{
		int32 RemainingTaskCount = m_vWorkerThreadPool[i]->GetRemainingTaskCount();
		m_RemainingTaskCounts[i] = RemainingTaskCount;
		SumRemainingTasks += RemainingTaskCount;
	}

	int32 PushCount = bigball::min( m_PendingSyncTasks.size(), (nPool - 1) * 2 );
	int32 InFlight = SumRemainingTasks + PushCount;
	int32 TaskSettledIdx = 0;
	
	for( i = 0; i < nPool - 1 /* last is for async */; i++ )
	{
		int32 CurrentCount = m_RemainingTaskCounts[i];
		int32 ToAdd = InFlight / (nPool - 1 - i) - CurrentCount;
		InFlight -= CurrentCount;
		for( int32 AddIdx = 0; AddIdx < ToAdd; ++AddIdx )
		{
			InFlight--;
			m_vWorkerThreadPool[i]->PushTask( m_PendingSyncTasks[TaskSettledIdx++] );
		}
	}
	m_PendingSyncTasks.erase( 0, TaskSettledIdx );
}

bool WorkerThreadManager::PushTask( Task* pTask )
{
	pTask->SetAsync( false );
	m_PendingSyncTasks.push_back( pTask );

	return true;
}

bool WorkerThreadManager::PushAsyncTask( Task* pTask )
{
	m_NbAsyncTasks++;

	// Last worker thread is reserved for async
	pTask->SetAsync( true );
	m_vWorkerThreadPool.Last()->PushTask( pTask );

	return true;
}


void WorkerThreadManager::WaitThreadFinish()
{
	bool bWaitFinish = true, bStillBusy;
	int32 j, nPool = m_vWorkerThreadPool.size();

	while( bWaitFinish )
	{
		bStillBusy = false;

		for( j=0; j<nPool; j++)
		{
			WorkerThread* pWorker = m_vWorkerThreadPool[j];
			pWorker->SetExit();
		}

		for( j=0; j<nPool; j++)
		{
			WorkerThread* pWorker = m_vWorkerThreadPool[j];
			if( pWorker->IsRunning() )
			{
				bStillBusy = true;
				break;
			}
		}

		if ( !bStillBusy )
		{
			bWaitFinish = false;
		}
		else
		{
			ThreadTools::Sleep( 0 );
		}
	}
}



} /*namespace bigball*/