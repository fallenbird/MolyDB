// Sundis.cpp : Defines the entry point for the console application.
//
#include "./Server.h"



enum eRunningSta
{
	ers_NONE = 0,
	ers_INIT = 1,
	ers_RUNNING = 2,
	ers_STOP = 3,
};


typedef struct 
{

	char*	ServerlIP;
	int		Serverport;
}ServerInfo;


_ServerInof g_SundisServer;


void InitServer()
{
	g_SundisServer._serverIP = "127.0.0.1";
	g_SundisServer._serverPort = 3690;
	g_SundisServer._runningSta = ers_NONE;
}

int main(int argc, char* argv[])
{
	// --init server
	InitServer();

	// --main loop
	while( 1 )
	{
		if( ers_RUNNING != g_SundisServer._runningSta)
		{
			break;
		}
	}
	return 0;
}