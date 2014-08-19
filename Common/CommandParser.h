
#ifndef _JK_HASHMAP_JXD7CH5_H_
#define _JK_HASHMAP_JXD7CH5_H_


#define MAX_CMD_LEN 256
#define MAX_PARA_CNT 10

#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif


enum enGmCommandType
{
	ect_COMMAND_NONE	= 0,
	ect_COMMAND_HELP	= 1,
	ect_COMMAND_SET		= 2,
	ect_COMMAND_GET		= 3,
	ect_COMMAND_QUIT	= 4,
};


class CommandParser
{
public:
	CommandParser(){}
	~CommandParser(){}

	void static ParseCommandStr(char* cmdstr, unsigned int& cmdType, char argv[MAX_PARA_CNT][MAX_CMD_LEN], unsigned int& argc)
	{
		//char buffer[MAX_CMD_LEN];
		//strcpy_s(buffer, MAX_CMD_LEN, pMsg);
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
			//if (cmdType == TYPE_NOTIFY && i == 3 && strcmp(next_token, ""))
			//{
			//	strcat_s(argv[3], next_token);
			//	argc = 4;
			//	return;
			//}
			//if (i == MAX_PARAMETERS_COUNT)
			//{
			//	argc = i;
			//	return;
			//}
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
		else if (!strcmp(cmdstr, "quit"))
		{
			return ect_COMMAND_QUIT;
		}
		else 
		{
			return ect_COMMAND_NONE;
		}
	}

};



#endif
