#ifndef _SESSION_H_
#define _SESSION_H_


#pragma warning(disable:4996)

#include <windows.h>

#define SEND_POSTED			1
#define RECV_POSTED			2
#define ACCEPT_POSTED		3
#define DISCONNECT_POSTED	4

class SendBuffer;
class RecvBuffer;
class SessionPool;
class NetworkObject;
class Session;

/// OVERLAPPED 扩展
struct OVERLAPPEDEX : public OVERLAPPED
{
	DWORD		dwIoSize;
	DWORD		dwFlags;
	DWORD		dwOperationType;
	Session*	pSession;
};

//=============================================================================================================================
/// 数据单元类 
/**
	@remarks
			用于网络传输数据的投递,接收
	@par
			-
			- OnAccept:		
			- OnDisConnect:	
			- OnRecv:		
			- OnConnect:	
			- OnUpdate:		
	@note
			- ws2_32.lib
			- winsock2.h

*/
//=============================================================================================================================
class Session
{
	friend unsigned __stdcall io_thread( LPVOID param );
	friend class SessionPool;

public:
	/// dwTimeOut 空闲等待时间
	Session( DWORD dwSendBufferSize, DWORD dwRecvBufferSize, DWORD dwMaxPacketSize, DWORD dwTimeOut );
	virtual ~Session();

	VOID					Init();
	VOID					Reuse( HANDLE hIOCP );
	BOOL					Send( BYTE *pMsg, WORD wSize );
	BOOL					SendEx( DWORD dwNumberOfMessages, BYTE **ppMsg, WORD *pwSize );
	BOOL					PreSend();
	BOOL					PreRecv();
	BOOL					PreAccept( SOCKET listenSocket );
	SOCKET					CreateSocket();
	BOOL					ProcessRecvdPacket( DWORD dwMaxPacketSize );
	VOID					BindNetworkObject( NetworkObject *pNetworkObject );
	VOID					UnbindNetworkObject();
	VOID					OnAccept();
	VOID					OnConnect( BOOL bSuccess );
	VOID					OnLogString( char *pszLog, ... );

	inline VOID				SetSocket( SOCKET socket ) { m_socket = socket; }
	inline VOID				SetSockAddr( SOCKADDR_IN& sockaddr ) { m_sockaddr = sockaddr; }
	inline VOID				CloseSocket() { closesocket( m_socket ); m_socket = INVALID_SOCKET; }
	inline NetworkObject*	GetNetworkObject() { return m_pNetworkObject; }
	inline SendBuffer*		GetSendBuffer() { return m_pSendBuffer; }				///< 取得sendbuffer.
	inline RecvBuffer*		GetRecvBuffer() { return m_pRecvBuffer; }				///< 取得recvbuffer.
	inline SOCKET			GetSocket()		{ return m_socket; }					///< 取得socket.
	inline SOCKADDR*		GetSockAddr()	{ return (SOCKADDR*)&m_sockaddr; }		///< 取得socketaddr.
	inline char*			GetIP() { return inet_ntoa( m_sockaddr.sin_addr ); }	///< 取得IP
	inline u_short			GetPort() { return  m_sockaddr.sin_port; }

	inline DWORD			GetIdleTick() const	{ return m_dwTimeOut? m_dwLastSyncTick + m_dwTimeOut : 0; }
	inline BOOL				IsOnIdle()
		{ return m_dwTimeOut ? m_dwLastSyncTick + m_dwTimeOut < GetTickCount() : FALSE; }
	inline DWORD			GetIndex() { return m_dwIndex; }
	inline BOOL				IsAcceptSocket() { return m_bAcceptSocket; }
	inline VOID				SetAcceptSocketFlag() { m_bAcceptSocket = TRUE; }
	VOID					Remove() { InterlockedExchange( (long*)&m_bRemove, TRUE ); }
	inline VOID				ResetKillFlag() { InterlockedExchange( (long *)&m_bRemove, FALSE ); }
	inline BOOL				ShouldBeRemoved() { return m_bRemove; }
	VOID					Disconnect( BOOL bGracefulDisconnect );
	inline BOOL				HasDisconnectOrdered() { return m_bDisconnectOrdered; }

private:
	VOID					SetIndex( DWORD index ) { m_dwIndex = index; }
	inline VOID				ResetTimeOut() { m_dwLastSyncTick = GetTickCount(); }

	NetworkObject			*m_pNetworkObject;
	SendBuffer				*m_pSendBuffer;
	RecvBuffer				*m_pRecvBuffer;

	SOCKET					m_socket;
	SOCKADDR_IN				m_sockaddr;

	OVERLAPPEDEX			m_sendIoData;
	OVERLAPPEDEX			m_recvIoData;
	OVERLAPPEDEX			m_acceptIoData;
	OVERLAPPEDEX			m_disconnectIoData;
	DWORD					m_dwLastSyncTick;
	BOOL					m_bRemove;
	DWORD					m_dwTimeOut;
	DWORD					m_dwIndex;
	BOOL					m_bAcceptSocket;
	BOOL					m_bDisconnectOrdered;

	// 
	int						m_iCategory;
	int						m_iProtocol;
	int						m_iSize;
};

#endif
