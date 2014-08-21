
#include <stdio.h>
#include "Dumper.h"
#include "JK_Utility.h"
#include "LogManager.h"



Dumper::Dumper()
{

}

Dumper::~Dumper()
{

}

int Dumper::LoadDumpFile(char* srcFileName)
{

	
	//char magic[10];
	//int j;
	//long long now = mstime();
	//rio rdb;
	//uint64_t cksum;

	char tmpfile[256];
	JK_SPRITF_S(tmpfile, 256, "molydb_df-%d.dmf", (int)JK_GETPID() );
	FILE *fp = NULL;
	JK_OPENFILE_S( fp,srcFileName,"w");
	if (!fp) 
	{
		MOLYLOG(MOLY_LOG_ERORR, "Failed opening dump file, errno: %d!", errno);
		return 1;
	}

	

	return 0;
}

int Dumper::DumpToFile(char* dstFileName)
{
	return 0;
}
