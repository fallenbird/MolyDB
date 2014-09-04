#include "DataSpace.h"
#include "AppendCmdQueue.h"
#include "JK_Utility.h"
#include "JK_MemMgr.h"
#include "JK_Console.h"
#include "SlaveMgr.h"
#include "NetMsg.h"

DataSpace::DataSpace(): m_bReplicated(false)
{

}

DataSpace::~DataSpace()
{

}

bool	DataSpace::InitDB( bool bSlave )
{
	m_bSlave = bSlave;
	return m_normalDict.InitDictionary() && m_expireDict.InitDictionary();
}

void	DataSpace::UpdateDB(int iUpdateMS )
{
	m_normalDict.UpdateDict(iUpdateMS);
}



bool DataSpace::InsertKV( char* key, int keylen, char* val, int vallen, bool ops )
{
	void* pkey = JK_MALLOC( keylen+1 );
	void* pval = JK_MALLOC( vallen+1 );
	JK_MEMCPY_S(pkey,keylen+1,key,keylen+1);
	JK_MEMCPY_S(pval,vallen+1,val,vallen+1);

	if (  m_normalDict.AddElement( pkey, pval ) )
	{
		if ( ops )
		{
			Operation( 101, pkey, pval );
			Replication( 101, key, pval );
		}
		return true;
	}
	JK_FREE( pkey );
	JK_FREE( pval );
	return false;
}

void* DataSpace::GetValue(void* key)
{
	return m_normalDict.GetElement(key);
}


bool DataSpace::RemoveKV(void* key, bool ops )
{
	if ( m_normalDict.RemoveElement(key) )
	{
		if ( ops )
		{
			Operation( 102, key, "" );
			Replication( 102, key, "" );
		}
		return true;
	}
	return false;
}


void DataSpace::Operation( int cmd, void* key, void* val )
{
	char* strTemp = (char*)JK_MALLOC(128);		// char[128]; 
	JK_SPRITF_S( strTemp, 128, "%d %s %s \n", cmd, (char*)key, (char*)val );
	AppendCmdQueue::Enqueue( strTemp );
}



void DataSpace::Replication( int cmd, void* key, void* val )
{
	if ( m_bSlave )
	{
		return;
	}
	MSG_M2S_APPENDCOMMAND_CMD cmdMsg;
	cmdMsg.m_byLen = JK_SPRITF_S( cmdMsg.m_strCmd, MAX_CMD_LEN, "%d %s %s \n", cmd, (char*)key, (char*)val );
	SlaveMgr::GetInstance().BroadAllSlave( (BYTE*)&cmdMsg, cmdMsg.GetMsgSize() );
}



bool DataSpace::IsServerReady()
{
	if ( m_bSlave && !m_bReplicated  )
	{
		return false;
	}
	return true;
}


