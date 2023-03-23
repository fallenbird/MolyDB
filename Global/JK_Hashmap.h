
#ifndef _JK_HASHMAP_MDXDC9F_H_
#define _JK_HASHMAP_MDXDC9F_H_

#include "JK_Lock.h"
#include "JK_MemMgr.h"
#include "JK_Assert.h"
#include "JK_Utility.h"
#include "Define.h"


enum eValueType
{
	evt_NONE	= 0,
	evt_STR		= 1,
	evt_LIST	= 2,
	evt_HASH	= 3,
};



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
	unsigned short		m_valType;
	class HashEntity*	next;

public:
	HashEntity()
	{
		m_key = NULL;
		v.m_val = 0;
		next = NULL;
		m_valType = 0;
	}
};

//extern unsigned int dict_hash_function_seed;


//template< typename T, bool bThread = false >
//class __declspec(dllexport) JK_Hashmap

template< typename T, bool m_bThread >
class JK_Hashmap
{

	template< typename T, bool m_bThread = false > friend  class JK_Hashmap_iterator;

private:
	HashEntity**	m_table;
	unsigned long	m_lSize;
	unsigned long	m_lUsed;
	//bool			m_bThread;
	JK_Lock			m_lock;

public:
	JK_Hashmap() : m_lSize(0), m_lUsed(0)
	{
	}

	typedef JK_Hashmap_iterator<T, m_bThread> iterator;

	inline void Reset()
	{
		lock_if_necessary();
		m_table = NULL;
		m_lSize = 0;
		m_lUsed = 0;
		unlock_if_necessary();
	}


