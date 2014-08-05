#include "IOCPServer.h"
#include "NetworkObject.h"

//-------------------------------------------------------------------------------------------------
// Sample Echo Server
//-------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#pragma comment( lib, "Network_d.lib" )
#else
#pragma comment( lib, "Network.lib" )
#endif

//-------------------------------------------------------------------------------------------------
// User Class
//-------------------------------------------------------------------------------------------------
class User : public NetworkObject
{
protected:
	virtual VOID	OnAccept( DWORD dwNetworkIndex ) {};
	virtual VOID	OnDisconnect() {};
	virtual	VOID	OnRecv( BYTE *pMsg, WORD wSize ) { Send( pMsg, wSize ); }
	virtual VOID	OnConnect( BOOL bSuccess, DWORD dwNetworkIndex ) {}    
};

//-------------------------------------------------------------------------------------------------
// Callback Functions
//-------------------------------------------------------------------------------------------------
NetworkObject* CreateAcceptedObject() {	return new User; }
VOID DestroyAcceptedObject( NetworkObject *pNetworkObject ) { delete pNetworkObject; }
VOID DestroyConnectedObject( NetworkObject *pNetworkObject ) {delete pNetworkObject; }


//-------------------------------------------------------------------------------------------------
// Main
//-------------------------------------------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
	const DWORD CLIENT_IOHANDLER_KEY = 0;

	// IOCP 서버 인스턴스 생성
	IOCPServer *pIOCPServer = new IOCPServer;

	// 디스크립션 세팅
	IOHANDLER_DESC desc;

	desc.dwIoHandlerKey				= CLIENT_IOHANDLER_KEY;
	desc.dwMaxAcceptSession			= 1000;
	desc.dwMaxConnectSession		= 0;
	desc.dwSendBufferSize			= 60000;
	desc.dwRecvBufferSize			= 60000;
	desc.dwTimeOut					= 30000;
	desc.dwMaxPacketSize			= 4096;
	desc.dwNumberOfIoThreads		= 1;
	desc.dwNumberOfAcceptThreads	= 1;
	desc.dwNumberOfConnectThreads	= 0;
	desc.fnCreateAcceptedObject		= CreateAcceptedObject;
	desc.fnDestroyAcceptedObject	= DestroyAcceptedObject;
	desc.fnDestroyConnectedObject	= DestroyConnectedObject;

	// IOCP 서버 초기화
	if( !pIOCPServer->Init( &desc, 1 ) )
	{
		printf( "IOCP 서버 초기화 실패" );
		return 0;
	}

	// 리슨 시작
	if( !pIOCPServer->StartListen( CLIENT_IOHANDLER_KEY, "10.1.28.134", 6000 ) )
	{
		printf( "리슨 실패" );
		return 0;
	}

	// assert( pIOCPServer->IsListening( CLIENT_IOHANDLER_KEY ) == TRUE );

	printf( "Server started!\n" );

	// 메인 루프에서 IOCP 서버 업데이트
    while( TRUE )
	{
		Sleep( 10 );
		pIOCPServer->Update();
	}

	printf( "Server is terminated...\n" );

	// IOCP 서버 셧다운
	pIOCPServer->Shutdown();

	// IOCP 서버 인스턴스 파괴
	delete pIOCPServer;

	return 0;
}
