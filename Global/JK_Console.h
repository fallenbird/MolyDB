
#ifndef _JK_CONSOLE_D0W9V3F_H_
#define _JK_CONSOLE_D0W9V3F_H_
#include "JK_Singleton.h"
#include <stdio.h>

#ifdef WIN32
#include <Windows.h>
#else
#endif


enum TextColor
{
	BLACK,						//RED,GREEN,BLUE
	BLUE = FOREGROUND_BLUE,
	GREEN = FOREGROUND_GREEN,	//0,255,0
	RED = FOREGROUND_RED,
	AQUA = GREEN | BLUE,			//0,255,255
	FUCHSIA = RED | BLUE,		//255,0,255
	YELLOW = RED | GREEN,		//255,255,0
	WHITE = BLUE | GREEN | RED,
	MAXCOLOR
};



class JK_Console : public JK_Singleton<JK_Console>
{
public:

	JK_Console() :m_hConsole(INVALID_HANDLE_VALUE)
	{
	}

	virtual ~JK_Console()
	{
		if (m_hConsole)
		{
			FreeConsole();
		}
	}


	bool Create()
	{
		m_hConsole = ::GetStdHandle(STD_OUTPUT_HANDLE);
		return true;
	}


	bool WriteText(TextColor rTextColor, char* fmt, ... )
	{
		if (m_hConsole == INVALID_HANDLE_VALUE)
		{
			Create();
		}
		if (m_hConsole == INVALID_HANDLE_VALUE)
		{
			printf( "[ERR] Console start failed!\n" );
			return false;
		}

		WORD t_wOldTextAtt = 0;
		CONSOLE_SCREEN_BUFFER_INFO t_kSBuffInfo;
		if (::GetConsoleScreenBufferInfo(m_hConsole, &t_kSBuffInfo))
		{
			t_wOldTextAtt = t_kSBuffInfo.wAttributes;
		}

		if (!::SetConsoleTextAttribute(m_hConsole, (WORD)rTextColor))
		{
			return false;
		}
		char sprint_buf[256];
		va_list args;
		va_start(args, fmt);
		int iLen = vsprintf_s(sprint_buf, 256, fmt, args);
		va_end(args);

		DWORD t_dwNumCharWritten;


		if (!::WriteConsoleA(m_hConsole, sprint_buf, iLen, &t_dwNumCharWritten, NULL))
		{
			::SetConsoleTextAttribute(m_hConsole, t_wOldTextAtt);
			return false;
		}

		if (!::SetConsoleTextAttribute(m_hConsole, t_wOldTextAtt))
		{
			return false;
		}
		return true;
	}

private:
	HANDLE m_hConsole;
	
};

#ifdef _DEBUG 
#define DISPMSG_DEBUG(fmt, ...)		JK_Console::GetInstance().WriteText(FUCHSIA,fmt,__VA_ARGS__)
#else 
#define DISPMSG_DEBUG(fmt, ...)		((void*)0)
#endif

#define DISPMSG_SUCCESS(fmt, ...)	JK_Console::GetInstance().WriteText(GREEN,fmt,__VA_ARGS__)	
#define DISPMSG_NOTICE(fmt, ...)	printf(fmt,__VA_ARGS__)
#define DISPMSG_WARNNING(fmt, ...)	JK_Console::GetInstance().WriteText(YELLOW,fmt,__VA_ARGS__)
#define DISPMSG_ERROR(fmt, ...)		JK_Console::GetInstance().WriteText(RED,fmt,__VA_ARGS__)


#endif