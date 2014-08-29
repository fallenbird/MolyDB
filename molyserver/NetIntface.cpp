#include "NetIntface.h"
#include "JK_MemMgr.h"
#include "ClientAgent.h"
#include "JK_Console.h"

NetInterface::NetInterface()
{

}


NetInterface::~NetInterface()
{

}


NetworkObject* NetInterface::CreateAcceptedObject()
{
	//return (NetworkObject*)JK_NEW(ClientAgent);
	return new ClientAgent();
}


void NetInterface::DestroyAcceptedObject( NetworkObject *pNetworkObject )
{
	delete pNetworkObject;
}


void NetInterface::DestroyConnectedObject( NetworkObject *pNetworkObject )
{
	delete pNetworkObject;
}


int NetInterface::initInterface( char* szIP, unsigned short usPort )
{
	IOHANDLER_DESC desc;
	desc.dwIoHandlerKey				= CLIENT_IOHANDLER_KEY;
	desc.dwMaxAcceptSession			= 1000;
	desc.dwMaxConnectSession		= 0;
	desc.dwSendBufferSize			= 60000;
	desc.dwRecvBufferSize			= 60000;
	desc.dwTimeOut					= 1000 * 60 * 20;
	desc.dwMaxPacketSize			= 4096;
	desc.dwNumberOfIoThreads		= 1;
	desc.dwNumberOfAcceptThreads	= 1;
	desc.dwNumberOfConnectThreads	= 0;
	desc.fnCreateAcceptedObject		= CreateAcceptedObject;
	desc.fnDestroyAcceptedObject	= DestroyAcceptedObject;
	desc.fnDestroyConnectedObject	= DestroyConnectedObject;

	if( !m_IOCPServer.Init( &desc, 1 ) )
	{
		DISPMSG_ERROR( "IOCP initialize failed!" );
		return 0;
	}

	if( !m_IOCPServer.StartListen( CLIENT_IOHANDLER_KEY, szIP, usPort ) )
	{
		printf( "Listen failed! IP:%s Port:%d !", szIP, usPort );
		return 0;
	}
	DISPMSG_SUCCESS( "Server started!\n" );
	return 1;
}


void NetInterface::RunInterface()
{
	m_IOCPServer.Update();
}


void NetInterface::CloseInterface()
{
	DISPMSG_ERROR( "Server is terminated...\n" );
	m_IOCPServer.Shutdown();
}
