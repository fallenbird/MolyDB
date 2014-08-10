#include "ConfigManager.h"
#include <string.h>


//--------------------------------------------------------------------------
// Prototype	:		CConfigManager::CConfigManager()
// Fuction		:		CConfigManager constructor
// Paras[in]	:		- 
// Paras[out]	:		-
// Return		:		-
// Notes		:		- 
//---------------------------------------------------------------------------
ConfigManager::ConfigManager()
{
	m_bIsFileOpen = false;
}


//---------------------------------------------------------------------------
// Prototype	:		CConfigManager::~CConfigManager()
// Desc			:		CConfigManager disconstructor
// Paras[in]	:		- 
// Paras[out]	:		-
// Return		:		-
// Notes		:		- 
//---------------------------------------------------------------------------
ConfigManager::~ConfigManager()
{
	if( m_bIsFileOpen )
	{
		m_ConfigFile.close();
		m_bIsFileOpen = false;
	}
}


//-------------------------------------------------------------------------------
// Prototype	:		bool CConfigManager::OpenConfigFile(char *ConfigFileName)
// Desc			:		Open the assigned configuration file
// Paras[in]	:		char *ConfigFileName (the config file name) 
// Paras[out]	:		-
// Return		:		true, (open file success)
//						false (open file failed)
// Notes		:		
//--------------------------------------------------------------------------------
bool ConfigManager::OpenConfigFile(char *ConfigFileName)
{
	//----if some config file is open, colse it------
	if( m_bIsFileOpen )
	{
		m_ConfigFile.close();
		m_bIsFileOpen = false;
	}
	m_ConfigFile.open( ConfigFileName, ios::in);//|ios::out
	if( !m_ConfigFile )
	{
		return false;
	}
	m_bIsFileOpen = true;
	return true;
}


//-------------------------------------------------------------------------------
// Prototype	:		void CConfigManager::CloseConfigFile()
// Desc			:		close config file
// Paras[in]	:		-
// Paras[out]	:		-
// Return		:		-
// Notes		:		-
//--------------------------------------------------------------------------------
void ConfigManager::CloseConfigFile()
{
	if(m_bIsFileOpen)
	{
		m_ConfigFile.close();
		m_bIsFileOpen = false;
	}
}


//-------------------------------------------------------------------------------
// Prototype	:		void CConfigManager::SaveConfigFile()
// Desc			:		save current setting to config file
// Paras[in]	:		-
// Paras[out]	:		-
// Return		:		-
// Notes		:		-
//--------------------------------------------------------------------------------
void ConfigManager::SaveConfigFile()
{

}


//-------------------------------------------------------------------------------
// Prototype	:		void CConfigManager::RecoverDefaultConfig()
// Desc			:		recover current setting to default
// Paras[in]	:		-
// Paras[out]	:		-
// Return		:		-
// Notes		:		-
//--------------------------------------------------------------------------------
void ConfigManager::RecoverDefaultConfig()
{
}


//-------------------------------------------------------------------------------
// Prototype	:		bool CConfigManager::GetFileState()
// Desc			:		Get the state of current file operation
// Paras[in]	:		-
// Paras[out]	:		-
// Return		:		true, (there is a open file)
//						false (No open file)
// Notes		:		
//--------------------------------------------------------------------------------
bool ConfigManager::GetFileState()
{
	return m_bIsFileOpen;
}


//------------------------------------------------------------------------------------------------
// Prototype	:		int CConfigManager::GetValue(char *Section, char *ValueName, char *RetStr)
// Desc			:		Get value of the assigned from the config file
// Paras[in]	:		char *Section	(name of section)
//						char *ValueName (name of value)
// Paras[out]	:		char *RetStr	(the pointer of return characters)
// Return		:		int
// Notes		:		-
//------------------------------------------------------------------------------------------------
int ConfigManager::GetValue(char *section, char *valueName, char *retStr)
{
	char	tmpstr[MAX_LINE_LENGTH];
	short	Flag;
	short	Ret;

	m_ConfigFile.seekg(0,ios::beg);
	//memset( tmpstr, 0, sizeof( tmpstr ) );
	//-----Get the assigned section------------
	Flag = -1;
	while(!m_ConfigFile.eof())
	{
		m_ConfigFile.getline(tmpstr,MAX_LINE_LENGTH);
		if ( GetSection(tmpstr,section) )
		{
			Flag = 0;
			break;
		}
	}

	if ( Flag == -1 )
	{
		return -1;
	}

	//------Get the value of variable-------
	Flag = -1;
	while(!m_ConfigFile.eof())
	{
		m_ConfigFile.getline(tmpstr,MAX_LINE_LENGTH);
		Ret = GetContent(tmpstr, valueName, retStr);
		if (  (Ret == IS_COMMENT) || (Ret == NOT_FOUND))
		{
			continue;
		}
		else if( Ret == IS_SECTION )					//run to the next section
		{ 
			break;
		}
		Flag = 0;
		//cerr << Session << " " << ValueName << "=" << RetStr << endl;
		break;
	}
	return Flag;
}


