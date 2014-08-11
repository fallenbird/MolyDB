
#ifndef _JK_MEMMGR_253BEA9_H_
#define _JK_MEMMGR_253BEA9_H_

#include "JK_Lock.h"
#include <assert.h>



class JK_MEMMGR
{
public:
	JK_MEMMGR()
	{

	}

	~JK_MEMMGR()
	{

	}

	static void* JK_Malloc( long iSize )
	{
		return malloc( iSize );
	}


	static void JK_Free( void* pMem )
	{
		free( pMem );
	}

private:
	
};


#define JK_MALLOC( x ) (JK_MEMMGR::JK_Malloc( x ))
#define JK_FREE( x ) (JK_MEMMGR::JK_Free( x ))
#define JK_NEW( x ) (JK_MEMMGR::JK_Malloc( sizeof(x) ))

#endif
