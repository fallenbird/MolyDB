
#ifndef _JK_TIMER_CX0W2YH3_H_
#define _JK_TIMER_CX0W2YH3_H_

#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif


class Utility
{

public:
	long long static GetCurrentTimeTick()
	{
#ifdef WIN32
		return GetTickCount();
#else
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
#endif
	}
};






#endif