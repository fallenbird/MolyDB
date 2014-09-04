
#ifndef _JK_MEMMGR_253BEA9_H_
#define _JK_MEMMGR_253BEA9_H_

#include <new>
#include "JK_Lock.h"
#include "JK_Assert.h"



class JK_MEMMGR
{
public:
	JK_MEMMGR()
	{
		//m_iAllocSize = 0;
		//m_iFreeSize  = 0;
	}

	~JK_MEMMGR()
	{

	}

	static void* JK_Malloc( long iSize )
	{
		return malloc( iSize );
		//m_iAllocSize += iSize;
	}

	static void JK_Free( void* pMem )
	{
		free( pMem );
	}

	template< typename T>
	static void* JK_New( long iSize )
	{
		T* pX = (T*)JK_Malloc(iSize);
		if ( !pX )
		{
			JK_ASSERT( "mem exhausted!\n" );
			return NULL;
		}
		pX->T::T();
		return pX;
	}


	template< typename T>
	static void JK_Delete( void* pMem )
	{
		JK_ASSERT( pMem );
		((T*)pMem)->T::~T();
		JK_Free( pMem );
		pMem = NULL;
	}

private:
};


#define JK_MALLOC( x ) (JK_MEMMGR::JK_Malloc( x ))
#define JK_FREE( x ) (JK_MEMMGR::JK_Free( x ))

#define JK_NEW( x ) new( JK_MEMMGR::JK_Malloc(sizeof(x)))x
#define JK_DELETE( t,x ) JK_MEMMGR::JK_Delete<t>(x)

//#define JK_NEW( type ) {type* pX=(type*)JK_MEMMGR::JK_Malloc( sizeof(type) );if( pX){pX->type::type();}


#endif
