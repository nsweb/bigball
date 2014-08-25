
#ifndef BB_WORKERTHREADMANAGER_H
#define BB_WORKERTHREADMANAGER_H

#include "../engine/basemanager.h"

namespace bigball
{

class BIGBALL_API WorkerThread;
class BIGBALL_API Task;
class BIGBALL_API AsyncTask;

//////////////////////////////////////////////////////////////////////////
class BIGBALL_API WorkerThreadManager : public BaseManager
{

	STATIC_MANAGER_H( WorkerThreadManager )

public:
						WorkerThreadManager();
	virtual				~WorkerThreadManager();

	virtual void		Create();
	virtual void		Destroy();
	virtual void		Tick( struct TickContext& TickCtxt );

	Array<WorkerThread*> const& GetPool()		{ return m_vWorkerThreadPool; }

	bool				PushTask( Task* pTask );
	bool				PushAsyncTask( Task* pTask );
	void				UpdateAsyncTasks();
	void				UpdateTasks( int32& nFinishedTasks );

private:
	Array<WorkerThread*>	m_vWorkerThreadPool;

	Array<Task*>			m_PendingSyncTasks;
	Array<int32>			m_RemainingTaskCounts;
	int32					m_NbAsyncTasks;

	void				WaitThreadFinish();
};

} /*namespace bigball*/

#endif // BB_WORKERTHREADMANAGER_H