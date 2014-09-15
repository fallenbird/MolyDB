
#ifndef _JK_DATAOBJECT_871RHNN_H_
#define _JK_DATAOBJECT_871RHNN_H_


#define MAX_ARG_CNT 8

class DataObject
{
public:

	unsigned char	datatype;
	unsigned char	encoding;
	unsigned short	lur;
	void*			dataptr;
	
};


#endif