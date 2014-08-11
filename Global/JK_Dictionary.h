

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

	}


private:
	JK_Hashmap	m_hashtable[2];
	int			m_iRehashidx;		// rehashing not in progress if rehashidx == -1
	int			m_Iterators;		// number of iterators currently running 
};


#endif