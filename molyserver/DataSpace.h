
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
	bool	InsertKV( void* key, void* val );
	void*	GetValue( void* key );
	bool	RemoveKV( void* key );


private:
	JK_Dictionary m_normalDict;
	JK_Dictionary m_expireDict;
};


#endif