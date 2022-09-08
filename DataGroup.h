// DataGroup.h: interface for the CDataGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATAGROUP_H__CC332025_4BDB_11D3_96D7_00C04FCCB957__INCLUDED_)
#define AFX_DATAGROUP_H__CC332025_4BDB_11D3_96D7_00C04FCCB957__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDataGroup  
{
public:
	bool Reset();
	CDataGroup();
	virtual ~CDataGroup();

	CDataGroup(int sizeElement, int nSubscript1,int nSubscript2=0,int nSubscript3=0,int nSubscript4=0);
	bool New(int sizeElement, int nSubscript1,int nSubscript2=0,int nSubscript3=0,int nSubscript4=0);
	void * Set(void *,int nSubscript1,int nSubscript2=-99999,int nSubscript3=-99999,int nSubscript4=-99999);
	void * Get(int nSubscript1,int nSubscript2=-99999,int nSubscript3=-99999,int nSubscript4=-99999);		

	bool IsEmpty();
	bool Fill(void* src);
	long Check(long nSub1,long nSub2=-99999,long nSub3=-99999,long nSub4=-99999);
	int GetSubscriptNum(){return m_nTotalSubscript;};
	int GetSubscript(int n);

	
private:
	long m_nTotalSubscript;
	long m_nSubscript1;
	long m_nSubscript2;
	long m_nSubscript3;
	long m_nSubscript4;

	long m_nEle21;
	long m_nEle321;
	
	 long m_sizeElement;
	 long m_nTotalSpace;
	char *m_pData;

	long m_nErrorData;
};

#endif // !defined(AFX_DATAGROUP_H__CC332025_4BDB_11D3_96D7_00C04FCCB957__INCLUDED_)
