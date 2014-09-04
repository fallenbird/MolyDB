#include "SlaveMgr.h"
#include "JK_Assert.h"

void SlaveMgr::InsertSlave( ClientAgent* pAgent )
{
	JK_ASSERT(pAgent);
	m_SlaveVector.push_back( pAgent );
}



void SlaveMgr::RemoveSlave( ClientAgent* pAgent )
{

}


void SlaveMgr::BroadAllSlave( BYTE* pMsg, unsigned short usSize )
{
	ClientAgent* ptmpAgent = NULL;
	vector<ClientAgent*>::iterator itr = m_SlaveVector.begin();
	for ( ; itr != m_SlaveVector.end(); ++itr )
	{
		ptmpAgent = *itr;
		ptmpAgent->Send( pMsg, usSize );
	}
}
