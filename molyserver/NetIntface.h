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
	int initInterface( char* szIP, unsigned short usPort );
	void RunInterface();
	void CloseInterface();

private:
	IOCPServer m_IOCPServer;

};

#endif