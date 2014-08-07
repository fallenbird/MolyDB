#include "ConfigManager.h"



ConfigManager::ConfigManager()
{

}

ConfigManager::~ConfigManager()
{

}

void ConfigManager::LoadConfig( char* szConfigFile )
{
	CTest* pTemp1 = new CTest();
	pTemp1->m_iX = 11;
	pTemp1->m_iY = 12;


	CTest* pTemp2 = new CTest();
	pTemp2->m_iX = 21;
	pTemp2->m_iY = 22;

	CTest* pTemp3 = new CTest();
	pTemp3->m_iX = 31;
	pTemp3->m_iY = 32;

	CTest* pTemp4 = new CTest();
	pTemp4->m_iX = 41;
	pTemp4->m_iY = 42;

	CTest* pTemp5 = new CTest();
	pTemp5->m_iX = 51;
	pTemp5->m_iY = 52;

	m_configMap.Init( 64 );
	m_configMap.Add( "aaa", pTemp1 );
	m_configMap.Add( "bbb", pTemp2 );
	m_configMap.Add( "ccc", pTemp3 );
	m_configMap.Add( "ddd", pTemp4 );
	//m_configMap.Add( "ccc", pTemp5 );


	m_configMap.Add( "9qjfwo", pTemp1 );
	m_configMap.Add( "bqjfwf", pTemp2 );

	CTest* pMY = (CTest*)m_configMap.Get( "bbb" );
	int xxx = pMY->m_iX;
	int yyy = pMY->m_iY;

	CTest* pMY2 = (CTest*)m_configMap.Get( "opp" );
	if( pMY2 )
	{
	}


	CTest* pMY3 = (CTest*)m_configMap.Get( "bqjfwf" );
	int xxx3 = pMY3->m_iX;
	int yyy3 = pMY3->m_iY;

	m_configMap.Remove( "bqjfwf" );
	CTest* pMY4 = (CTest*)m_configMap.Get( "bqjfwf" );
	if( pMY4 )
	{
	}
}
