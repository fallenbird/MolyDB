#include "IOCPServer.h"
#include "NetworkObject.h"
#include "ClientAgent.h"

#include "JK_Singleton.h"


#ifdef _DEBUG
#pragma comment( lib, "Network_d.lib" )
#else
#pragma comment( lib, "Network.lib" )
#endif

//-------------------------------------------------------------------------------------------------
// User Class
//-------------------------------------------------------------------------------------------------
class client : public NetworkObject
{
protected:
	virtual void	OnAccept( DWORD dwNetworkIndex ) {};
	virtual void	OnDisconnect() {};
	virtual	void	OnRecv( BYTE *pMsg, WORD wSize ) { Send( pMsg, wSize ); }
	virtual void	OnConnect( BOOL bSuccess, DWORD dwNetworkIndex ) {}    
};

//-------------------------------------------------------------------------------------------------
// Callback Functions
//-------------------------------------------------------------------------------------------------
NetworkObject* CreateAcceptedObject() {	return new client; }
void DestroyAcceptedObject( NetworkObject *pNetworkObject ) { delete pNetworkObject; }
void DestroyConnectedObject( NetworkObject *pNetworkObject ) {delete pNetworkObject; }


const DWORD CLIENT_IOHANDLER_KEY = 0;

class NetInterface : public JK_Singleton<NetInterface>
{

public:
	NetInterface()
	{

	};
	~NetInterface();

public:
	int initInterface( char* szIP, unsigned short usPort )
	{
		IOHANDLER_DESC desc;
		desc.dwIoHandlerKey				= CLIENT_IOHANDLER_KEY;
		desc.dwMaxAcceptSession			= 1000;
		desc.dwMaxConnectSession		= 0;
		desc.dwSendBufferSize			= 60000;
		desc.dwRecvBufferSize			= 60000;
		desc.dwTimeOut					= 30000;
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

