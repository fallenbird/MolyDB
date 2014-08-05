#include <assert.h>
#include "SessionPool.h"
#include "Session.h"
#include "SessionList.h"

//=============================================================================================================================
/**
	@remarks
			初始化
	@param	nSize
			个数 发送缓冲 接收缓冲 包最大字节数 最长等待时间 起始索引 是否用于连接响应
*/
//=============================================================================================================================
SessionPool::SessionPool( DWORD dwSize,
						  DWORD dwSendBufferSize,
						  DWORD dwRecvBufferSize, 
						  DWORD dwMaxPacketSize,
						  DWORD dwTimeOutTick,
						  DWORD dwIndexStart,
						  BOOL bAcceptSocket )
{
	m_pList = NULL;
	Create( dwSize, dwSendBufferSize, dwRecvBufferSize, dwMaxPacketSize, dwTimeOutTick, dwIndexStart, bAcceptSocket );
}

//=============================================================================================================================
/**
	@remarks
			释放链表
*/
//=============================================================================================================================
SessionPool::~SessionPool()
{
	if( m_pList ) delete m_pList;
}

//=============================================================================================================================
/**
	@remarks
			创建Session
	@param	dwSize
			最大支持Session数
*/
//=============================================================================================================================
VOID SessionPool::Create( DWORD dwSize, DWORD dwSendBufferSize, DWORD dwRecvBufferSize, DWORD dwMaxPacketSize, DWORD dwTimeOutTick, DWORD dwIndexStart, BOOL bAcceptSocket )
{
	m_dwMaxSize			= dwSize;

	// 创建链表管理
	m_pList = new SessionList;

	// 循环创建Session 并加入链表管理中
	Session *pSession;

	for( DWORD i = 0; i < dwSize; ++i )
	{
		pSession = new Session( dwSendBufferSize, dwRecvBufferSize, dwMaxPacketSize, dwTimeOutTick );

		pSession->SetIndex( dwIndexStart + i );

		if( bAcceptSocket )
		{
			pSession->SetAcceptSocketFlag();
		}

		m_pList->push_back( pSession );
	}
}

//=============================================================================================================================
/**
	@remarks
			从池中取出一块空余的Session
	@retval	Session*
			如果Session存在返回指针，否则返回NULL
*/
//=============================================================================================================================
Session* SessionPool::Alloc()
{
	m_pList->Lock();

	if( m_pList->empty() )
	{
		m_pList->Unlock();
		
		return NULL;
	}

	Session *pSession = m_pList->front();

	m_pList->pop_front();

	//printf( "New 技记钱(%x) 巢篮 技记(%d)\n", this, m_pList->size() ); 

	m_pList->Unlock();

	return pSession;
}

//=============================================================================================================================
/**
	@remarks
			释放
	@param	pSession
			将要释放的Session指针
*/
//=============================================================================================================================
VOID SessionPool::Free( Session* pSession )
{
	m_pList->Lock();

	assert( m_pList->size() < m_dwMaxSize );

	m_pList->push_back( pSession );

	m_pList->Unlock();
}

//=============================================================================================================================
/**
	@remarks
			取得当前使用的Session个数.
	@retval	int
			个数
*/
//=============================================================================================================================
int SessionPool::GetLength()
{
	m_pList->Lock();

	int size = (int)( m_pList->size() );

	m_pList->Unlock();

	return size;
}
