
#ifndef _JK_APPENDCMDQUEUE_SXF13690_H_
#define _JK_APPENDCMDQUEUE_SXF13690_H_

#include "JK_LockFreeQueue.h"

class AppendCmdQueue 
{
public:
	static void Enqueue( void* dataptr );
	static void* Dequeue();

private:
	static JK_LockFreeQueue m_afDataQueue;

};

#endif