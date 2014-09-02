#include <stdio.h>
#include "Appender.h"
#include "JK_Utility.h"
#include "JK_Assert.h"
#include "LogManager.h"
#include "JK_Console.h"
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
	int res = OpenAppendFile( "r" );
	if(  0!= res )
	{
		return res;
	}
	char linebuf[128];
	while( fgets(linebuf, 128, m_fpAppendfile ) ) 
	{
		HandleCmdLine( linebuf );
	}
	return 0;
}



void Appender::HandleCmdLine( char* strLine ) 
{
	char* cmdArray[16];
	JK_Utility::jk_str_split( cmdArray, strLine, " ");
	return ;
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
				DISPMSG_DEBUG( "³Ö¾Ã»¯:%s",  (char*)pData );
			}
		}
		if (  m_fpAppendfile )
		{
			fclose( m_fpAppendfile );
			m_fpAppendfile = NULL;
		}
		Sleep(500);
	}
	return 0;
}


int Appender::OpenAppendFile( char* mod )
{
	JK_ASSERT( NULL == m_fpAppendfile );
	char tmpfile[256];
	JK_SPRITF_S(tmpfile, 256, "molydb_af.dmf", (int)JK_GETPID() );
	JK_OPENFILE_S( m_fpAppendfile,tmpfile, mod);
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
		int res = OpenAppendFile( "a" );
		if(  0!= res )
		{
			return res;
		}
	}
	fprintf( m_fpAppendfile, "%s", pdata );
	return 0;
}

