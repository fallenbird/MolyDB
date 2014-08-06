/*
< 2007.07.10 >
哈希表模版类： 
声明方法：
多线程     j_List < class, count, true > m_variable
单线程     j_List < class, count > m_variable

count为开辟的基本变量个数

在表中添加一个节点
class* pNew = m_variable.AddElement();
在表中删除一个节点
m_variable.DeleteElement( pNew );

*/

#pragma once

//#include "j_List.h"

template< typename T > 
void swap( T& _Left, T& _Right)
{	
	T _Tmp = _Left;
	_Left = _Right, _Right = _Tmp;
}

template< typename T, int acount, bool bThread = false >
class __declspec(dllexport) j_Hash
{
protected:

	struct j_DataNode
	{
		T data;
		T**	m_ppListHandle;
		char m_KeyString[MAX_PATH];
	};
	struct j_HashNode
	{
		j_List< j_DataNode, bThread > m_pListNode;
	};

	DWORD m_aCount;
	DWORD m_dwCount;
	CRITICAL_SECTION m_CritSect;
	j_HashNode*		m_pMap;

	j_List< T* > m_pPtrList;
public:
	j_Hash();
	~j_Hash();

	__inline void Close();
	__inline T* AddElement( char* pKey );
	__inline T* AddElement( DWORD dwKey );
	__inline bool DeleteElement( char* pKey );
	__inline bool DeleteElement( DWORD dwKey );
	__inline T* GetElement( char* pKey );
	__inline T* GetElement( DWORD dwKey );
	__inline DWORD Size() const;
	__inline bool IsEmpty();

	__inline T** Begin();
	__inline T** End();
	__inline  T** GetNextValue( T** pValue );
	__inline  T** GetPrevValue( T** pValue );
protected:
	__inline DWORD GetKeyValue( char* str );
	__inline T* GetElement( char* pKey, Int2Type< false > );
	__inline T* GetElement( char* pKey, Int2Type< true >  );
	__inline T* GetElement( DWORD dwKey, Int2Type< false > );
	__inline T* GetElement( DWORD dwKey, Int2Type< true >  );
};

template < typename T, int acount, bool bThread > j_Hash< T, acount, bThread >::j_Hash():m_dwCount( 0 )
{
	m_pMap = JNEW( j_HashNode, acount);
	m_aCount = acount;
	if( bThread )
		InitializeCriticalSection( &m_CritSect );
}

template < typename T, int acount, bool bThread > j_Hash< T, acount, bThread >::~j_Hash()
{
	Close();
	if( bThread )
		DeleteCriticalSection( &m_CritSect );
	JDELETE( m_pMap);
}

template < typename T, int acount, bool bThread > void j_Hash< T, acount, bThread >::Close()
{
	for( DWORD i = 0; i < m_aCount; ++i )
	{
		m_pMap[i].m_pListNode.Close();		
	}
	m_dwCount = 0;
	m_pPtrList.Close();
}

template < typename T, int acount, bool bThread > DWORD j_Hash< T, acount, bThread >::Size() const
{
	return m_dwCount;
}

template < typename T, int acount, bool bThread > bool j_Hash< T, acount, bThread >::IsEmpty()
{
	if ( m_dwCount == 0 )
	{
		return true;
	}
	return false;
}

template < typename T, int acount, bool bThread > T* j_Hash< T, acount, bThread >::AddElement( char* pKey )
{
	T* pObject = GetElement( pKey );
	if( pObject != NULL )
		return NULL;

	DWORD _key = GetKeyValue( pKey );

	j_DataNode* pNode = m_pMap[_key].m_pListNode.AddElement();

	//memcpy( pNode->m_KeyString, pKey, MAX_PATH );		//容易引起拷贝越界崩溃
	sprintf_s( pNode->m_KeyString, "%s",pKey );

	T** ppListData = m_pPtrList.AddElement();
	pNode->m_ppListHandle = ppListData;
	*ppListData = &pNode->data;

	m_dwCount++;
	return &pNode->data;
}
template < typename T, int acount, bool bThread > T* j_Hash< T, acount, bThread >::AddElement( DWORD dwKey )
{
	T* pObject = GetElement( dwKey );
	if( pObject != NULL )
		return NULL;
	DWORD _key = dwKey % m_aCount;

	j_DataNode* pNode = m_pMap[_key].m_pListNode.AddElement();

	*(DWORD*)(pNode->m_KeyString) = dwKey;

	T** ppListData = m_pPtrList.AddElement();
	pNode->m_ppListHandle = ppListData;
	*ppListData = &pNode->data;
	m_dwCount++;
	return &pNode->data;
}

