
#ifndef _JK_HASHMAP_MDXDC9F_H_
#define _JK_HASHMAP_MDXDC9F_H_

#include "JK_Lock.h"
#include "JK_MemMgr.h"
#include <assert.h>


class HashEntity
{
private:
	void*			m_key;
	union
	{
		void*		m_val;
		uint64_t	m_u64;
		int64_t		m_s64;
	} v;
	class HashEntity *next;
};



template < typename T >
class JK_Hashmap
{

private:
	HashEntity**	m_table;
	unsigned long	m_lSize;
	unsigned long	m_lUsed;
};



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
		if( NULL == key )
		{
			return false;
		}
		if( NULL == m_table )
		{
			m_table = JKNew( HashEntity );
			if( NULL == m_table )
			{
				return false;
			}
		}
		unsigned int hashidx = HashFunction( key, strlen(key ) );
		return true;
	}


	void RemoveElement( void* key )
	{

	}

	void  Reset()
	{
		m_table = NULL;
	}


private:

	// --djb hash 
	unsigned int HashFunction( const unsigned char *buf, int len ) 
	{
		unsigned int hash = (unsigned int)dict_hash_function_seed;
		while (len--)
		{
			hash = ((hash << 5) + hash) + (tolower(*buf++)); /* hash * 33 + c */
		}
		return hash;
	}


private:
	JK_Hashmap	m_hashtable[2];
	int			m_iRehashidx;		// rehashing not in progress if rehashidx == -1
	int			m_Iterators;		// number of iterators currently running 
};







#endif