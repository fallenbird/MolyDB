
#ifndef _JK_ASSERT_PG06M53H_H_
#define _JK_ASSERT_PG06M53H_H_

#include <assert.h>

#ifdef _DEBUG
#define JK_ASSERT(expr) (assert(expr))
#else
#define JK_ASSERT(expr) ((void)0)
#endif

#endif
