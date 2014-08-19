#ifndef _JK_NETINTERFACE_J5U3X0X8_H_
#define _JK_NETINTERFACE_J5U3X0X8_H_

#include "IOCPServer.h"
#include "JK_Singleton.h"
#include "ClientAgent.h"

const DWORD CLIENT_IOHANDLER_KEY = 0;

class NetInterface : public JK_Singleton<NetInterface>
{

public:
	NetInterface();
	~NetInterface();

private:
	static NetworkObject* CreateAcceptedObject();
	static void DestroyAcceptedObject( NetworkObject *pNetworkObject );
	static void DestroyConnectedObject( NetworkObject *pNetworkObject );

public:
	int initInterface( char* szIP, unsigned short usPort )
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
			printf( "IOCP initialize failed!" );
			return 0;
		}

		if( !m_IOCPServer.StartListen( CLIENT_IOHANDLER_KEY, szIP, usPort ) )
		{
			printf( "Listen failed! IP:%s Port:%d !", szIP, usPort );
			return 0;
		}
		printf( "Server started!\n" );
		return 1;
	}


	void RunInterface()
	{
		m_IOCPServer.Update();
	}

	void CloseInterface()
	{
		printf( "Server is terminated...\n" );
		m_IOCPServer.Shutdown();
	}

private:
	IOCPServer m_IOCPServer;

};

#endif