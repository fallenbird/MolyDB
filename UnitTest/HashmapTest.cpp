#include "HashmapTest.h"
#include <assert.h>
#include <stdexcept>
//#include <gtest/gtest.h>

HashmapTest::HashmapTest(): m_configMap()
{

}

HashmapTest::~HashmapTest()
{

}




void HashmapTest::TestFunction()
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

	m_configMap.Add( "sxf", "888" );
	m_configMap.Remove("sxf");

	m_configMap.Add( "aaa", pTemp1 );
	m_configMap.Add( "bbb", pTemp2 );
	m_configMap.Add( "ccc", pTemp3 );
	m_configMap.Add( "ddd", pTemp4 );
	
	bool bhit = false;
	try
	{
		m_configMap.Add( "ccc", pTemp5 );
	}
	catch (std::exception e)
	{
		bhit = true;
	}
	assert(bhit);


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


// --avoid memmery bug
void HashmapTest::TestDoubleGet()
{
	char			strKey[32];
	char			strVal[32];

	for( int i=0; i<51; ++i )
	{

		sprintf_s( strKey, "key%d", i );
		sprintf_s( strVal, "val%d", i );
		m_configMap.Add( strKey, strVal );
	}
	char* pChar = (char*)m_configMap.Get( "key0" );
	pChar = (char*)m_configMap.Get( "key0" );
	assert(pChar);
}

