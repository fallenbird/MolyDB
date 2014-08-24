#include <stdio.h>
#include "Appender.h"
#include "JK_Utility.h"
#include "LogManager.h"

Appender::Appender()
{

}


Appender::~Appender()
{

}


int Appender::LoadAppendFile()
{
	char tmpfile[256];
	JK_SPRITF_S(tmpfile, 256, "molydb_af-%d.dmf", (int)JK_GETPID() );
	FILE *fp = NULL;
	JK_OPENFILE_S( fp,tmpfile,"w");
	if (!fp) 
	{
		MOLYLOG(MOLY_LOG_ERORR, "Failed opening dump file, errno: %d!", errno);
		return 1;
	}
	return 0;
}



int Appender::AppendToFile()
{
	char tmpfile[256];
	JK_SPRITF_S(tmpfile, 256, "molydb_af-%d.dmf", (int)JK_GETPID() );
	FILE *fp = NULL;
	JK_OPENFILE_S( fp,tmpfile,"w");
	if (!fp) 
	{
		MOLYLOG(MOLY_LOG_ERORR, "Failed opening dump file, errno: %d!", errno);
		return 1;
	}
	return 0;
}

