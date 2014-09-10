#ifndef _CLIENTAGENT_U53XE9J_H_
#define _CLIENTAGENT_U53XE9J_H_


#include "NetworkObject.h"


class ClientAgent : public NetworkObject
{
public:
	ClientAgent();
	~ClientAgent();


protected:
	virtual void	OnAccept(DWORD connindex);							// --有新的连接连进来了
	virtual void	OnDisconnect();
	virtual	void	OnRecv(BYTE *pMsg, WORD wSize);						// --成功读取了一条完整消息
	virtual void	OnConnect(BOOL bSuccess, DWORD dwNetworkIndex);

	

private:
	int				m_iAgentType;

	inline bool		CheckSvrReady(); 

};


#endif