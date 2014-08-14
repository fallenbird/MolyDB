
#ifndef _JK_DATASPACE_FVX4UXK_H_
#define _JK_DATASPACE_FVX4UXK_H_


#include "JK_Dictionary.h"

class DataSpace
{
public:
	DataSpace();
	~DataSpace();


private:
	JK_Dictionary m_normalDict;
	JK_Dictionary m_expireDict;
};


#endif