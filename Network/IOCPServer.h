#pragma once

#pragma comment( lib, "ws2_32.lib" )

#include <windows.h>
#include <map>

class NetworkObject;
class IoHandler;

typedef std::map<DWORD, IoHandler*>		IOHANDLER_MAP;
typedef std::pair<DWORD, IoHandler*>	IOHANDLER_MAP_PAIR;
typedef IOHANDLER_MAP::iterator			IOHANDLER_MAP_ITER;

typedef NetworkObject* (*fnCallBackCreateAcceptedObject)();
typedef VOID (*fnCallBackDestroyAcceptedObject)( NetworkObject *pNetworkObject );
typedef VOID (*fnCallBackDestroyConnectedObject)( NetworkObject *pNetworkObject );

//-------------------------------------------------------------------------------------------------
/// I/O 勤甸矾 檬扁拳俊 鞘夸茄 沥焊甫 淬绰 备炼眉
//-------------------------------------------------------------------------------------------------
typedef struct tagIOHANDLER_DESC
{
	DWORD								dwIoHandlerKey;					///< I/O 键
	DWORD								dwMaxAcceptSession;				///< 最大玩家数
	DWORD								dwMaxConnectSession;			///< 最大等待连接数
	DWORD								dwSendBufferSize;				///< 发送缓冲长度
	DWORD								dwRecvBufferSize;				///< 接收缓冲长度
	DWORD								dwTimeOut;						///< 心跳包时间
	DWORD								dwMaxPacketSize;				///< 单包最大长度
	DWORD								dwNumberOfIoThreads;			///< I/O 线程数
	DWORD								dwNumberOfAcceptThreads;		///< Accept 线程数
	DWORD								dwNumberOfConnectThreads;		///< Connect 线程数
	fnCallBackCreateAcceptedObject		fnCreateAcceptedObject;			///< 
	fnCallBackDestroyAcceptedObject		fnDestroyAcceptedObject;		///<		回调函数指针
	fnCallBackDestroyConnectedObject	fnDestroyConnectedObject;		///< 
} IOHANDLER_DESC, *LPIOHANDLER_DESC;

//-------------------------------------------------------------------------------------------------
/// IOCP 辑滚 努贰胶
/**
	@remarks
			- IOCP 采用 I/O 线程池管理
			- Update 在该函数中处理数据，要在外部循环调用.
	@see
			IoHandler
			tagIOHANDLER_DESC
*/
//-------------------------------------------------------------------------------------------------
class IOCPServer
{
	friend unsigned __stdcall send_thread( LPVOID param );

public:
	IOCPServer();
	virtual ~IOCPServer();

	BOOL				Init( LPIOHANDLER_DESC lpDesc, DWORD dwNumberofIoHandlers );
	BOOL				StartListen( DWORD dwIoHandlerKey, char *pIP, WORD wPort );
	VOID				Update();
	VOID				Shutdown();
	DWORD				Connect( DWORD dwIoHandlerKey, NetworkObject *pNetworkObject, char *pszIP, WORD wPort, DWORD dwSendBufferSize, DWORD dwRecvBufferSize, DWORD dwMaxPacketSize );
	BOOL				IsListening( DWORD dwIoHandlerKey );
	DWORD				GetNumberOfConnections( DWORD dwIoHandlerKey );

private:
	BOOL				InitWinsock();
	VOID				CreateIoHandler( LPIOHANDLER_DESC lpDesc );

	HANDLE				m_hSendThread;
	BOOL				m_bShutdown;
	IOHANDLER_MAP		m_mapIoHandlers;
};
