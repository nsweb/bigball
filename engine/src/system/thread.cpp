
#include "../bigball.h"
#include "thread.h"

namespace bigball
{

#define STATIC_TASK_QUEUE		20
#define FINISHED_TASK_QUEUE		20


Thread::Thread()
{
	//m_hThread = NULL;
}
Thread::~Thread()
{

}

bool Thread::Create()
{
	uint32 iThreadID;
#if !defined _MSC_VER
    //m_hThread = std::thread(s_ThreadMain, this, iThreadID);
#else
	m_hThread = (HANDLE)_beginthreadex( NULL, 0, s_ThreadMain, this/*&hEvent*/, 0, &iThreadID );
	if( m_hThread == NULL )
	{
		return false;
	}
#endif
	return true;
}
void Thread::Destroy()
{
#if !defined _MSC_VER
	m_hThread.detach();
#else
	if( m_hThread )
		CloseHandle( m_hThread );
#endif
}

void Thread::ThreadMain()
{

}

uint32 Thread::s_ThreadMain( void* _pArgs )
{
	Thread* pThread = static_cast<Thread*>( _pArgs );
	pThread->ThreadMain();

	return 0;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

WorkerThread::WorkerThread()
{
	m_pCurrentTask = NULL;
	//m_pTaskProxy = NULL;
}
WorkerThread::~WorkerThread()
{

}

bool WorkerThread::Create()
{
	m_bRunning = 1;
	m_bExit = 0;
	m_oStaticTaskQueue.Resize( STATIC_TASK_QUEUE );
	m_oFinishedTaskQueue.Resize( FINISHED_TASK_QUEUE );

	bool bSuccess = m_oBusyEvent.Create();
	bSuccess &= Thread::Create();

	return bSuccess;
}
void WorkerThread::Destroy()
{
	m_oBusyEvent.Destroy();

	Thread::Destroy();
}

void WorkerThread::ThreadMain()
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
			Task* pTask = NULL;

			if( !m_oFinishedTaskQueue.IsFull() )
			{
				// [18/5/2010 serouart] First check local task queue
				if( !m_oStaticTaskQueue.Pop( pTask ) )
				{
					//TaskProxy* pTaskProxy = m_pTaskProxy;
					//if( pTaskProxy )
					//	pTask = pTaskProxy->PopTask();
				}
			}

			if( pTask )
			{
				m_pCurrentTask = pTask;
			}
			else
			{
				m_pCurrentTask = nullptr;
                
                // Wait until main() sends data
				m_oBusyEvent.ResetEvent();
				m_oBusyEvent.WaitForSingleObject( 10 );
			}
		}
	}
	m_bRunning = false;
}
void WorkerThread::PushTask( Task* _pTask )
{
	m_oStaticTaskQueue.Push( _pTask );
}
Task* WorkerThread::PopFinishedTask()
{
	Task* pTask = NULL;
	m_oFinishedTaskQueue.Pop( pTask );
	return pTask;
}
void WorkerThread::SetBusy()
{
	m_oBusyEvent.SetEvent();
}
void WorkerThread::SetExit()
{
	m_bExit = true;
	m_oBusyEvent.SetEvent();
}

#if 0
void WorkerThread::SetTaskProxy( TaskProxy* _pTaskProxy )
{	
	m_pTaskProxy = _pTaskProxy;
	m_oBusyEvent.SetEvent();
}
#endif


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

ThreadEvent::ThreadEvent()
{
	//m_hEvent = NULL;
}

ThreadEvent::~ThreadEvent()
{

}

bool ThreadEvent::Create()
{
#if !defined _MSC_VER
    return true;
#else
    m_hEvent = ::CreateEventA( NULL, TRUE, TRUE, "ThreadEvent" );
    return m_hEvent ? true:false;
#endif
}
void ThreadEvent::Destroy()
{
#if !defined _MSC_VER
    
#else
	if( m_hEvent )
	{
		::CloseHandle( m_hEvent );
	}
#endif
}

void ThreadEvent::SetEvent()  // signal the event
{
#if !defined _MSC_VER
    m_Ready = true;
    m_Cond.notify_one();
#else
	if( m_hEvent )
	{
		::SetEvent( m_hEvent );
	}
#endif
}

void ThreadEvent::ResetEvent()
{
#if !defined _MSC_VER
    
#else
	if( m_hEvent )
	{
		::ResetEvent( m_hEvent );
	}
#endif
}

void ThreadEvent::WaitForSingleObject( uint32 _nMs )
{
#if !defined _MSC_VER

#else
    if( m_hEvent )
    {
        ::WaitForSingleObject( m_hEvent, _nMs );
    }
#endif
}

} /*namespace bigball*/
