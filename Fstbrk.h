
////////////////////////////////////////////////////////////////////////////////////////
//  每一个初至文件对应一个解编文件

#include "SvSys.h"

#ifndef fbk_h
#define fbk_h

struct SFbkIndex  :public CObject
{
	long m_lFileNo;
	long m_lFilePos;
};

struct SFstBrkRecord{
	long m_lFileNo;		//4 bytes, from 0 to 65535. enough.			   
	int m_iTraceNo;
	float m_fFstBrk;  
};	

class CFstBrk{

private:

	FILE *m_fpFstBrk;
	CObArray m_arrayIndex;

	long m_lShotNum;
	bool MakeIndex(CSvSys &SvSys);
	bool Reset();
	
public:
	CFstBrk();
	CFstBrk(CString UnformFile,CSvSys& SvSys);
	~CFstBrk();

	bool Open(CString UnfromFile,CSvSys& SvSys);
	bool IsEmpty();
	long GetShotNum();
	long Get(SFstBrkRecord*,long pos,long n=1);
	long Put(SFstBrkRecord*,long pos,long n=1);
	SFbkIndex* GetIndex(long nShot);
};

#endif


