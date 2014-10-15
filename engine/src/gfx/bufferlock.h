

#ifndef BB_BUFFERLOCK_H
#define BB_BUFFERLOCK_H

// Inspired from apitest on github

namespace bigball
{

struct BufferRange
{
	uint32 m_StartOffset;
	uint32 m_Length;

	bool Overlaps(const BufferRange& _rhs) const 
	{
		return m_StartOffset < (_rhs.m_StartOffset + _rhs.m_Length)
			&& _rhs.m_StartOffset < (m_StartOffset + m_Length);
	}
};

struct BufferLock
{
	BufferRange m_Range;
	GLsync m_SyncObj;
};

class BufferLockManager
{
public:
	BufferLockManager( bool bCPUUpdates );
	~BufferLockManager();

	static BufferLockManager*	GetStaticInstance()		{ return m_pStaticInstance; }
	void WaitForLockedRange(uint32 _lockBeginBytes, uint32 _lockLength);
	void LockRange(uint32 _lockBeginBytes, uint32 _lockLength);

private:
	void wait(GLsync* _syncObj);
	void cleanup(BufferLock* _bufferLock);

	static BufferLockManager*	m_pStaticInstance;
	Array<BufferLock>			m_BufferLocks;
	/** Whether it's the CPU (true) that updates, or the GPU (false) */
	bool						m_bCPUUpdates;
};

} /* namespace bigball */

#endif // BB_BUFFERLOCK_H