
#ifndef _SLAVEAGENT_J1BXE9T_H_
#define _SLAVEAGENT_J1BXE9T_H_


#include "NetworkObject.h"

class SlaveAgent : public NetworkObject
{
public:
private:

protected:
	virtual void	OnAccept(DWORD connindex);							// --有新的连接连进来了
	virtual void	OnDisconnect();
	virtual	void	OnRecv(BYTE *pMsg, WORD wSize);						// --成功读取了一条完整消息
	virtual void	OnConnect(BOOL bSuccess, DWORD dwNetworkIndex);

};
#endif