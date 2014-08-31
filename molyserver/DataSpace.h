
#ifndef _JK_DATASPACE_FVX4UXK_H_
#define _JK_DATASPACE_FVX4UXK_H_

#include "JK_Dictionary.h"
#include "JK_Singleton.h"

class DataSpace : public JK_Singleton<DataSpace>
{
public:
	DataSpace();
	~DataSpace();

	bool	InitDB();
	void	UpdateDB( int iUpdateMS );
	bool	InsertKV( char* key, int keylen, char* val, int valen );
	void*	GetValue( void* key );
	bool	RemoveKV( void* key );
	void	Operation( int cmd, void* key, void* val, char* opt );



private:
	JK_Dictionary m_normalDict;
	JK_Dictionary m_expireDict;
};


#endif