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

private:
	JK_Hashmap  m_configMap;
};