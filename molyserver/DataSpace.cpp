#include "DataSpace.h"


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
	return m_normalDict.AddElement( key, val );
}

void* DataSpace::GetValue(void* key)
{
	return m_normalDict.GetElement(key);
}


bool DataSpace::RemoveKV(void* key)
{
	return m_normalDict.RemoveElement(key);
}