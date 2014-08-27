#include "AppendCmdQueue.h"


JK_LockFreeQueue AppendCmdQueue::m_afDataQueue;


void AppendCmdQueue::Enqueue( void* dataptr )
{
	m_afDataQueue.Enqueue( dataptr );
}


void* AppendCmdQueue::Dequeue()
{
	return m_afDataQueue.Dequeue();
}

