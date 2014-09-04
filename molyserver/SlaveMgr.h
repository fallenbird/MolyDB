
#ifndef _SLAVEMGR_J1BXE9T_H_
#define _SLAVEMGR_J1BXE9T_H_

#include "JK_Singleton.h"
#include "ClientAgent.h"
#include <vector>

using namespace std;

class SlaveMgr : public JK_Singleton< SlaveMgr >
{
public:
	void InsertSlave( ClientAgent* pAgent );
	void RemoveSlave( ClientAgent* pAgent );
	void BroadAllSlave( BYTE* pMsg, unsigned short usSize );

private:
	vector<ClientAgent*>	m_SlaveVector;

};
#endif