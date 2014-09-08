
#ifndef _JK_DATASPACE_FVX4UXK_H_
#define _JK_DATASPACE_FVX4UXK_H_

#include "JK_Dictionary.h"
#include "JK_Singleton.h"

class DataSpace : public JK_Singleton<DataSpace>
{
public:
	DataSpace();
	~DataSpace();

	bool	InitDB( bool bSlave );
	void	UpdateDB( int iUpdateMS );
	bool	InsertKV( char* key, int keylen, char* val, int valen, bool ops = true );
	void*	GetValue( void* key );
	bool	RemoveKV( void* key, bool ops = true );
	void	Operation( int cmd, void* key, void* val );
	void	Replication( int cmd, void* key, void* val );


	void	SetRepState( bool bRep ){ m_bReplicated = bRep; }
	bool	IsServerReady();


	// commands
	void	FetchKeys( char* fmt, int istart, int ilimit, char** keysarray, int& kescnt );

private:
	JK_Dictionary<true>	m_normalDict;
	JK_Dictionary<true>	m_expireDict;
	bool				m_bSlave;
	bool				m_bReplicated;
};


#endif