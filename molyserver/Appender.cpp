#include <stdio.h>
#include "Appender.h"
#include "JK_Utility.h"
#include "JK_Assert.h"
#include "LogManager.h"
#include "AppendCmdQueue.h"



bool	Appender::m_bAppendOpen = true;
FILE*	Appender::m_fpAppendfile = NULL;

Appender::Appender()
{
}


Appender::~Appender()
{

}




int Appender::LoadAppendFile()
{
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



unsigned int Appender::AppendThread()
{
	void* pData = NULL;
	int iRes = 0;
	while ( m_bAppendOpen )
	{
		while ( pData = AppendCmdQueue::Dequeue() )
		{
			iRes = WriteAppendFile( pData );
			if( 0 != iRes )
			{
				MOLYLOG(MOLY_LOG_ERORR, "Failed to write append file, errno: %d!", errno);
				return iRes;
			}
			else
			{
				printf( "³Ö¾Ã»¯:%s\n",  (char*)pData  );
			}
		}
		Sleep(500);
	}
	return 0;
}


int Appender::OpenAppendFile()
{
	JK_ASSERT( NULL == m_fpAppendfile );
	char tmpfile[256];
	JK_SPRITF_S(tmpfile, 256, "molydb_af-%d.dmf", (int)JK_GETPID() );
	JK_OPENFILE_S( m_fpAppendfile,tmpfile,"w");
	if (!m_fpAppendfile ) 
	{
		MOLYLOG(MOLY_LOG_ERORR, "Failed opening dump file, errno: %d!", errno);
		return 1;
	}
	return 0;
}


void Appender::CloseAppendFile()
{
	m_bAppendOpen = false;
	fclose( m_fpAppendfile );
	m_fpAppendfile = NULL;
}



int Appender::WriteAppendFile( void* pdata )
{
	if ( !m_fpAppendfile )
	{
		int res = OpenAppendFile();
		if(  0!= res )
		{
			return res;
		}
	}
	fprintf( m_fpAppendfile, "%s", pdata );
	return 0;
}

