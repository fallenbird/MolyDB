#include "MasterConnector.h"
#include "NetMsg.h"


void MasterConnector::OnAccept( DWORD connindex )
{

}

void MasterConnector::OnDisconnect()
{

}



void MasterConnector::OnConnect( BOOL bSuccess, DWORD dwNetworkIndex )
{

}


void MasterConnector::OnRecv( BYTE *pMsg, WORD wSize )
{
	MSG_BASE* pMsgBase = (MSG_BASE*)pMsg;
	switch (pMsgBase->m_byCategory)
	{
	case emc_MS_CATEGORY:
		{
			switch (pMsgBase->m_byProtocol)
			{
			case C2S_INSERT_ITEM_SYN:
				{
					MSG_C2S_INSERT_ITEM_SYN* pInsertMsg = (MSG_C2S_INSERT_ITEM_SYN*)pMsg;
					//if (DataSpace::GetInstance().InsertKV(pInsertMsg->strKey, pInsertMsg->strVal))
					//{
					//	MSG_S2C_GERERAL_RES_CMD genermsg;
					//	genermsg.m_iRes = egr_INSERTSUCCESS;
					//	Send((BYTE*)&genermsg, sizeof(MSG_S2C_GERERAL_RES_CMD));
					//}
					//else
					//{
					//	MSG_S2C_GERERAL_RES_CMD genermsg;
					//	genermsg.m_iRes = egr_INSERTFAILD;
					//	Send((BYTE*)&genermsg, sizeof(MSG_S2C_GERERAL_RES_CMD));
					//}
				}
				break;

			case C2S_SELECT_ITEM_SYN:
				{
					//MSG_C2S_SELECT_ITEM_SYN* pInsertMsg = (MSG_C2S_SELECT_ITEM_SYN*)pMsg;
					//char* strVal = (char*)DataSpace::GetInstance().GetValue(pInsertMsg->strKey);
					//if (NULL == strVal)
					//{
					//	MSG_S2C_GERERAL_RES_CMD genermsg;
					//	genermsg.m_iRes = egr_CANTFINDVAL;
					//	Send((BYTE*)&genermsg, sizeof(MSG_S2C_GERERAL_RES_CMD));
					//}
					//else
					//{
					//	MSG_S2C_SELECT_ITEM_ACK ackmsg;
					//	strcpy_s(ackmsg.strKey, 168, pInsertMsg->strKey);
					//	strcpy_s(ackmsg.strVal, 1024, strVal);
					//	Send((BYTE*)&ackmsg, sizeof(MSG_S2C_SELECT_ITEM_ACK));
					//}
				}
				break;

			case C2S_REMOVE_ITEM_SYN:
				{
					//MSG_C2S_REMOVE_ITEM_SYN* pInsertMsg = (MSG_C2S_REMOVE_ITEM_SYN*)pMsg;
					//if ( !DataSpace::GetInstance().RemoveKV(pInsertMsg->strKey) )
					//{
					//	MSG_S2C_GERERAL_RES_CMD genermsg;
					//	genermsg.m_iRes = egr_CANTFINDVAL;
					//	Send((BYTE*)&genermsg, sizeof(MSG_S2C_GERERAL_RES_CMD));
					//}
					//else
					//{
					//	MSG_S2C_GERERAL_RES_CMD genermsg;
					//	genermsg.m_iRes = egr_REMOVESUCCESS;
					//	Send((BYTE*)&genermsg, sizeof(MSG_S2C_GERERAL_RES_CMD));
					//}
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