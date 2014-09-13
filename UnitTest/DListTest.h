#include "JK_DList.h"
#include "JK_Assert.h"

class CPrint
{
public:
	void	operator()( char* str )
	{
		printf("%s\n", str );
	}
};


class DListTest
{
public:
	DListTest(){}
	~DListTest(){}

	void TestDList()
	{

		// --case 0;
		m_TestList.RPush( "xxx" );
		m_TestList.RPush( "yyy" );

		char* temp = m_TestList.LPop();
		JK_ASSERT( 0 == strcmp( temp, "xxx") );

		temp = m_TestList.LPop();
		JK_ASSERT( 0 == strcmp( temp, "yyy") );

		temp = m_TestList.LPop();
		JK_ASSERT( !temp );


		// --case 1:
		m_TestList.LPush("bbb");
		m_TestList.LPush("aaa");
		m_TestList.RPush("ccc");
		m_TestList.RPush("ddd");


		CPrint op;
		m_TestList.Foreach( op );

	}

private:
		JK_DList<char, true> m_TestList;
};