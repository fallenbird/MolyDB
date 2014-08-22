
#ifndef _JK_DUMPER_H9O1UTA_H_
#define _JK_DUMPER_H9O1UTA_H_


class Dumper
{
public:
	Dumper();
	~Dumper();

	static unsigned int DumpThread();

	int LoadDumpFile( char* srcFileName );
	int DumpToFile(char* dstFileName);

private:
	static int	m_iAccuTime;
};

#endif