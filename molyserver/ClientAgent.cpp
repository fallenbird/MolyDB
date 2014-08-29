#include <stdio.h>
#include "ClientAgent.h"
#include "NetMsg.h"
#include "DataSpace.h"
#include "JK_Console.h"

ClientAgent::ClientAgent()
{

}

ClientAgent::~ClientAgent()
{

}



void ClientAgent::OnAccept(DWORD connindex)
{
	//const char* lpszIPAddr = SessionManager::GetInstance()->GetSession(connindex)->GetRemoteIP();

	//DISPMSG_NOTICE("Acceptor", "Client IP:[%s] Connected, ID:[%d]", lpszIPAddr, connindex);
	//printf("Client Acceptor Client IP:[%s] Connected, ID:[%d] \n", lpszIPAddr, connindex);
	//WRITE_LOGFILE(LCONNECT_LOG_NAME, e_Error, "[建立连接] 客户端IP=[%s] 与服务器建立连接 ConnID=[%d] 总连接数:[%d]", lpszIPAddr, connindex, Socket::s_nTotalConneced);


	// --给Client终端发送状态
	MSG_S2C_SVR_READY_CMD readyPacket;
	readyPacket.sHighVer = 7;
	readyPacket.sLowVer = 8;
	readyPacket.iEncKey = 666;
	DISPMSG_SUCCESS( "Accept client[%d] success!\n", connindex );
	//Send(connindex, (char*)&readyPacket, sizeof(MSG_S2C_SVR_READY_CMD));
	Send((BYTE*)&readyPacket, sizeof(MSG_S2C_SVR_READY_CMD));
}


void ClientAgent::OnRecv(BYTE *pMsg, WORD wSize)
{
	MSG_BASE* pMsgBase = (MSG_BASE*)pMsg;
	switch (pMsgBase->m_byCategory)
	{
	case CS_AGENT:
		{
			switch (pMsgBase->m_byProtocol)
			{
			case C2S_INSERT_ITEM_SYN:
				{
					MSG_C2S_INSERT_ITEM_SYN* pInsertMsg = (MSG_C2S_INSERT_ITEM_SYN*)pMsg;
					if (DataSpace::GetInstance().InsertKV(pInsertMsg->strKey, pInsertMsg->strVal))
					{
						MSG_S2C_GERERAL_RES_CMD genermsg;
						genermsg.m_iRes = egr_INSERTSUCCESS;
						Send((BYTE*)&genermsg, sizeof(MSG_S2C_GERERAL_RES_CMD));
					}
					else
					{
						MSG_S2C_GERERAL_RES_CMD genermsg;
						genermsg.m_iRes = egr_INSERTFAILD;
						Send((BYTE*)&genermsg, sizeof(MSG_S2C_GERERAL_RES_CMD));
					}
				}
				break;

			case C2S_SELECT_ITEM_SYN:
				{
					MSG_C2S_SELECT_ITEM_SYN* pInsertMsg = (MSG_C2S_SELECT_ITEM_SYN*)pMsg;
					char* strVal = (char*)DataSpace::GetInstance().GetValue(pInsertMsg->strKey);
					if (NULL == strVal)
					{
						MSG_S2C_GERERAL_RES_CMD genermsg;
						genermsg.m_iRes = egr_CANTFINDVAL;
						Send((BYTE*)&genermsg, sizeof(MSG_S2C_GERERAL_RES_CMD));
					}
					else
					{
						MSG_S2C_SELECT_ITEM_ACK ackmsg;
						strcpy_s(ackmsg.strKey, 168, pInsertMsg->strKey);
						strcpy_s(ackmsg.strVal, 1024, strVal);
						Send((BYTE*)&ackmsg, sizeof(MSG_S2C_SELECT_ITEM_ACK));
					}
				}
				break;

			case C2S_REMOVE_ITEM_SYN:
				{
					MSG_C2S_REMOVE_ITEM_SYN* pInsertMsg = (MSG_C2S_REMOVE_ITEM_SYN*)pMsg;
					if ( !DataSpace::GetInstance().RemoveKV(pInsertMsg->strKey) )
					{
						MSG_S2C_GERERAL_RES_CMD genermsg;
						genermsg.m_iRes = egr_CANTFINDVAL;
						Send((BYTE*)&genermsg, sizeof(MSG_S2C_GERERAL_RES_CMD));
					}
					else
					{
						MSG_S2C_GERERAL_RES_CMD genermsg;
						genermsg.m_iRes = egr_REMOVESUCCESS;
						Send((BYTE*)&genermsg, sizeof(MSG_S2C_GERERAL_RES_CMD));
					}
				}
				break;

			default:
				{
				}
				break;
			}
		}
		break;

	default:
		{
		}
		break;
	}
}


void ClientAgent::OnDisconnect()
{

}


void ClientAgent::OnConnect(BOOL bSuccess, DWORD dwNetworkIndex)
{

}



