// molyclient.cpp : Defines the entry point for the console application.
//

#include "NetBase.h"
#include "NetMsg.h "
#include <stdio.h>
#include "CommandParser.h"
#include "JK_Assert.h"
#include "JK_Utility.h"

CNetBase*			g_pNetBase;						// 网络接口

#define MAX_CONNECT_NUM 50
#define NET_MSG WM_USER + 100




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

	JK_ASSERT(emc_CS_CATEGORY == pMsgHead->m_byCategory );

	//解析消息头
	switch ( pMsgHead->m_byProtocol )
	{

	case S2C_SVR_READY_CMD:
		{
			MSG_C2S_CLTREGISTER_SYN regmsg;
			g_pNetBase->Send( (char*)&regmsg, sizeof(regmsg) );
			printf("Connet server success!\n");
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

			case egr_SVRNOTREADY:
				{
					printf("server's not ready yet!\n");
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
		{

		}
		break;

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
			setPacket.m_usKeyLen = JK_SPRITF_S(setPacket.strKey, "%s", argv[0]);
			setPacket.m_usValLen = JK_SPRITF_S(setPacket.strVal, "%s", argv[1]);
			g_pNetBase->Send((char*)&setPacket, setPacket.GetMsgSize() );

			//int TCNT = 1; 
			//while (  true )
			//{
			//	DWORD dwBegTM = GetTickCount();
			//	for( int i=0; i<9999; ++i )
			//	{
			//		MSG_C2S_INSERT_ITEM_SYN setPacket;
			//		setPacket.m_usKeyLen = sprintf_s( setPacket.strKey, "%s%d", argv[0], i );
			//		setPacket.m_usValLen = sprintf_s( setPacket.strVal, "%d",  i );
			//		g_pNetBase->Send((char*)&setPacket, setPacket.GetMsgSize() );
			//		Sleep(1);
			//	}
			//	DWORD dwEndTM = GetTickCount();
			//	printf( "总共耗时: %d 秒!\n", (dwEndTM-dwBegTM)/1000 );

			//	Sleep(3000);

			//	dwBegTM = GetTickCount();
			//	for( int i=0; i<9999; ++i )
			//	{
			//		MSG_C2S_REMOVE_ITEM_SYN setPacket;
			//		sprintf_s( setPacket.strKey, "%s%d", argv[0], i );
			//		g_pNetBase->Send((char*)&setPacket, sizeof(MSG_C2S_REMOVE_ITEM_SYN) );
			//		Sleep(1);
			//	}
			//	dwEndTM = GetTickCount();
			//	printf( "总共耗时: %d 秒!\n", (dwEndTM-dwBegTM)/1000 );
			//	++TCNT;
			//	Sleep( 1000 );
			//	printf( "第 %d 轮测试即将开始!\n", TCNT );
			//	Sleep( 5000 );
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
			strcpy_s(setPacket.strKey, MAX_KEY_LEN, argv[0]);
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
			strcpy_s(setPacket.strKey, MAX_KEY_LEN, argv[0]);
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