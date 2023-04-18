#include <stdio.h>
#include "assert.h"
#include "Define.h"
#include "NetBase.h"
#include "JK_Utility.h"

#define PROTOCOL_HEAD_LEN 4

#pragma comment( lib, "ws2_32.lib" )
#pragma warning(disable:4996)

//extern DWORD	g_dwTotalSendNum;			// �ܷ�����
//extern DWORD	g_dwTotalRecvNum;			// ���հ���
//extern DWORD	g_dwSendBuffUsed;			// ��ʹ�õķ��ͻ����С
//extern DWORD	g_dwRecvBuffUsed;			// ��ʹ�õĽ��ջ����С

bool CNetBase::m_bConnectDestroy = false;
void NetProcess(WPARAM wParam, LPARAM lParam);

unsigned int CNetBase::NetWorkThread(DWORD Onwer)
{
	if( m_bConnectDestroy )
	{
		return 0;
	}
	ConnectInfo* pConnect = (ConnectInfo*)Onwer;

	char*	pnetdata = new char[RECEIVEBUFLEN];
	int	iLen;
	while( pConnect->m_bConnect )
	{
		Sleep( 1 );
		iLen = recv( pConnect->m_Socket, pnetdata, RECEIVEBUFLEN, 0 );
		if( iLen > 0 )
		{
			pConnect->SaveData( pnetdata, iLen );
		}

		if( pConnect->IsHavdData() )			//ȡ����������
		{
			//�����¼�
			//PostMessage( pConnect->m_hWnd, pConnect->m_dwEventID, (DWORD)pConnect,0 );
			NetProcess((DWORD)pConnect, 0 );
		}

		if( pConnect->m_SendLen > 0 )								//��������
		{
			EnterCriticalSection( &pConnect->m_SendCritSect );

			iLen = send( pConnect->m_Socket, pConnect->m_pSendMemPool, pConnect->m_SendLen, 0 );
			if( iLen > 0 )
			{
				pConnect->m_SendLen = 0;
			}
			else
			{
				int er = WSAGetLastError();
				char temp[32];
				JK_SPRITF(temp, "%d\n", er);
				//OutputDebugString( temp );
			}
			LeaveCriticalSection( &pConnect->m_SendCritSect );
		}
	}

	delete[] pnetdata;
	pConnect->m_bIsClosed = true;
	return 0;
}
//--------------------------------------------------------------------------------------

void NetProcess(WPARAM wParam,LPARAM lParam)
{
	ConnectInfo* pConnect = (ConnectInfo*)wParam;

	char pData[RECEIVEBUFLEN];
	int len;
	if( pConnect->GetData( pData, len ) )
	{
		pConnect->m_pRecvFunc( pConnect->m_iID, pData, len );
	}
}

//--------------------------------------------------------------------------------------
ConnectInfo::ConnectInfo() : m_bConnect(false),m_bIsClosed(false), m_hWnd(NULL), m_dwThreadHandle(NULL), m_pRecvFunc(NULL)
{
	InitializeCriticalSection( &m_SendCritSect );
	InitializeCriticalSection( &m_RecvCritSect );
	m_SendLen = 0;
}


ConnectInfo::~ConnectInfo()
{
	DeleteCriticalSection( &m_SendCritSect );
	DeleteCriticalSection( &m_RecvCritSect );
}


bool ConnectInfo::SaveData( char* pData, int iLen )
{
	EnterCriticalSection( &m_RecvCritSect );

	if( MAXNETPOOL - m_iEndPtr < iLen )		//���治����  ��Ҫ����
	{
		char* tempData = new char[m_iEndPtr-m_iStartPtr];
		memcpy( tempData, &m_pMemPool[m_iStartPtr], m_iEndPtr-m_iStartPtr );
		memcpy( m_pMemPool, tempData, m_iEndPtr-m_iStartPtr );
		m_iEndPtr = m_iEndPtr-m_iStartPtr;
		m_iStartPtr = 0;
		delete[] tempData;
	}
	if( MAXNETPOOL - m_iEndPtr < iLen )		//������Ȼ����
	{
		//MessageBox( NULL, "������� ���治��",NULL, MB_OK );
		assert( "���绺�治�㣡" );
		LeaveCriticalSection(&m_RecvCritSect);
		return false;
	}
	else
	{
		memcpy( &m_pMemPool[m_iEndPtr], pData, iLen );
		m_iEndPtr += iLen;
	}
	// --�հ�����
	//g_dwTotalRecvNum++;
	// --���ջ������
	//g_dwRecvBuffUsed = m_iEndPtr;
	LeaveCriticalSection( &m_RecvCritSect );
	return true;
}


