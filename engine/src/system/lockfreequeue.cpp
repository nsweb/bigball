

#include "../bigball.h"
#include "lockfreequeue.h"

TaskNode LockFreeQueue::ms_oNodeNull;

LockFreeQueue::LockFreeQueue()
{
	ms_oNodeNull.m_pNextNode = NULL;
	m_oNodeTail.m_pNode = &ms_oNodeNull;
	m_oNodeHead.m_pNode = &ms_oNodeNull;

}
LockFreeQueue::~LockFreeQueue()
{

}

LockFreeQueue::NodeCAS::NodeCAS()
{
	m_pNode = NULL;
	m_pCASCounter = 0;
}

void LockFreeQueue::Push( TaskNode* _pItem )
{
	NodeCAS oTail, oNewTail;
	//
	_pItem->m_pNextNode = NULL;
	//
	while( 1 )
	{
		ThreadTools::InterlockedExchange( &oTail , *(s64*)&m_oNodeTail );

		// if tail next if 0 replace it with new item
		if( ThreadTools::InterlockedCompareExchange( &oTail.m_pNode->m_pNextNode, (s32)_pItem, 0 ) != 0 ) 
			break;

		// else push tail back until it reaches end
		oNewTail.m_pCASCounter = oTail.m_pCASCounter+1;
		oNewTail.m_pNode = oTail.m_pNode->m_pNextNode;
		ThreadTools::InterlockedCompareExchange2( &m_oNodeTail, (s32)oTail.m_pNode->m_pNextNode, oTail.m_pCASCounter+1, (s32)oTail.m_pNode, oTail.m_pCASCounter );

		ThreadTools::SpinLoop();
	}

	// try and change tail pointer (it is also fixed in Pop)
	oNewTail.m_pCASCounter = oTail.m_pCASCounter+1;
	oNewTail.m_pNode = _pItem;
	ThreadTools::InterlockedCompareExchange2( &m_oNodeTail, (s32)_pItem, oTail.m_pCASCounter+1, (s32)oTail.m_pNode, oTail.m_pCASCounter );
}

TaskNode* LockFreeQueue::Pop()
{
	NodeCAS oHead, oTail;
	//
	while( 1 )
	{
		ThreadTools::InterlockedExchange( &oHead , *(s64*)&m_oNodeHead );
		ThreadTools::InterlockedExchange( &oTail , *(s64*)&m_oNodeTail );
		//
		TaskNode* pNodeNext = oHead.m_pNode->m_pNextNode;

		// is queue empty
		if( oHead.m_pNode == oTail.m_pNode )
		{
			// is it really empty
			if( !pNodeNext ) 
				return 0;

			// or is just tail falling behind...
			ThreadTools::InterlockedCompareExchange2( &m_oNodeTail, (s32)pNodeNext, oTail.m_pCASCounter+1 , (int)oTail.m_pNode, oTail.m_pCASCounter );
		}
		else
		{
			if( ThreadTools::InterlockedCompareExchange2( &m_oNodeHead, (s32)pNodeNext, oHead.m_pCASCounter+1 , (int)oHead.m_pNode, oHead.m_pCASCounter ) ) 
				return pNodeNext;
		}
		//
		ThreadTools::SpinLoop();
	}
}