#ifndef __NETPROTOCOL_PX7G7U4_H__
#define __NETPROTOCOL_PX7G7U4_H__

#include "Define.h"

/*   ���ṹ�������� Jake.Sun
1) ����(Request)			_SYN
2) ����Ӧ��(Answer)			_ACK
3) ֪ͨ(Command)			-CMD
4) �㲥(Broadcasting)		_BRD
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
	S2C_SVR_READY_CMD		= 0,			// S2C:������׼��
	S2C_GERERAL_RES_CMD		= 4,			// S2C:������ͨ��֪ͨ
	S2C_SELECT_ITEM_ACK		= 6,			// S2C:��ѯ�ظ�

	C2S_HEARTBEAT_SYN		= 11,			// C2S:����
	S2C_HEARTBEAT_ACK		= 12,			// S2C:����

	C2S_CLTREGISTER_SYN		= 31,			// C2S:�Ǽ�
	C2S_INSERT_ITEM_SYN		= 51,			// C2S:�������key-value pair
	C2S_REMOVE_ITEM_SYN		= 55,			// C2S:����ɾ��ָ��key
	C2S_UPDATE_ITEM_SYN		= 57,			// C2S:�������ָ��key��ֵ
	C2S_SELECT_ITEM_SYN		= 59,			// C2S:�����ѯָ��key
	C2S_SELECT_KEYS_SYN		= 61,			// C2S:����KEYS
	S2C_SELECT_KEYS_ACK		= 62,			// S2C:�ظ�KEYS

	C2S_EXISTS_KEY_SYN		= 71,			// C2S:�Ƿ����

	C2S_LPUSH_ITEM_SYN		= 81,			// C2S:lpush command
	C2S_RPUSH_ITEM_SYN		= 83,			// C2S:rpush command
	C2S_LPOP_ITEM_SYN		= 85,			// C2S:lpop command
	C2S_RPOP_ITEM_SYN		= 87,			// C2S:rpop command
	C2S_LLEN_ITEM_SYN		= 89,			// C2S:llen command
	C2S_EXPIRE_KEY_SYN		= 91,			// C2S:expire command

	C2S_HSET_ITEM_SYN		= 111,			// C2S:hash set command
	C2S_HGET_ITEM_SYN		= 112,

	C2S_ZADD_ITEM_SYN		= 121,			// C2S:hash set command
	C2S_ZRANGE_ITEM_SYN		= 122,


	C2S_PAGERECORD_SYN		= 121,			// C2S:��¼PV

};



// master & slave protocol number
enum elp_MS_PROTOCOL
{
	M2S_GERERAL_RES_CMD		= 0,			// M2S:Masterͨ��֪ͨ

	S2M_SLVREGISTER_SYN		= 1,			// S2M:register
	S2M_APPENDFILE_SYN		= 3,			// S2M:����ͬ��append�ļ�
	M2S_APPENDFILE_ACK		= 2,			// M2S:append �ļ��ظ�
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
	egr_HSETSUCCESS			= 21,
	egr_HSETFAILD			= 22,
};

#pragma pack(push,1)

//-------------------------------------------------------------------------------------------------------
// --������
// --������� Packet,���а��ĸ���
class MSG_BASE
{
public:
	BYTE						m_byCategory;	// Э�����
	BYTE						m_byProtocol;
};


/* ����: MSG_SERVER_TYPE
 * ˵��: ���������½��Ϣ��ָʾ�˷��������ͣ�ID�����ƣ��������������Ϣ
 */
struct MSG_SERVER_TYPE : public MSG_BASE
{
	BYTE						m_byServerType;					// ����������
 	char						m_szServerName[32];				// ����������
 	UINT16						m_nChanleID;					// ������ID
	UINT16						m_nServerID;					// ������ID	С���ڱ���Ψһ
	UINT16						m_nMaxPlayerCount;				// ����������û�����
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
	SHORT			sHighVer;					// --�߰汾��
	SHORT			sLowVer;					// --�Ͱ汾��
	int				iEncKey;					// --��Կ
	char			svrIp[MAX_KEY_LEN];			// --server ip
	unsigned int	svrPort;					// --server �˿�
};


// Client-->Server ��heartbeat
class MSG_C2S_HEARTBEAT_SYN : public MSG_BASE
{
public:
	MSG_C2S_HEARTBEAT_SYN()
	{
		m_byCategory = emc_CS_CATEGORY;
		m_byProtocol = C2S_HEARTBEAT_SYN;
	}
};


// Server-->Client ��heartbeat
class MSG_S2C_HEARTBEAT_ACK : public MSG_BASE
{
public:
	MSG_S2C_HEARTBEAT_ACK()
	{
		m_byCategory = emc_CS_CATEGORY;
		m_byProtocol = S2C_HEARTBEAT_ACK;
	}
};



// Client-->Server ��register to server
class MSG_C2S_CLTREGISTER_SYN : public MSG_BASE
{
public:
	MSG_C2S_CLTREGISTER_SYN()
	{
		m_byCategory = emc_CS_CATEGORY;
		m_byProtocol = C2S_CLTREGISTER_SYN;
	}
};


// Client-->Server ������д��һ������
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
	char			strKey[MAX_KEY_LEN];
	char			strVal[1024];
	int GetMsgSize()
	{
		return sizeof(MSG_C2S_INSERT_ITEM_SYN)-( (1024-m_usValLen) ) + 1; // --1 for "\0"
	}
};


// --Client-->Server �������ѯһ��key
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


// --Server-->Client ����ѯ�ظ�
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


