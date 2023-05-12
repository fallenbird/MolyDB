#ifndef __PRIORITY_QUEUE_HPP__
#define __PRIORITY_QUEUE_HPP__


#include <stdio.h>
#include <stdlib.h>
#include "JK_MemMgr.h"
#pragma warning(disable:26495)


/* ��ǽڵ㣬�����������еı�ͷ�ڵ�
* ��ֵ����С��������С���е�Ԫ�أ�����ֵΪ-1
*/

/*
* ʹ������ʵ�ֶ�
* capacity ������������
* size     ����ĳ���
* elements ���е�Ԫ�ش�ŵ�����
*/
class DataUnit
{
public:
	char	m_szKey[MAX_KEY_LEN];
	int		m_iValue;
	DataUnit& operator =(const DataUnit& dst)//��ֵ�����
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
	DataUnit*		m_elements; // �ѵ�Ԫ�ظ���Ϊsize��ʵ���������洢������ĳ���Ϊsize + 1��������һ��sentinelԪ��

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
		// ����ĵ�0��Ԫ���Ǹ�sentinel��ǽڵ㣬�������������У���������capcaity��size��
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
			// ��GNU99��׼�У�free(NULL)ʲô������ֱ�ӷ��أ����Բ����ж�pqueue->elements�Ƿ�ΪNULL
			JK_FREE(m_elements );
		}
	}

	void makeEmpty(JK_PriorityQueue* pqueue)
	{
		m_iSize = 0;
	}

	/*
	* ����ʱ�����е�Ԫ��ִ�����˲���
	* ɾ��ʱ�����е�Ԫ��ִ�����˲���
	*/

	/*
	* ��ĩβ�����ϸ�
	* �����ʱ�临�Ӷ�ΪO(log N)��NΪ��С���е�Ԫ�ظ���
	* ʵ���ϣ���ƽ��ִ��ʱ��ΪO(1)
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
				m_elements[i] = m_elements[i/2]; // ���˲���
				if (0 == i) 
				{
					break;
				}
			}
			m_elements[i] = x;
		}
	}

	/*
	* �����һ��Ԫ���滻��λԪ�أ�Ȼ��ʼ�³�
	* ɾ��������ƽ��ʱ��ΪO(log N)
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
			// �ڵ�iֻ��һ������ʱ����i * 2 = pqueue->size
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
				m_elements[i] = m_elements[child]; // ���˲���
			}
		}
		m_elements[i] = lastElement;
		return minElement; // ���ر�ɾ����Ԫ��
	}

	/*
	* ִ��ʱ�䣺O(1)
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
	* ͨ��n�β���Ԫ�ؽ����ѣ�����ÿ�β����ƽ��ִ��ʱ��ΪO(1)�����Խ���ƽ��ʱ��ΪO(N)
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