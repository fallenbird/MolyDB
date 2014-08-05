#include <winsock2.h>
#include <conio.h>
#include <iostream>
#include <assert.h>
#include <map>
#include "IOCPServer.h"
#include "IoHandler.h"
#include "SessionList.h"


//=============================================================================================================================
/**
	@remarks
			发包线程函数
	@param	param
			IOCP 指针
*/
//=============================================================================================================================
unsigned __stdcall send_thread( LPVOID param )
{
	IOCPServer *pIOCPServer = (IOCPServer*)param;
	
	IOHANDLER_MAP_ITER	it;
	
	while( !pIOCPServer->m_bShutdown )
	{
		Sleep( 10 );

		for( it = pIOCPServer->m_mapIoHandlers.begin(); it != pIOCPServer->m_mapIoHandlers.end(); ++it )
		{
			it->second->ProcessSend();
		}
	}

	return 0;
}

IOCPServer::IOCPServer()
{
	m_bShutdown				= FALSE;
	m_hSendThread			= NULL;
}

IOCPServer::~IOCPServer()
{
	if( !m_bShutdown )		Shutdown();

	if( m_hSendThread )		CloseHandle( m_hSendThread );
	
	WSACleanup();
}

//=============================================================================================================================
/**
	@remarks
			初始化
	@param	lpDesc
			初始设定值
	@retval	BOOL
			成功返回TRUE 否则 返回 FALSE
*/
//=============================================================================================================================
BOOL IOCPServer::Init( LPIOHANDLER_DESC lpDesc, DWORD dwNumberofIoHandlers )
{
	// 初始化网络
	if( !InitWinsock() )	return FALSE;

	for( DWORD i = 0; i < dwNumberofIoHandlers; ++i )
	{
		CreateIoHandler( lpDesc + i );
	}

	// 创建Send线程  
	unsigned threadID;
	m_hSendThread = (HANDLE)_beginthreadex( NULL, 0, send_thread, (LPVOID)this, 0, &threadID );

	return TRUE;
}

VOID IOCPServer::CreateIoHandler( LPIOHANDLER_DESC lpDesc )
{
	IoHandler *pIoHandler = new IoHandler;

	pIoHandler->Init( this, lpDesc );

	m_mapIoHandlers.insert( IOHANDLER_MAP_PAIR( pIoHandler->GetKey(), pIoHandler ) );
}


VOID IOCPServer::Shutdown()
{
	m_bShutdown = TRUE;

	// Send 等待线程事件
	WaitForSingleObject( m_hSendThread, INFINITE );

	// IoHandler关闭
	IOHANDLER_MAP_ITER		it;
	IoHandler				*pIoHandler;

	for( it = m_mapIoHandlers.begin(); it != m_mapIoHandlers.end(); ++it )
	{
		pIoHandler = it->second;
		pIoHandler->Shutdown();
		delete pIoHandler;
	}
	m_mapIoHandlers.clear();
}

//=============================================================================================================================
/**
	@remarks
*/
//=============================================================================================================================
VOID IOCPServer::Update()
{
	IOHANDLER_MAP_ITER it;

	for( it = m_mapIoHandlers.begin(); it != m_mapIoHandlers.end(); ++it )
	{
		it->second->Update();
	}
}

BOOL IOCPServer::StartListen( DWORD dwIoHandlerKey, char *pIP, WORD wPort )
{
	IOHANDLER_MAP_ITER it = m_mapIoHandlers.find( dwIoHandlerKey );

	assert( it != m_mapIoHandlers.end() );

	return it->second->StartListen( pIP, wPort );
}

//=============================================================================================================================
/**
	@remarks
			- connect
	@param	dwIoHandlerKey
			- 
			- 
	@param	szIP
			连接 IP
	@param	wPort
			连接 端口
	@retval	DWORD
			返回 Session索引
*/
//=============================================================================================================================
DWORD IOCPServer::Connect( DWORD dwIoHandlerKey, NetworkObject *pNetworkObject, char *pszIP, WORD wPort,  DWORD dwSendBufferSize, DWORD dwRecvBufferSize, DWORD dwMaxPacketSize )
{
	if( pNetworkObject == NULL ) return 0;

	IOHANDLER_MAP_ITER it = m_mapIoHandlers.find( dwIoHandlerKey );

	assert( it != m_mapIoHandlers.end() );

	return it->second->Connect( pNetworkObject, pszIP, wPort, dwSendBufferSize, dwRecvBufferSize, dwMaxPacketSize );
}

BOOL IOCPServer::IsListening( DWORD dwIoHandlerKey )
{
	IOHANDLER_MAP_ITER it = m_mapIoHandlers.find( dwIoHandlerKey );

	assert( it != m_mapIoHandlers.end() );

	return it->second->IsListening();
}

DWORD IOCPServer::GetNumberOfConnections( DWORD dwIoHandlerKey )
{
	IOHANDLER_MAP_ITER it = m_mapIoHandlers.find( dwIoHandlerKey );

	assert( it != m_mapIoHandlers.end() );

	return it->second->GetNumberOfConnections();
}

//=============================================================================================================================
/**
	@remarks
			初始化网络
*/
//=============================================================================================================================
BOOL IOCPServer::InitWinsock()
{
	WSADATA wsaData;

	WSAStartup( MAKEWORD( 2, 2 ), &wsaData );

	if( wsaData.wVersion != MAKEWORD( 2, 2 ) )
	{
		WSACleanup();

		return FALSE;
	}
	
	return TRUE;
}