//------------------------------------------------------------------------------------------------
// Prototype	:		bool CConfigManager::GetSection(char *pStr, char *pSectionName)
// Desc			:		Get the section we need from characters
// Paras[in]	:		char *pStr
//						*pSectionName
// Paras[out]	:		-
// Return		:		true	(got the section)
//						false	(failed to get the section)
// Notes		:		-
//------------------------------------------------------------------------------------------------
bool ConfigManager::GetSection(char *pStr, char *pSectionName)
{
	char TmpStr[100];
	int i=0;
	int j = 0;

	while( pStr[i] == ' ' )
	{
		i++;											//ignore space sign	
	}

	if( pStr[i] != '[' ) 
	{
		return false;									//It's not the name of section
	}

	i++;												//ignore '[' sign
	while( pStr[i] == ' ' )
	{
		i++;											//ignore space sign
	}

	//-----get the assigned section-----
	while( pStr[i] != ' ' && pStr[i] != ']' )
	{
		TmpStr[j] = pStr[i];
		i++;
		j++;
	}
	TmpStr[j] = 0;

	if( strcmp(TmpStr, pSectionName) != 0 )
	{
		return false;									//Not the assigned section
	}
	return true;

}


//------------------------------------------------------------------------------------------------
// Prototype	:		int CConfigManager::GetContent(char *pStr, char *ValueName, char *RetStr)
// Desc			:		Get content we need from the assigned characters
// Paras[in]	:		char *pStr		(pointer to assigned characters)
//						char *ValueName (name of value)
// Paras[out]	:		char *RetStr	(the pointer of return characters)
// Return		:		IS_VALUE	(value of variable)
//						IS_COMMENT	(comment)
//						IS_SECTION	(section)
//						NOT_FOUND   (not found assigend variable)
// Notes		:		-
//------------------------------------------------------------------------------------------------
int ConfigManager::GetContent(char *pStr, char *ValueName, char *RetStr)
{
	char TmpStr[100];
	int i=0;
	int j = 0;
	RetStr[0] = 0;

	while( pStr[i] == ' ' )
	{
		i++;											//ignore space sign
	}
	if( pStr[i] == '#' ) 
	{
		return IS_COMMENT;								//comment
	}
	if( pStr[i] == '[' )
	{
		return IS_SECTION;								//section
	}
	if( pStr[i] == 0   ) 
	{
		return NOT_FOUND;								//not found variable,read to the end of line
	}				

	//-------get the name of variable----------
	while( pStr[i] != ' '&& pStr[i]!='\t' && pStr[i] != '='&& pStr[i] != 0)
	{
		TmpStr[j] = pStr[i];
		i++;
		j++;
	}
	TmpStr[j] = 0;
	if( strcmp(TmpStr, ValueName) != 0 ) 
	{
		return NOT_FOUND;								//Not the assigned variable
	}
	while( pStr[i] == ' ' || pStr[i]=='\t' || pStr[i] == '=' )
	{ 
		i++;											//ignore space£¬table and '='sign
	}

	//-----get the value of variable-------------
	j=0;
	while( pStr[i] != ' ' && pStr[i] != '#' && pStr[i] != 127 )	//while( pStr[i] >;' ' &amp;&amp; pStr[i] != '#' &amp;&amp; pStr[i] != 127)
	{
		RetStr[j] = pStr[i];
		i++;
		j++;
	}
	RetStr[j] = 0;
	return IS_VALUE;
}