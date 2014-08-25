
#ifndef _JK_APPENDER_O791RES_H_
#define _JK_APPENDER_O791RES_H_

#include "CmdObject.h"
#include <vector>

using namespace std;

class Appender
{
public:
	Appender();
	~Appender();

	int LoadAppendFile();
	int AppendToFile();

	static char* CatGenericCommand( char* dst, int argc, char** argv ) ;

private:
	vector<CmdObject>  m_vecCmdBuff;
};

#endif