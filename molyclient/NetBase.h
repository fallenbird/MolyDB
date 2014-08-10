#pragma once
#include <winsock2.h>

#define  RECEIVEBUFLEN	8*1024*2
#define	 MAXNETPOOL		256*1024
struct PacketHeader
{
	WORD wSize;
};

void NetProcess(WPARAM wParam,LPARAM lParam);

typedef int (*RECVFUN) ( int id, char* pszData, int nLen);

//--------------------------------------------------------------------------------
//--------	SXF 2012-07-27	重新定义成员函数指针  
//--------------------------------------------------------------------------------


class ConnectInfo
{
public:
	ConnectInfo();
	~ConnectInfo();

	bool	SaveData( char* pData, int iLen );
	bool	IsHavdData();
	bool	GetData( char* pData, int& iLen );

	void	SaveSendData( char* pData, int iLen );
//	void	Send();

	char	m_strTmep[256];
	bool	m_bConnect;
	bool	m_bIsClosed;
	int		m_iID;
	DWORD	m_dwEventID;
	HWND	m_hWnd;
	SOCKET	m_Socket;
	char	m_strIP[16];
	DWORD	m_dwPort;
	HANDLE	m_dwThreadHandle;

	int		m_iStartPtr;
	int		m_iEndPtr;
	char	m_pMemPool[MAXNETPOOL];

	DWORD	m_SendLen;
	char	m_pSendMemPool[MAXNETPOOL];
	RECVFUN	m_pRecvFunc;

	CRITICAL_SECTION m_SendCritSect;
	CRITICAL_SECTION m_RecvCritSect;

};

class CNetBase
{
public:
	CNetBase(void);
	~CNetBase(void);

	static unsigned int NetWorkThread(DWORD Onwer);

	bool InitNet( int iConnectCount, RECVFUN recvFunc, HWND hWnd,  DWORD eventID );
	bool ConncetToServer( int iConnectID, const char *pIPAddress, int iPort);
	void CloseConnect( int iConnectID );
	void Send( char* pData, int iLen, int iConnectID = 0);
	void ClearNet();

private:
	RECVFUN	m_pRecvFunc;
	ConnectInfo* m_pConnectHandles;
	int	m_iMaxConnectCount;
	static bool m_bConnectDestroy;
};
