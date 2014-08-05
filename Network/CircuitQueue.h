#ifndef _CIRCUITQUEUE_H_
#define _CIRCUITQUEUE_H_

#include <windows.h>
#include <stdio.h>
#include <assert.h>

//=============================================================================================================================
/// 循环队列
//=============================================================================================================================
template<typename T>
class CircuitQueue
{
public:
	CircuitQueue() : m_pData( NULL ), m_nLength( 0 ), m_nSize( 0 ), m_nHead( 0 ), m_nTail( 0 )
	{
		InitializeCriticalSection( &m_cs );		
	}
	
	virtual ~CircuitQueue()
	{
		if( m_pData ) delete [] m_pData;
		DeleteCriticalSection( &m_cs );
	}
	
//=============================================================================================================================
/**
	@remarks
			创建函数
	@param	nSize
			个数
	@param	nExtraSize
			过渡数据区大小
*/
//=============================================================================================================================
	void Create( int nSize, int nExtraSize = 0 )
	{
		EnterCriticalSection( &m_cs );
		if( m_pData ) delete [] m_pData;

		m_pData			= new T[nSize + nExtraSize];
		m_nSize			= nSize;
		m_nExtraSize	= nExtraSize;
		LeaveCriticalSection( &m_cs );
	}

//=============================================================================================================================
/**
	@remarks
			清空
*/
//=============================================================================================================================
	inline void Clear()
	{
		EnterCriticalSection( &m_cs );

		m_nLength       = 0;
		m_nHead         = 0;
		m_nTail         = 0;

		LeaveCriticalSection( &m_cs );
	}
	
	/// 取得剩余空间大小
	inline int      GetSpace()
	{
		int iRet;

		EnterCriticalSection( &m_cs );
		iRet = m_nSize - m_nLength;		
		LeaveCriticalSection( &m_cs );

		return iRet;
	}

	/// 取得使用空间大小
	inline int      GetLength()
	{
		int iRet;

		EnterCriticalSection( &m_cs );
		iRet = m_nLength;
		LeaveCriticalSection( &m_cs );

		return iRet;
	}

	/// 取得从头指针到最后的空间大小
	inline int      GetBackDataCount()
	{
		int iRet;

		EnterCriticalSection( &m_cs );
		iRet = m_nSize - m_nHead;
		LeaveCriticalSection( &m_cs );

		return iRet;
	}

	/// 取得可读头指针
	inline T*       GetReadPtr()
	{
		T *pRet;

		EnterCriticalSection( &m_cs );
		pRet = m_pData + m_nHead;

		int nSplitFirstDataCount;
		if( m_nHead > m_nTail && ( nSplitFirstDataCount = m_nSize - m_nHead ) < m_nExtraSize )
		{
			memcpy( m_pData + m_nSize, m_pData, sizeof(T) * ( m_nExtraSize - nSplitFirstDataCount ) );
		}

		LeaveCriticalSection( &m_cs );

		return pRet;
	}

	/// 取得可写指针
	inline T*       GetWritePtr()
	{
		T *pRet;

		EnterCriticalSection( &m_cs );
		pRet = m_pData + m_nTail;
		LeaveCriticalSection( &m_cs );

		return pRet;
	}
	
	/// 取得可读区域大小
	inline int GetReadableLen()
	{
		int iRet;

		EnterCriticalSection( &m_cs );
		if( m_nHead == m_nTail )		iRet = GetLength() > 0 ? m_nSize - m_nHead: 0;
		else if( m_nHead < m_nTail )	iRet = m_nTail - m_nHead;
		else							 iRet = m_nSize - m_nHead;
		LeaveCriticalSection( &m_cs );

		return iRet;
	}
	
