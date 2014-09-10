#include <stdio.h>
#include "ClientAgent.h"
#include "NetMsg.h"
#include "DataSpace.h"
#include "JK_Console.h"
#include "Appender.h"
#include "SlaveMgr.h"

ClientAgent::ClientAgent() : m_iAgentType(0)
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
	Send((BYTE*)&readyPacket, sizeof(MSG_S2C_SVR_READY_CMD));
	//Send(connindex, (char*)&readyPacket, sizeof(MSG_S2C_SVR_READY_CMD));
	
}


void ClientAgent::OnDisconnect()
{
	switch (m_iAgentType )
	{
	case 1:
		{
			DISPMSG_ERROR( "Client[%s] disconnected!\n", GetIP() );
		}
		break;

	case 2:
		{
			DISPMSG_ERROR( "Slave[%s] disconnected!\n", GetIP() );
		}
		break;
	default:
		{
			DISPMSG_ERROR( "Agent[%s] disconnected!\n", GetIP() );
		}
		break;
	}
	
}


void ClientAgent::OnConnect(BOOL bSuccess, DWORD dwNetworkIndex)
{

}




void ClientAgent::OnRecv(BYTE *pMsg, WORD wSize)
{
	MSG_BASE* pMsgBase = (MSG_BASE*)pMsg;
	switch (pMsgBase->m_byCategory)
	{
	case emc_CS_CATEGORY:
		{
			switch (pMsgBase->m_byProtocol)
			{
			case C2S_CLTREGISTER_SYN:
				{
					DISPMSG_SUCCESS( "Accept client[%s] success!\n", GetIP() );
					m_iAgentType = 1;
				}
				break;

			case C2S_INSERT_ITEM_SYN:
				{
					MSG_C2S_INSERT_ITEM_SYN* pInsertMsg = (MSG_C2S_INSERT_ITEM_SYN*)pMsg;
					if ( !CheckSvrReady() )
					{
						return;
					}
					if (DataSpace::GetInstance().InsertKV(pInsertMsg->strKey, pInsertMsg->m_usKeyLen, pInsertMsg->strVal, pInsertMsg->m_usValLen))
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
						strcpy_s(ackmsg.strKey, MAX_KEY_LEN, pInsertMsg->strKey);
						strcpy_s(ackmsg.strVal, 1024, strVal);
						Send((BYTE*)&ackmsg, sizeof(MSG_S2C_SELECT_ITEM_ACK));
					}
				}
				break;

			case C2S_REMOVE_ITEM_SYN:
				{
					MSG_C2S_REMOVE_ITEM_SYN* pInsertMsg = (MSG_C2S_REMOVE_ITEM_SYN*)pMsg;

					if ( !CheckSvrReady() )
					{
						return;
					}
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

			case C2S_SELECT_KEYS_SYN:
				{
					MSG_C2S_SELECT_KEYS_SYN* pKeysMsg = (MSG_C2S_SELECT_KEYS_SYN*)pMsg;

					if ( !CheckSvrReady() )
					{
						return;
					}

					MSG_S2C_SELECT_KEYS_ACK ackmsg;
					DataSpace::GetInstance().FetchKeys(pKeysMsg->m_szPattern, 0, 100, ackmsg.m_szKeys, ackmsg.m_iKeysCnt );
					if ( 0 == ackmsg.m_iKeysCnt )
					{
						MSG_S2C_GERERAL_RES_CMD genermsg;
						genermsg.m_iRes = egr_NOSUCHKEYS;
						Send((BYTE*)&genermsg, sizeof(MSG_S2C_GERERAL_RES_CMD));
					}
					Send((BYTE*)&ackmsg, ackmsg.GetMsgSize() );

				}
				break;


			default:
				{
					DISPMSG_ERROR("Error protocol type, cat[%d], pro[%d]", pMsgBase->m_byCategory, pMsgBase->m_byProtocol );
				}
				break;
			}
		}
		break;


	case emc_MS_CATEGORY:
		{
			switch (pMsgBase->m_byProtocol)
			{
			case S2M_SLVREGISTER_SYN:
				{
					DISPMSG_SUCCESS( "Accept slave[%s] success!\n", GetIP() );
					m_iAgentType = 2;
					Appender::ReplicateAppendFile( this );
					SlaveMgr::GetInstance().InsertSlave( this );
				}
				break;

			case S2M_APPENDFILE_SYN:
				{
					// sync replication
					
				}
				break;
			}
		}
		break;

	default:
		{
			DISPMSG_ERROR("Error protocol type, cat[%d], pro[%d]", pMsgBase->m_byCategory, pMsgBase->m_byProtocol );
		}
		break;
	}
}

bool ClientAgent::CheckSvrReady()
{
	if ( !DataSpace::GetInstance().IsServerReady() )
	{
		MSG_S2C_GERERAL_RES_CMD genermsg;
		genermsg.m_iRes = egr_SVRNOTREADY;
		Send((BYTE*)&genermsg, sizeof(MSG_S2C_GERERAL_RES_CMD));
		return false;
	}
	return true;
}






