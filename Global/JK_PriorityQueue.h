#ifndef __PRIORITY_QUEUE_HPP__
#define __PRIORITY_QUEUE_HPP__


#include <stdio.h>
#include <stdlib.h>
#include "JK_MemMgr.h"

/* 标记节点，类似于链表中的表头节点
* 该值必须小于所有最小堆中的元素，设其值为-1
*/

/*
* 使用数组实现堆
* capacity 数组的最大容量
* size     数组的长度
* elements 堆中的元素存放的数组
*/
class DataUnit
{
public:
	char	m_szKey[MAX_KEY_LEN];
	int		m_iValue;
	DataUnit& operator =(const DataUnit& dst)//赋值运算符
	{
		if (this != &dst)
		{
			this->m_iValue = dst.m_iValue;
			strcpy_s(this->m_szKey, strlen(dst.m_szKey) + 1, dst.m_szKey);
		}
		return *this;
	}
};

class JK_PriorityQueue
{
private:
	int				m_iCapacity;
	int				m_iSize;
	DataUnit*		m_elements; // 堆的元素个数为size，实际上用来存储的数组的长度为size + 1，还包括一个sentinel元素

public:
	JK_PriorityQueue()
	{
		initialize(128);
	}

	void PQueueNULLWarning()
	{
		printf("Warning: Minimum Priority Queue is NULL");
	}

	void outOfSpaceFatalError()
	{
		printf("Fatal Error: Out of space");
		abort();
	}


	void initialize(int maxElements)
	{
		if (maxElements <= 0)
		{
			printf("Fail to initialize: maxElements <= 0");
		}
		// 数组的第0个元素是个sentinel标记节点，计入数组容量中，但不计入capcaity或size中
		m_iSize = 0;
		m_iCapacity = maxElements;
		m_elements = (DataUnit*)JK_MALLOC(sizeof(DataUnit)* (m_iCapacity+1) );
		if (m_elements == NULL)
		{
			outOfSpaceFatalError();
		}
		//memset(m_elements, 0, sizeof(DataUnit)*(m_iCapacity + 1));
		for ( int i = 0; i < (m_iCapacity+1); ++i ) 
		{
			m_elements[i].m_iValue = 0;
			memset(m_elements[i].m_szKey, 0, MAX_KEY_LEN);
		}
	}


	void destroy(JK_PriorityQueue* pqueue)
	{
		if (m_elements != NULL)
		{
			// 在GNU99标准中，free(NULL)什么都不做直接返回，所以不用判断pqueue->elements是否为NULL
			JK_FREE(m_elements );
		}
	}

	void makeEmpty(JK_PriorityQueue* pqueue)
	{
		m_iSize = 0;
	}

	/*
	* 插入时，堆中的元素执行下滤操作
	* 删除时，堆中的元素执行上滤操作
	*/

	/*
	* 从末尾插入上浮
	* 插入的时间复杂度为O(log N)，N为最小堆中的元素个数
	* 实际上，其平均执行时间为O(1)
	*/
	void insert(DataUnit x )
	{
		if (isFull())
		{
			printf("Fail to insert: Priority Queue is Full");
			return;
		}
		else
		{
			int i = 0;
			for ( i = m_iSize++; x.m_iValue < m_elements[i/2].m_iValue; i /= 2)
			{
				m_elements[i] = m_elements[i/2]; // 下滤操作
				if (0 == i) 
				{
					break;
				}
			}
			m_elements[i] = x;
		}
	}

	/*
	* 用最后一个元素替换首位元素，然后开始下沉
	* 删除操作的平均时间为O(log N)
	*/
	DataUnit deleteMin()
	{
		if (isEmpty())
		{
			printf("Fail to delete: Priority Queue is Empty");
			DataUnit tmpDataUnit;
			tmpDataUnit.m_iValue = 0;
			memset(&tmpDataUnit, 0, MAX_KEY_LEN );
			return tmpDataUnit;
		}

		int i, child;
		//DataUnit minElement, lastElement;
		DataUnit minElement =m_elements[0];
		DataUnit lastElement = m_elements[--m_iSize];
		m_elements[0] = lastElement;
		for (i = 0; i * 2 <= m_iSize; i = child)
		{
			if (0 == i)
			{
				child = 1;
			}
			else 
			{
				child = i * 2;
			}
			// 节点i只有一个儿子时必有i * 2 = pqueue->size
			if (child < m_iSize && m_elements[child].m_iValue > m_elements[child + 1].m_iValue)
			{
				child++;
			}
			if (lastElement.m_iValue < m_elements[child].m_iValue)
			//if (m_elements[i].m_iValue < m_elements[child].m_iValue)
			{
				break;
			}
			else
			{
				m_elements[i] = m_elements[child]; // 上滤操作
			}
		}
		m_elements[i] = lastElement;
		return minElement; // 返回被删除的元素
	}

	/*
	* 执行时间：O(1)
	*/
	DataUnit* findMin()
	{
		if (m_elements == NULL || 0 == m_iSize )
		{
			return NULL;
		}
		return &m_elements[0];
	}

	int isEmpty()
	{
		return (m_iSize == 0);
	}

	int isFull()
	{
		return (m_iSize == m_iCapacity);
	}

	void percolateDown(DataUnit *arr, int len, int i)
	{
		int child;
		int n = len - 1;
		DataUnit tmp;

		for (tmp = arr[i]; i * 2 < n; i = child)
		{
			child = i * 2;
			if (child < n && arr[child].m_iValue > arr[child + 1].m_iValue)
				child++;

			if (tmp.m_iValue > arr[child].m_iValue)
				arr[i] = arr[child];
			else
				break;
		}
		arr[i] = tmp;
	}

	JK_PriorityQueue* buildHeap_percolate(DataUnit *arr, int n)
	{
		if (arr == NULL)
		{
			printf("Error: Array is NULL");
			return NULL;
		}

		DataUnit *elements = (DataUnit*)JK_MALLOC(sizeof(DataUnit)*(n+1));
		if (elements == NULL)
			outOfSpaceFatalError();

		int i;
		for (int i = 1; i <= n; i++)
		{
			elements[i] = arr[i - 1];
		}
		//elements[0] = SentinelElement;
		for (i = n / 2; i > 0; i--)
		{
			percolateDown(elements, n + 1, i);
		}
		m_elements = elements;
		m_iSize = n;
		m_iCapacity = n * 2;
		return this;
	}

	/*
	* 通过n次插入元素建立堆，由于每次插入的平均执行时间为O(1)，所以建堆平均时间为O(N)
	*/
	JK_PriorityQueue* buildHeap_insert(DataUnit *arr, int n)
	{
		if (arr == NULL)
		{
			printf("Array is NULL, fail to build heap");
			return NULL;
		}
		initialize(n * 2);
		for (int i = 0; i < n; i++)
		{
			insert(arr[i]);
		}
		return this;
	}

	void printMinHeap(JK_PriorityQueue* pqueue)
	{
		if (pqueue == NULL)
		{
			PQueueNULLWarning();
			return;
		}

		if (m_elements == NULL)
		{
			printf("Fail to print: Elements of priority queue is NULL");
			return;
		}

		if (isEmpty())
		{
			printf("Empty Prioirty Queue\n");
			return;
		}

		printf("Priority Queue\n");
		for (int i = 1; i <= m_iSize; i++)
		{
			printf("Element[%d] = %d\n", i, m_elements[i].m_iValue);
		}
		printf("\n");
	}

};

#endif