#ifndef __NETPROTOCOL_PX7G7U4_H__
#define __NETPROTOCOL_PX7G7U4_H__

#include "Define.h"

/*   包结构命名规则 Jake.Sun
1) 请求(Request)			_SYN
2) 请求应答(Answer)			_ACK
3) 通知(Command)			-CMD
4) 广播(Broadcasting)		_BRD
*/


// message category
enum eMSGCATEGORY
{
	emc_CS_CATEGORY			= 1,			// client & server message
	emc_MS_CATEGORY			= 2,			// master & slave message
};


// client & server protocol number
enum elp_CS_PROTOCOL
{
	S2C_SVR_READY_CMD		= 0,			// S2C:服务器准备
	S2C_GERERAL_RES_CMD		= 4,			// S2C:服务器通用通知
	S2C_SELECT_ITEM_ACK		= 6,			// S2C:查询回复

	C2S_CLTREGISTER_SYN		= 31,			// C2S:登记
	C2S_INSERT_ITEM_SYN		= 51,			// C2S:请求插入key-value pair
	C2S_REMOVE_ITEM_SYN		= 55,			// C2S:请求删除指定key
	C2S_UPDATE_ITEM_SYN		= 57,			// C2S:请求更新指定key的值
	C2S_SELECT_ITEM_SYN		= 59,			// C2S:请求查询指定key
	C2S_SELECT_KEYS_SYN		= 61,			// C2S:请求KEYS
	S2C_SELECT_KEYS_ACK		= 62,			// S2C:回复KEYS

	C2S_EXISTS_KEY_SYN		= 71,			// C2S:是否存在

	C2S_LPUSH_ITEM_SYN		= 81,			// C2S:lpush command
	C2S_RPUSH_ITEM_SYN		= 83,			// C2S:rpush command
	C2S_LPOP_ITEM_SYN		= 85,			// C2S:lpop command
	C2S_RPOP_ITEM_SYN		= 87,			// C2S:rpop command
	C2S_LLEN_ITEM_SYN		= 89,			// C2S:llen command
	C2S_EXPIRE_KEY_SYN		= 91,			// C2S:expire command

	C2S_HSET_ITEM_SYN		= 111,			// C2S:hash set command

};



// master & slave protocol number
enum elp_MS_PROTOCOL
{
	M2S_GERERAL_RES_CMD		= 0,			// M2S:Master通用通知

	S2M_SLVREGISTER_SYN		= 1,			// S2M:register
	S2M_APPENDFILE_SYN		= 3,			// S2M:请求同步append文件
	M2S_APPENDFILE_ACK		= 2,			// M2S:append 文件回复
	M2S_APPENDCOMMAND_CMD	= 4,			// M2S:append command

};


enum GENERALRESULT
{
	egr_NONE				= 0,
	egr_INSERTSUCCESS		= 1,
	egr_INSERTFAILD			= 2,
	egr_CANTFINDVAL			= 3,
	egr_REMOVESUCCESS		= 4,
	egr_SVRNOTREADY			= 5,
	egr_NOSUCHKEYS			= 6,
	egr_KEYEXISTS			= 7,
	egr_EXPIRESUCCESS		= 8,
	egr_EXPIREFAILD			= 9,
};

#pragma pack(push,1)

//-------------------------------------------------------------------------------------------------------
// --基础包
// --最基础的 Packet,所有包的父类
class MSG_BASE
{
public:
	BYTE						m_byCategory;	// 协议分类
	BYTE						m_byProtocol;
};


/* 类名: MSG_SERVER_TYPE
 * 说明: 服务器间登陆消息，指示了服务器类型，ID，名称，最大玩家数相关信息
 */
struct MSG_SERVER_TYPE : public MSG_BASE
{
	BYTE						m_byServerType;					// 服务器类型
 	char						m_szServerName[32];				// 服务器名称
 	UINT16						m_nChanleID;					// 所属区ID
	UINT16						m_nServerID;					// 服务器ID	小区内必须唯一
	UINT16						m_nMaxPlayerCount;				// 服务器最大用户数量
};


struct MSG_DBPROXY_RESULT : public MSG_BASE
{
	void *						m_pData;
};



