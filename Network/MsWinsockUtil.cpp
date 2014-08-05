#include <winsock2.h>
#include <mswsock.h>
#include "MsWinsockUtil.h"

void MsWinsockUtil::LoadExtensionFunction( SOCKET ActiveSocket )
{
	//AcceptEx
	GUID acceptex_guid = WSAID_ACCEPTEX;
	LoadExtensionFunction( ActiveSocket, acceptex_guid, (void**) &m_lpfnAccepteEx);

	//TransmitFile
	GUID transmitfile_guid = WSAID_TRANSMITFILE;
	LoadExtensionFunction( ActiveSocket, transmitfile_guid, (void**) &m_lpfnTransmitFile);

	//GetAcceptExSockaddrs
	GUID guidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
	LoadExtensionFunction( ActiveSocket, guidGetAcceptExSockaddrs, (void**) &m_lpfnGetAcceptExSockAddrs);

	//DisconnectEx 
	GUID guidDisconnectEx = WSAID_DISCONNECTEX;
	LoadExtensionFunction( ActiveSocket, guidDisconnectEx, (void**) &m_lpfnDisconnectEx );
}

bool MsWinsockUtil::LoadExtensionFunction( SOCKET ActiveSocket,	GUID FunctionID, void **ppFunc )
{
	DWORD	dwBytes = 0;

	if (0 != WSAIoctl(
		ActiveSocket, 
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&FunctionID,
		sizeof(GUID),
		ppFunc,
		sizeof(void *),
		&dwBytes,
		0,
		0))
	{
		return false;
	}

	return true;
}

LPFN_ACCEPTEX				MsWinsockUtil::m_lpfnAccepteEx				= NULL;
LPFN_TRANSMITFILE			MsWinsockUtil::m_lpfnTransmitFile			= NULL;
LPFN_GETACCEPTEXSOCKADDRS	MsWinsockUtil::m_lpfnGetAcceptExSockAddrs	= NULL;
LPFN_DISCONNECTEX			MsWinsockUtil::m_lpfnDisconnectEx			= NULL;