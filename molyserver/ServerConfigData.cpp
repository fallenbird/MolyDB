#include "ServerConfigData.h"
#include "JK_Console.h"
#include "JK_Utility.h"

#define DEFAULT_CONSOLE_OUTPUT_LEVEL 3
#define DEFAULT_LOG_RECORD_LEVEL 3

#define DEFAULT_MASTER_IP "127.0.0.1"
#define DEFAULT_MASTER_PORT 3690
#define DEFAULT_LOCAL_PORT (DEFAULT_MASTER_PORT+1)

ServerConfigData::ServerConfigData()
{
}




bool ServerConfigData::LoadConfigData( char* fileName )
{
	// consle out put level
	m_iConsoleOutputLevel  = GetPrivateProfileIntA("log", "ConsoleOutputLevel", -1, fileName);
	if( -1 == m_iConsoleOutputLevel)
	{
		m_iConsoleOutputLevel = DEFAULT_CONSOLE_OUTPUT_LEVEL;
		DISPMSG_WARNNING("Init config failed, use default console output level:[%d]\n", m_iConsoleOutputLevel );
	}

	// log record level
	m_iLogRecordLevel  = GetPrivateProfileIntA("log", "LogRecordLevel ", -1, fileName);
	if( -1 == m_iLogRecordLevel)
	{
		m_iLogRecordLevel = DEFAULT_LOG_RECORD_LEVEL;
		DISPMSG_WARNNING("Init config failed, use default log record level:[%d]\n", m_iLogRecordLevel );
	}

	// master or slave
	m_bIsMaster = GetPrivateProfileIntA( "server","IsMaster",-1, fileName );
	if( -1 == m_bIsMaster)
	{
		m_bIsMaster = 1;
		DISPMSG_WARNNING("Init config failed, default as master\n" );
	}

	// master ip
	GetPrivateProfileStringA( "server","MasterIP",0, (LPSTR)m_MasterIP, 32, fileName );
	if ( "" == m_MasterIP )
	{
		JK_SPRITF_S( m_MasterIP, 16, DEFAULT_MASTER_IP );
		DISPMSG_WARNNING("Init config failed, use default master ip:[%s]\n", DEFAULT_MASTER_IP );
	}

	// master port
	m_sMasterPort = (unsigned short)GetPrivateProfileIntA( "server","MasterPort",0, fileName );
	if ( 0 == m_sMasterPort )
	{
		m_sMasterPort = DEFAULT_MASTER_PORT;
		DISPMSG_WARNNING("Init config failed, use default master port:[%d]\n", m_sMasterPort );
	}

	// local port
	m_sLocalPort = (unsigned short)GetPrivateProfileIntA( "server","LocalPort",0, fileName );
	if ( 0 == m_sLocalPort )
	{
		m_sLocalPort = DEFAULT_LOCAL_PORT;
		DISPMSG_WARNNING("Init config failed, use default local port:[%d]\n", m_sLocalPort );
	}


	return true;
}


int				ServerConfigData::m_iLogRecordLevel;
int				ServerConfigData::m_iConsoleOutputLevel;
int				ServerConfigData::m_bIsMaster;
char			ServerConfigData::m_MasterIP[16];
unsigned short	ServerConfigData::m_sMasterPort;
unsigned short	ServerConfigData::m_sLocalPort;
