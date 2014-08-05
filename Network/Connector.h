#ifndef __CONNECTOR_H__
#define __CONNECTOR_H__

#include <windows.h>

#define MAX_CONNECT_THREAD	16

class IoHandler;
class Session;
class SessionList;

//=============================================================================================================================
/// 连接函数
/**
	@remarks
			-无
	@see
			Session
*/
//=============================================================================================================================
class Connector
{
	friend unsigned __stdcall connect_thread( LPVOID param );

public:
	Connector(void);
	~Connector(void);

	VOID			Init( IoHandler *pIoHandler, DWORD dwNumberOfThreads );
	VOID			Connect( Session *pSession );
	VOID			Shutdown();

private:
	IoHandler		*m_pIoHandler;
	SessionList		*m_pConnectingList;
	HANDLE			m_hThread[MAX_CONNECT_THREAD];
	HANDLE			m_hEvent[2];
	BOOL			m_bShutdown;
	DWORD			m_numThreads;
};

#endif