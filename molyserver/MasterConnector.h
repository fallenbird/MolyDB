
#ifndef _MASTERCON_EBX2ET3_H_
#define _MASTERCON_EBX2ET3_H_


#include "NetworkObject.h"

class  MasterConnector: public NetworkObject
{

public:

private:
	virtual void	OnAccept(DWORD connindex);							// --有新的连接连进来了
	virtual void	OnDisconnect();
	virtual	void	OnRecv(BYTE *pMsg, WORD wSize);						// --成功读取了一条完整消息
	virtual void	OnConnect(BOOL bSuccess, DWORD dwNetworkIndex);

};
#endif