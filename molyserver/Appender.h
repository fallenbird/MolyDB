
#ifndef _JK_APPENDER_O791RES_H_
#define _JK_APPENDER_O791RES_H_

#include "JK_LockFreeQueue.h"
#include "CmdObject.h"
#include <vector>


using namespace std;

class Appender
{
public:
	Appender();
	~Appender();


	static unsigned int AppendThread();
	static int OpenAppendFile();
	static void CloseAppendFile();
	static int WriteAppendFile( void* pdata );

	int LoadAppendFile();


	static char* CatGenericCommand( char* dst, int argc, char** argv ) ;

private:
	static bool			m_bAppendOpen;
	static FILE*		m_fpAppendfile;
};

#endif