#include "JK_Hashmap.h"
#include "JK_Singleton.h"

class CTest
{
public :
	int m_iX;
	int m_iY;
};

class HashmapTest : public JK_Singleton<HashmapTest>
{
public:
	HashmapTest();

	~HashmapTest();

	void TestFunction();
	void TestDoubleGet();

private:
	JK_Hashmap<char*, true>  m_configMap;
};