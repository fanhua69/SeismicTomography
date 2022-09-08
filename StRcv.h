
#ifndef tag_ShotRecieve
#define tag_ShotRecieve

#include "Project.h"
#include "ShotPar.h"


class SShotRecieve:public CObject
{
public:
	long ShotPh;
	int nBeginGroup[100];
	int nEndGroup[100];

	SShotRecieve()
	{
		ShotPh=0;
		for(int i=0;i<100;i++){
			nBeginGroup[i]=-1;
			nEndGroup[i]=-1;
		}
	}
};

class CShotRecieve
{
private:
	CObArray m_array;
	int m_nSectionLimit;

public:	
	bool Make(CProject &pro);
	CShotRecieve();
	CShotRecieve(CString sFile,CShotPar& ShotPar,CSvSys &SvSys);
	~CShotRecieve();
	bool IsEmpty();
	void Reset();

	bool Open(CString sFile,CShotPar& ShotPar,CSvSys &SvSys);

	int GetSize();
	SShotRecieve* Get(int nIndex);

	int Add(SShotRecieve *);
	bool Save(CString);	
	
};

#endif tag_ShotRecieve