
#include <time.h>
#include "DataSpace.h"
#include "AppendCmdQueue.h"
#include "JK_Utility.h"
#include "JK_MemMgr.h"
#include "JK_Console.h"
#include "JK_DList.h"
#include "SlaveMgr.h"
#include "NetMsg.h"


typedef JK_DList<char, true> moly_list_type;

DataSpace::DataSpace(): m_bReplicated(false)
{

}

DataSpace::~DataSpace()
{

}

bool	DataSpace::InitDB( bool bSlave )
{
	m_bSlave = bSlave;
	return m_normalDict.InitDictionary();
}

void	DataSpace::UpdateDB(int iUpdateMS )
{
	m_normalDict.UpdateDict(iUpdateMS);
	
	// --检查过期键
	while (true) 
	{
		DataUnit* pTmpUnit = m_expireQueue.findMin();
		if (NULL == pTmpUnit)
		{
			return;
		}
		int tmpTime = (int)time((time_t*)NULL);
		if (pTmpUnit->m_iValue > tmpTime  )
		{
			break;
		}
		// --过期键堆弹出
		DataUnit removeUnit = m_expireQueue.deleteMin();

		// --数据库删除过期键
		RemoveKV(removeUnit.m_szKey);
	}

}



bool DataSpace::InsertKV( char* key, int keylen, char* val, int vallen, bool ops )
{
	void* pkey = JK_MALLOC( keylen+1 );
	void* pval = JK_MALLOC( vallen+1 );
	JK_MEMCPY_S(pkey,keylen+1,key,keylen+1);
	JK_MEMCPY_S(pval,vallen+1,val,vallen+1);
	if ( m_normalDict.GetElement(pkey) )
	{
		m_normalDict.UpdateElement(pkey, pval );
		if ( ops )
		{
			Operation	( LOG_CMD_STRING_UPD, pkey, pval );
			Replication	( LOG_CMD_STRING_UPD, key, pval );
		}
		return true;
	}

	if (  m_normalDict.AddElement( pkey, pval ) )
	{
		if ( ops )
		{
			Operation	( LOG_CMD_STRING_ADD, pkey, pval );
			Replication	( LOG_CMD_STRING_ADD, key, pval );
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
			Operation	( LOG_CMD_STRING_DEL, key, "" );
			Replication	( LOG_CMD_STRING_DEL, key, "" );
		}
		return true;
	}
	return false;
}


