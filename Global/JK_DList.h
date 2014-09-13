

#ifndef _JK_DLIST_ND8YEDE_H_
#define _JK_DLIST_ND8YEDE_H_

#include "JK_Lock.h"


class JK_DListNode
{
public:
	JK_DListNode():m_pNext(NULL),m_pPrev(NULL),m_pData(NULL)
	{
	}
	void*				m_pData;
	JK_DListNode*		m_pNext;
	JK_DListNode*		m_pPrev;
};


template< typename T, bool m_bThread >
class JK_DList
{

	template< typename T, bool m_bThread > friend  class JK_DList_iterator;

public:
	typedef JK_DList_iterator<T,m_bThread> iterator;

	JK_DList():m_pHead(NULL),m_pTail( NULL ),m_uiCount(0)
	{

	}

	~JK_DList()
	{
		Destroy();
	}

	T*	RPush( T* pElement )
	{
		JK_ASSERT( pElement );
		if ( 0 == m_uiCount )
		{
			lock_if_necessary();
			m_pHead = (JK_DListNode*)JK_NEW(JK_DListNode);
			if( !m_pHead )
			{
				unlock_if_necessary();
				return NULL;
			}
			m_pHead->m_pData = pElement;
			m_pTail = m_pHead;
			++m_uiCount;
			unlock_if_necessary();
			return (char*)(m_pHead->m_pData);
		}
		lock_if_necessary();
		m_pTail->m_pNext = (JK_DListNode*)JK_NEW(JK_DListNode);
		if ( !(m_pTail->m_pNext) )
		{
			unlock_if_necessary();
			return NULL;
		}
		m_pTail->m_pNext->m_pPrev = m_pTail;
		m_pTail = m_pTail->m_pNext;
		m_pTail->m_pData = pElement;
		++m_uiCount;
		unlock_if_necessary();
		return (char*)(m_pTail->m_pData);
	}


	T*	LPop()
	{
		lock_if_necessary();
		if ( 0 == m_uiCount )
		{
			unlock_if_necessary();
			return NULL;
		}
		JK_DListNode* temp = m_pHead;
		void* tempData = m_pHead->m_pData;
		m_pHead = m_pHead->m_pNext;
		JK_FREE( temp );
		--m_uiCount;
		unlock_if_necessary();
		return (char*)tempData;
	}




	T*	LPush( T* pElement )
	{
		JK_ASSERT( pElement );
		if ( 0 == m_uiCount )
		{
			lock_if_necessary();
			m_pHead = (JK_DListNode*)JK_MALLOC(sizeof(JK_DListNode));
			if( !m_pHead )
			{
				unlock_if_necessary();
				return NULL;
			}
			m_pHead->m_pData = pElement;
			m_pTail = m_pHead;
			++m_uiCount;
			unlock_if_necessary();
			return (char*)(m_pHead->m_pData);
		}
		lock_if_necessary();
		m_pHead->m_pPrev = (JK_DListNode*)JK_MALLOC(sizeof(JK_DListNode));
		if ( !(m_pHead->m_pPrev) )
		{
			unlock_if_necessary();
			return NULL;
		}
		m_pHead->m_pPrev->m_pNext = m_pHead;
		m_pHead = m_pHead->m_pPrev;
		m_pHead->m_pData = pElement;
		++m_uiCount;
		unlock_if_necessary();
		return (char*)(m_pHead->m_pData);
	}


	T*	RPop()
	{
		lock_if_necessary();
		if ( 0 == m_uiCount )
		{
			unlock_if_necessary();
			return NULL;
		}
		JK_DListNode* temp = m_pTail;
		void* tempData = m_pTail->m_pData;
		m_pTail = m_pTail->m_pPrev;
		JK_FREE( temp );
		--m_uiCount;
		unlock_if_necessary();
		return (char*)tempData;
	}


	unsigned int GetSize()
	{
		return m_uiCount;
	}


	void	Destroy()
	{
		JK_DListNode* pTemp; 
		lock_if_necessary();
		while ( m_pHead ) 
		{
			pTemp = m_pHead->m_pNext;
			JK_FREE( m_pHead);
			m_pHead = pTemp;
		}
		m_uiCount = 0;
		unlock_if_necessary();
	}

	template<typename FUNC  >
	void Foreach( FUNC Op )
	{
		iterator itr = Begin();
		for ( ; itr != End(); ++itr )
		{
			Op(*itr);
		}
	}


private:
	iterator Begin()
	{
		return iterator( m_pHead );
	}

	iterator End()
	{
		return iterator( NULL );
	}

private:
	JK_DListNode*	m_pHead;
	JK_DListNode*	m_pTail;
	JK_Lock			m_lock;
	unsigned int	m_uiCount;

};



template< typename T, bool m_bThread >
class JK_DList_iterator
{ 
public:

	JK_DList_iterator( JK_DListNode* pHead )
	{
		m_pCurrNode = pHead;
	}

	T*	operator*() const  
	{  
		return (char*)(m_pCurrNode->m_pData);
	}


	JK_DListNode* operator->() const  
	{  
		return m_pCurrNode;  
	}


	JK_DList_iterator<T, m_bThread>& operator++()
	{
		m_pCurrNode = m_pCurrNode->m_pNext;
		return *this;
	}


	JK_DList_iterator<T, m_bThread>& operator++(int)
	{
		JK_SList_iterator<T, m_bThread> tmp = *this;
		++*this;
		return tmp;
	}


	bool operator==( const JK_DList_iterator<T, m_bThread>& it)
	{
		return m_pCurrNode == it.m_pCurrNode;
	}


	bool operator!=( const JK_DList_iterator<T, m_bThread>& it)
	{
		return m_pCurrNode != it.m_pCurrNode;
	}

private:  

	JK_DListNode* m_pCurrNode;

};



#endif