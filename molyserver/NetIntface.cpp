#include "NetIntface.h"
#include "JK_MemMgr.h"
#include "ClientAgent.h"


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