void DataSpace::Operation( int cmd, void* key, void* val )
{
	char* strTemp = (char*)JK_MALLOC(MAX_CMD_LEN);		// char[128]; 
	JK_SPRITF_S( strTemp, MAX_CMD_LEN, "%d %s %s \n", cmd, (char*)key, (char*)val );
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


void DataSpace::FetchKeys( char* fmt, int istart, int ilimit, char keysarray[1024][MAX_KEY_LEN], int& kescnt )
{
	m_normalDict.FetchKeys( fmt, istart, ilimit, keysarray, kescnt );
}



bool DataSpace::IsExists( char* key )
{
	return m_normalDict.IsExists( key );
}



bool DataSpace::ListPushLeft( char* key, int keylen, char* val, int vallen, bool ops /*= true */ )
{
	void* pkey = JK_MALLOC( keylen+1 );
	if ( !pkey )
	{
		DISPMSG_ERROR( "malloc memory failed!\n");
		return false;
	}
	void* pval = JK_MALLOC( vallen+1 );
	if ( !pval )
	{
		DISPMSG_ERROR( "malloc memory failed!\n");
		JK_FREE( pkey );
		return false;
	}
	JK_MEMCPY_S(pkey,keylen+1,key,keylen+1);
	JK_MEMCPY_S(pval,vallen+1,val,vallen+1);
	moly_list_type* pDList = (moly_list_type*)m_normalDict.GetElement( pkey );
	if ( pDList )
	{
		if ( NULL == pDList->LPush( (char*)pval ) )
		{
			JK_FREE( pkey );
			JK_FREE( pval );
			return false;
		}
		if ( ops )
		{
			Operation	( LOG_CMD_LIST_LPUSH, pkey, pval );
			Replication	( LOG_CMD_LIST_LPUSH, key, pval );
		}
		return true;
	}
	pDList = JK_NEW(moly_list_type);
	if ( !pDList )
	{
		DISPMSG_ERROR( "malloc memory failed!\n");
		JK_FREE( pkey );
		JK_FREE( pval );
		return false;
	}
	if( NULL == pDList->LPush( (char*)pval ) )
	{
		JK_FREE( pkey );
		JK_FREE( pval );
		JK_DELETE( moly_list_type, pDList );
		return false;
	}
	if ( !m_normalDict.AddElement(pkey, pDList ) )
	{
		JK_FREE( pkey );
		JK_FREE( pval );
		JK_DELETE( moly_list_type, pDList );
		return false;
	}
	if ( ops )
	{
		Operation	( LOG_CMD_LIST_LPUSH, pkey, pval );
		Replication	( LOG_CMD_LIST_LPUSH, key, pval );
	}
	return true;
}


bool DataSpace::ListPushRight( char* key, int keylen, char* val, int vallen, bool ops /*= true */ )
{
	void* pkey = JK_MALLOC( keylen+1 );
	if ( !pkey )
	{
		DISPMSG_ERROR( "malloc memory failed!\n");
		return false;
	}
	void* pval = JK_MALLOC( vallen+1 );
	if ( !pval )
	{
		DISPMSG_ERROR( "malloc memory failed!\n");
		JK_FREE( pkey );
		return false;
	}
	JK_MEMCPY_S(pkey,keylen+1,key,keylen+1);
	JK_MEMCPY_S(pval,vallen+1,val,vallen+1);
	moly_list_type* pDList = (moly_list_type*)m_normalDict.GetElement( pkey );
	if ( pDList )
	{
		if ( NULL == pDList->RPush( (char*)pval ) )
		{
			JK_FREE( pkey );
			JK_FREE( pval );
			return false;
		}
		if ( ops )
		{
			Operation	( LOG_CMD_LIST_RPUSH, pkey, pval );
			Replication	( LOG_CMD_LIST_RPUSH, key, pval );
		}
		return true;
	}
	pDList = JK_NEW(moly_list_type);
	if ( !pDList )
	{
		DISPMSG_ERROR( "malloc memory failed!\n");
		JK_FREE( pkey );
		JK_FREE( pval );
		return false;
	}
	if( NULL == pDList->RPush( (char*)pval ) )
	{
		JK_FREE( pkey );
		JK_FREE( pval );
		JK_DELETE( moly_list_type, pDList );
		return false;
	}
	if ( !m_normalDict.AddElement(pkey, pDList ) )
	{
		JK_FREE( pkey );
		JK_FREE( pval );
		JK_DELETE( moly_list_type, pDList );
		return false;
	}
	if ( ops )
	{
		Operation	( LOG_CMD_LIST_RPUSH, pkey, pval );
		Replication	( LOG_CMD_LIST_RPUSH, key, pval );
	}
	return true;
}


void* DataSpace::ListPopLeft( char* key, bool ops /*= true */ )
{
	moly_list_type* pDList = (moly_list_type*)m_normalDict.GetElement( key );
	if ( !pDList )
	{
		return NULL;
	}
	void* ptemp = pDList->LPop();
	if ( ptemp && ops )
	{
		Operation	( LOG_CMD_LIST_LPOP, key, "" );
		Replication	( LOG_CMD_LIST_LPOP, key, "" );
	}
	return ptemp;
}


void* DataSpace::ListPopRight( char* key, bool ops /*= true */ )
{
	moly_list_type* pDList = (moly_list_type*)m_normalDict.GetElement( key );
	if ( !pDList )
	{
		return NULL;
	}
	void* ptemp = pDList->RPop();
	if ( ptemp && ops )
	{
		Operation	( LOG_CMD_LIST_RPOP, key, "" );
		Replication	( LOG_CMD_LIST_RPOP, key, "" );
	}
	return ptemp;
}


int DataSpace::GetListLength( char* key )
{
	moly_list_type* pDList = (moly_list_type*)m_normalDict.GetElement( key );
	if ( !pDList )
	{
		return NULL;
	}
	return pDList->GetSize();
}



bool DataSpace::UpdateKV( void* key, void* val, int vallen, bool ops /*= true */)
{
	void* pval = JK_MALLOC( vallen+1 );
	if ( !pval )
	{
		DISPMSG_ERROR( "malloc memory failed!\n");
		return false;
	}
	JK_MEMCPY_S(pval,vallen+1,val,vallen+1);
	if( false == m_normalDict.UpdateElement( key, pval ) )
	{
		JK_FREE( pval );
		return false;
	}
	if (ops)
	{
		Operation(LOG_CMD_STRING_UPD, key, pval);
		Replication(LOG_CMD_STRING_UPD, key, pval);
	}
	return true;
}

bool DataSpace::ExpireKey(char * key, int seconds)
{
	// --验证是否有该key
	if (NULL == m_normalDict.GetElement(key)) 
	{
		return false;
	}
	int tmpTime = (int)time((time_t*)NULL);
	tmpTime += seconds;
	DataUnit expireUnit;
	strcpy_s(expireUnit.m_szKey, strlen(key) + 1, key);
	expireUnit.m_iValue = tmpTime;
	// ---TEST
	//expireUnit.m_iValue = seconds;
	m_expireQueue.insert(expireUnit);
	return true;
}


bool DataSpace::HashSet(char * key, int seconds)
{
	DataUnit dataUnit = m_expireQueue.deleteMin();
	if ( NULL == dataUnit.m_szKey ) 
	{
		int i = 9;
	}

	return true;
}
