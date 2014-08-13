

#ifndef _JK_DICTIONARY_MGYDCR6_H_
#define _JK_DICTIONARY_MGYDCR6_H_

#include "JK_Lock.h"
#include "JK_MemMgr.h"
#include "JK_Hashmap.h"
#include <assert.h>


class JK_Dictionary
{
public:
	JK_Dictionary()
	{

	}

	~JK_Dictionary()
	{

	}

	bool AddElement( void* key, void* val )
	{

		return true;
	}


	void RemoveElement( void* key )
	{

	}




	void Rehash()
	{
		if ( m_iCurrIdx < 0 )
		{
			return;
		}
		ResizeDict();
		m_iCurrIdx -= 2;
	}


	void UpdateDict()
	{
		Rehashing();
	}



private:

	void ResizeDict()
	{
		if (m_iCurrIdx < 0)
		{
			return;
		}
		int idx = (0 == m_iCurrIdx) ? 1 : 0;
		m_hashtable[idx].Init(m_hashtable[idx].GetSize() * 2);
	}

	void Rehashing()
	{
		if (m_iCurrIdx >= 0 )
		{
			return;
		}
	}

private:
	JK_Hashmap	m_hashtable[2];
	int			m_iCurrIdx;			// current index
	int			m_iRehashidx;		// rehashing not in progress if rehashidx == -1
	int			m_Iterators;		// number of iterators currently running 
};


#endif