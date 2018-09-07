
#ifndef _JK_DATASPACE_FVX4UXK_H_
#define _JK_DATASPACE_FVX4UXK_H_

#include "JK_Dictionary.h"
#include "JK_Singleton.h"
#include "JK_PriorityQueue.h"


class DataSpace : public JK_Singleton<DataSpace>
{
public:
	DataSpace();
	~DataSpace();

	bool	InitDB( bool bSlave );
	void	UpdateDB( int iUpdateMS );
	bool	InsertKV( char* key, int keylen, char* val, int vallen, bool ops = true );
	bool	UpdateKV(void* key, void* val, int vallen, bool ops = true);
	bool	RemoveKV(void* key, bool ops = true);
	void*	GetValue( void* key );
	void	Operation( int cmd, void* key, void* val );
	void	Replication( int cmd, void* key, void* val );
	bool	IsExists( char* key );


	bool	ListPushLeft ( char* key, int keylen, char* val, int vallen, bool ops = true );
	bool	ListPushRight( char* key, int keylen, char* val, int vallen, bool ops = true );
	void*	ListPopLeft	 ( char* key, bool ops = true );
	void*	ListPopRight ( char* key, bool ops = true );
	int		GetListLength( char* key );
	bool	ExpireKey(char* key, int seconds);


	// ----HASH----
	bool	HashSet( char* key, int seconds );

	void	SetRepState( bool bRep ){ m_bReplicated = bRep; }
	bool	IsServerReady();


	// commands
	void	FetchKeys( char* fmt, int istart, int ilimit, char keysarray[1024][MAX_KEY_LEN], int& kescnt );

private:
	JK_Dictionary<true>	m_normalDict;
	JK_PriorityQueue	m_expireQueue;

	//JK_Hashmap<int, true>  m_expireMap;
	bool				m_bSlave;
	bool				m_bReplicated;
};


#endif