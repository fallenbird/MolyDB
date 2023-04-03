
#ifndef _JK_HASHMAP_JXD7CH5_H_
#define _JK_HASHMAP_JXD7CH5_H_


#include "Define.h"

#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif


enum enGmCommandType
{
	ect_COMMAND_NONE	= 0,
	ect_COMMAND_HELP	= 1,

	//--KEY类
	ect_COMMAND_SET		= 2,
	ect_COMMAND_DEL		= 3,
	ect_COMMAND_GET		= 4,
	ect_COMMAND_KEYS	= 5,
	ect_COMMAND_EXIST	= 6,
	ect_COMMAND_EXPIRE	= 7,

	//--LIST类
	ect_COMMAND_LPUSH	= 11,
	ect_COMMAND_RPUSH	= 12,
	ect_COMMAND_LPOP	= 13,
	ect_COMMAND_RPOP	= 14,
	ect_COMMAND_LLEN	= 15,

	//--HASH类
	ect_COMMAND_HSET	= 21,
	ect_COMMAND_HGET	= 22,

	//--ZSET类
	ect_COMMAND_ZADD	= 31,
	ect_COMMAND_ZREVRANK= 32,
	ect_COMMAND_ZRANGE	= 33,

	//--SERVER类
	ect_COMMAND_QUIT	= 95,
};


class CommandParser
{
public:
	CommandParser(){}
	~CommandParser(){}

	void static ParseCommandStr(char* cmdstr, unsigned int& cmdType, char argv[MAX_PARA_CNT][MAX_CMD_LEN], unsigned int& argc)
	{
		char seps[] = " ,;:\t";
		char* next_token;

		//得到命令种类
		char* str = strtok_s(cmdstr, seps, &next_token);
		if (!str)
		{
			return;
		}
		char command[MAX_CMD_LEN];
		strcpy_s(command, str);
		_strlwr_s(command);
		cmdType = GetCmdType(command);
		if (0 == cmdType )
		{
			return;
		}
		//解析剩余参数
		int i = 0;
		str = strtok_s(NULL, seps, &next_token);
		while (str)
		{
			strcpy_s(argv[i], MAX_CMD_LEN, str);
			i++;
			str = strtok_s(NULL, seps, &next_token);
		}
		argc = i;
	}

	unsigned int static GetCmdType(char* cmdstr)
	{
		if (NULL == cmdstr)
		{
			return ect_COMMAND_NONE;
		}
		if (!strcmp(cmdstr, "help"))
		{
			return ect_COMMAND_HELP;
		}
		else if (!strcmp(cmdstr, "get"))
		{
			return ect_COMMAND_GET;
		}
		else if (!strcmp(cmdstr, "set"))
		{
			return ect_COMMAND_SET;
		}
		else if (!strcmp(cmdstr, "del"))
		{
			return ect_COMMAND_DEL;
		}
		else if (!strcmp(cmdstr, "keys"))
		{
			return ect_COMMAND_KEYS;
		}
		else if (!strcmp(cmdstr, "exists"))
		{
			return ect_COMMAND_EXIST;
		}
		else if (!strcmp(cmdstr, "lpush"))
		{
			return ect_COMMAND_LPUSH;
		}
		else if (!strcmp(cmdstr, "rpush"))
		{
			return ect_COMMAND_RPUSH;
		}
		else if (!strcmp(cmdstr, "lpop"))
		{
			return ect_COMMAND_LPOP;
		}
		else if (!strcmp(cmdstr, "rpop"))
		{
			return ect_COMMAND_RPOP;
		}
		else if (!strcmp(cmdstr, "llen"))
		{
			return ect_COMMAND_LLEN;
		}
		else if (!strcmp(cmdstr, "quit"))
		{
			return ect_COMMAND_QUIT;
		}
		else if (!strcmp(cmdstr, "expire")) {
			return ect_COMMAND_EXPIRE;
		}


		else if (!strcmp(cmdstr, "hset"))
		{
			return ect_COMMAND_HSET;
		}
		else if (!strcmp(cmdstr, "hget"))
		{
			return ect_COMMAND_HGET;
		}

		else if (!strcmp(cmdstr, "zadd"))
		{
			return ect_COMMAND_ZADD;
		}
		else if (!strcmp(cmdstr, "zrevrank"))
		{
			return ect_COMMAND_ZREVRANK;
		}
		else if (!strcmp(cmdstr, "zrange"))
		{
			return ect_COMMAND_ZRANGE;
		}

		else 
		{
			return ect_COMMAND_NONE;
		}
	}

};



#endif
