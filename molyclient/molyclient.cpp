// molyclient.cpp : Defines the entry point for the console application.
//

#include "NetBase.h"
#include "NetMsg.h "
#include <stdio.h>
#include "JK_Console.h"
#include "CommandParser.h"
#include "JK_Assert.h"
#include "JK_Utility.h"

					// 网络接口

#define MAX_CONNECT_NUM 50
#define NET_MSG WM_USER + 100
#define CHECK_PARA_CNT(cnt,argc) if(cnt!=argc){DISPMSG_ERROR("incorrect number of arguments!\n");return;}


CNetBase*		g_pNetBase;
JK_Console		g_ConsoleMgr;




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
			DISPMSG_SUCCESS("Connet server[%s:%d] success!\n", g_pNetBase->GetIP(), g_pNetBase->GetPort() );
		}break;


	case S2C_GERERAL_RES_CMD:
		{
			MSG_S2C_GERERAL_RES_CMD* pReadyMsg = (MSG_S2C_GERERAL_RES_CMD*)pMsg;
			switch (pReadyMsg->m_iRes)
			{
			case egr_INSERTSUCCESS:
				{
					DISPMSG_SUCCESS("set ok\n");
				}break;

			case egr_INSERTFAILD:
				{
					DISPMSG_ERROR("set failed!\n");
				}break;

			case egr_CANTFINDVAL:
				{
					printf("(nil)\n");
				}break;

			case egr_REMOVESUCCESS:
				{
					DISPMSG_SUCCESS("del ok\n");
				}
				break;

			case egr_NOSUCHKEYS:
				{
					printf("(nil)\n");
				}
				break;

			case egr_KEYEXISTS:
				{
					printf("yes\n");
				}
				break;

			case egr_SVRNOTREADY:
				{
					DISPMSG_ERROR("server's not ready yet!\n");
				}
				break;

			case egr_EXPIRESUCCESS: 
			{
				DISPMSG_SUCCESS("expire key set ok!\n");
			}break;

			case egr_EXPIREFAILD: 
			{
				DISPMSG_ERROR("expire key set failed, no such key!\n");
			}break;

			default:
				break;
			}
		}break;


	case S2C_SELECT_ITEM_ACK:
		{
			MSG_S2C_SELECT_ITEM_ACK* pReadyMsg = (MSG_S2C_SELECT_ITEM_ACK*)pMsg;
			printf("%s\n", pReadyMsg->strVal);
		}break;

	case S2C_SELECT_KEYS_ACK:
		{
			MSG_S2C_SELECT_KEYS_ACK* pKeysMsg = (MSG_S2C_SELECT_KEYS_ACK*)pMsg;
			for ( int i=0; i<pKeysMsg->m_iKeysCnt; ++i )
			{
				printf("%s\n", pKeysMsg->m_szKeys[i] );
			}
		}
		break;

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
	int svrPort = 3690;
	if (2 == argc &&   atoi(argv[1]) >0 )
	{	
		svrPort = atoi(argv[1]);
	}
	if (!g_pNetBase->ConncetToServer(0, "127.0.0.1", svrPort))
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
	printf("set	Set the string value of a key.\n");
	printf("get	Get the value of a key.\n");
	printf("del	Delete a key.\n");
	printf("exists	Determine if a key exists.\n");
	printf("keys	Prepend one or multiple values to a list.\n");
	printf("lpush	Append one or multiple values to a list.\n");
	printf("rpush	Find all keys matching the given pattern.\n");
	printf("lpop	Remove and get the first element in a list.\n");
	printf("rpop	Remove and get the last element in a list.\n");
	printf("llen	Get the length of a list.\n");
}


