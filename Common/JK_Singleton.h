#include "JK_Lock.h"
#include <assert.h>

template < typename T >
class JK_Singleton
{
public:
	JK_Singleton()
	{
		m_Lock.Lock();
		assert( !m_pInstance );
		//int offset = (int)(T*)1 - (int)(j_Singleton<T>*)(T*)1;
		//m_pSingleton = (T*)((int)this + offset );
		m_pInstance = (T*)this;
		m_Lock.Unlock();
	}

	~JK_Singleton()
	{
		assert( m_pInstance );
		m_pInstance = NULL;
	}


	static T& GetInstance()
	{
		assert( m_pInstance );
		return(*m_pInstance);
	}


	static T* GetInstancePtr()
	{
		assert( m_pInstance );
		return m_pInstance;
	}

private:
	static T* m_pInstance;
	JK_Lock m_Lock;
};

template< typename T > T* JK_Singleton<T>::m_pInstance = NULL;
