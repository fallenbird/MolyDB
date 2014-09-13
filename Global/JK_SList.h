

#ifndef _JK_LIST_WCNMLGB_H_
#define _JK_LIST_WCNMLGB_H_

#include "JK_Lock.h"


class JK_SListNode
{
public:
	JK_SListNode():pNext( NULL ),m_pData(NULL)
	{
	}
	void*				m_pData;
	JK_SListNode*		pNext;
};


template< typename T, bool m_bThread >
class JK_SList
{

	template< typename T, bool m_bThread > friend  class JK_SList_iterator;

public:
	typedef JK_SList_iterator<T,m_bThread> iterator;

	JK_SList():m_pHead(NULL),m_pTail( NULL ),m_uiCount(0)
	{

	}

	~JK_SList()
	{
		Destroy();
	}

	T*	Push( T* pElement )
	{
		JK_ASSERT( pElement );
		if ( 0 == m_uiCount )
		{
			lock_if_necessary();
			m_pHead = (JK_SListNode*)JK_MALLOC(sizeof(JK_SListNode));
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
		m_pTail->pNext = (JK_SListNode*)JK_MALLOC(sizeof(JK_SListNode));
		if ( !(m_pTail->pNext) )
		{
			unlock_if_necessary();
			return NULL;
		}
		m_pTail = m_pTail->pNext;
		m_pTail->m_pData = pElement;
		++m_uiCount;
		unlock_if_necessary();
		return (char*)(m_pTail->m_pData);
	}


	T*	Pop()
	{
		lock_if_necessary();
		if ( 0 == m_uiCount )
		{
			unlock_if_necessary();
			return NULL;
		}
		JK_SListNode* temp = m_pHead;
		void* tempData = m_pHead->m_pData;
		m_pHead = m_pHead->pNext;
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
		JK_SListNode* pTemp; 
		lock_if_necessary();
		while ( m_pHead ) 
		{
			pTemp = m_pHead->pNext;
			JK_FREE( m_pHead);
			m_pHead = pTemp;
		}
		m_uiCount = 0;
		unlock_if_necessary();
	}


private:
	iterator Begin()
	{
		return iterator( m_pHead );
	}

	iterator End()
	{
		return iterator( m_pTail );
	}

private:
	JK_SListNode*	m_pHead;
	JK_SListNode*	m_pTail;
	JK_Lock			m_lock;
	unsigned int	m_uiCount;

};



template< typename T, bool m_bThread >
class JK_SList_iterator
{ 
public:

	JK_SList_iterator( JK_SListNode* pHead )
	{
		m_pCurrNode = pHead;
	}

	T*	operator*() const  
	{  
		return m_pCurrNode->m_pData;  
	}


	JK_SListNode* operator->() const  
	{  
		return m_pCurrNode;  
	}


	JK_SList_iterator<T, m_bThread>& operator++()
	{
		m_pCurrNode = m_pCurrNode->pNext;
		return *this;
	}


	JK_SList_iterator<T, m_bThread>& operator++(int)
	{
		JK_SList_iterator<T, m_bThread> tmp = *this;
		++*this;
		return tmp;
	}


	bool operator==( const JK_SList_iterator<T, m_bThread>& it)
	{
		return m_pCurrNode == it.m_pCurrNode;
	}


	bool operator!=( const JK_SList_iterator<T, m_bThread>& it)
	{
		return m_pCurrNode != it.m_pCurrNode;
	}

private:  

	JK_SListNode* m_pCurrNode;

};



#endif