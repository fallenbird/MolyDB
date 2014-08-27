#include "molyserver.h"
#include "Appender.h"


int MolyServer::GetState()
{
	return m_runningSta;
}

bool MolyServer::InitServer()
{
	m_serverIP = "127.0.0.1";;
	m_serverPort = 3690;

	// --init config
	ConfigManager::GetInstance().OpenConfigFile( "config.ini" );

	// --init database
	if( !DataSpace::GetInstance().InitDB() )
	{
		return false;
	}

	// --init append background thread
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Appender::AppendThread, NULL, 0, 0);

	// --init net interface
	NetInterface::GetInstance().initInterface( "127.0.0.1", 3690 );
	m_runningSta = ers_RUNNING;

	return true;
}



void MolyServer::RunServer()
{
	// --run net interface
	NetInterface::GetInstance().RunInterface();
	DataSpace::GetInstance().UpdateDB(100);
}


void MolyServer::StopServer()
{
	m_runningSta = ers_STOP;
}


 