// Sundis.cpp : Defines the entry point for the console application.
//
#ifndef _JK_MOLYSERVER_YUX9U1P_H_
#define _JK_MOLYSERVER_YUX9U1P_H_



#include "./Server.h"
#include "NetIntface.h"
#include "DataSpace.h"
#include "JK_Console.h"
#include "LogManager.h"
#include "SlaveMgr.h"

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
	MolyServer();
	~MolyServer();
	bool InitServer( bool bSlave = false );
	void RunServer();
	void StopServer();
	int GetState();

private:
	NetInterface	m_NetInterface;
	JK_Console		m_ConsoleMgr;
	DataSpace		m_DataEngine;
	LogManager		m_LogManager;
	SlaveMgr		m_SlaveManager;

	char*			m_serverIP;
	int				m_serverPort;
	int				m_runningSta;
};


#endif