// Server --> Client
class MSG_S2C_GERERAL_RES_CMD : public MSG_BASE
{
public:
	MSG_S2C_GERERAL_RES_CMD()
	{
		m_byCategory = emc_CS_CATEGORY;
		m_byProtocol = S2C_GERERAL_RES_CMD;
	}
	int		m_iRes;
};


// Server --> Client
class MSG_S2C_SVR_READY_CMD : public MSG_BASE
{
public:
	MSG_S2C_SVR_READY_CMD()
	{
		m_byCategory = emc_CS_CATEGORY;
		m_byProtocol = S2C_SVR_READY_CMD;
	}
	SHORT			sHighVer;					// --高版本号
	SHORT			sLowVer;					// --低版本号
	int				iEncKey;					// --密钥
	char			svrIp[MAX_KEY_LEN];			// --server ip
	unsigned int	svrPort;					// --server 端口
};


// Client-->Server ：register to server
class MSG_C2S_CLTREGISTER_SYN : public MSG_BASE
{
public:
	MSG_C2S_CLTREGISTER_SYN()
	{
		m_byCategory = emc_CS_CATEGORY;
		m_byProtocol = C2S_CLTREGISTER_SYN;
	}
};


// Client-->Server ：请求写入一条数据
class MSG_C2S_INSERT_ITEM_SYN : public MSG_BASE
{
public:
	MSG_C2S_INSERT_ITEM_SYN()
	{
		m_byCategory = emc_CS_CATEGORY;
		m_byProtocol = C2S_INSERT_ITEM_SYN;
		m_usValLen = 1024;
	}
	unsigned short	m_usKeyLen;
	unsigned short	m_usValLen;
	char			strKey[168];
	char			strVal[1024];
	int GetMsgSize()
	{
		return sizeof(MSG_C2S_INSERT_ITEM_SYN)-( (1024-m_usValLen) ) + 1; // --1 for "\0"
	}
};


// --Client-->Server ：请求查询一个key
class MSG_C2S_SELECT_ITEM_SYN : public MSG_BASE
{
public:
	MSG_C2S_SELECT_ITEM_SYN()
	{
		m_byCategory = emc_CS_CATEGORY;
		m_byProtocol = C2S_SELECT_ITEM_SYN;
	}
	char			strKey[MAX_KEY_LEN];
};


// --Server-->Client ：查询回复
class MSG_S2C_SELECT_ITEM_ACK : public MSG_BASE
{
public:
	MSG_S2C_SELECT_ITEM_ACK()
	{
		m_byCategory = emc_CS_CATEGORY;
		m_byProtocol = S2C_SELECT_ITEM_ACK;
	}
	char			strKey[MAX_KEY_LEN];
	char			strVal[1024];
};


// --Client-->Server ：请求删除一个key
class MSG_C2S_REMOVE_ITEM_SYN : public MSG_C2S_SELECT_ITEM_SYN
{
public:
	MSG_C2S_REMOVE_ITEM_SYN()
	{
		m_byProtocol = C2S_REMOVE_ITEM_SYN;
	}
};


// --Client-->Server ：请求KEYS
class MSG_C2S_SELECT_KEYS_SYN : MSG_BASE
{
public:
	MSG_C2S_SELECT_KEYS_SYN()
	{
		m_byCategory = emc_CS_CATEGORY;
		m_byProtocol = C2S_SELECT_KEYS_SYN;
	}
	char		m_szPattern[MAX_KEY_LEN];
};


// --Server-->Client ：reply KEYS command
class MSG_S2C_SELECT_KEYS_ACK : public MSG_BASE
{
public:
	MSG_S2C_SELECT_KEYS_ACK()
	{
		m_byCategory = emc_CS_CATEGORY;
		m_byProtocol = S2C_SELECT_KEYS_ACK;
	}
	int			m_iKeysCnt;
	char		m_szKeys[1024][MAX_KEY_LEN];
	int GetMsgSize()
	{
		return sizeof(MSG_S2C_SELECT_KEYS_ACK)-( (1024-m_iKeysCnt)*MAX_KEY_LEN ); // --1 for "\0"
	}
};

// Client-->Server ：key 是否存在
class MSG_C2S_EXISTS_KEY_SYN : public MSG_C2S_SELECT_ITEM_SYN
{
public:
	MSG_C2S_EXISTS_KEY_SYN()
	{
		m_byProtocol = C2S_EXISTS_KEY_SYN;
	}
};



