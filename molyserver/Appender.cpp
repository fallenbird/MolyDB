#include <stdio.h>
#include "Define.h"
#include "Appender.h"
#include "JK_Utility.h"
#include "JK_Assert.h"
#include "LogManager.h"
#include "JK_Console.h"
#include "AppendCmdQueue.h"
#include "DataSpace.h"
#include "ClientAgent.h"
#include "NetMsg.h"
#include "ServerConfigData.h"

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
	FILE* fptemp = OpenAppendFile( "r" );
	if( !fptemp )
	{
		return 1;
	}
	char linebuf[MAX_CMD_LEN];
	memset(linebuf,0, MAX_CMD_LEN);
	while( fgets(linebuf, MAX_CMD_LEN, fptemp ) ) 
	{
		HandleCmdLine( linebuf, false );
	}
	fclose( fptemp );
	DISPMSG_NOTICE( "Load from append file success!\n");
	return 0;
}



int Appender::ReplicateAppendFile( ClientAgent* pAgent )
{
	JK_ASSERT( pAgent );
	FILE* fptemp = OpenAppendFile( "r" );
	if( !fptemp )
	{
		return 1;
	}
	char linebuf[MAX_CMD_LEN];
	memset(linebuf,0, MAX_CMD_LEN);
	MSG_M2S_APPENDFILE_ACK ackmsg;
	ackmsg.m_iCmdCnt = 0;
	
	while( fgets(linebuf, MAX_CMD_LEN, fptemp ) ) 
	{
		JK_MEMCPY_S( ackmsg.m_CmdArray[ackmsg.m_iCmdCnt], MAX_CMD_LEN, linebuf, MAX_CMD_LEN );
		++ackmsg.m_iCmdCnt;
		if ( ackmsg.m_iCmdCnt > 10 )
		{
			pAgent->Send( (BYTE*)&ackmsg, ackmsg.GetMsgSize() );
			ackmsg.m_iCmdCnt = 0;
		}
	}
	pAgent->Send((BYTE*)&ackmsg, ackmsg.GetMsgSize() );
	fclose( fptemp );
	return 0;
}




void Appender::HandleCmdLine( char* strLine, bool bOps ) 
{
	char* cmdArray[MAX_PARA_CNT];
	JK_Utility::jk_str_split( cmdArray, strLine, " ");
	switch ( atoi(cmdArray[0]) )
	{
	case LOG_CMD_STRING_ADD:
		{
			DataSpace::GetInstance().InsertKV( cmdArray[1], strlen(cmdArray[1]), cmdArray[2], strlen(cmdArray[2]), bOps );
		}
		break;

	case LOG_CMD_STRING_DEL:
		{
			DataSpace::GetInstance().RemoveKV( cmdArray[1], bOps );
		}
		break;

	case LOG_CMD_STRING_UPD:
		{
			DataSpace::GetInstance().UpdateKV( cmdArray[1], cmdArray[2], strlen(cmdArray[2]), bOps);
		}
		break;


	case LOG_CMD_LIST_LPUSH:
		{
			DataSpace::GetInstance().ListPushLeft( cmdArray[1], strlen(cmdArray[1]), cmdArray[2], strlen(cmdArray[2]), bOps );
		}
		break;

	case LOG_CMD_LIST_RPUSH:
		{
			DataSpace::GetInstance().ListPushRight( cmdArray[1], strlen(cmdArray[1]), cmdArray[2], strlen(cmdArray[2]), bOps );
		}
		break;

	case LOG_CMD_LIST_LPOP:
		{
			DataSpace::GetInstance().ListPopLeft( cmdArray[1], bOps );
		}
		break;


	case LOG_CMD_LIST_RPOP:
		{
			DataSpace::GetInstance().ListPopRight( cmdArray[1], bOps );
		}
		break;


	case LOG_CMD_HASH_SET:
		{
			DataSpace::GetInstance().HashSet(cmdArray[1], strlen(cmdArray[2]), cmdArray[2], strlen(cmdArray[3]), cmdArray[3], bOps);
		}
		break;

	default:
		{
			DISPMSG_ERROR( "Error command : %s \n", cmdArray[0]);
		}
		break;
	}
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
			JK_FREE(pData);
			pData = NULL;
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


FILE* Appender::OpenAppendFile( char* mod )
{
	char tmpfile[MAX_FILENAME_LEN];
	FILE* fp = NULL;
	JK_SPRITF_S(tmpfile, MAX_FILENAME_LEN, "molydb_af_%d.dmf", ServerConfigData::IsMaster() );
	JK_OPENFILE_S( fp,tmpfile, mod);
	return fp;
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
		m_fpAppendfile = OpenAppendFile( "a" );
		if( !m_fpAppendfile  )
		{
			MOLYLOG(MOLY_LOG_ERORR, "Failed opening dump file, errno: %d!", errno);
			return 1;
		}
	}
	fprintf( m_fpAppendfile, "%s", (char*)pdata );
	return 0;
}

