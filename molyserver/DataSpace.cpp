#include "DataSpace.h"
#include "AppendCmdQueue.h"
#include "JK_Utility.h"


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



bool DataSpace::InsertKV(void* key, void* val)
{
	if (  m_normalDict.AddElement( key, val ) )
	{
		Operation( 101, key, val, "" );
		return true;
	}
	return false;
}

void* DataSpace::GetValue(void* key)
{
	return m_normalDict.GetElement(key);
}


bool DataSpace::RemoveKV(void* key)
{
	return m_normalDict.RemoveElement(key);
}


void DataSpace::Operation( int cmd, void* key, void* val, char* opt )
{
	char* strTemp = new char[128]; 
	JK_SPRITF_S( strTemp, 128, "%d %s %s %s\n", cmd, (char*)key, (char*)val, opt );
	AppendCmdQueue::Enqueue( strTemp );
}
