#include "JK_Assert.h"
#include "JK_Console.h"
#include "MasterConnector.h"
#include "NetMsg.h"
#include "Appender.h"
#include "DataSpace.h"

void MasterConnector::OnAccept( DWORD connindex )
{
	DISPMSG_SUCCESS( "Connect master success!/n");
}

void MasterConnector::OnDisconnect()
{
	DISPMSG_ERROR("Master[%s:%d] disconnected!\n", GetIP(), GetPort());
}



void MasterConnector::OnConnect( BOOL bSuccess, DWORD dwNetworkIndex )
{
}


void MasterConnector::OnRecv( BYTE *pMsg, WORD wSize )
{
	MSG_BASE* pMsgBase = (MSG_BASE*)pMsg;
	switch (pMsgBase->m_byCategory)
	{
	case emc_CS_CATEGORY:
		{
			JK_ASSERT( S2C_SVR_READY_CMD == pMsgBase->m_byProtocol );
			MSG_S2M_SLVREGISTER_SYN regmsg;
			Send( (BYTE*)&regmsg, sizeof(regmsg) ); 
		}
		break;

	case emc_MS_CATEGORY:
		{
			switch (pMsgBase->m_byProtocol)
			{
			case M2S_GERERAL_RES_CMD:
				{
				}
				break;

			case M2S_APPENDFILE_ACK:
				{
					MSG_M2S_APPENDFILE_ACK* packMsg = (MSG_M2S_APPENDFILE_ACK*)pMsg;
					JK_ASSERT( packMsg );
					for ( int i=0; i<packMsg->m_iCmdCnt; ++i  )
					{
						Appender::HandleCmdLine( packMsg->m_CmdArray[i], false );
					}
					DataSpace::GetInstance().SetRepState( true );
					DISPMSG_NOTICE( "replication is done!\n");
				}
				break;

			case M2S_APPENDCOMMAND_CMD:
				{
					MSG_M2S_APPENDCOMMAND_CMD* packMsg = (MSG_M2S_APPENDCOMMAND_CMD*)pMsg;
					JK_ASSERT( packMsg );
					Appender::HandleCmdLine( packMsg->m_strCmd, true );
				}
				break;

			default:
				{
					DISPMSG_ERROR("Error protocol type, cat[%d], pro[%d]", emc_MS_CATEGORY, pMsgBase->m_byProtocol );
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