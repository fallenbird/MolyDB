
#ifndef _JK_HASHMAP_MDXDC9F_H_
#define _JK_HASHMAP_MDXDC9F_H_

#include "JK_Lock.h"
#include "JK_MemMgr.h"
#include "JK_Assert.h"


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
	bool			m_bThread;
	JK_Lock			m_lock;

public:
	JK_Hashmap( bool bThread = false  )
	{
		m_bThread = bThread;
	}


	inline unsigned long GetSize()
	{
		lock_if_necessary();
		return m_lSize;
		unlock_if_necessary();
	}


	inline unsigned long GetUsed()
	{
		lock_if_necessary();
		return m_lUsed;
		unlock_if_necessary();
	}


	inline void IncUsed()
	{
		lock_if_necessary();
		++m_lUsed;
		unlock_if_necessary();
	}


	inline void DecUsed()
	{
		lock_if_necessary();
		--m_lUsed;
		unlock_if_necessary();
	}


	bool Init( unsigned long lMaxSize )
	{
		lock_if_necessary();
		m_lSize = lMaxSize;
		m_lUsed = 0;
		m_table = (HashEntity**)JK_MALLOC( sizeof(HashEntity*)*lMaxSize );
		if( m_table )
		{
			memset( m_table, 0, sizeof(HashEntity*)*lMaxSize );
			unlock_if_necessary();
			return true;
		}
		unlock_if_necessary();
		return false;
	}


	// release hash and all node
	bool ReleaseAll()
	{
		lock_if_necessary();
		for( unsigned long i = 0; i < m_lSize; ++i )
		{
			ReleaseEntity( m_table[i] );
		}
		JK_FREE(m_table);
		unlock_if_necessary();
		m_lSize = 0;
		m_lUsed = 0;
		m_table = NULL;
	}


	// release hash only
	bool Release()
	{
		lock_if_necessary();
		JK_FREE(m_table);
		unlock_if_necessary();
		m_lSize = 0;
		m_lUsed = 0;
		m_table = NULL;
	}


	HashEntity* GetEntityByIdx(int idx)
	{
		lock_if_necessary();
		return m_table[idx];
		unlock_if_necessary();
	}


	void SetEntityByIdx( int idx, HashEntity* he )
	{
		lock_if_necessary();
		m_table[idx] = he;
		unlock_if_necessary();
	}

	
	void* Get( void* key )
	{
		if( NULL == key )
		{
			return NULL;
		}
		unsigned int hashidx = HashFunction( (unsigned char*)key );
		hashidx %= m_lSize;
		lock_if_necessary();
		HashEntity** ppEntity= &m_table[hashidx];
		HashEntity* pEntity = GetEntity( m_table[hashidx],  key );
		unlock_if_necessary();
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
		unsigned int hashidx = HashFunction( (unsigned char*)key );
		hashidx %= m_lSize;

		lock_if_necessary();
		HashEntity** ppEntity= &m_table[hashidx];
		while( NULL != *ppEntity )
		{
			( 0!= strcmp( (char*)(*ppEntity)->m_key, (char*)key ) );
			ppEntity = &( (*ppEntity)->next );
		}
		HashEntity* pTemp = (HashEntity*)JK_MALLOC( sizeof(HashEntity) );
		if( NULL == pTemp )
		{
			unlock_if_necessary();
			return false;
		}
		pTemp->m_key = key;
		pTemp->v.m_val = val;
		pTemp->next = NULL;
		*ppEntity = pTemp;
		++m_lUsed;
		unlock_if_necessary();
		return true;
	}


	bool Remove( void* key )
	{
		if( NULL == key )
		{
			return false;
		}
		unsigned int hashidx = HashFunction( ( unsigned char*)key );
		hashidx %= m_lSize;
		lock_if_necessary();
		HashEntity* pParent = GetParentEntity( m_table[hashidx], key );
		JK_ASSERT(NULL != pParent->next);
		HashEntity* pTemp = pParent->next;
		pParent->next = pParent->next->next;
		JK_FREE( pTemp );
		--m_lUsed;
		unlock_if_necessary();
		return true;
	}


	// --djb hash 
	unsigned int HashFunction( const unsigned char *buf ) 
	{
		//unsigned int hash = (unsigned int)dict_hash_function_seed;
		unsigned int hash = 5381;
		int len = strlen((char*)buf);
		while (len--)
		{
			hash = ((hash << 5) + hash) + (tolower(*buf++)); /* hash * 33 + c */
		}
		return hash;
	}


private:
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
			JK_FREE( pTemp );
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





#endif