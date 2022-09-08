// Project.h: interface for the CProject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROJECT_H__CDA51563_554C_11D3_96D8_00C04FCCB957__INCLUDED_)
#define AFX_PROJECT_H__CDA51563_554C_11D3_96D8_00C04FCCB957__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum ELineFile
{
	LineName,
	ShotPar,
	ShotRcv,
	Survey,
	SCData,
	Float,
	Unform,
	FstBrk,
	Trace,
	EndPoint,
	WellDepth
};

class CLineFile:public CObject
{
public :
	CString m_sLineName;
	CString m_sShotPar;
	CString m_sShotRcv;
	CString m_sSurvey;
	CString m_sSCData;
	CString m_sFloat;
	CString m_sUnform;
	CString m_sFstBrk;
	CString m_sTrace;
	CString m_sEndPoint;
	CString m_sWellDepth;
	long m_nValidBeginPh;
	long m_nValidEndPh;
	CSize m_sizeGridMerge;
	int m_nGridSurface;

	CLineFile(){
		m_nValidBeginPh=0;
		m_nValidEndPh=0;
		m_sizeGridMerge=CSize(1,1);
		m_nGridSurface=2;
	}
};

class CProject : public CObject  
{
private:
	CString m_sProjectName;
	CObArray m_array;
	int m_nCurLinePos;

public:
	CProject();
	virtual ~CProject();
	void Create(CString sFile);
	bool Open(CString sProjectName);
	bool Save(CString sProjectName="");
	void Reset();

	
	bool OpenLine(CString sLineName);
	bool SaveLine();

	int AddLine(CLineFile* pFile);
	bool RemoveLine(CString sLineName);

	bool AddFile(ELineFile nType, CString sFile);
	bool RemoveFile(ELineFile nType);

	CString m_sSvSys;

	CString m_sLineName;
	CString m_sShotPar;
	CString m_sShotRcv;
	CString m_sSurvey;
	CString m_sSCData;
	CString m_sFloat;   
	CString m_sUnform;
	CString m_sFstBrk;
	CString m_sTrace;
	CString m_sEndPoint;  // Containning the two end points of the trace.
	CString m_sWellDepth;
	long m_nValidBeginPh;  // this is the shot poiint Ph that firstly valid.
	long m_nValidEndPh;     // this is the shot poiint Ph that lastly valid.
	CSize m_sizeGridMerge;
	int m_nGridSurface;		  // 
};

#endif // !defined(AFX_PROJECT_H__CDA51563_554C_11D3_96D8_00C04FCCB957__INCLUDED_)