	void SetSize(unsigned long val) 
	{ 
		JK_ASSERT( val >= 0 && val < 999999 );
		m_lSize = val; 
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
		SetSize( lMaxSize );
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
	void Release()
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

	
	void* Get( void* key, eValueType valtype )
	{
		if( NULL == key  || 0 == m_lUsed )
		{
			return NULL;
		}
		unsigned int hashidx = HashFunction( (unsigned char*)key );
		hashidx %= m_lSize;
		lock_if_necessary();
		HashEntity** ppEntity= &m_table[hashidx];
		HashEntity* pEntity = GetEntity( m_table[hashidx],  key, valtype);
		unlock_if_necessary();
		if( NULL == pEntity  )
		{
			return NULL;
		}
		return pEntity->v.m_val;
	}



	bool Add( void* key, void* val, eValueType valtype)
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
			if (0 == strcmp((char*)(*ppEntity)->m_key, (char*)key))
			{
				unlock_if_necessary();
				return false;
			}
			ppEntity = &( (*ppEntity)->next );
		}
		HashEntity* pTemp = (HashEntity*)JK_NEW( HashEntity );
		if( NULL == pTemp )
		{
			unlock_if_necessary();
			return false;
		}
		pTemp->m_key = key;
		pTemp->v.m_val = val;
		pTemp->m_valType = valtype;
		pTemp->next = NULL;
		*ppEntity = pTemp;
		++m_lUsed;
		unlock_if_necessary();
		return true;
	}


	bool Update( void* key, void* val )
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
			if (0 == strcmp((char*)(*ppEntity)->m_key, (char*)key))
			{
				JK_FREE((*ppEntity)->v.m_val );
				(*ppEntity)->v.m_val = val;
				unlock_if_necessary();
				return true;
			}
			ppEntity = &( (*ppEntity)->next );
		}
		unlock_if_necessary();
		return false;
	}


	bool Remove( void* key )
	{
		if( NULL == key || 0 == m_lUsed  )
		{
			return false;
		}
		HashEntity* ptmpEntity;
		unsigned int hashidx = HashFunction( ( unsigned char*)key );
		hashidx %= m_lSize;
		lock_if_necessary();
		HashEntity* pParent = GetParentEntity( m_table[hashidx], key );
		if ( !pParent )
		{
			if ( !m_table[hashidx] )
			{
				unlock_if_necessary();
				return false;
			}
			if ( 0 != strcmp( (char*)m_table[hashidx]->m_key, (char*)key ) )
			{
				unlock_if_necessary();
				return false;
			}
			ptmpEntity = m_table[hashidx];
			m_table[hashidx] = m_table[hashidx]->next;
			JK_FREE(ptmpEntity->v.m_val );
			JK_DELETE(HashEntity, ptmpEntity);
			--m_lUsed;
			unlock_if_necessary();
			return true;
		}
		JK_ASSERT(NULL != pParent->next);
		HashEntity* pTemp = pParent->next;
		pParent->next = pParent->next->next;
		JK_FREE( pTemp->v.m_val );
		JK_DELETE( HashEntity, pTemp );
		pTemp = NULL;
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

	// --murmurhash2
	unsigned int murmur_hash2(char *data, size_t len)
	{
		uint32_t h, k;
		h = 0 ^ len;
		while (len >= 4) 
		{
			k = data[0];
			k |= data[1] << 8;
			k |= data[2] << 16;
			k |= data[3] << 24;

			k *= 0x5bd1e995;
			k ^= k >> 24;
			k *= 0x5bd1e995;
			h *= 0x5bd1e995;
			h ^= k;
			data += 4;
			len -= 4;
		}

		switch (len) 
		{
		case 3:
			h ^= data[2] << 16;
		case 2:
			h ^= data[1] << 8;
		case 1:
			h ^= data[0];
			h *= 0x5bd1e995;
		}

		h ^= h >> 13;
		h *= 0x5bd1e995;
		h ^= h >> 15;
		return h;
	}


	void  FetchKeys( char* fmt, int istart, int ilimit, char keysarray[1024][MAX_KEY_LEN], int& keyscnt )
	{
		HashEntity* ptemp;
		keyscnt = 0;
		bool bAll = (fmt[0] == '*' && fmt[1] == '\0');
		lock_if_necessary();
		iterator itr = Begin();
		for ( ; itr != End() && keyscnt < ilimit; ++itr  )
		{
			ptemp = *(itr);
			if ( !ptemp )
			{
				continue;
			}
			if ( bAll || JK_Utility::jk_str_match( fmt, strlen(fmt), (char*)(ptemp->m_key), strlen( ((char*)ptemp->m_key)), 0 )  )
			{
				sprintf_s( keysarray[keyscnt], MAX_KEY_LEN, "%s", (char*)ptemp->m_key ); // TODO
				++keyscnt;
			}
		}
		unlock_if_necessary();
	}


	bool IsExists( char* key )
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
			if (0 == strcmp((char*)(*ppEntity)->m_key, (char*)key))
			{
				unlock_if_necessary();
				return true;
			}
			ppEntity = &( (*ppEntity)->next );
		}
		unlock_if_necessary();
		return false;
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

	HashEntity* GetEntity( HashEntity* pEntity, void* key, eValueType valtype)
	{
		HashEntity* pTempEntity = pEntity;
		while( NULL != pTempEntity )
		{
			if (pTempEntity->m_valType != valtype) 
			{
				pTempEntity = pTempEntity->next;
				continue;
			}

			if ( 0 != strcmp( (char*)pTempEntity->m_key, (char*)key ) )
			{
				pTempEntity = pTempEntity->next;
				continue;
			}
			return pTempEntity;
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
			if( NULL == pEntity->next )
			{
				return NULL;
			}
			if ( 0 == strcmp( (char*)pEntity->next->m_key, (char*)key ) )
			{
				return pEntity;
			}
			pEntity = pEntity->next;
		}
		return NULL;
	}

	iterator Begin()
	{
		for ( unsigned long i=0; i<m_lSize; ++i)
		{
			if ( m_table[i] )
			{
				return iterator( m_table[i], this );
			}
		}
		return End();
	}

	iterator End()
	{
		return iterator( NULL, this);
	}


	HashEntity* GetNextUsedBucket( void* key )
	{
		unsigned int hashidx = HashFunction( (unsigned char*)key );
		hashidx %= m_lSize;
		while( ++hashidx < m_lSize )
		{
			if ( m_table[hashidx] )
			{
				return m_table[hashidx];
			}
		}
		return NULL;
	}

};

template< typename T, bool bThread >
class JK_Hashmap_iterator
{ 
public:

	JK_Hashmap_iterator( HashEntity* pEntity, JK_Hashmap<T, bThread>* pHash )
	{
		JK_ASSERT( pHash );
		m_pCurrEntity = pEntity;
		m_pHashPtr = pHash;
	}

	HashEntity*	 operator*() const  
	{  
		return m_pCurrEntity;  
	}


	T* operator->() const  
	{  
		return m_pCurrEntity;  
	}


	JK_Hashmap_iterator<T, bThread>& operator++()
	{
		const HashEntity* old = m_pCurrEntity;
		m_pCurrEntity = m_pCurrEntity->next;
		if ( !m_pCurrEntity )
		{
			// find next bucket head
			m_pCurrEntity = m_pHashPtr->GetNextUsedBucket( old->m_key );
		}
		return *this;
	}


	
	JK_Hashmap_iterator<T, bThread>& operator++(int)
	{
		JK_Hashmap_iterator<T, bThread> tmp = *this;
		++*this;  // operator++()
		return tmp;
	}


	bool operator==( const JK_Hashmap_iterator<T, bThread>& it)
	{
		return m_pCurrEntity == it.m_pCurrEntity;
	}


	bool operator!=( const JK_Hashmap_iterator<T, bThread>& it)
	{
		return m_pCurrEntity != it.m_pCurrEntity;
	}

private:  
	HashEntity*	m_pCurrEntity;
	JK_Hashmap<T, bThread>* m_pHashPtr;

};


#endif