template < typename T, int acount, bool bThread > bool j_Hash< T, acount, bThread >::DeleteElement( char* pKey )
{
	T* pObject = GetElement( pKey );
	if( pObject == NULL )
		return false;
	DWORD _key = GetKeyValue( pKey );

	m_pPtrList.DeleteElement( ((j_DataNode*)pObject)->m_ppListHandle );

	m_pMap[_key].m_pListNode.DeleteElement( (j_DataNode*)pObject );
	m_dwCount--;
	return true;
}
template < typename T, int acount, bool bThread > bool j_Hash< T, acount, bThread >::DeleteElement( DWORD dwKey )
{
	T* pObject = GetElement( dwKey );
	if( pObject == NULL )
		return false;
	DWORD _key = dwKey % m_aCount;

	m_pPtrList.DeleteElement( ((j_DataNode*)pObject)->m_ppListHandle );

	m_pMap[_key].m_pListNode.DeleteElement( (j_DataNode*)pObject );
	m_dwCount--;
	return true;
}
template < typename T, int acount, bool bThread > T* j_Hash< T, acount, bThread >::GetElement( char* pKey )
{
	return GetElement( pKey, Int2Type<bThread>() );
}
template < typename T, int acount, bool bThread > T* j_Hash< T, acount, bThread >::GetElement( DWORD dwKey )
{
	return GetElement( dwKey, Int2Type<bThread>() );
}
template < typename T, int acount, bool bThread > T* j_Hash< T, acount, bThread >::GetElement( char* pKey, Int2Type< false > )
{
	DWORD _key = GetKeyValue( pKey );

	j_DataNode* pBegin = m_pMap[_key].m_pListNode.Begin();
	for( ; pBegin != NULL; pBegin = m_pMap[_key].m_pListNode.GetNextValue( pBegin ) )
	{
		if( !strcmp( pBegin->m_KeyString, pKey ) )
		{
			return &pBegin->data;
		}
	}
	return NULL;
}
template < typename T, int acount, bool bThread > T* j_Hash< T, acount, bThread >::GetElement( DWORD dwKey, Int2Type< false > )
{
	DWORD _key = dwKey % m_aCount;

	j_DataNode* pBegin = m_pMap[_key].m_pListNode.Begin();
	for( ; pBegin != NULL; pBegin = m_pMap[_key].m_pListNode.GetNextValue( pBegin ) )
	{
		if( *(DWORD*)(pBegin->m_KeyString) == dwKey )
		{
			return &pBegin->data;
		}
	}
	return NULL;
}
template < typename T, int acount, bool bThread > T* j_Hash< T, acount, bThread >::GetElement( char* pKey, Int2Type< true >  )
{
	DWORD _key = GetKeyValue( pKey );

	j_DataNode* pBegin = m_pMap[_key].m_pListNode.Begin();
	m_pMap[_key].m_pListNode.Lock();
	for( ; pBegin != NULL; pBegin = m_pMap[_key].m_pListNode.GetNextValue( pBegin ) )
	{
		if( !strcmp( pBegin->m_KeyString, pKey ) )
		{
			return &pBegin->data;
		}
	}
	m_pMap[_key].m_pListNode.UnLock();
	return NULL;
}

template < typename T, int acount, bool bThread > T* j_Hash< T, acount, bThread >::GetElement( DWORD dwKey, Int2Type< true >  )
{
	DWORD _key = dwKey % m_aCount;

	j_DataNode* pBegin = m_pMap[_key].m_pListNode.Begin();
	m_pMap[_key].m_pListNode.Lock();
	for( ; pBegin != NULL; pBegin = m_pMap[_key].m_pListNode.GetNextValue( pBegin ) )
	{
		if( *(DWORD*)(pBegin->m_KeyString) == dwKey )
		{
			return &pBegin->data;
		}
	}
	m_pMap[_key].m_pListNode.UnLock();
	return NULL;
}
template < typename T, int acount, bool bThread > DWORD j_Hash< T, acount, bThread >::GetKeyValue( char* str )
{
	int iLen = (int)strlen(str);
	DWORD key = 0;
	for( int i = iLen - 1; i >=0 && i > iLen - 6; --i )
	{
		key += str[i];
	}
	key = key % m_aCount;
	return key;
}



template< typename T, int acount, bool bThread > T** j_Hash< T,acount,bThread >::Begin()
{
	return m_pPtrList.Begin();
}

template< typename T, int acount, bool bThread > T** j_Hash< T,acount,bThread >::End()
{
	return m_pPtrList->End();
}

template< typename T, int acount, bool bThread > T** j_Hash< T,acount,bThread >::GetNextValue( T** pValue )
{
	return m_pPtrList.GetNextValue( pValue );
}

template< typename T, int acount, bool bThread > T** j_Hash< T,acount,bThread >::GetPrevValue( T** pValue )
{
	return m_pPtrList.GetPrevValue( pValue );
}
