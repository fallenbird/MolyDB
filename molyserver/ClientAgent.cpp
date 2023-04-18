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
	readyPacket.sHighVer = 5;
	readyPacket.sLowVer = 4;
	readyPacket.iEncKey = 666;
	//readyPacket.svrIp = 
	Send((BYTE*)&readyPacket, sizeof(MSG_S2C_SVR_READY_CMD));
	//Send(connindex, (char*)&readyPacket, sizeof(MSG_S2C_SVR_READY_CMD));

}


VOID ClientAgent::OnDisconnect()
{
	switch (m_iAgentType )
	{
	case 1:
		{
			DISPMSG_ERROR( "Client[%s:%d] disconnected!\n", GetIP(),GetPort() );
		}
		break;

	case 2:
		{
			DISPMSG_ERROR( "Slave[%s:%d] disconnected!\n", GetIP(), GetPort());
		}
		break;
	default:
		{
			DISPMSG_ERROR( "Agent[%s:%d] disconnected!\n", GetIP(), GetPort());
		}
		break;
	}

}


void ClientAgent::OnConnect(BOOL bSuccess, DWORD dwNetworkIndex)
{

}

BOOL ClientAgent::Send(BYTE *pMsg, WORD wSize) 
{
	assert(wSize <= MAX_PACK_SIZE);
	return NetworkObject::Send(pMsg, wSize);
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
			case C2S_HEARTBEAT_SYN: 
			{
				MSG_S2C_HEARTBEAT_ACK ackMsg;
				Send((BYTE*)&ackMsg, sizeof(MSG_S2C_HEARTBEAT_ACK));
			}
			break;

			case C2S_CLTREGISTER_SYN:
				{
					DISPMSG_SUCCESS( "Accept client[%s:%d] success!\n", GetIP(),GetPort() );
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
						ReplyResult( egr_INSERTSUCCESS );
					}
					else
					{
						ReplyResult( egr_INSERTFAILD );
					}
				}
				break;

			case C2S_SELECT_ITEM_SYN:
				{
					MSG_C2S_SELECT_ITEM_SYN* pInsertMsg = (MSG_C2S_SELECT_ITEM_SYN*)pMsg;
					char* strVal = (char*)DataSpace::GetInstance().GetValue(pInsertMsg->strKey, evt_STR );
					if (NULL == strVal)
					{
						ReplyResult( egr_CANTFINDVAL );
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
						ReplyResult( egr_CANTFINDVAL );
					}
					else
					{
						ReplyResult( egr_REMOVESUCCESS );
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
						ReplyResult( egr_NOSUCHKEYS );
					}
					Send((BYTE*)&ackmsg, ackmsg.GetMsgSize() );

				}
				break;

			case C2S_EXISTS_KEY_SYN:
				{
					MSG_C2S_EXISTS_KEY_SYN* pKeysMsg = (MSG_C2S_EXISTS_KEY_SYN*)pMsg;

					if ( !CheckSvrReady() )
					{
						return;
					}
					if( DataSpace::GetInstance().IsExists(pKeysMsg->strKey ) )
					{
						ReplyResult( egr_KEYEXISTS );
					}
					else
					{
						ReplyResult( egr_NOSUCHKEYS );
					}
				}
				break;

			case C2S_LPUSH_ITEM_SYN:
				{
					MSG_C2S_LPUSH_ITEM_SYN* pInsertMsg = (MSG_C2S_LPUSH_ITEM_SYN*)pMsg;

					if ( !CheckSvrReady() )
					{
						return;
					}
					if (DataSpace::GetInstance().ListPushLeft(pInsertMsg->strKey, pInsertMsg->m_usKeyLen, pInsertMsg->strVal, pInsertMsg->m_usValLen))
					{
						ReplyResult( egr_INSERTSUCCESS );
					}
					else
					{
						ReplyResult( egr_INSERTFAILD );
					}
				}
				break;


			case C2S_RPUSH_ITEM_SYN:
				{
					MSG_C2S_RPUSH_ITEM_SYN* pInsertMsg = (MSG_C2S_RPUSH_ITEM_SYN*)pMsg;
					if ( !CheckSvrReady() )
					{
						return;
					}
					if (DataSpace::GetInstance().ListPushRight(pInsertMsg->strKey, pInsertMsg->m_usKeyLen, pInsertMsg->strVal, pInsertMsg->m_usValLen))
					{
						ReplyResult( egr_INSERTSUCCESS );
					}
					else
					{
						ReplyResult( egr_INSERTFAILD );
					}
				}
				break;

			case C2S_LPOP_ITEM_SYN:
				{
					MSG_C2S_LPOP_ITEM_SYN* pInsertMsg = (MSG_C2S_LPOP_ITEM_SYN*)pMsg;
					if ( !CheckSvrReady() )
					{
						return;
					}

					char* strVal = (char*)( DataSpace::GetInstance().ListPopLeft(pInsertMsg->strKey) );
					if (NULL == strVal)
					{
						ReplyResult( egr_CANTFINDVAL );
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

			case C2S_RPOP_ITEM_SYN:
				{
					if ( !CheckSvrReady() )
					{
						return;
					}
					MSG_C2S_RPOP_ITEM_SYN* pInsertMsg = (MSG_C2S_RPOP_ITEM_SYN*)pMsg;
					char* strVal = (char*)( DataSpace::GetInstance().ListPopRight(pInsertMsg->strKey) );
					if (NULL == strVal)
					{
						ReplyResult( egr_CANTFINDVAL );
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

			case C2S_LLEN_ITEM_SYN:
				{
					if ( !CheckSvrReady() )
					{
						return;
					}
					MSG_C2S_LLEN_ITEM_SYN* pllMsg = (MSG_C2S_LLEN_ITEM_SYN*)pMsg;
					int iLength = DataSpace::GetInstance().GetListLength(pllMsg->strKey);
					MSG_S2C_SELECT_ITEM_ACK ackmsg;
					strcpy_s(ackmsg.strKey, MAX_KEY_LEN, pllMsg->strKey);
					JK_SPRITF_S( ackmsg.strVal, 1024, "%d", iLength );
					Send((BYTE*)&ackmsg, sizeof(MSG_S2C_SELECT_ITEM_ACK));

				}
				break;

			case C2S_EXPIRE_KEY_SYN:{
					if (!CheckSvrReady())
					{
						return;
					}
					MSG_C2S_EXPIRE_KEY_SYN*  pllMsg = (MSG_C2S_EXPIRE_KEY_SYN*)pMsg;
					bool bExpire = DataSpace::GetInstance().ExpireKey(pllMsg->strKey, pllMsg->m_iSeconds);
					if (bExpire) 
					{
						ReplyResult(egr_EXPIRESUCCESS);
					}
					else 
					{
						ReplyResult(egr_EXPIREFAILD);
					}
			}break;

			case C2S_HSET_ITEM_SYN: {
				if (!CheckSvrReady())
				{
					return;
				}
				MSG_C2S_HSET_ITEM_SYN*  pllMsg = (MSG_C2S_HSET_ITEM_SYN*)pMsg;
				bool bExpire = DataSpace::GetInstance().HashSet(pllMsg->strMap, pllMsg->m_usKeyLen, pllMsg->strKey, pllMsg->m_usValLen, pllMsg->strVal );
				if (bExpire)
				{
					ReplyResult(egr_HSETSUCCESS);
				}
				else
				{
					ReplyResult(egr_HSETFAILD);
				}
			}break;

			case C2S_HGET_ITEM_SYN: {
				if (!CheckSvrReady())
				{
					return;
				}
				MSG_C2S_HGET_ITEM_SYN* pllMsg = (MSG_C2S_HGET_ITEM_SYN*)pMsg;
				char* value = DataSpace::GetInstance().HashGet(pllMsg->strMap, pllMsg->strKey );
				if (value)
				{
					MSG_S2C_SELECT_ITEM_ACK ackmsg;
					strcpy_s(ackmsg.strKey, MAX_KEY_LEN, pllMsg->strKey);
					strcpy_s(ackmsg.strVal, 1024, value);
					Send((BYTE*)&ackmsg, sizeof(MSG_S2C_SELECT_ITEM_ACK));
				}
				else
				{
					ReplyResult(egr_CANTFINDVAL);
				}
			}break;


			case C2S_ZADD_ITEM_SYN: {
				if (!CheckSvrReady())
				{
					return;
				}
				MSG_C2S_ZADD_ITEM_SYN* pllMsg = (MSG_C2S_ZADD_ITEM_SYN*)pMsg;
				bool bRes = DataSpace::GetInstance().ZSetAdd(pllMsg->strKey, pllMsg->m_usScore, pllMsg->m_usValLen,  pllMsg->strVal );
				if (bRes)
				{
					MSG_S2C_SELECT_ITEM_ACK ackmsg;
					strcpy_s(ackmsg.strKey, MAX_KEY_LEN, pllMsg->strKey);
					strcpy_s(ackmsg.strVal, 1024, pllMsg->strVal);
					Send((BYTE*)&ackmsg, sizeof(MSG_S2C_SELECT_ITEM_ACK));
				}
				else
				{
					ReplyResult(egr_CANTFINDVAL);
				}
			}break;



			case C2S_ZRANGE_ITEM_SYN: {

				if (!CheckSvrReady())
				{
					return;
				}
				MSG_C2S_ZRANGE_ITEM_SYN*  pRangeMsg = (MSG_C2S_ZRANGE_ITEM_SYN*)pMsg;

				MSG_S2C_ZRANGE_ACK ackMsg;
				KVPair  tmpKVPair[1024];
				ackMsg.m_iKeysCnt = 0;
				DataSpace::GetInstance().ZSetRange(tmpKVPair, ackMsg.m_iKeysCnt, pRangeMsg->strKey, pRangeMsg->m_usStart, pRangeMsg->m_usStop );
				if(ackMsg.m_iKeysCnt >0 )
				{
					memcpy_s(ackMsg.m_szPairs, sizeof(MSG_S2C_ZRANGE_ACK::pair)* ackMsg.m_iKeysCnt, tmpKVPair, sizeof(KVPair)* ackMsg.m_iKeysCnt);

				}
				Send((BYTE*)&ackMsg, ackMsg.GetMsgSize() );

			}break;


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


void ClientAgent::ReplyResult( int iRes )
{
	MSG_S2C_GERERAL_RES_CMD genermsg;
	genermsg.m_iRes = iRes;
	Send((BYTE*)&genermsg, sizeof(MSG_S2C_GERERAL_RES_CMD));
}






