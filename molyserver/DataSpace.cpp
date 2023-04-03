
#include <time.h>
#include "DataSpace.h"
#include "AppendCmdQueue.h"
#include "JK_Utility.h"
#include "JK_MemMgr.h"
#include "JK_Console.h"
#include "JK_DList.h"
#include "JK_SkipList.h"
#include "SlaveMgr.h"
#include "NetMsg.h"


typedef JK_DList<char, true> moly_list_type;
typedef JK_Hashmap<char*, true> moly_hash_type;
typedef JK_SkipList<char, true>moly_zset_type;

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
	if ( m_normalDict.GetElement(pkey, evt_STR ) )
	{
		m_normalDict.UpdateElement(pkey, pval );
		if ( ops )
		{
			Operation	( LOG_CMD_STRING_UPD, pkey, pval );
			Replication	( LOG_CMD_STRING_UPD, key, pval );
		}
		return true;
	}

	if (  m_normalDict.AddElement( pkey, pval, evt_STR) )
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

void* DataSpace::GetValue(void* key, eValueType valtype)
{
	return m_normalDict.GetElement(key, valtype);
}


/// <summary>
/// TODO:if hash type, release hash node first
/// </summary>
/// <param name="key"></param>
/// <param name="ops"></param>
/// <returns></returns>
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

void DataSpace::Operation(int cmd, void* element, void* key, void* val)
{
	char* strTemp = (char*)JK_MALLOC(MAX_CMD_LEN);		// char[128]; 
	JK_SPRITF_S(strTemp, MAX_CMD_LEN, "%d %s %s %s \n", cmd, (char*)element, (char*)key, (char*)val);
	AppendCmdQueue::Enqueue(strTemp);
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

void DataSpace::Replication(int cmd, void* element, void* key, void* val)
{
	if (m_bSlave)
	{
		return;
	}
	MSG_M2S_APPENDCOMMAND_CMD cmdMsg;
	cmdMsg.m_byLen = JK_SPRITF_S(cmdMsg.m_strCmd, MAX_CMD_LEN, "%d %s %s %s \n", cmd, (char*)element, (char*)key, (char*)val);
	SlaveMgr::GetInstance().BroadAllSlave((BYTE*)&cmdMsg, cmdMsg.GetMsgSize());
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
	moly_list_type* pDList = (moly_list_type*)m_normalDict.GetElement( pkey, evt_LIST );
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
	if ( !m_normalDict.AddElement(pkey, pDList, evt_LIST) )
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
	moly_list_type* pDList = (moly_list_type*)m_normalDict.GetElement( pkey, evt_LIST );
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
		DISPMSG_ERROR( "malloc list memory failed!\n");
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
	if ( !m_normalDict.AddElement(pkey, pDList, evt_LIST) )
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
	moly_list_type* pDList = (moly_list_type*)m_normalDict.GetElement( key, evt_LIST );
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
	moly_list_type* pDList = (moly_list_type*)m_normalDict.GetElement( key , evt_LIST);
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
	moly_list_type* pDList = (moly_list_type*)m_normalDict.GetElement( key , evt_LIST);
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
	///TODO: any 
	if (NULL == m_normalDict.GetElement(key, evt_STR ))
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


bool DataSpace::HashSet(char * map,int keylen, char* key, int vallen, char* val, bool ops )
{
	int maplen = strlen(map);
	void* pmap = JK_MALLOC(maplen + 1);
	void* pkey = JK_MALLOC(keylen + 1);
	void* pval = JK_MALLOC(vallen + 1);

	moly_hash_type* pHash = (moly_hash_type*)m_normalDict.GetElement(map, evt_HASH );
	bool bIsNewHash = false;
	
	if (!pHash )
	{
		pHash = JK_NEW(moly_hash_type);
		if (!pHash)
		{
			JK_FREE(pmap);
			JK_FREE(pkey);
			JK_FREE(pval);
			DISPMSG_ERROR("malloc hash memory failed!\n");
			return false;
		}

		bIsNewHash = true;
		pHash->Init(DICT_INITIAL_SIZE);
	}

	JK_MEMCPY_S(pmap, maplen + 1, map, maplen + 1);
	JK_MEMCPY_S(pkey, keylen + 1, key, keylen + 1);
	JK_MEMCPY_S(pval, vallen + 1, val, vallen + 1);

	bool bRes = pHash->Add(pkey, pval, evt_HASH );
	if (!bRes) 
	{
		JK_FREE(pmap);
		JK_FREE(pkey);
		JK_FREE(pval);
		if (bIsNewHash)
		{
			JK_DELETE(moly_hash_type, pHash);
		}
		return false;
	}

	if (!m_normalDict.AddElement(pmap, pHash, evt_HASH))
	{
		JK_FREE(pmap);
		JK_FREE(pkey);
		JK_FREE(pval);
		if (bIsNewHash)
		{
			JK_DELETE(moly_hash_type, pHash);
		}
		return false;
	}
	if (ops)
	{
		Operation(LOG_CMD_HASH_SET, map, pkey, pval);
		Replication(LOG_CMD_HASH_SET, map, pkey, pval);
	}
	return true;
}



char* DataSpace::HashGet(char* map, char* key )
{
	moly_hash_type* pHash = (moly_hash_type*)m_normalDict.GetElement(map, evt_HASH );
	if (!pHash)
	{
		return NULL;
	}
	return (char*)pHash->Get(key, evt_HASH );
}



bool DataSpace::ZSetAdd(char* key, unsigned int score, unsigned int vallen, char* value, bool ops )
{
	void* pval = JK_MALLOC(vallen + 1);
	JK_MEMCPY_S(pval, vallen + 1, value, vallen + 1);
	
	moly_zset_type* pZSet = (moly_zset_type*)m_normalDict.GetElement(key, evt_ZSET);
	if (NULL != pZSet) 
	{
		return pZSet->InsertNode(score, value);
	}
	moly_zset_type* newZSet = JK_NEW(moly_zset_type);
	if ( !newZSet) 
	{
		JK_FREE(pval);
		return false;
	}
	bool result =  newZSet->InsertNode(score, value);
	if (!result) 
	{
		JK_FREE(pval);
		JK_DELETE(moly_zset_type, newZSet );
		return false;
	}

	if (!m_normalDict.AddElement(key, newZSet, evt_ZSET))
	{
		JK_FREE(pval);
		JK_DELETE(moly_zset_type, newZSet);
		return false;
	}
	return true;
		
}
