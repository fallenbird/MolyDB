#pragma once
#include "NetMsg.h"
#include "ClientStruct.h"
#include "GDefines.h"

#pragma pack( push )
#pragma pack( 1 )


const BYTE HMsg_TLConnect	= 10;			// --连接认证

enum enSCConnectAuth
{
	l2t_AUTH_CONNECT_ACK	= 1,			// --登陆准备完成
	t2l_AUTH_VERSION_SYN	= 2,			// --版本验证请求
	l2t_AUTH_VERSION_ACK	= 3,			// --版本验证反馈
	t2l_AUTH_VERIFY_SYN		= 101,			// --密码验证请求
	l2t_AUTH_VERIFY_ACK		= 5,			// --密码验证反馈
	l2t_AUTH_GMINFO_ACK		= 105,			// --密码验证反馈
	t2l_AUTH_SVRLIST_SYN	= 6,			// --服务器列表请求
	l2t_AUTH_SVRLIST_ACK	= 10,			// --服务器列表反馈
	t2l_AUTH_SELESVR_SYN	= 8,			// --服务器选择请求
	l2t_AUTH_SELESVR_ACK	= 9,			// --服务器选择反馈
};


BEGIN_MAKE_MSG_CLASS( HMsg_TLConnect, l2t_AUTH_CONNECT_ACK )
DWORD	m_dwEncKey;
END_MAKE_MSG_CLASS()


BEGIN_MAKE_MSG_CLASS( HMsg_TLConnect, t2l_AUTH_VERSION_SYN )
BYTE		m_byHVersion;							// --版本
BYTE		m_byMVersion;
BYTE		m_byLVersion;
char		m_szLocalIP[MAX_IP_ADDR_LEN];
END_MAKE_MSG_CLASS()


BEGIN_MAKE_MSG_CLASS( HMsg_TLConnect, l2t_AUTH_VERSION_ACK )
BYTE		m_byResult;
END_MAKE_MSG_CLASS()


BEGIN_MAKE_MSG_CLASS( HMsg_TLConnect, t2l_AUTH_VERIFY_SYN )
DWORD		m_dwAuthUserID;							// --验证 服务器ID号
char		m_szID	[MAX_USER_ID_LEN];				// --帐号
char		m_szPwd [MAX_PASSWORD_LEN];				// --密码
END_MAKE_MSG_CLASS()


BEGIN_MAKE_MSG_CLASS( HMsg_TLConnect, l2t_AUTH_VERIFY_ACK )
short		m_sErrCode;
END_MAKE_MSG_CLASS()


BEGIN_MAKE_MSG_CLASS( HMsg_TLConnect, t2l_AUTH_SVRLIST_SYN )
END_MAKE_MSG_CLASS()


BEGIN_MAKE_MSG_CLASS( HMsg_TLConnect, l2t_AUTH_SVRLIST_ACK )
BYTE		m_bySvrCnt;								// --服务器个数
AGENT_SERVER_INFO m_szServerList[MAX_SERVERS_CNT];	// --列表
END_MAKE_MSG_CLASS()


BEGIN_MAKE_MSG_CLASS( HMsg_TLConnect, t2l_AUTH_SELESVR_SYN )
USHORT		m_usSvrIndex;							// 选择频道服务器ID
END_MAKE_MSG_CLASS()


BEGIN_MAKE_MSG_CLASS( HMsg_TLConnect, l2t_AUTH_SELESVR_ACK )
DWORD		m_dwAuthUserID;							// LoginServer 分配的KEY 必须使用这个KEY才能登陆到AgentServer
BYTE		m_szSerialKey[MAX_AUTH_KEY_LEN];		// connection 
char		m_szSvrIP[MAX_IP_ADDR_LEN];				// 游戏 服务器 IP
DWORD		m_dwSvrPort;							// 游戏 服务器 
BYTE		m_byResult;								//
END_MAKE_MSG_CLASS()


BEGIN_MAKE_MSG_CLASS( HMsg_TLConnect, l2t_AUTH_GMINFO_ACK )
short		m_sGMRightLv;
DWORD		m_dwGMGuid;
END_MAKE_MSG_CLASS()


#pragma pack( pop )