// Client-->Server ：请求LPUSH
class MSG_C2S_LPUSH_ITEM_SYN : public MSG_C2S_INSERT_ITEM_SYN
{
public:
	MSG_C2S_LPUSH_ITEM_SYN()
	{
		m_byProtocol = C2S_LPUSH_ITEM_SYN;
	}
};


// Client-->Server ：请求RPUSH
class MSG_C2S_RPUSH_ITEM_SYN : public MSG_C2S_INSERT_ITEM_SYN
{
public:
	MSG_C2S_RPUSH_ITEM_SYN()
	{
		m_byProtocol = C2S_RPUSH_ITEM_SYN;
	}
};


// Client-->Server ：请求LPOP
class MSG_C2S_LPOP_ITEM_SYN : public MSG_C2S_SELECT_ITEM_SYN
{
public:
	MSG_C2S_LPOP_ITEM_SYN()
	{
		m_byProtocol = C2S_LPOP_ITEM_SYN;
	}
};


// Client-->Server ：请求RPOP
class MSG_C2S_RPOP_ITEM_SYN : public MSG_C2S_SELECT_ITEM_SYN
{
public:
	MSG_C2S_RPOP_ITEM_SYN()
	{
		m_byProtocol = C2S_RPOP_ITEM_SYN;
	}
};


// Client-->Server ：请求LIST长度
class MSG_C2S_LLEN_ITEM_SYN : public MSG_C2S_SELECT_ITEM_SYN
{
public:
	MSG_C2S_LLEN_ITEM_SYN()
	{
		m_byProtocol = C2S_LLEN_ITEM_SYN;
	}
};

// Client-->Server ：設置过期键
class MSG_C2S_EXPIRE_KEY_SYN : public MSG_C2S_SELECT_ITEM_SYN
{
public:
	MSG_C2S_EXPIRE_KEY_SYN()
	{
		m_byProtocol = C2S_EXPIRE_KEY_SYN;
		m_iSeconds = 0;
	}
	unsigned int m_iSeconds;
};


// Client-->Server ：設置哈希数据结构
class MSG_C2S_HSET_ITEM_SYN : public MSG_C2S_SELECT_ITEM_SYN
{
public:
	MSG_C2S_HSET_ITEM_SYN()
	{
		m_byProtocol = C2S_HSET_ITEM_SYN;
		m_iSeconds = 0;
	}
	unsigned int m_iSeconds;
};




// Slave-->Master ：register to master
class MSG_S2M_SLVREGISTER_SYN : public MSG_BASE
{
public:
	MSG_S2M_SLVREGISTER_SYN()
	{
		m_byCategory = emc_MS_CATEGORY;
		m_byProtocol = S2M_SLVREGISTER_SYN;
	}
};


// Slave-->Master ：request replication
class MSG_S2M_APPENDFILE_SYN : public MSG_BASE
{
public:
	MSG_S2M_APPENDFILE_SYN()
	{
		m_byCategory = emc_MS_CATEGORY;
		m_byProtocol = S2M_APPENDFILE_SYN;
	}
};

// Master-->Master ：ack replication
class MSG_M2S_APPENDFILE_ACK : public MSG_BASE
{
public:
	MSG_M2S_APPENDFILE_ACK()
	{
		m_byCategory = emc_MS_CATEGORY;
		m_byProtocol = M2S_APPENDFILE_ACK;
	}
	int		m_iCmdCnt;
	char	m_CmdArray[1024][MAX_CMD_LEN];

	int GetMsgSize()
	{
		return sizeof(MSG_M2S_APPENDFILE_ACK)-( (1024-m_iCmdCnt)*MAX_CMD_LEN ); // --1 for "\0"
	}
};



class MSG_M2S_APPENDCOMMAND_CMD : public MSG_BASE
{
public:
	MSG_M2S_APPENDCOMMAND_CMD()
	{
		m_byCategory = emc_MS_CATEGORY;
		m_byProtocol = M2S_APPENDCOMMAND_CMD;
	}
	unsigned char m_byLen;
	char	m_strCmd[MAX_CMD_LEN];

	int GetMsgSize()
	{
		return sizeof(MSG_M2S_APPENDCOMMAND_CMD)-( (MAX_CMD_LEN-m_byLen) ) + 1; // --1 for "\0"
	}
};


#pragma pack(pop)




#endif // __PROTOCOL_CL_H__


