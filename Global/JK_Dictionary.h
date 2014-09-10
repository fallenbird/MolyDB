

#ifndef _JK_DICTIONARY_MGYDCR6_H_
#define _JK_DICTIONARY_MGYDCR6_H_

#include "JK_Lock.h"
#include "JK_MemMgr.h"
#include "JK_Hashmap.h"
#include "JK_Timer.h"
#include "JK_Lock.h"
#include <assert.h>


#define DICT_INITIAL_SIZE   4
#define	REHASH_MIN_REFER	10
#define	REHASH_MAX_REFER	2

template<bool bThread>
class JK_Dictionary
{
public:
	JK_Dictionary()
	{
		m_iRehashidx = -1;
	}

	~JK_Dictionary()
	{

	}

	bool InitDictionary()
	{
		return m_hashtable[0].Init(DICT_INITIAL_SIZE);
	}

	bool AddElement( void* key, void* val )
	{
		if (-1 == m_iRehashidx)
		{
			return m_hashtable[0].Add(key, val);
		}
		return m_hashtable[1].Add(key, val);
	}

	void* GetElement(void* key )
	{
		if (-1 == m_iRehashidx)
		{
			return m_hashtable[0].Get(key);
		}
		void* tempVal = m_hashtable[0].Get(key);
		if (NULL != tempVal )
		{
			return tempVal;
		}
		return m_hashtable[1].Get(key);
	}


	bool RemoveElement( void* key )
	{
		if (-1 == m_iRehashidx)
		{
			return m_hashtable[0].Remove(key);
		}
		if (true == m_hashtable[0].Remove(key))
		{
			return true;
		}
		return m_hashtable[1].Remove(key);
	}


	void TryRehashDict()
	{
		if (!IsNeedResize())
		{
			return;
		}
		BeginResizeDict( m_hashtable[0].GetUsed()*2 );
	}


	void UpdateDict( int iUpdateTick )
	{
		long long start = Utility::GetCurrentTimeTick();
		int iTimes = 0;
		TryRehashDict();
		while (!UpdateResize())
		{
			if ((iTimes >= 100) || (Utility::GetCurrentTimeTick() - start > iUpdateTick))
			{
				break;
			}
			++iTimes;
		}
	}


	void  FetchKeys( char* fmt, int istart, int ilimit, char keysarray[1024][MAX_KEY_LEN], int& kescnt )
	{
		if (-1 == m_iRehashidx)
		{
			m_hashtable[0].FetchKeys( fmt, istart, ilimit, keysarray, kescnt);
			return;
		}
		m_hashtable[1].FetchKeys( fmt, istart, ilimit, keysarray, kescnt);
	}



private:


	bool BeginResizeDict( unsigned long size)
	{
		JK_ASSERT( 0 == m_hashtable[1].GetUsed() );
		unsigned long realsize = GetRealSize(size);
		m_hashtable[1].Init(realsize);
		m_iRehashidx = 0;
		return true;
	}



	bool IsNeedResize() 
	{
		if (-1 != m_iRehashidx )
		{
			return false;
		}
		long long size = m_hashtable[0].GetSize(); // +m_hashtable[1].GetSize();
		long long used = m_hashtable[0].GetUsed(); // +m_hashtable[1].GetUsed();
		if (used / size >= REHASH_MAX_REFER )
		{
			return true;
		}
		if (size && used && size > DICT_INITIAL_SIZE && (used * 100 / size < REHASH_MIN_REFER ))
		{
			return true;
		}
		return false;
	}


	// return true if reshashed
	bool UpdateResize()
	{
		if (-1 == m_iRehashidx)
		{
			return true;
		}

		// Check if rehashed the whole hashtable
		if ( 0 == m_hashtable[0].GetUsed() ) 
		{
			m_hashtable[0].Release();
			m_hashtable[0] = m_hashtable[1];
			m_hashtable[1].Reset();
			m_iRehashidx = -1;
			return true;
		}
		
		JK_ASSERT(m_hashtable[0].GetSize() > (unsigned long)m_iRehashidx);
		while (NULL == m_hashtable[0].GetEntityByIdx(m_iRehashidx))
		{
			++m_iRehashidx;
		}

		HashEntity* he = m_hashtable[0].GetEntityByIdx(m_iRehashidx);
		HashEntity* nexthe = NULL;

		// Move from m_hashtable[0] to m_hashtable[1]
		unsigned long sizemask = m_hashtable[1].GetSize() - 1;
		unsigned int iNewIdx;
		while (he)
		{
			nexthe = he->next;
			// Get the index in the new hash, this idear comes from redis
			// There is a more efficient way to comute the new index as avoiding % calculation.
			// X & (2^n -1) == X % 2^n
			iNewIdx = m_hashtable[0].HashFunction( (const unsigned char*)(he->m_key)) & sizemask;
			he->next = m_hashtable[1].GetEntityByIdx(iNewIdx);
			m_hashtable[1].SetEntityByIdx( iNewIdx, he);
			m_hashtable[0].DecUsed();
			m_hashtable[1].IncUsed();
			he = nexthe;
		}
		m_hashtable[0].SetEntityByIdx(m_iRehashidx, NULL);
		++m_iRehashidx;
		return false;
	}


	static unsigned long GetRealSize( unsigned long size )
	{
		unsigned long i = DICT_INITIAL_SIZE;
		if (size >= LONG_MAX)
		{
			return LONG_MAX;
		}
		while (1)
		{
			if (i >= size)
			{
				return i;
			}
			i *= 2;
		}
	}



private:
	JK_Hashmap<char*, bThread>	m_hashtable[2];
	int					m_iRehashidx;		// rehashing not in progress if rehashidx == -1
	int					m_Iterators;		// number of iterators currently running 
};


#endif