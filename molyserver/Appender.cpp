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
	JK_OPENFILE_S( fp,tmpfile,"at");
	if (!fp) 
	{
		MOLYLOG(MOLY_LOG_ERORR, "Failed opening dump file, errno: %d!", errno);
		return 1;
	}

	vector<CmdObject>::iterator iter; 
	for (iter=m_vecCmdBuff.begin();iter!=m_vecCmdBuff.end();iter++)  
	{
		fprintf( fp, "%d %s\n", (*iter).m_byCmdID, (*iter).m_strArgv );
	}
	
	return 0;
}


char* Appender::CatGenericCommand( char* dst, int argc, char** argv) 
{
	//char buf[32];
	//int len, j;
	//char* tempstr;

	//buf[0] = '*';
	//len = 1+ll2string(buf+1,sizeof(buf)-1,argc);
	//buf[len++] = '\r';
	//buf[len++] = '\n';
	//dst = sdscatlen(dst,buf,len);

	//for (j = 0; j < argc; j++)
	//{
	//	tempstr = argv[j];
	//	buf[0] = '$';
	//	len = 1+ll2string(buf+1,sizeof(buf)-1,sdslen(tempstr));
	//	buf[len++] = '\r';
	//	buf[len++] = '\n';
	//	dst = sdscatlen(dst,buf,len);
	//	dst = sdscatlen(dst,tempstr,sdslen(tempstr));
	//	dst = sdscatlen(dst,"\r\n",2);
	//}
	//return dst;
	return NULL;
}