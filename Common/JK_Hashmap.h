
#ifndef _JK_HASHMAP_MDXDC9F_H_
#define _JK_HASHMAP_MDXDC9F_H_

#include "JK_Lock.h"
#include "JK_MemMgr.h"
#include <assert.h>


class HashEntity
{
public:
	void*				m_key;
	union
	{
		void*			m_val;
		unsigned int	m_ui64;
		int				m_i64;
	} v;
	class HashEntity*	next;

public:
	HashEntity()
	{
		m_key = NULL;
		v.m_val = 0;
		next = NULL;
	}
};

//extern unsigned int dict_hash_function_seed;


//template< typename T, bool bThread = false >
//class __declspec(dllexport) JK_Hashmap
class JK_Hashmap
{

private:
	HashEntity**	m_table;
	unsigned long	m_lSize;
	unsigned long	m_lUsed;

public:

	bool Init( unsigned long lMaxSize )
	{
		m_lSize = lMaxSize;
		m_lUsed = 0;
		m_table = (HashEntity**)JKMalloc( sizeof(HashEntity*)*lMaxSize );
		if( m_table )
		{
			memset( m_table, 0, sizeof(HashEntity*)*lMaxSize );
			return true;
		}
		return true;
	}


	bool Realse()
	{
		for( unsigned long i = 0; i < m_lSize; ++i )
		{
			ReleaseEntity( m_table[i] );
		}
		m_lSize = 0;
		m_lUsed = 0;
		m_table = NULL;
	}


	bool Reset()
	{

	}

	
	void* Get( void* key )
	{
		if( NULL == key )
		{
			return NULL;
		}
		unsigned int hashidx = HashFunction( (unsigned char*)key, strlen( (char*)key ) );
		hashidx %= m_lSize;
		HashEntity** ppEntity= &m_table[hashidx];
		HashEntity* pEntity = GetEntity( m_table[hashidx],  key );
		if( NULL == pEntity )
		{
			return NULL;
		}
		return pEntity->v.m_val;
	}



	bool Add( void* key, void* val )
	{
		if( NULL == key )
		{
			return false;
		}
		unsigned int hashidx = HashFunction( (unsigned char*)key, strlen( (char*)key ) );
		hashidx %= m_lSize;
		HashEntity** ppEntity= &m_table[hashidx];
		while( NULL != *ppEntity )
		{
			assert( 0!= strcmp( (char*)(*ppEntity)->m_key, (char*)key ) );
			ppEntity = &( (*ppEntity)->next );
		}
		HashEntity* pTemp = (HashEntity*)JKMalloc( sizeof(HashEntity) );
		if( NULL == pTemp )
		{
			return false;
		}
		pTemp->m_key = key;
		pTemp->v.m_val = val;
		pTemp->next = NULL;
		*ppEntity = pTemp;
		++m_lUsed;
		return true;
	}


	bool Remove( void* key )
	{
		if( NULL == key )
		{
			return false;
		}
		unsigned int hashidx = HashFunction( ( unsigned char*)key, strlen( (char*)key ) );
		hashidx %= m_lSize;
		HashEntity* pParent = GetParentEntity( m_table[hashidx], key );
		assert( NULL != pParent->next );
		HashEntity* pTemp = pParent->next;
		pParent->next = pParent->next->next;
		JKFree( pTemp );
		--m_lUsed;
		return true;
	}


private:

	// --djb hash 
	unsigned int HashFunction( const unsigned char *buf, int len ) 
	{
		//unsigned int hash = (unsigned int)dict_hash_function_seed;
		unsigned int hash = 5381;
		while (len--)
		{
			hash = ((hash << 5) + hash) + (tolower(*buf++)); /* hash * 33 + c */
		}
		return hash;
	}

	void ReleaseEntity( HashEntity* pEntity )
	{
		if( NULL == pEntity ) 
		{
			return;
		}
		HashEntity* pTemp;
		while( pEntity->next )
		{
			pTemp = pEntity;
			pEntity = pEntity->next;
			JKFree( pTemp );
		}
	}

	HashEntity* GetEntity( HashEntity* pEntity, void* key  )
	{
		while( NULL != pEntity )
		{
			if ( 0 == strcmp( (char*)pEntity->m_key, (char*)key ) )
			{
				return pEntity;
			}
			pEntity = pEntity->next;
		}
		return NULL;
	}


	HashEntity* GetParentEntity( HashEntity* pEntity, void* key  )
	{
		if ( NULL == pEntity )
		{
			return NULL;
		}
		while( true )
		{
			if( NULL != pEntity->next )
			{
				return pEntity;
			}
			if ( 0 == strcmp( (char*)pEntity->next->m_key, (char*)key ) )
			{
				return pEntity;
			}
			pEntity = pEntity->next;
		}
		return NULL;
	}

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

		return true;
	}


	void RemoveElement( void* key )
	{

	}

	void  Reset()
	{
	}





private:
	JK_Hashmap	m_hashtable[2];
	int			m_iRehashidx;		// rehashing not in progress if rehashidx == -1
	int			m_Iterators;		// number of iterators currently running 
};







#endif