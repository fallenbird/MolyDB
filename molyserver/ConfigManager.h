#include "JK_Singleton.h"
#include "JK_Hashmap.h"

class CTest
{
public :
	int m_iX;
	int m_iY;
};


class ConfigManager : public JK_Singleton<ConfigManager>
{
public:
	ConfigManager();
	~ConfigManager();

	void LoadConfig( char* szConfigFile );


private:
	JK_Hashmap  m_configMap;


};