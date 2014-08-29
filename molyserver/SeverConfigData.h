
#ifndef _JK_SERVERCONFIG_SXF13690_H_
#define _JK_SERVERCONFIG_SXF13690_H_

#include "JK_LockFreeQueue.h"

class SeverConfigData 
{
public:
	static bool InitServerConfig( void* dataptr );
	

private:
	static bool m_bIsMaster;

};

#endif