

#ifndef BB_THREAD_H
#define BB_THREAD_H 

#include "lockfreequeue.h"
#include "task.h"

namespace bigball
{

//////////////////////////////////////////////////////////////////////////
class ThreadEvent
{
public:
	ThreadEvent();
	~ThreadEvent();

	bool	Create();
	void	Destroy();
	void	SetEvent();
	void	ResetEvent();
	void	WaitForSingleObject( uint32 _nMs );

private:
    //std::mutex  m_hEvent;
	HANDLE		m_hEvent;
};

//////////////////////////////////////////////////////////////////////////
class Thread
{
public:
	Thread();
	virtual ~Thread();

	virtual bool	Create();
	virtual void	Destroy();

	virtual void	ThreadMain();

private:
	HANDLE	m_hThread;
    //std::thread m_hThread;

    static uint32 /*__stdcall*/ s_ThreadMain( void* _pArgs );
};


//////////////////////////////////////////////////////////////////////////
class CACHE_ALIGN WorkerThread : public Thread
{
public:
	WorkerThread();
	virtual ~WorkerThread();

	virtual bool	Create();
	virtual void	Destroy();

	virtual void	ThreadMain();

	void			PushTask( Task* _pTask );
	Task*			PopFinishedTask();
	void			SetExit();
	void			ForceBusy();
	//void			SetTaskProxy(TaskProxy* _pTaskProxy );
	uint32			IsRunning()							{ return m_bRunning;	}
	int32			GetRemainingTaskCount() const		{ return m_oStaticTaskQueue.GetElementCount();		}

private:
	uint32					m_bRunning	: 1;
	uint32					m_bExit		: 1;

	//TaskProxy*				m_pTaskProxy;
	ThreadEvent				m_oBusyEvent;
	Task*					m_pCurrentTask;             // current job being worked on
	CircularQueue<Task*>	m_oStaticTaskQueue;			// jobs that need to be processed
	CircularQueue<Task*>	m_oFinishedTaskQueue;		// jobs that have been completed and may be reported back to the application
}; 


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

} /*namespace bigball*/

#endif // BB_THREAD_H