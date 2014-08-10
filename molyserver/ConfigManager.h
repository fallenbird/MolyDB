

/*										CConfigManager.h
==========================================================================================================
Module       : 
Related		 :		CConfigManager.cpp
Author       :		Sun Xiongfei
Version      :		1.0
Intro        :		
Notes        : 
-----------------------------------------------------------------------------------------------------------
Change History :
Date				Version				Changed By					Changes 

2008/01/08			1.0					Sun Xiongfei				Create
==========================================================================================================
*Copyright(c) Jake Sun 2010--2015, All rights reserved
==========================================================================================================*/
#ifndef _CCONFIGMANAGER_H_
#define _CCONFIGMANAGER_H_

#include <fstream>
#include <iostream>
#include "JK_Singleton.h"

#define MAX_LINE_LENGTH 260
using namespace std;

/*---------------------------------------------------------------------------------------------------------
Class		 :							CConfigManager
Desc		 : 
Exception	 : 
Notes		 : 
-----------------------------------------------------------------------------------------------------------
Author		 :							Sun Xiongfei
----------------------------------------------------------------------------------------------------------*/
class ConfigManager : public JK_Singleton<ConfigManager>
{

	//constructor & disconstructor
public:
	ConfigManager();
	~ConfigManager();

	//property
private:
	fstream m_ConfigFile;
	bool	m_bIsFileOpen;
	enum
	{
		IS_COMMENT,    //comment
		IS_SECTION,    //section
		IS_VALUE,      //value of variable
		NOT_FOUND      //Not found assigned variable
	};

	//method
private:
	bool			GetSection(char *pStr,char *SectionName);				//Get the assigned section
	int				GetContent(char *pStr,char *ValueName,char *RetStr);	//Get the content of assigned variable

public:
	bool			OpenConfigFile(char *ConfigFileName);					//Open the assigned configuration file
	void			CloseConfigFile();										//Close the opened file
	void			SaveConfigFile();										//Save current config to file
	void			RecoverDefaultConfig();									//Recover current config as default cofig
	int				GetValue(char *Section,char *ValueName,char *RetStr);	//Get variable from the opened file,according to the section & value name
	bool			GetFileState();											//Get file operating state(open or not)
};

#endif