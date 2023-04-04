#include "molyserver.h"



int main(int argc, char* argv[])
{
	// --init server
	MolyServer molySvr;
	bool bSlave = false;
	if ( 2 == argc && ( 0 == strcmp( "slave",argv[1] ) )  )
	{
		bSlave = true;
	}
	if (!molySvr.InitServer( bSlave ) )
	{
		printf( "initialize server failed!\n");
		Sleep(2000);
		return 1;
	}


	// --main loop
	while( true )
	{
		if( ers_RUNNING != molySvr.GetState() )
		{
			break;
		}
		molySvr.RunServer();
	}
	return 0;
}