// molyclient.cpp : Defines the entry point for the console application.
//

#include "NetBase.h"
#include "NetMsg.h "
#include <stdio.h>

CNetBase*			g_pNetBase;						// 网络接口

#define MAX_CONNECT_NUM 50
#define NET_MSG WM_USER + 100


void ProcessLTConnectMsg(int nConctID, MSG_BASE* pMsg)
{

	switch (pMsg->m_byProtocol)
	{
		case S2C_SVR_READY_CMD:
		{
			MSG_S2C_SVR_READY_CMD* pReadyMsg = (MSG_S2C_SVR_READY_CMD*)pMsg;
			printf("Connet server success!");
			int i = pReadyMsg->sHighVer;
		}break;

	default:
		break;
	}

}



//-----------------------------------------------------------------------------------------------------------------------
// Prototype	:		int	CMyAppDlg::ProcessLSMsg( char* pMsg )
// Function		:		处理登陆服务器消息
// Input  Param :		- 
// Output Param :		-
// Info			:		SXF	/ 2012.08.01 / 1.0
//-----------------------------------------------------------------------------------------------------------------------
int ServerPro( int nConctID, char* pMsg, int nLen )
{
	MSG_BASE* pMsgHead = (MSG_BASE*)pMsg;

	//解析消息头
	switch( pMsgHead->m_byCategory )
	{
	case CS_LOGON:
		ProcessLTConnectMsg(nConctID, pMsgHead );
		break;

	//case HMsg_TLConnect:									// LoginServer
	//	ProcessLTConnectMsg( nConctID, pMsg );
	//	break;

	//case HMsg_TWConnect:									// --World连接
	//	ProcessTWConnectMsg( nConctID, pMsg );
	//	break;

	}
	return 0;
}



unsigned int CommondThread();

int main(int argc, char* argv[])
{
	g_pNetBase = new CNetBase;
	g_pNetBase->InitNet( MAX_CONNECT_NUM, ServerPro, NULL, NET_MSG );
	g_pNetBase->ConncetToServer(8, "127.0.0.1", 3690 );

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CommondThread, NULL, 0, 0);

	while ( true )
	{
		Sleep(1000);
	}

	return 0;
}


void PrintHelpInfo()
{
	printf("Set  设置K-V值\n");
	printf("Get  根据key获取相应value\n");
}



unsigned int CommondThread()
{
	char commondLine[128];
	while (true)
	{
		gets_s(commondLine);
		if (!strcmp(commondLine, "Get"))
		{
			printf("OK!");
		}
		else if (!strcmp(commondLine, "Set"))
		{
			break;
		}
		else if (!strcmp(commondLine, "quit"))
		{
		}
		else if (!strcmp(commondLine, "help"))
		{
			PrintHelpInfo();
		}
		else
		{
			printf("无效的命令\n");
		}
	}
	return 0;
}