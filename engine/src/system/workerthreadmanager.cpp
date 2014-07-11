
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
}

void WorkerThreadManager::Tick( struct TickContext& TickCtxt )
{
	// Check end of async
	while( Task* pTask = m_vWorkerThreadPool.Last()->PopFinishedTask() )
	{
		BB_ASSERT( pTask->IsAsync() );
		((AsyncTask*)pTask)->OnFinished();
		m_NbAsyncTasks--;
	}
}

bool WorkerThreadManager::PushAsyncTask( AsyncTask* pTask )
{
	m_NbAsyncTasks++;

	// Last worker thread is for async
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