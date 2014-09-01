#include "NetIntface.h"
#include "JK_MemMgr.h"
#include "ClientAgent.h"
#include "MasterConnector.h"
#include "JK_Console.h"

NetInterface::NetInterface()
{

}


NetInterface::~NetInterface()
{

}


NetworkObject* NetInterface::CreateAcceptedObject()
{
	ClientAgent* pAgent = JK_NEW(ClientAgent);
	pAgent->ClientAgent::ClientAgent();
	return pAgent;
}


void NetInterface::DestroyAcceptedObject( NetworkObject *pNetworkObject )
{
	JK_DELETE(ClientAgent, pNetworkObject);
}


void NetInterface::DestroyConnectedObject( NetworkObject *pNetworkObject )
{
	JK_DELETE(ClientAgent, pNetworkObject);
}


int NetInterface::initInterface( int iMaster, char* strMasterIP, unsigned short usMasterPort, unsigned short uslocalPort  )
{
	unsigned int dwIOKey;
	if ( 1== iMaster )
	{
		dwIOKey = SLAVE_IOHANDLER_KEY;
	}
	else
	{
		dwIOKey = MASTER_IOHANDLER_KEY;
	}
	IOHANDLER_DESC desc;
	desc.dwIoHandlerKey				= dwIOKey;
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

	if( !m_IOCPServer.StartListen( dwIOKey, NULL, uslocalPort ) )
	{
		DISPMSG_ERROR( "Listen failed! IP:%s Port:%d !", "null", uslocalPort );
		return 0;
	}

	if ( 1 != iMaster )
	{
		m_IOCPServer.Connect( dwIOKey, JK_NEW(MasterConnector), strMasterIP, usMasterPort,1024 * 1024*8,1024 * 1024 * 8,1024 * 64 );
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
