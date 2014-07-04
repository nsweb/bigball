
#ifndef BB_WORKERTHREADMANAGER_H
#define BB_WORKERTHREADMANAGER_H

#include "basemanager.h"

namespace bigball
{

class BIGBALL_API WorkerThread;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
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

	const vector<WorkerThread*>& GetPool()		{ return m_vWorkerThreadPool; }

private:
	Array<WorkerThread*>	m_vWorkerThreadPool;

	void				WaitThreadFinish();
};



#endif // BB_WORKERTHREADMANAGER_H