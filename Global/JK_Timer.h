
#ifndef _JK_TIMER_CX0W2YH3_H_
#define _JK_TIMER_CX0W2YH3_H_


#ifdef WIN32
#include <Windows.h>
long long GetCurrentTick()
{
	return GetTickCount();
}

#else

#include <sys/time.h>
long long GetCurrentTick()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}

#endif


#endif