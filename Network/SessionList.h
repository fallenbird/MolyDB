#ifndef _SESSIONLIST_H_
#define _SESSIONLIST_H_

#include <windows.h>
#include <process.h>
#include <list>

class Session;

typedef std::list<Session*>			SESSION_LIST;
typedef SESSION_LIST::iterator		SESSION_LIST_ITER;

//=============================================================================================================================
/// Session多线程链表
//=============================================================================================================================
class SessionList : public std::list<Session*>
{
public:
	SessionList();
	~SessionList();

	VOID Clear();

	inline VOID		Lock()		{ EnterCriticalSection( &m_cs ); }
	inline VOID		Unlock()	{ LeaveCriticalSection( &m_cs ); }

private:
	CRITICAL_SECTION m_cs;
};

#endif
