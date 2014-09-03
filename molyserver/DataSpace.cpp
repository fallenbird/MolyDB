#include "DataSpace.h"
#include "AppendCmdQueue.h"
#include "JK_Utility.h"
#include "JK_MemMgr.h"

DataSpace::DataSpace()
{

}

DataSpace::~DataSpace()
{

}

bool	DataSpace::InitDB()
{
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
			Operation( 101, pkey, pval, "" );
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
			Operation( 102, key, "", "" );
		}
		return true;
	}
	return false;
}


void DataSpace::Operation( int cmd, void* key, void* val, char* opt )
{
	char* strTemp = (char*)JK_MALLOC(128);		// char[128]; 
	JK_SPRITF_S( strTemp, 128, "%d %s %s %s\n", cmd, (char*)key, (char*)val, opt );
	AppendCmdQueue::Enqueue( strTemp );
}