bool ConnectInfo::IsHavdData()
{
	unsigned int curDatalen = m_iEndPtr - m_iStartPtr;
	if( curDatalen == 0 )
	{
		return false;
	}
	else if( curDatalen < 0 )
	{
		//MessageBox( NULL, "������� ���ݰ�����",NULL, MB_OK );
		assert( "������� ���ݰ�����");
		return false;
	}
	else
	{
		PacketHeader* pHeader = (PacketHeader*)&m_pMemPool[m_iStartPtr];
		if (pHeader->dwSize + PROTOCOL_HEAD_LEN > curDatalen)		//�ϰ�
		{
			return false;
		}
		else
			return true;
	}
	return true;
}


// --[SXF 2012/08/08 16:27:08]
bool ConnectInfo::GetData( char* pData, int& iLen )
{
	unsigned int curDatalen = m_iEndPtr - m_iStartPtr;
	if( curDatalen == 0 )
	{
		return false;
	}
	else if( curDatalen < 0 )
	{
		//MessageBox( NULL, "������� ���ݰ�����", NULL, MB_OK );
		assert( "������� ���ݰ�����" );
		return false;
	}
	else
	{
		PacketHeader* pHeader = (PacketHeader*)&m_pMemPool[m_iStartPtr];
		if (pHeader->dwSize + PROTOCOL_HEAD_LEN  > curDatalen)		//�ϰ�
		{
			return false;
		}
		else
		{
			EnterCriticalSection( &m_RecvCritSect );
			memcpy(pData, &m_pMemPool[m_iStartPtr + PROTOCOL_HEAD_LEN], pHeader->dwSize);

			m_iStartPtr += pHeader->dwSize + PROTOCOL_HEAD_LEN;
			iLen = pHeader->dwSize;
			LeaveCriticalSection( &m_RecvCritSect );
			return true;
		}
	}

	return true;
}



// --[SXF 2012/08/08 16:27:08]
void ConnectInfo::SaveSendData( char* pData, int iLen )
{
	if( iLen < 0 || iLen > 1024 )
	{
		assert(1);
	}
	EnterCriticalSection( &m_SendCritSect );

	// --�����е���Ϣ��ǰ�� ����һ����Ϣ������ �����͸�Ϊint [SXF 2012/07/23 15:00:33]
	if( int( MAXNETPOOL - m_SendLen ) < iLen + sizeof(int) )
	{
		//MessageBox( NULL, "���ͻ�����",NULL,MB_OK );
		printf( "���ͻ�����" );
		LeaveCriticalSection(&m_SendCritSect);
		return;
	}

	// --ͬ�� [SXF 2012/07/23 15:01:14]
	memcpy(&m_pSendMemPool[m_SendLen], &iLen, sizeof(unsigned int));
	memcpy(&m_pSendMemPool[m_SendLen + sizeof(unsigned int)], pData, iLen);
	m_SendLen += iLen + sizeof(unsigned int);

	// --��������
	//g_dwTotalSendNum++;
	// --���ͻ������
	//g_dwSendBuffUsed = m_SendLen;

	LeaveCriticalSection( &m_SendCritSect );
}



// --[SXF 2012/08/08 16:27:08]
CNetBase::CNetBase(void) : m_iMaxConnectCount( 0 ), m_pConnectHandles(NULL), m_pRecvFunc(NULL)
{
}


// --[SXF 2012/08/08 16:27:08]
CNetBase::~CNetBase(void)
{
	m_iMaxConnectCount = 0;
	m_bConnectDestroy = true;
	if( m_pConnectHandles )
	{
		delete[] m_pConnectHandles;
		m_pConnectHandles = NULL;
	}
}


