
#ifndef BB_TASK_H
#define BB_TASK_H


namespace bigball
{

//class BIGBALL_API LockFreeQueue;

struct BIGBALL_API TaskNode
{
	TaskNode*	m_pNextNode;
};


/** Task executed in one frame by a worker thread */
class BIGBALL_API Task : public TaskNode
{
public:
	Task() : 
	  m_bCancelled(false),
		  m_pUserData(0),
		  m_iUserId(0)
	  {
	  }

	  virtual void Execute() = 0;
	  void SetUserData( void* _pUserData, uint32 _iUserId )	{ m_pUserData = _pUserData; m_iUserId = _iUserId; }
	  uint32 IsCancelled() const							{ return m_bCancelled;	}
	  void Cancel()											{ m_bCancelled = true;	}
	  virtual bool IsAsync()								{ return false; }

protected:

	uint32				m_bCancelled;
	void*				m_pUserData;
	uint32              m_iUserId;
};

/** Task that may take multiple frames and executed by a worker thread */
class BIGBALL_API AsyncTask : public Task
{
public:
	AsyncTask()
	{
	}
	virtual bool IsAsync()									{ return true; }
	virtual void OnFinished() = 0;

protected:

};


#if 0
//////////////////////////////////////////////////////////////////////////
class BIGBALL_API TaskGroup
{
public:
	TaskGroup()		{}
	~TaskGroup()	{}

	Array<Task*>	m_vTask;
};

//////////////////////////////////////////////////////////////////////////
class BIGBALL_API TaskProxy
{
public:
				TaskProxy();
	virtual		~TaskProxy();

	void		SetTaskQueue( LockFreeQueue* _pTaskQueue );
	void		PushTask( Task* _pItem );
	Task*		PopTask();

	void		ExecuteTasks( bool _bUseCallerThread, const uint32 _cnWaitTask );
	void		ExecuteTaskGroups( bool _bUseCallerThread, TaskGroup* _aGroup, const uint32 _cnGroup );

private:
	LockFreeQueue*	m_pTaskQueue;
};
#endif

} /*namespace bigball*/

#endif // BB_TASK_H