// --Client-->Server ������ɾ��һ��key
class MSG_C2S_REMOVE_ITEM_SYN : public MSG_C2S_SELECT_ITEM_SYN
{
public:
	MSG_C2S_REMOVE_ITEM_SYN()
	{
		m_byProtocol = C2S_REMOVE_ITEM_SYN;
	}
};


// --Client-->Server ������KEYS
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


// --Server-->Client ��reply KEYS command
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


// Client-->Server ��key �Ƿ����
class MSG_C2S_EXISTS_KEY_SYN : public MSG_C2S_SELECT_ITEM_SYN
{
public:
	MSG_C2S_EXISTS_KEY_SYN()
	{
		m_byProtocol = C2S_EXISTS_KEY_SYN;
	}
};


// Client-->Server ������LPUSH
class MSG_C2S_LPUSH_ITEM_SYN : public MSG_C2S_INSERT_ITEM_SYN
{
public:
	MSG_C2S_LPUSH_ITEM_SYN()
	{
		m_byProtocol = C2S_LPUSH_ITEM_SYN;
	}
};


// Client-->Server ������RPUSH
class MSG_C2S_RPUSH_ITEM_SYN : public MSG_C2S_INSERT_ITEM_SYN
{
public:
	MSG_C2S_RPUSH_ITEM_SYN()
	{
		m_byProtocol = C2S_RPUSH_ITEM_SYN;
	}
};


// Client-->Server ������LPOP
class MSG_C2S_LPOP_ITEM_SYN : public MSG_C2S_SELECT_ITEM_SYN
{
public:
	MSG_C2S_LPOP_ITEM_SYN()
	{
		m_byProtocol = C2S_LPOP_ITEM_SYN;
	}
};


// Client-->Server ������RPOP
class MSG_C2S_RPOP_ITEM_SYN : public MSG_C2S_SELECT_ITEM_SYN
{
public:
	MSG_C2S_RPOP_ITEM_SYN()
	{
		m_byProtocol = C2S_RPOP_ITEM_SYN;
	}
};


// Client-->Server ������LIST����
class MSG_C2S_LLEN_ITEM_SYN : public MSG_C2S_SELECT_ITEM_SYN
{
public:
	MSG_C2S_LLEN_ITEM_SYN()
	{
		m_byProtocol = C2S_LLEN_ITEM_SYN;
	}
};


// Client-->Server ���O�ù��ڼ�
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


// Client-->Server ���O�ù��ڼ�
class MSG_C2S_PAGERECORD_SYN : public  MSG_BASE
{
public:
	MSG_C2S_PAGERECORD_SYN()
	{
		m_byProtocol = C2S_PAGERECORD_SYN;
	}
	char strAppKey[64];
};


// Client-->Server ���O�ù�ϣ���ݽṹ
class MSG_C2S_HSET_ITEM_SYN : public MSG_BASE
{
public:
	MSG_C2S_HSET_ITEM_SYN()
	{
		m_byCategory = emc_CS_CATEGORY;
		m_byProtocol = C2S_HSET_ITEM_SYN;
	}
	char			strMap[MAX_KEY_LEN];
	char			strKey[MAX_KEY_LEN];
	unsigned short	m_usKeyLen;
	unsigned short	m_usValLen;
	char			strVal[1024];

	int GetMsgSize()
	{
		return sizeof(MSG_C2S_HSET_ITEM_SYN) - ((1024 - m_usValLen)) + 1; // --1 for "\0"
	}
};


// Client-->Server ����ȡ��ϣ���ݽṹ
class MSG_C2S_HGET_ITEM_SYN : public  MSG_BASE
{
public:
	MSG_C2S_HGET_ITEM_SYN()
	{
		m_byCategory = emc_CS_CATEGORY;
		m_byProtocol = C2S_HGET_ITEM_SYN;
	}
	char			strMap[MAX_KEY_LEN];
	char			strKey[MAX_KEY_LEN];
};



// Client-->Server ���O�ù�ϣ���ݽṹ
class MSG_C2S_ZADD_ITEM_SYN : public MSG_BASE
{
public:
	MSG_C2S_ZADD_ITEM_SYN()
	{
		m_byCategory = emc_CS_CATEGORY;
		m_byProtocol = C2S_ZADD_ITEM_SYN;
	}
	char			strKey[MAX_KEY_LEN];
	unsigned short	m_usScore;
	unsigned short	m_usValLen;
	char			strVal[1024];

	int GetMsgSize()
	{
		return sizeof(MSG_C2S_ZADD_ITEM_SYN) - ((1024 - m_usValLen)) + 1; // --1 for "\0"
	}

};


// Client-->Server ���O�ù�ϣ���ݽṹ
class MSG_C2S_ZRANGE_ITEM_SYN : public MSG_BASE
{
public:
	MSG_C2S_ZRANGE_ITEM_SYN()
	{
		m_byCategory = emc_CS_CATEGORY;
		m_byProtocol = C2S_ZRANGE_ITEM_SYN;
	}
	char			strKey[MAX_KEY_LEN];
	unsigned short	m_usStart;
	unsigned short	m_usStop;
};




// Slave-->Master ��register to master
class MSG_S2M_SLVREGISTER_SYN : public MSG_BASE
{
public:
	MSG_S2M_SLVREGISTER_SYN()
	{
		m_byCategory = emc_MS_CATEGORY;
		m_byProtocol = S2M_SLVREGISTER_SYN;
	}
};


// Slave-->Master ��request replication
class MSG_S2M_APPENDFILE_SYN : public MSG_BASE
{
public:
	MSG_S2M_APPENDFILE_SYN()
	{
		m_byCategory = emc_MS_CATEGORY;
		m_byProtocol = S2M_APPENDFILE_SYN;
	}
};

// Master-->Master ��ack replication
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


