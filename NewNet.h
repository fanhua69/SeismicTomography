// RectNet.h: interface for the CNewNet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECTNET_H__E7FEE6E2_F70E_11D1_98EA_3C4504C10000__INCLUDED_)
#define AFX_RECTNET_H__E7FEE6E2_F70E_11D1_98EA_3C4504C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif 

#include "MyPoint.h"

class CNewNet:public CObject
{
public:
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Used to contain the grids that a trace passed, 
	// It contains CMyPoint object, in which , the x value is for the grid number, 
	// and the y value is for the length that the trace in this grid.
	// 1999.8.20
	CObArray m_arrayTrace;   
	CString m_fileSlowness;
	double *m_pdValue;

public:
	double * GetSlow();
	bool SetValue(double* pValue);
	CNewNet();
	virtual ~CNewNet();

	bool Create(CString SurveyFile,long nBeginPh=0,long nEndPh=0,int ColSpace=0,int RowSpace=0,int descendDepth=0);
	bool Use(CString SurveyFile);
	bool ReadValue();
	bool SaveValue();
	void Reset();
	
	CObArray& GetTrace(CMyPoint& p1,CMyPoint &p2);
	CMyPoint GetNetPoint(long n);
	CRect GetNetRect(long n);
	long GetNetNo(CMyPoint& );
	long GetNetNo(double x,double y);	
	long GetNetNo(long nx,long ny);
	CMyPoint ChgToNetPoint(double x,double y);
	bool PointInNet(CMyPoint& );	

	long GetXCor(long col);
	long GetYCor(long row);
	long GetNetNum(){return m_lNetNum;}
	long GetLowY(){return m_lLowY;}
	long GetHighY(){return m_lHighY;}
	long GetLeftX(){return m_lLeftX;}
	long GetRightX(){return m_lRightX;}
	long GetColNum(){ return m_lColNum;}
	long GetRowNum(){ return m_lRowNum;}
	long GetColSpacing(){ return m_nColSpacing;}
	long GetRowSpacing(){ return m_nRowSpacing;}
	bool InSameGrid(CMyPoint& point1, CMyPoint& point2);
	
public:
	bool ChgToOrgPoint(CMyPoint &point);
	bool ChgToNetPoint(CMyPoint &point);
	long m_lLeftX;
	long m_lLowY;
	long m_lRightX;
	long m_lHighY;

	long m_lColNum;
	long m_lRowNum;
	long m_lNetNum;
	int m_nRowSpacing;
	int m_nColSpacing;

	double m_dMaxValue;
	double m_dMinValue;

protected:
	bool OnYCor(CMyPoint p);
	bool OnXCor(CMyPoint p);

	double TIME_THRESHOLD;

};

#endif // !defined(AFX_RECTNET_H__E7FEE6E2_F70E_11D1_98EA_3C4504C10000__INCLUDED_)
