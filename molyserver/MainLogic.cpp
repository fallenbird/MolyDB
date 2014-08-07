#include "molyserver.h"




int main(int argc, char* argv[])
{
	// --init server
	MolyServer molySvr;
	molySvr.InitServer();


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