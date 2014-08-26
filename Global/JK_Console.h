#pragma once
#include "JK_Singleton.h"


#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

class JK_Console : public JK_Singleton<JK_Console>
{
public:
	enum TextColor
	{
		BLACK,						//RED,GREEN,BLUE
		BLUE = FOREGROUND_BLUE,
		LIME = FOREGROUND_GREEN,	//0,255,0
		RED = FOREGROUND_RED,
		AQUA = LIME | BLUE,			//0,255,255
		FUCHSIA = RED | BLUE,		//255,0,255
		YELLOW = RED | LIME,		//255,255,0
		WHITE = BLUE | LIME | RED,
		MAXCOLOR
	};


	JK_Console() :m_hConsole(INVALID_HANDLE_VALUE)
	{
	}

	virtual ~JK_Console()
	{
		if (m_hConsole)
			FreeConsole();
	}


	bool Create()
	{
		m_hConsole = ::GetStdHandle(STD_OUTPUT_HANDLE);
		return true;
	}


	bool WriteText(char* pText, TextColor rTextColor = WHITE)
	{
		if (m_hConsole == INVALID_HANDLE_VALUE)
			return false;

		WORD t_wOldTextAtt = 0;
		CONSOLE_SCREEN_BUFFER_INFO t_kSBuffInfo;
		if (::GetConsoleScreenBufferInfo(m_hConsole, &t_kSBuffInfo))
		{
			t_wOldTextAtt = t_kSBuffInfo.wAttributes;
		}

		if (!::SetConsoleTextAttribute(m_hConsole, (WORD)rTextColor))
			return false;

		DWORD t_dwNumCharWritten;
		if (!::WriteConsole(m_hConsole, pText, (DWORD)_tcslen(pText), &t_dwNumCharWritten, NULL))
		{
			::SetConsoleTextAttribute(m_hConsole, t_wOldTextAtt);
			return false;
		}

		if (!::SetConsoleTextAttribute(m_hConsole, t_wOldTextAtt))
			return false;

		return true;
	}

private:
	HANDLE m_hConsole;
};