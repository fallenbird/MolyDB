#ifndef _JK_NETINTERFACE_J5U3X0X8_H_
#define _JK_NETINTERFACE_J5U3X0X8_H_

#include "IOCPServer.h"
#include "JK_Singleton.h"
#include "ClientAgent.h"

const DWORD MASTER_IOHANDLER_KEY	= 0;
const DWORD SLAVE_IOHANDLER_KEY		= 1;

class NetInterface : public JK_Singleton<NetInterface>
{

public:
	NetInterface();
	virtual ~NetInterface();

private:
	static NetworkObject* CreateAcceptedObject();
	static void DestroyAcceptedObject( NetworkObject *pNetworkObject );
	static void DestroyConnectedObject( NetworkObject *pNetworkObject );
	void ConnectMaster();

public:
	int initInterface( int iMaster, char* strMasterIP, unsigned short usMasterPort, unsigned short uslocalPort );
	void RunInterface();
	void CloseInterface();

private:
	IOCPServer m_IOCPServer;

};

#endif