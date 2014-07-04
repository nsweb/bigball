
#include "stdafx.h"


#ifndef peTThread_h
#include "peTThread.h"
#endif

#define STATIC_TASK_QUEUE		20
#define FINISHED_TASK_QUEUE		20

peTThread::peTThread()
{
	m_hThread = NULL;
}
peTThread::~peTThread()
{

}

bool peTThread::Create()
{
	u32 iThreadID;
	m_hThread = (HANDLE)_beginthreadex( NULL, 0, s_ThreadMain, this/*&hEvent*/, 0, &iThreadID );
	if( m_hThread == NULL ) 
	{
		return false;
	}
	return true;
}
void peTThread::Destroy()
{
	if( m_hThread )
		CloseHandle( m_hThread );
}

void peTThread::ThreadMain()
{

}

u32 peTThread::s_ThreadMain( void* _pArgs )
{
	peTThread* pThread = static_cast<peTThread*>( _pArgs );
	pThread->ThreadMain();

	return 0;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

peTWorkerThread::peTWorkerThread()
{
	m_pCurrentTask = NULL;
	m_pTaskProxy = NULL;
}
peTWorkerThread::~peTWorkerThread()
{

}

bool peTWorkerThread::Create()
{
	m_bRunning = 1;
	m_bExit = 0;
	m_oStaticTaskQueue.Resize( STATIC_TASK_QUEUE );
	m_oFinishedTaskQueue.Resize( FINISHED_TASK_QUEUE );

	bool bSuccess = peTThread::Create();
	bSuccess &= m_oBusyEvent.Create();

	return bSuccess;
}
void peTWorkerThread::Destroy()
{
	m_oBusyEvent.Destroy();

	peTThread::Destroy();
}

void peTWorkerThread::ThreadMain()
{
	while( !m_bExit )
	{
		if( m_pCurrentTask ) // if I have a job to do...
		{
			if( !m_pCurrentTask->IsCancelled() )
			{
				m_pCurrentTask->Execute();              // execute the job.
			}
			m_oFinishedTaskQueue.Push( m_pCurrentTask );
		}

		if( !m_bExit )
		{
			peTTask* pTask = NULL;

			if( !m_oFinishedTaskQueue.IsFull() )
			{
				// [18/5/2010 serouart] First check local task queue
				if( !m_oStaticTaskQueue.Pop( pTask ) )
				{
					peTTaskProxy* pTaskProxy = m_pTaskProxy;
					if( pTaskProxy )
						pTask = pTaskProxy->PopTask();
				}
			}

			if( pTask )
			{
				m_pCurrentTask = pTask;
			}
			else
			{
				m_pCurrentTask = NULL;
				m_oBusyEvent.ResetEvent();
				m_oBusyEvent.WaitForSingleObject( 10 );
			}
		}
	}
	m_bRunning = false;
}
void peTWorkerThread::PushTask( peTTask* _pTask )
{
	m_oStaticTaskQueue.Push( _pTask );
}
peTTask* peTWorkerThread::PopFinishedTask()
{
	peTTask* pTask = NULL;
	m_oFinishedTaskQueue.Pop( pTask );
	return pTask;
}
void peTWorkerThread::ForceBusy()
{
	m_oBusyEvent.SetEvent();
}
void peTWorkerThread::SetExit()
{
	m_bExit = true;
	m_oBusyEvent.SetEvent();
}

void peTWorkerThread::SetTaskProxy( peTTaskProxy* _pTaskProxy )
{	
	m_pTaskProxy = _pTaskProxy;
	m_oBusyEvent.SetEvent();
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

peTThreadEvent::peTThreadEvent()
{
	m_hEvent = NULL;
}

peTThreadEvent::~peTThreadEvent()
{

}

bool peTThreadEvent::Create()
{
	m_hEvent = ::CreateEventA( NULL, TRUE, TRUE, "ThreadEvent" );
	return m_hEvent ? true:false;
}
void peTThreadEvent::Destroy()
{
	if( m_hEvent )
	{
		::CloseHandle( m_hEvent );
	}
}

void peTThreadEvent::SetEvent()  // signal the event
{
	if( m_hEvent )
	{
		::SetEvent( m_hEvent );
	}
}

void peTThreadEvent::ResetEvent()
{
	if( m_hEvent )
	{
		::ResetEvent( m_hEvent );
	}
}

void peTThreadEvent::WaitForSingleObject( u32 _nMs )
{
	if( m_hEvent )
	{
		::WaitForSingleObject( m_hEvent, _nMs );
	}
}