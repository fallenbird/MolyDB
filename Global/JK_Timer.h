
#ifndef _JK_TIMER_CX0W2YH3_H_
#define _JK_TIMER_CX0W2YH3_H_


#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif



#ifdef _
#define JK_ASSERT(expr) (assert(expr))
#else
#define JK_ASSERT(expr) ((void)0)
#endif

#endif
