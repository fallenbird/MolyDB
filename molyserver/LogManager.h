
#ifndef _JK_LOGMANAGER_KL05XKR_H_
#define _JK_LOGMANAGER_KL05XKR_H_
#include "JK_Singleton.h"
#include "Define.h"

#define MAX_LOGMSG_LEN 256


class LogManager : public JK_Singleton<LogManager>
{
public:
	LogManager();
	~LogManager();

	void WriteLog( int itype, char* fmt, ...);
private:
};


#define MOLYLOG(type,fmt,...) LogManager::GetInstance().WriteLog(type,fmt,__VA_ARGS__)

#endif