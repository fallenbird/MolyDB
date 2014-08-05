#ifndef _MSWINSOCKUTIL_H_
#define _MSWINSOCKUTIL_H_

class MsWinsockUtil
{
public:
	static void	LoadExtensionFunction( SOCKET ActiveSocket );		//dll

	static LPFN_ACCEPTEX				m_lpfnAccepteEx;					//AcceptEx 函数指针
	static LPFN_TRANSMITFILE			m_lpfnTransmitFile;					//TransMitFile 函数指针
	static LPFN_GETACCEPTEXSOCKADDRS	m_lpfnGetAcceptExSockAddrs;			//GetAcceptExSockaddrs 函数指针
	static LPFN_DISCONNECTEX			m_lpfnDisconnectEx;					//DisconnectEx 函数指针

private:
	static bool LoadExtensionFunction( SOCKET ActiveSocket,	GUID FunctionID, void **ppFunc);
};

#endif