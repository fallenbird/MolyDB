#pragma once



class CommandMgr
{
public:
	CommandMgr();
	~CommandMgr();

	bool	SaveData(char* pData, int iLen);
	bool	IsHavdData();
	bool	GetData(char* pData, int& iLen);

	void	SaveSendData(char* pData, int iLen);

	char	m_strTmep[256];
	bool	m_bConnect;
	bool	m_bIsClosed;
	int		m_iID;

};