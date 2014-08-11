#include "molyserver.h"


int MolyServer::GetState()
{
	return m_runningSta;
}

void MolyServer::InitServer()
{
	m_serverIP = "127.0.0.1";;
	m_serverPort = 3690;

	// --init config
	ConfigManager::GetInstance().OpenConfigFile( "config.ini" );

	// --init net interface
	NetInterface::GetInstance().initInterface( "127.0.0.1", 3690 );
	m_runningSta = ers_RUNNING;
}



void MolyServer::RunServer()
{
	// --run net interface
	NetInterface::GetInstance().RunInterface();

}


void MolyServer::StopServer()
{
	m_runningSta = ers_STOP;
}


 