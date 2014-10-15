

#include "../bigball.h"
#include "bufferlock.h"


namespace bigball
{

BufferLockManager* BufferLockManager::m_pStaticInstance = NULL;

BufferLockManager::BufferLockManager(bool bCPUUpdates) :
	m_bCPUUpdates(bCPUUpdates)
{
	m_pStaticInstance = this;
}

BufferLockManager::~BufferLockManager()
{
	for( int i = 0; i < m_BufferLocks.size(); ++i) 
	{
		cleanup( &m_BufferLocks[i] );
	}

	m_BufferLocks.clear();

	m_pStaticInstance = nullptr;
}

void BufferLockManager::WaitForLockedRange(uint32 _lockBeginBytes, uint32 _lockLength)
{
	BufferRange testRange = { _lockBeginBytes, _lockLength };
	Array<BufferLock> swapLocks;
	for( int i = 0; i < m_BufferLocks.size(); ++i) 
	{
		if( testRange.Overlaps( m_BufferLocks[i].m_Range ) ) 
		{
			wait( &m_BufferLocks[i].m_SyncObj );
			cleanup( &m_BufferLocks[i] );
		} 
		else 
		{
			swapLocks.push_back( m_BufferLocks[i] );
		}
	}

	m_BufferLocks = swapLocks;
}

void BufferLockManager::LockRange(uint32 _lockBeginBytes, uint32 _lockLength)
{
	BufferRange newRange = { _lockBeginBytes, _lockLength };
	GLsync syncName = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 );
	BufferLock newLock = { newRange, syncName };

	m_BufferLocks.push_back(newLock);
}

void BufferLockManager::wait(GLsync* _syncObj)
{
	const GLuint64 kOneSecondInNanoSeconds = 1000000000;
	if( m_bCPUUpdates ) 
	{
		GLbitfield waitFlags = 0;
		GLuint64 waitDuration = 0;
		while (1) 
		{
			GLenum waitRet = glClientWaitSync(*_syncObj, waitFlags, waitDuration);
			if (waitRet == GL_ALREADY_SIGNALED || waitRet == GL_CONDITION_SATISFIED) {
				return;
			}

			if (waitRet == GL_WAIT_FAILED) 
			{
				BB_ASSERT(!"Not sure what to do here. Probably raise an exception or something.");
				return;
			}

			// After the first time, need to start flushing, and wait for a looong time.
			waitFlags = GL_SYNC_FLUSH_COMMANDS_BIT;
			waitDuration = kOneSecondInNanoSeconds;
		}
	} 
	else 
	{
		glWaitSync(*_syncObj, 0, GL_TIMEOUT_IGNORED);
	}
}

void BufferLockManager::cleanup(BufferLock* _bufferLock)
{
	glDeleteSync(_bufferLock->m_SyncObj);
}

} /*namespace bigball*/