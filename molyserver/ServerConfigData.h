
#ifndef _JK_SERVERCONFIG_SXF13690_H_
#define _JK_SERVERCONFIG_SXF13690_H_

#include "JK_LockFreeQueue.h"
#include "ConfigManager.h"


class ServerConfigData 
{
public:
	ServerConfigData();

	static bool LoadConfigData( char* fileName );
	
	static int IsMaster(){ return m_bIsMaster; }
	static char* GetMasterIP(){ return m_MasterIP; }
	static unsigned short GetMasterPort(){ return m_sMasterPort; }
	static unsigned short GetLocalPort(){ return m_sLocalPort; }

private:
	
	static int m_bIsMaster;
	static char	m_MasterIP[16];
	static unsigned short m_sMasterPort;
	static unsigned short m_sLocalPort;
	static int m_iConsoleOutputLevel;
	static int m_iLogRecordLevel;

};

#endif