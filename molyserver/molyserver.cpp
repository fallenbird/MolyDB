#include "molyserver.h"
#include "Appender.h"
#include "ServerConfigData.h"

//#include <hash_map>

MolyServer::MolyServer()
{

}


MolyServer::~MolyServer()
{

}



int MolyServer::GetState()
{
	return m_runningSta;
}

bool MolyServer::InitServer( bool bSlave )
{
	m_serverIP = "127.0.0.1";;
	m_serverPort = 3690;

	// --init config
	//ConfigManager::GetInstance().OpenConfigFile( "config.ini" );
	ServerConfigData::LoadConfigData( ".\\config.ini");
	if ( bSlave )
	{
		ServerConfigData::SetSlave();
	}

	// --init database
	if( !DataSpace::GetInstance().InitDB( bSlave ) )
	{
		return false;
	}

	// --load append file and recover it
	Appender::LoadAppendFile();

	// --init append background thread
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Appender::AppendThread, NULL, 0, 0);

	// --init net interface
	NetInterface::GetInstance().initInterface(  
		ServerConfigData::IsMaster(),
		ServerConfigData::GetMasterIP(),
		ServerConfigData::GetMasterPort(), 
		ServerConfigData::GetLocalPort() );
	m_runningSta = ers_RUNNING;
	
	//hash_map<int, string> mymap;
	//mymap.insert(make_pair(8, "sxf"));
	//mymap.insert(make_pair(9, "xf"));
	//mymap.insert(make_pair(0, "f"));
	//hash_map<int, string>::iterator itx = mymap.begin();
	//if ( itx != mymap.end() )
	//{
	//	++itx;
	//}
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



