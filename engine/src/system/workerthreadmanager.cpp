
#include "../bigball.h"

#include "workerthreadmanager.h"
#include "thread.h"
#include "threadingutils.h"

#define WORKER_THREAD_POOL		4

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

namespace bigball
{

STATIC_MANAGER_CPP( WorkerThreadManager )

WorkerThreadManager::WorkerThreadManager()
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
		SAFE_DELETE( pWorker );
	}
	m_vWorkerThreadPool.clear();
}

void WorkerThreadManager::Tick( struct TickContext& TickCtxt )
{

}


void WorkerThreadManager::WaitThreadFinish()
{
	bool bWaitFinish = true, bStillBusy;
	u32 j, nPool = m_vWorkerThreadPool.size();

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