#pragma once


#ifndef _JK_SKIPLIST_OMYGD32_H_
#define _JK_SKIPLIST_OMYGD32_H_

#include "JK_Lock.h"

#define MAX_SKIPLIST_LEVEL 16




class JK_SkipListNode
{
public:
	JK_SkipListNode() :m_iKey(0), m_pValue(NULL), m_pLeft(NULL), m_pRight(NULL), m_pUp(NULL), m_pDown(NULL)
	{
	}
	unsigned int		m_iKey;
	void*				m_pValue;
	JK_SkipListNode*	m_pLeft;
	JK_SkipListNode*	m_pRight;
	JK_SkipListNode*	m_pUp;
	JK_SkipListNode*	m_pDown;

};


template< typename T, bool m_bThread >
class JK_SkipList
{

	template< typename T, bool m_bThread > friend  class JK_SkipList_iterator;

public:
	typedef JK_SkipList_iterator<T, m_bThread> iterator;

	JK_SkipList( int maxlv = MAX_SKIPLIST_LEVEL ) :m_pHead(NULL), m_pTail(NULL), m_uiSize(0)
	{
		srand((unsigned)time(0));

		m_iMaxLevel = maxlv;
		int iLevel = 1;

		m_pHead = new JK_SkipListNode();
		m_pTail = new JK_SkipListNode();
		m_pHead->m_pRight = m_pTail;
		m_pTail->m_pLeft  = m_pHead;

		JK_SkipListNode* pHead = m_pHead;
		JK_SkipListNode* pTail = m_pTail;

		// create level of head & tail
		while ( iLevel < m_iMaxLevel )
		{
			pHead->m_pUp = new JK_SkipListNode();
			pTail->m_pUp = new JK_SkipListNode();

			pHead = pHead->m_pUp;
			pTail = pTail->m_pUp;

			pHead->m_pRight = pTail;
			pTail->m_pLeft = pHead;
			iLevel++;
		}
	}


	~JK_SkipList()
	{
		Destroy();
	}


	bool InsertNode( unsigned int key, T* pVal ) 
	{
		// 查找适合插入的位子
		JK_SkipListNode* pNode = FindNode(key);

		// 如果跳跃表中存在含有key值的节点，则进行value的修改操作即可完成
		if ( pNode->m_iKey == key ) {
			 pNode->m_pValue = pVal;
			return true;
		}

		// 如果跳跃表中不存在含有key值的节点，则进行新增操作
		JK_SkipListNode* newNode = new JK_SkipListNode();
		newNode->m_iKey = key;
		newNode->m_pLeft = pNode;
		newNode->m_pRight = pNode->m_pRight;
		pNode->m_pRight->m_pLeft = newNode;
		pNode->m_pRight = newNode;


		// 再使用随机数决定是否要向更高level攀升
		unsigned int iLevel = 0;
		while ( randomLevel() ) {

			// 如果新元素的级别已经达到跳跃表的最大高度，则新建空白层
			if (iLevel >= m_iMaxLevel ) {
				addEmptyLevel();
			}

			// 从p向左扫描含有高层节点的节点
			while ( pNode->m_pUp  == NULL) {
				pNode = pNode->m_pLeft;
			}
			pNode = pNode->m_pUp;

			// 新增和q指针指向的节点含有相同key值的节点对象
			// 这里需要注意的是除底层节点之外的节点对象是不需要value值的
			JK_SkipListNode* zNode = new JK_SkipListNode();
			zNode->m_iKey = key;
			zNode->m_pLeft = pNode;
			zNode->m_pRight = pNode->m_pRight;
			pNode->m_pRight->m_pLeft = zNode;
			pNode->m_pRight = zNode;
			zNode->m_pDown = newNode;
			newNode->m_pUp = zNode;
			newNode = zNode;
			++iLevel;
		}
		++m_uiSize;
		return true;
	}


	T* GetNode(unsigned int key )
	{
		JK_SkipListNode* tmpNode = FindNode(key);
		if (NULL == tmpNode) 
		{
			return NULL;
		}
		if (tmpNode->m_iKey == key) 
		{
			return tmpNode->m_pValue;
		}
		return NULL;
	}



	unsigned int GetSize()
	{
		return m_uiCount;
	}


	void	Destroy()
	{
		JK_SkipListNode* pTemp;
		lock_if_necessary();
		while (m_pHead)
		{
			pTemp = m_pHead->m_pRight;
			JK_FREE(m_pHead);
			m_pHead = pTemp;
		}
		m_uiSize = 0;
		unlock_if_necessary();
	}


private:


	JK_SkipListNode* FindNode( unsigned int key )
	{
		// begin from head
		JK_SkipListNode* p = m_pHead;
		while (true) 
		{
			// query from left to right, right key, 
			// until the right node's key is greater than the key we want 
			while ( p->m_pRight->m_iKey <= key && p->m_pRight != m_pTail )
			{
				p = p->m_pRight;
			}

			// If there are nodes at lower levels，move to down
			if (p->m_pDown != NULL ) 
			{
				p = p->m_pDown;
			}
			else 
			{
				break;
			}
		}
		// return p, Note that the key of p here is less than or equal to the passed key（p.key <= key）
		return p;
	}


	void addEmptyLevel() 
	{

	}

	bool randomLevel() 
	{
		int tmp = rand() % 2;
		if (tmp >= 1) 
		{
			return true;
		}
		return false;
	}


	iterator Begin()
	{
		return iterator(m_pHead);
	}


	iterator End()
	{
		return iterator(m_pTail);
	}


private:
	JK_SkipListNode*	m_pHead;		// head node ptr
	JK_SkipListNode*	m_pTail;		// tail node ptr
	JK_Lock				m_lock;			// lock 
	unsigned int		m_uiSize;		// size of skiplist
	unsigned int		m_iMaxLevel;	// max level of skiplist
};



template< typename T, bool m_bThread >
class JK_SList_iterator
{
public:

	JK_SList_iterator(JK_SkipListNode* pHead)
	{
		m_pCurrNode = pHead;
	}

	T* operator*() const
	{
		return m_pCurrNode->m_pData;
	}


	JK_SkipListNode* operator->() const
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
		++* this;
		return tmp;
	}


	bool operator==(const JK_SList_iterator<T, m_bThread>& it)
	{
		return m_pCurrNode == it.m_pCurrNode;
	}


	bool operator!=(const JK_SList_iterator<T, m_bThread>& it)
	{
		return m_pCurrNode != it.m_pCurrNode;
	}

private:

	JK_SkipListNode* m_pCurrNode;

};



#endif