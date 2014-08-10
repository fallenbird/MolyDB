// molyclient.cpp : Defines the entry point for the console application.
//

#include "NetBase.h"
#include "NetMsg.h "


CNetBase*			g_pNetBase;						// 网络接口

#define MAX_CONNECT_NUM 50
#define NET_MSG WM_USER + 100

//-----------------------------------------------------------------------------------------------------------------------
// Prototype	:		int	CMyAppDlg::ProcessLSMsg( char* pMsg )
// Function		:		处理登陆服务器消息
// Input  Param :		- 
// Output Param :		-
// Info			:		SXF	/ 2012.08.01 / 1.0
//-----------------------------------------------------------------------------------------------------------------------
int ServerPro( int nConctID, char* pMsg, int nLen )
{
	//stNetMsgHead* pMsgHead = (stNetMsgHead*)pMsg;
	sMSG_BASE* pMsgHead = (sMSG_BASE*)pMsg;

	//解析消息头
	//switch( pMsgHead->byHostCmd )
	switch( pMsgHead->m_byCategory )
	{
	case HMsg_SystemMsg:
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


int main(int argc, char* argv[])
{
	g_pNetBase = new CNetBase;
	g_pNetBase->InitNet( MAX_CONNECT_NUM, ServerPro, NULL, NET_MSG );

	return 0;
}

