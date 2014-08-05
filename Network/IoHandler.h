#ifndef _IOHANDLER_H_
#define _IOHANDLER_H_

#include <windows.h>
#include <process.h>
#include <vector>

using namespace std;

#define MAX_IO_WORKER_THREAD	16
#define EXTRA_ACCEPTEX_NUM		10

class Session;
class SessionPool;
class Acceptor;
class Connector;
class SessionList;
class IOCPServer;
class NetworkObject;
struct tagIOHANDLER_DESC;

typedef tagIOHANDLER_DESC IOHANDLER_DESC, *LPIOHANDLER_DESC;

typedef NetworkObject* (*fnCallBackCreateAcceptedObject)();
typedef VOID (*fnCallBackDestroyAcceptedObject)( NetworkObject *pNetworkObject );
typedef VOID (*fnCallBackDestroyConnectedObject)( NetworkObject *pNetworkObject );
typedef VOID (*fnCallBackLog)( NetworkObject pNetworkObject, char *pszLog );

//=============================================================================================================================
/// Io句柄类
//=============================================================================================================================
class IoHandler
{
	friend unsigned __stdcall io_thread( LPVOID param );
	friend unsigned __stdcall send_thread( LPVOID param );
	friend unsigned __stdcall accept_thread( LPVOID param );
	friend unsigned __stdcall connect_thread( LPVOID param );

public:
	IoHandler();
	~IoHandler();

	VOID				Init( IOCPServer *pIOCPServer, LPIOHANDLER_DESC lpDesc );
	BOOL				StartListen( char *pIP, WORD wPort );
	VOID				Update();
	VOID				Shutdown();
	DWORD				Connect( NetworkObject *pNetworkObject, char *pszIP, WORD wPort, DWORD dwSendBufferSize, DWORD dwRecvBufferSize, DWORD dwMaxPacketSize );
	BOOL				IsListening();
	inline DWORD		GetNumberOfConnections()		{ return m_numActiveSessions; }
	inline DWORD		GetKey() { return m_dwKey; }

private:
	Session*			AllocAcceptSession();
	Session*			AllocConnectSession();
	VOID				FreeSession( Session *pSession );
	VOID				ReuseSession( Session *pSession );
	VOID				ProcessConnectSuccessList();
	VOID				ProcessConnectFailList();
	VOID				ProcessAcceptedSessionList();
	VOID				ProcessActiveSessionList();
	VOID				KickDeadSessions();
	VOID				ProcessSend();
	VOID				KickAllSessions();

	IOCPServer			*m_pIOCPServer;
	SessionPool			*m_pAcceptSessionPool;
	SessionPool			*m_pConnectSessionPool;
	Acceptor			*m_pAcceptor;
	Connector			*m_pConnector;
	SessionList			*m_pActiveSessionList;
	SessionList			*m_pAcceptedSessionList;
	SessionList			*m_pConnectSuccessList;
	SessionList			*m_pConnectFailList;
	SessionList			*m_pTempList;
	BOOL				m_bShutdown;

	DWORD				m_dwKey;
	HANDLE				m_hIOCP;							// IOCP 句柄
	DWORD				m_numIoThreads;						// IO 线程个数
	HANDLE				m_hIoThread[MAX_IO_WORKER_THREAD];	// IO 线程 句柄
	DWORD				m_dwMaxPacketSize;
	DWORD				m_numActiveSessions;
	DWORD				m_dwMaxAcceptSession;

	// 连接回调函数指针
	fnCallBackCreateAcceptedObject		m_fnCreateAcceptedObject;
	fnCallBackDestroyAcceptedObject		m_fnDestroyAcceptedObject;
	fnCallBackDestroyConnectedObject	m_fnDestroyConnectedObject;
};

#endif


