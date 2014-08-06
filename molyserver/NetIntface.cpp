#include "NetIntface.h"
#include "JK_MemMgr.h"

NetInterface::NetInterface()
{

}

NetInterface::~NetInterface()
{

}


NetworkObject* NetInterface::CreateAcceptedObject()
{
	return (NetworkObject*)JKNew(client);
}

void NetInterface::DestroyAcceptedObject( NetworkObject *pNetworkObject )
{
	delete pNetworkObject;
}

void NetInterface::DestroyConnectedObject( NetworkObject *pNetworkObject )
{
	delete pNetworkObject;
}
