
#ifndef _JK_LOCK_97066A3D_H_
#define _JK_LOCK_97066A3D_H_


#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif


class JK_Lock
{
public:

	JK_Lock()
	{
#ifdef WIN32
		InitializeCriticalSection( &m_cs );
#else
		m_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
	}

	~JK_Lock()
	{

	}


	void Lock()
	{
#ifdef WIN32
		EnterCriticalSection( &m_cs );
#else
		pthread_mutex_lock(&m_mutex);
#endif
		
	};


	void Unlock()
	{
#ifdef WIN32
		LeaveCriticalSection( &m_cs );
#else
		pthread_mutex_unlock(&m_mutex)
#endif
	}

private:
#ifdef WIN32
	CRITICAL_SECTION m_cs;
#else
	pthread_mutex_t m_mutex;
#endif

};

#define lock_if_necessary() if( m_bThread ){m_lock.Lock();}else{}
#define unlock_if_necessary() if( m_bThread ){m_lock.Unlock();}else{}

#endif
