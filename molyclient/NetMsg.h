#pragma  once

#pragma pack( push )
#pragma pack( 1 )

#define GAME_PROTOCOL_VER 7
const BYTE HMsg_SystemMsg = 0;
const BYTE ENDMSG  = 0xbf;


//class stNetMsgHead
//{
//public:
//	BYTE byVar;
//	WORD wLen;
//	BYTE byHostCmd;
//	WORD wAssistantCmd;
//	stNetMsgHead()
//	{
//		byVar = 0x00;
//		wLen = 0;
//		byHostCmd = 0x00;
//		wAssistantCmd = 0x00;
//	}
//	int	GetHeadSize(){ return sizeof(stNetMsgHead); };
//	inline void CleanMsg(){};
//	inline void RevertMsg(){};
//	WORD GetMsgLen() { return wLen; }
//};

struct sMSG_BASE
{
	BYTE			m_byCategory;	// 协议分类
	BYTE			m_byProtocol;	// 分类下的具体类型
};

struct sMSG_FORWARD : public sMSG_BASE
{
	DWORD			m_dwKey;
};

struct sMSG_FORWARDEx : public sMSG_FORWARD
{
	DWORD			m_dwASConnctID;
};


#pragma pack( pop )


//#define BEGIN_MAKE_MSG_CLASS( HostCmd , x ) class CMsg_##x : public stNetMsgHead \
//{\
//public:\
//	CMsg_##x::CMsg_##x() {\
//	byVar = GAME_PROTOCOL_VER;\
//	wLen = sizeof( CMsg_##x );\
//	byHostCmd = HostCmd;\
//	wAssistantCmd = x;  } \
//	CMsg_##x& CMsg_##x::operator=( const CMsg_##x& v ) {\
//	memcpy(this, &v,sizeof(CMsg_##x) );\
//	return *this; }\
//	void* CMsg_##x::GetData() {\
//	return (void*)this; }
//#define END_MAKE_MSG_CLASS( HostCmd, x ) }; 


#define BEGIN_MAKE_MSG_CLASS( HostCmd , x ) class CMsg_##x : public sMSG_BASE \
{\
public:\
	CMsg_##x::CMsg_##x() {\
	m_byCategory = HostCmd;\
	m_byProtocol = x; }\
	CMsg_##x& CMsg_##x::operator=( const CMsg_##x& v ) {\
	memcpy(this, &v,sizeof(CMsg_##x) );\
	return *this; }\
	void* CMsg_##x::GetData() {\
	return (void*)this; }
#define END_MAKE_MSG_CLASS() }; 


#define BEGIN_MAKE_FWD2_MSG_CLASS( HostCmd , x ) class CMsg_##x : public sMSG_FORWARDEx \
{\
public:\
	CMsg_##x::CMsg_##x() {\
	m_byCategory = HostCmd;\
	m_byProtocol = x; }\
	CMsg_##x& CMsg_##x::operator=( const CMsg_##x& v ) {\
	memcpy(this, &v,sizeof(CMsg_##x) );\
	return *this; }\
	void* CMsg_##x::GetData() {\
	return (void*)this; }
#define END_MAKE_FWD2_MSG_CLASS() }; 

#pragma pack( push )
#pragma pack( 1 )



#pragma pack( pop )
