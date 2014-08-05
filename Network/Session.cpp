#include <iostream>
#include <assert.h>
#include <winsock2.h>
#include <mswsock.h>
#include "Session.h"
#include "SessionPool.h"
#include "SendBuffer.h"
#include "RecvBuffer.h"
#include "NetBase.h"
#include "NetworkObject.h"
#include "MsWinsockUtil.h"

using namespace std;

//=============================================================================================================================
/**
	@remarks
			构造
			设定 发送缓冲大小，接收缓存大小，最大包大小，最大等待时间
	@param	dwSendBufferSize
			发送缓冲大小
	@param	dwRecvBufferSize
			接收缓存大小
	@param	dwTimeOut
			最大等待时间
*/
//=============================================================================================================================
Session::Session( DWORD dwSendBufferSize, DWORD dwRecvBufferSize, DWORD dwMaxPacketSize, DWORD dwTimeOut )
{
	m_pSendBuffer = new SendBuffer;
	m_pSendBuffer->Create( dwSendBufferSize, dwMaxPacketSize );

	m_pRecvBuffer = new RecvBuffer;
	m_pRecvBuffer->Create( dwRecvBufferSize, dwMaxPacketSize );

	m_dwTimeOut			= dwTimeOut;
	m_socket			= INVALID_SOCKET;
	m_bAcceptSocket		= FALSE;
}

Session::~Session()
{
	CloseSocket();

	if( m_pSendBuffer )		delete m_pSendBuffer;
	if( m_pRecvBuffer )		delete m_pRecvBuffer;
}

//=============================================================================================================================
/**
	@remarks
			初始化 清空已有信息。
*/
//=============================================================================================================================
VOID Session::Init()
{
	m_pSendBuffer->Clear();
	m_pRecvBuffer->Clear();

	ResetKillFlag();

	m_bDisconnectOrdered = FALSE;
}

VOID Session::Reuse( HANDLE hIOCP )
{
	//Disconnect清空 ovl IO 数据
	ZeroMemory( &m_disconnectIoData, sizeof(OVERLAPPEDEX) );
	m_disconnectIoData.dwOperationType	= DISCONNECT_POSTED;

	// TransmitFile
	if( !MsWinsockUtil::m_lpfnTransmitFile( m_socket, 0, 0, 0, &m_disconnectIoData, 0, TF_DISCONNECT | TF_REUSE_SOCKET )
		&& WSAGetLastError() != WSA_IO_PENDING )
	{		
		CloseSocket();		
		PostQueuedCompletionStatus( hIOCP, 0, (ULONG_PTR)this, &m_disconnectIoData );
	}
}

//=============================================================================================================================
/**
	@remarks
			发送数据函数
	@param	pMsg
			数据
	@param	wSize
			长度
	@retval	BOOL
			成功返回 TRUE 否则 返回 FALSE
*/
//=============================================================================================================================
BOOL Session::Send( BYTE *pMsg, WORD wSize )
{
	PACKET_HEADER header;

	// 填充长度
	header.size = wSize;
	
	// 写入head + 数据
	if( m_pSendBuffer->Write( &header, pMsg ) == FALSE )
	{
		OnLogString( "写入发送缓冲失败." );
		Remove();
		return FALSE;
	}

	return TRUE;
}

BOOL Session::SendEx( DWORD dwNumberOfMessages, BYTE **ppMsg, WORD *pwSize )
{
	assert( !IsBadReadPtr( ppMsg, sizeof(ppMsg) * dwNumberOfMessages ) );
	assert( !IsBadReadPtr( pwSize, sizeof(pwSize) * dwNumberOfMessages ) );

	PACKET_HEADER header;

	// 填充长度
	header.size = 0;
	DWORD i;
	for( i = 0; i < dwNumberOfMessages; ++i )
	{
		header.size += pwSize[i];
	}

	// 写入head
	if( !m_pSendBuffer->Write( (BYTE*)&header, sizeof(PACKET_HEADER) ) )
	{
		OnLogString( "写入发送缓冲失败." );
		Remove();
		return FALSE;
	}

	// 写入数据(dwNumberOfMessages为数据串中包个数)
	for( i = 0; i < dwNumberOfMessages; ++i )
	{
		if( !m_pSendBuffer->Write( ppMsg[i], pwSize[i] ) )
		{
			OnLogString( "写入发送缓冲失败." );
			Remove();
			return FALSE;
		}
	}

	return TRUE;
}

BOOL Session::ProcessRecvdPacket( DWORD dwMaxPacketSize )
{
	BYTE			*pPacket;
	PACKET_HEADER	*pHeader;

	// 循环取得接受包，并调用拥有该包的单位OnRecv函数
	while( pPacket = GetRecvBuffer()->GetFirstPacketPtr() )
	{
		pHeader = (PACKET_HEADER*)pPacket;

		if( pHeader->size <= 0 )
		{
			OnLogString( "错误的包数据 size(%d) category(%d) protocol(%d)\n",
				pHeader->size, *( pPacket + sizeof(PACKET_HEADER) ), *( pPacket + sizeof(PACKET_HEADER) + 1 ) );
			return FALSE;
		}

		if( pHeader->size + sizeof(PACKET_HEADER) > dwMaxPacketSize )
		{
			OnLogString( "缓冲池错误 : Cur[%d]/Max[%d] Category[%d] Protocol[%d]", 
				pHeader->size + sizeof(PACKET_HEADER), dwMaxPacketSize, *( pPacket + sizeof(PACKET_HEADER) ), *( pPacket + sizeof(PACKET_HEADER) + 1 ) );

			OnLogString( "缓冲池错误 : Category[%d] Protocol[%d] Size[%d]", 
				m_iCategory, m_iProtocol, m_iSize );
			return FALSE;
		}

		m_pNetworkObject->OnRecv( pPacket + sizeof(PACKET_HEADER), pHeader->size );

		GetRecvBuffer()->RemoveFirstPacket( sizeof(PACKET_HEADER) + pHeader->size );

		ResetTimeOut();

		m_iCategory = *( pPacket + sizeof(PACKET_HEADER) );
		m_iProtocol = *( pPacket + sizeof(PACKET_HEADER) + 1 );
		m_iSize = pHeader->size + sizeof(PACKET_HEADER);
	}

	return TRUE;
}

