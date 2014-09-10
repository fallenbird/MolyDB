#include "molyserver.h"
#include "Appender.h"
#include "ServerConfigData.h"
//#include <crtdbg.h>  


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
	//_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);  
	m_serverIP = "127.0.0.1";;
	m_serverPort = 3690;

	// --init config
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
	

	struct string_less : public binary_function<const string, const string, bool>  
	{   
	public:   
		result_type operator()(const first_argument_type& _Left, const second_argument_type& _Right) const   
		{   
			return(_Left.compare(_Right) < 0 ? true : false);   
		}   
	};  

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



