// molyclient.cpp : Defines the entry point for the console application.
//

#include "NetBase.h"
#include "NetMsg.h "
#include <stdio.h>
#include "CommandParser.h"
#include "JK_Assert.h"


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
			printf("Connet server success!\n");
			int i = pReadyMsg->sHighVer;
		}break;


	case S2C_GERERAL_RES_CMD:
		{
			MSG_S2C_GERERAL_RES_CMD* pReadyMsg = (MSG_S2C_GERERAL_RES_CMD*)pMsg;
			switch (pReadyMsg->m_iRes)
			{
			case egr_INSERTSUCCESS:
				{
					printf("insert success!\n");
				}break;

			case egr_INSERTFAILD:
				{
					printf("insert failed!\n");
				}break;

			case egr_CANTFINDVAL:
				{
					printf("can't find the value!\n");
				}break;
				
			case egr_REMOVESUCCESS:
				{
					printf("delete success!\n");
				}
				break;

			default:
				break;
			}
		}break;


	case S2C_SELECT_ITEM_ACK:
		{
			MSG_S2C_SELECT_ITEM_ACK* pReadyMsg = (MSG_S2C_SELECT_ITEM_ACK*)pMsg;
			printf("%s\n", pReadyMsg->strVal);
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
// Info			:		SXF	/ 2014.08.10 / 1.0
//-----------------------------------------------------------------------------------------------------------------------
int ServerPro(int nConctID, char* pMsg, int nLen)
{
	MSG_BASE* pMsgHead = (MSG_BASE*)pMsg;

	//解析消息头
	switch (pMsgHead->m_byCategory)
	{
	case emc_CS_CATEGORY:
		ProcessLTConnectMsg(nConctID, pMsgHead);
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
	g_pNetBase->InitNet(MAX_CONNECT_NUM, ServerPro, NULL, NET_MSG);
	if (!g_pNetBase->ConncetToServer(0, "127.0.0.1", 3690))
	{
		printf("Can't connent the molydb server!\n");
		Sleep(2000);
		return 1;
	}

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CommondThread, NULL, 0, 0);

	while (true)
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


void SendCmdMsg(unsigned int cmdtype, char argv[MAX_PARA_CNT][MAX_CMD_LEN], unsigned int argc)
{
	switch (cmdtype)
	{
	case ect_COMMAND_SET:
		{
			if (2 != argc)
			{
				printf("incorrect number of arguments!\n");
				return;
			}


			MSG_C2S_INSERT_ITEM_SYN setPacket;
			strcpy_s(setPacket.strKey, 168, argv[0]);
			strcpy_s(setPacket.strVal, 1024, argv[1]);
			g_pNetBase->Send((char*)&setPacket, sizeof(MSG_C2S_INSERT_ITEM_SYN));

			//for( int i=0; i<60; ++i )
			//{
			//	MSG_C2S_INSERT_ITEM_SYN setPacket;
			//	strcpy_s(setPacket.strKey, 168, argv[0]);
			//	strcpy_s(setPacket.strVal, 1024, argv[1]);
			//	sprintf_s( setPacket.strKey, "%d",  i );
			//	sprintf_s( setPacket.strVal, "%d",  i );
			//	g_pNetBase->Send((char*)&setPacket, sizeof(MSG_C2S_INSERT_ITEM_SYN));
			//	Sleep(1);
			//}

		}break;

	case ect_COMMAND_GET:
		{
			if (1 != argc)
			{
				printf("incorrect number of arguments!\n");
				return;
			}
			MSG_C2S_SELECT_ITEM_SYN setPacket;
			strcpy_s(setPacket.strKey, 168, argv[0]);
			g_pNetBase->Send((char*)&setPacket, sizeof(MSG_C2S_SELECT_ITEM_SYN));
		}break;

	case ect_COMMAND_DEL:
		{
			if (1 != argc)
			{
				printf("incorrect number of arguments!\n");
				return;
			}
			MSG_C2S_REMOVE_ITEM_SYN setPacket;
			strcpy_s(setPacket.strKey, 168, argv[0]);
			g_pNetBase->Send((char*)&setPacket, sizeof(MSG_C2S_REMOVE_ITEM_SYN));
		}
		break;

	default:
		break;
	}
}


unsigned int CommondThread()
{
	char commondLine[128];
	while (true)
	{
		gets_s(commondLine);

		unsigned int cmdtype = 0;
		unsigned int argc = 0;
		char argv[MAX_PARA_CNT][MAX_CMD_LEN];
		CommandParser::ParseCommandStr(commondLine, cmdtype, argv, argc);
		switch (cmdtype)
		{
		case ect_COMMAND_NONE:
			{
				printf("illegal command！\n");
			}
			break;

		case ect_COMMAND_HELP:
			{
				PrintHelpInfo();
			}
			break;

		default:
			{
				SendCmdMsg(cmdtype, argv, argc);
			}
			break;
		}

	}
	return 0;
}