void SendCmdMsg(unsigned int cmdtype, char argv[MAX_PARA_CNT][MAX_CMD_LEN], unsigned int argc)
{
	switch (cmdtype)
	{
	case ect_COMMAND_SET:
		{
			CHECK_PARA_CNT(2,argc);
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
			CHECK_PARA_CNT(1, argc);
			MSG_C2S_SELECT_ITEM_SYN setPacket;
			strcpy_s(setPacket.strKey, MAX_KEY_LEN, argv[0]);
			g_pNetBase->Send((char*)&setPacket, sizeof(MSG_C2S_SELECT_ITEM_SYN));
		}break;

	case ect_COMMAND_DEL:
		{
			CHECK_PARA_CNT(1, argc);
			MSG_C2S_REMOVE_ITEM_SYN setPacket;
			strcpy_s(setPacket.strKey, MAX_KEY_LEN, argv[0]);
			g_pNetBase->Send((char*)&setPacket, sizeof(MSG_C2S_REMOVE_ITEM_SYN));
		}
		break;

	case ect_COMMAND_KEYS:
		{
			CHECK_PARA_CNT(1, argc);
			MSG_C2S_SELECT_KEYS_SYN keysMsg;
			strcpy_s(keysMsg.m_szPattern, MAX_KEY_LEN, argv[0]);
			g_pNetBase->Send((char*)&keysMsg, sizeof(MSG_C2S_SELECT_KEYS_SYN));
		}
		break;

	case ect_COMMAND_EXIST:
		{
			CHECK_PARA_CNT(1, argc);
			MSG_C2S_EXISTS_KEY_SYN exmsg;
			strcpy_s(exmsg.strKey, MAX_KEY_LEN, argv[0]);
			g_pNetBase->Send((char*)&exmsg, sizeof(MSG_C2S_EXISTS_KEY_SYN));
		}
		break;

	case ect_COMMAND_LPUSH:
		{
			CHECK_PARA_CNT(2, argc);
			MSG_C2S_LPUSH_ITEM_SYN setPacket;
			setPacket.m_usKeyLen = JK_SPRITF_S(setPacket.strKey, "%s", argv[0]);
			setPacket.m_usValLen = JK_SPRITF_S(setPacket.strVal, "%s", argv[1]);
			g_pNetBase->Send((char*)&setPacket, sizeof(MSG_C2S_LPUSH_ITEM_SYN));
		}
		break;

	case ect_COMMAND_RPUSH:
		{
			CHECK_PARA_CNT(2, argc);
			MSG_C2S_RPUSH_ITEM_SYN setPacket;
			setPacket.m_usKeyLen = JK_SPRITF_S(setPacket.strKey, "%s", argv[0]);
			setPacket.m_usValLen = JK_SPRITF_S(setPacket.strVal, "%s", argv[1]);
			g_pNetBase->Send((char*)&setPacket, sizeof(MSG_C2S_RPUSH_ITEM_SYN));
		}
		break;

	case ect_COMMAND_LPOP:
		{
			CHECK_PARA_CNT(1, argc);
			MSG_C2S_LPOP_ITEM_SYN exmsg;
			strcpy_s(exmsg.strKey, MAX_KEY_LEN, argv[0]);
			g_pNetBase->Send((char*)&exmsg, sizeof(MSG_C2S_LPOP_ITEM_SYN));
		}
		break;

	case ect_COMMAND_RPOP:
		{
			CHECK_PARA_CNT(1, argc);
			MSG_C2S_RPOP_ITEM_SYN exmsg;
			strcpy_s(exmsg.strKey, MAX_KEY_LEN, argv[0]);
			g_pNetBase->Send((char*)&exmsg, sizeof(MSG_C2S_RPOP_ITEM_SYN));
		}
		break;

	case ect_COMMAND_LLEN:
		{
			CHECK_PARA_CNT(1, argc);
			MSG_C2S_LLEN_ITEM_SYN exmsg;
			strcpy_s(exmsg.strKey, MAX_KEY_LEN, argv[0]);
			g_pNetBase->Send((char*)&exmsg, sizeof(MSG_C2S_LLEN_ITEM_SYN));
		}
		break;

	case ect_COMMAND_EXPIRE:
		{
			CHECK_PARA_CNT(2, argc);
			MSG_C2S_EXPIRE_KEY_SYN exmsg;
			strcpy_s(exmsg.strKey, MAX_KEY_LEN, argv[0]);
			exmsg.m_iSeconds = atoi(argv[1]);
			g_pNetBase->Send((char*)&exmsg, sizeof(MSG_C2S_EXPIRE_KEY_SYN));
		}
		break;

	case ect_COMMAND_HSET: 
	{
		MSG_C2S_HSET_ITEM_SYN  hsetMsg;
		g_pNetBase->Send((char*)&hsetMsg, sizeof(MSG_C2S_HSET_ITEM_SYN));
		
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
				DISPMSG_ERROR("illegal command！\n");
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