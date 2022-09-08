#ifndef tag_CShotPar
#define tag_CShotPar

#include "SvSys.h"
#include "Project.h"

class  SShotPar:public CObject
{
public:
	long m_lPh;
	int m_iFileNo;
	int m_iZP;
	int m_iHP;
	int m_iGroupNumSmall;
	int m_iGapSmall;
	int m_iGroupNumBig;
	int m_iGapBig;
};

class CShotPar
{
public:
	CShotPar();
	CShotPar(CString sFile);
	~CShotPar();
	bool Use(CString  );
	int Add(SShotPar* pShotPar);

	long GetNum();	 
	bool IsEmpty();

	CObArray m_array;
	bool Save(CString sFile="");
	SShotPar *GetAt(long nIndex){return (SShotPar *)(m_array.GetAt(nIndex));}
	long GetPhOnRecieveNo(long ShotPh,int RecieveNo,CSvSys&SvSys);
	long GetRecieveNoOnPh(long ShotPh,long RecievePh,long& TotalRecieveNum,CSvSys&SvSys);
};

#endif