bool CNetBase::InitNet( int iConnectCount, RECVFUN recvFunc, HWND hWnd, DWORD eventID )
{
	WSADATA wsaData;

	WSAStartup( MAKEWORD( 2, 2 ), &wsaData );

	if( wsaData.wVersion != MAKEWORD( 2, 2 ) )
	{
		WSACleanup();
		return FALSE;
	}

	m_iMaxConnectCount = iConnectCount;
	m_pConnectHandles = new ConnectInfo[m_iMaxConnectCount];
	for( int i = 0; i< m_iMaxConnectCount; i++ )
	{
		m_pConnectHandles[i].m_iID = i;
		m_pConnectHandles[i].m_dwEventID = eventID;
		m_pConnectHandles[i].m_hWnd = hWnd;
	}

	m_pRecvFunc = recvFunc;
	return TRUE;
}


bool CNetBase::ConncetToServer( int iConnectID, const char *pIPAddress, int iPort)
{
	if( m_pConnectHandles[iConnectID].m_bConnect )
	{
		CloseConnect( iConnectID );
	}
	m_pConnectHandles[iConnectID].m_Socket = socket(AF_INET,SOCK_STREAM,0);
	if( m_pConnectHandles[iConnectID].m_Socket == INVALID_SOCKET )
	{
		return false;
	}
	m_pConnectHandles[iConnectID].m_dwPort = iPort;
	memcpy( m_pConnectHandles[iConnectID].m_strIP, pIPAddress, 16 );

	SOCKADDR_IN kAddr;
	kAddr.sin_addr.S_un.S_addr = inet_addr(m_pConnectHandles[iConnectID].m_strIP);
	kAddr.sin_family = AF_INET;
	kAddr.sin_port = htons( (u_short)m_pConnectHandles[iConnectID].m_dwPort);

	if( connect( m_pConnectHandles[iConnectID].m_Socket, (SOCKADDR*)&kAddr, sizeof(SOCKADDR) ) )
	{
		return false;
	}


	///--- ����
	//Send( "324242424", 10 );
	//---CESHI 

	unsigned   long   ul   =   1;   
	int ret   =   ioctlsocket(m_pConnectHandles[iConnectID].m_Socket,   FIONBIO,   (unsigned   long*)&ul);   
	if(ret==SOCKET_ERROR)   
	{   
		::closesocket   (m_pConnectHandles[iConnectID].m_Socket);   
		return false;   
	}

	m_pConnectHandles[iConnectID].m_bConnect = true;
	m_pConnectHandles[iConnectID].m_iEndPtr = 0;
	m_pConnectHandles[iConnectID].m_iStartPtr = 0;
	m_pConnectHandles[iConnectID].m_pRecvFunc = m_pRecvFunc;
	m_pConnectHandles[iConnectID].m_SendLen = 0;
	m_pConnectHandles[iConnectID].m_bIsClosed = false;

	m_pConnectHandles[iConnectID].m_dwThreadHandle = CreateThread( NULL,0,(LPTHREAD_START_ROUTINE)NetWorkThread,&m_pConnectHandles[iConnectID],0,0 );

	return true;
}


char* CNetBase::GetIP() 
{
	if( NULL == m_pConnectHandles )
	{
		return NULL;
	}
	return m_pConnectHandles->m_strIP;
}


DWORD CNetBase::GetPort() 
{
	if (NULL == m_pConnectHandles)
	{
		return NULL;
	}
	return m_pConnectHandles->m_dwPort;
}

void CNetBase::Send(  char* pData, int iLen, int iConnectID )
{	
	assert(iLen <= MAX_PACK_SIZE);
	m_pConnectHandles[iConnectID].SaveSendData( pData, iLen );
}


void CNetBase::CloseConnect( int iConnectID )
{
	m_pConnectHandles[iConnectID].m_bConnect = false;
	while( !m_pConnectHandles[iConnectID].m_bIsClosed ) Sleep(1);
	closesocket( m_pConnectHandles[iConnectID].m_Socket );
}


void CNetBase::ClearNet()
{
	for( int i = 0; i < m_iMaxConnectCount; i++ )
	{
		if( m_pConnectHandles[i].m_bConnect )
		{
			m_pConnectHandles[i].m_bConnect = false;
			while( !m_pConnectHandles[i].m_bIsClosed ) Sleep(1);
			closesocket( m_pConnectHandles[i].m_Socket );
		}
	}
	delete[]  m_pConnectHandles;
	WSACleanup();
}