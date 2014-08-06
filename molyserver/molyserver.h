// Sundis.cpp : Defines the entry point for the console application.
//
#include "./Server.h"
#include "NetIntface.h"
#include "ConfigManager.h"

typedef struct ServerInof
{

}_ServerInof;


enum eRunningSta
{
	ers_NONE = 0,
	ers_INIT = 1,
	ers_RUNNING = 2,
	ers_STOP = 3,
};


typedef struct 
{

	char*	ServerlIP;
	int		Serverport;
}ServerInfo;



class MolyServer
{
public:
	void InitServer();
	void RunServer();
	void StopServer();
	int GetState();


private:
	NetInterface	m_NetInterface;
	ConfigManager	m_ConfigManger;
	char*			m_serverIP;
	int				m_serverPort;
	int				m_runningSta;
};