	/// 取得可写区域大小
	inline int GetWritableLen()
	{
		int iRet;

		EnterCriticalSection( &m_cs );
		if( m_nHead == m_nTail )		iRet = GetLength() > 0 ? 0 : m_nSize - m_nTail;
		else if( m_nHead < m_nTail )	iRet = m_nSize - m_nTail;
		else							iRet = m_nHead - m_nTail;
		LeaveCriticalSection( &m_cs );

		return iRet;
	}

//=============================================================================================================================
/**
	@remarks
			添加数据到队列中
	@param	pSrc
			数据指针
	@param	nSize
			数据长度
	@retval	BOOL
			返回 成功 TRUE 失败 FALSE
*/
//=============================================================================================================================
	inline BOOL Enqueue( T *pSrc, int nSize )
	{
		EnterCriticalSection( &m_cs );

		if( GetSpace() < nSize )
		{
			LeaveCriticalSection( &m_cs );
			return FALSE;
		}

		/*
		BOOL bCopyToExtraBuffer = ( ( m_nHead <= m_nTail ) && ( m_nSize - m_nTail < nSize ) );
		*/

		if( pSrc )
		{
			if( m_nHead <= m_nTail )
			{
				// 1. head
				int nBackSpaceCount = m_nSize - m_nTail;

				if( nBackSpaceCount >= nSize )  
				{
					// head
					memcpy( m_pData + m_nTail, pSrc, sizeof(T) * nSize );
				}
				else
				{
					memcpy( m_pData + m_nTail, pSrc, sizeof(T) * nBackSpaceCount );
					memcpy( m_pData, pSrc + nBackSpaceCount, sizeof(T) * ( nSize - nBackSpaceCount ) );
				}
			}
			else
			{
				// 2. head
				memcpy( m_pData + m_nTail, pSrc, sizeof(T) * nSize );
			}
		}

		/*
		if( bCopyToExtraBuffer )
		{
			// 第率狼 肋赴 单捞磐甫 咯盒狼 滚欺俊档 汗荤秦霖促.
			memcpy( m_pData + m_nSize, pSrc + m_nSize - m_nTail, sizeof(T) * ( nSize - ( m_nSize - m_nTail ) ) );
		}
		*/

		m_nTail		+= nSize;
		m_nTail		%= m_nSize;
		m_nLength	+= nSize;

		LeaveCriticalSection( &m_cs );

		return TRUE;
	}

//=============================================================================================================================
/**
	@remarks
			取得数据从队列中
	@param	pTar
			数据将要保存的指针
	@param	nSize
			取出的大小
	@retval	BOOL
			成功返回 TRUE 否则 返回 FALSE
*/
//=============================================================================================================================
	inline BOOL Dequeue( T *pTar, int nSize )
	{    
		EnterCriticalSection( &m_cs );

		if( !Peek( pTar, nSize ) )
		{
			LeaveCriticalSection( &m_cs );
			return FALSE;
		}

		m_nHead		+= nSize;
		m_nHead		%= m_nSize;
		m_nLength	-= nSize;

		LeaveCriticalSection( &m_cs );

		return TRUE;
	}

//=============================================================================================================================
/**
	@remarks
			从队列中读取一串内容
	@param	pTar
			取出指针
	@param	nSize
			取出大小
	@retval	BOOL
			成功返回 TRUE 否则 返回 FALSE
*/
//=============================================================================================================================
	inline BOOL Peek( T *pTar, int nSize )
	{
		EnterCriticalSection( &m_cs );

		if( m_nLength < nSize )
		{
			LeaveCriticalSection( &m_cs );
			return FALSE;
		}

		// 取得数据
		if( pTar != NULL )
		{
			if( m_nHead < m_nTail )
			{
				// 1. head
				memcpy( pTar, m_pData + m_nHead, sizeof(T) * nSize );
			}
			else
			{
				// 2. head
				if( GetBackDataCount() >= nSize )
				{
						memcpy( pTar, m_pData + m_nHead, sizeof(T) * nSize );                           
				}
				else
				{
					memcpy( pTar, m_pData + m_nHead, sizeof(T) * GetBackDataCount() );
					memcpy( pTar + GetBackDataCount(), m_pData, sizeof(T) * ( nSize - GetBackDataCount() ) );
				}
			}
		}

		LeaveCriticalSection( &m_cs );

		return TRUE;
	}

//=============================================================================================================================
/**
	@remarks
			拷贝头数据到过渡区
	@param	nSize
			将要拷贝的数据大小
*/
//=============================================================================================================================
	inline void CopyHeadDataToExtraBuffer( int nSize )
	{
		assert( nSize <= m_nExtraSize );

		EnterCriticalSection( &m_cs );

		memcpy( m_pData + m_nSize, m_pData, nSize );

		LeaveCriticalSection( &m_cs );
	}

protected:
	CRITICAL_SECTION	m_cs;
	T					*m_pData;			/// 数据池指针
	int					m_nLength;			/// 使用长度
	int					m_nSize;			/// 总长度 
	int					m_nHead;			/// 头位置
	int					m_nTail;			/// 尾位置
	int					m_nExtraSize;		/// 过渡数据区大小
};

#endif










