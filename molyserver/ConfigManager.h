#include "JK_Singleton.h"



class ConfigManager : public JK_Singleton<ConfigManager>
{
public:
	ConfigManager();
	~ConfigManager();

	void LoadConfig( char* szConfigFile );


private:

};