#include <stdio.h>
#include "ClientAgent.h"
#include "NetMsg.h"


ClientAgent::ClientAgent( )
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
	printf("Sending Client Message:[CS_LOGON][MSG_S2C_SVR_READY_CMD] ...");
	//Send(connindex, (char*)&readyPacket, sizeof(MSG_S2C_SVR_READY_CMD));
	Send( (BYTE*)&readyPacket, sizeof(MSG_S2C_SVR_READY_CMD));
}



void ClientAgent::OnRecv(BYTE *pMsg, WORD wSize)
{
	//MSG_BASE* pMsgBase = (MSG_BASE*)msgptr->GetBuf();
	//if (pMsgBase->m_byCategory != CS_LOGON)
	//{
	//	return;
	//}

	//USER_INFOMATION* pUserInfo = UserInfoManager<USER_INFOMATION>::GetInstance()->FindUserByConnectID(connindex);
	//if (NULL == pUserInfo)
	//{
	//	return CloseSocket(connindex);
	//}
	//g_CSPackHander.ParsePacket_CS(pUserInfo, (MSG_BASE *)msgptr->GetBuf(), length);
}


void ClientAgent::OnDisconnect()
{

}


void ClientAgent::OnConnect(BOOL bSuccess, DWORD dwNetworkIndex)
{

}