//=============================================================================================================================
/**
	@remarks
			IOCP预 send
	@retval BOOL
			- 发送成功返回TRUE
			- send失败后 FALSE.
*/
//=============================================================================================================================
BOOL Session::PreSend()
{
	WSABUF wsabuf;

	if( GetSendBuffer()->GetSendParam( (BYTE**)&wsabuf.buf, (int&)wsabuf.len ) == FALSE )
	{
		return TRUE;
	}

	ZeroMemory( &m_sendIoData, sizeof(OVERLAPPEDEX) );

	m_sendIoData.dwOperationType = SEND_POSTED;
	
	int ret = WSASend( GetSocket(), &wsabuf, 1, &m_sendIoData.dwIoSize, m_sendIoData.dwFlags, &m_sendIoData, NULL );

	if( ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING )
	{
		return FALSE;
	}

	return TRUE;
}

//=============================================================================================================================
/**
	@remarks
			IOCP recv.
	@retval BOOL
			recv 失败 返回FALSE.
*/
//=============================================================================================================================
BOOL Session::PreRecv()
{
	WSABUF wsabuf;

	m_pRecvBuffer->GetRecvParam( (BYTE**)&wsabuf.buf, (int&)wsabuf.len );

	ZeroMemory( &m_recvIoData, sizeof(OVERLAPPEDEX) );

	m_recvIoData.dwOperationType = RECV_POSTED;

	int ret = WSARecv( GetSocket(), &wsabuf, 1, &m_recvIoData.dwIoSize, &m_recvIoData.dwFlags, &m_recvIoData, NULL ); 

	if( ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL Session::PreAccept( SOCKET listenSocket )
{
	DWORD	dwBytes		= 0;
	int		nLen		= 0;
	BYTE	*pRecvBuf	= NULL;

	if( GetSocket() == INVALID_SOCKET )
	{
		SetSocket( CreateSocket() );
	}

	Init();

	//Session 取得 Recv Ptr..
	GetRecvBuffer()->GetRecvParam( &pRecvBuf, nLen );

	//AcceptEx  清空ovl IO 
	ZeroMemory( &m_acceptIoData, sizeof(OVERLAPPEDEX) );
	m_acceptIoData.dwOperationType	= ACCEPT_POSTED;
	m_acceptIoData.pSession			= this;				//保存this ptr

	//printf("[ACCEPT][%d] Session::PreAccept, m_bRemove=%d\n", (int)GetSocket(), (int)m_bRemove);

	//AcceptEx
	int nRet = 	MsWinsockUtil::m_lpfnAccepteEx(	listenSocket, 
												GetSocket(), 
												pRecvBuf, 
												0, 
												sizeof(SOCKADDR_IN) + 16,
												sizeof(SOCKADDR_IN) + 16,
												&dwBytes,
												&m_acceptIoData );

	if( nRet == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING ) 
	{
		return FALSE;
	}

	return TRUE;
}

SOCKET Session::CreateSocket() 
{
	int nRet		= 0;
	int nZero		= 0;

	SOCKET newSocket = WSASocket( AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED ); 

	if( newSocket == INVALID_SOCKET ) 
	{
		return newSocket;
	}

	/*
	nZero	= 0;
	nRet	= setsockopt( newSocket, SOL_SOCKET, SO_SNDBUF, (char *)&nZero, sizeof(nZero) );

	if( nRet == SOCKET_ERROR ) 
	{
		return INVALID_SOCKET;
	}
	*/

	return newSocket;
}

VOID Session::BindNetworkObject( NetworkObject *pNetworkObject )
{
	m_pNetworkObject = pNetworkObject;
	pNetworkObject->SetSession( this );
}

VOID Session::UnbindNetworkObject()
{
	if( m_pNetworkObject == NULL )
	{
		return;
	}
	m_pNetworkObject->SetSession( NULL );

	m_pNetworkObject = NULL;
}

VOID Session::OnAccept()
{
	ResetKillFlag();

	ResetTimeOut();

	m_pNetworkObject->OnAccept( GetIndex() );

	//printf("[Session::OnAccept][%d] m_bRemove=%d\n", (int)GetSocket(), (int)m_bRemove);
}

VOID Session::OnConnect( BOOL bSuccess )
{
	Init();

	NetworkObject *pNetworkObject = m_pNetworkObject;

	if( !bSuccess )
	{		
		UnbindNetworkObject();
	}

	pNetworkObject->OnConnect( bSuccess, GetIndex() );
}

VOID Session::OnLogString( char *pszLog, ... )
{
	if( !m_pNetworkObject ) return;

	char		szBuffer[512] = "";
	va_list		pArguments;

	va_start( pArguments, pszLog );
	vsprintf_s( szBuffer, pszLog, pArguments );
	va_end( pArguments );

	printf( "%s(%s)\n", szBuffer, GetIP() );

	m_pNetworkObject->OnLogString( szBuffer );
}

VOID Session::Disconnect( BOOL bGracefulDisconnect )
{ 
	if( bGracefulDisconnect ) 
	{ 
		//printf("[REMOVE][%d] bGracefulDisconnect\n", (int)GetSocket()); 
		Remove(); 
	} 
	else 
	{
		m_bDisconnectOrdered = TRUE; 
	}
}
