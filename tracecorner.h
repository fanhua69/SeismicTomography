// Trace.h: interface for the CTrace class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRACE_H__734D6353_3B6A_11D3_96D6_00C04FCCB957__INCLUDED_)
#define AFX_TRACE_H__734D6353_3B6A_11D3_96D6_00C04FCCB957__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyPoint.h"
#include "DataGroup.h"

class CTrace  
{
public:
	long GetGridNo(int nx,int ny);
	bool IsGridEmpty(int i,int j);
	bool IsEmpty();
	CObArray& GetRoute(CMyPoint &pointDest);
	bool Reset();
	CTrace();
	virtual ~CTrace();

	long  m_nGridXNumber;
	long  m_nGridYNumber;
	int m_nGridWidth;
	int m_nGridHeight;

	
	bool Set(int nGridX,int nGridY, int nGridWidth,int nGridHeight,CSize sizeScan);
	bool SetVelocity(double *pVelocity,long nCol,long nRow);
	bool SetSlow(double *pSlow,long nCol,long nRow);
	bool SetEmptySlow(double dEmptySlow);
	bool Explode(CMyPoint &pSrc,CDC *pDC=NULL);
	CObArray& GetStraightRoute(CMyPoint p1,CMyPoint p2,double& dStraightTime);

	CDataGroup m_dgGridSlow;
	CDataGroup m_dgByteGridFlag; // 0 , NOT reached, 1 Passed, 2 Just reached.
	CDataGroup m_dgMyPointFrom; // Indicate which point is the wave from.
	CDataGroup m_dgDoubleTime;     // Record the traveled time .
	CDataGroup m_dgBoolFlag;     // Indicate if the grid will be scaned . 1, scan , 0 NOT scan.
	CDataGroup m_dgObRoute;         // Save the route in a Scanning Grid that a ray passed.

protected:
	
	CObArray m_arrayWaveFront;	     // temporily save the wave front whose flag are set to 2.
	CObArray m_arrayTrace;           // for returning the trace that from the source point to the target recieve point.
	CObArray m_arrayStraightTrace; // for returning the straight trace from the source point to the recieve point.	

	CMyPoint m_pointSource;
	CSize m_sizeScan;
	double m_dEmptySlow;
	double CalcTime(CMyPoint &pointSrc,CMyPoint &pointDest);
	bool ScanGrid(CMyPoint &pointLastSrc,CMyPoint& pointNewSrc,CMyPoint &pointMinTime);
	bool MakeFlags();
	
	
};


class CGridRoute :public CObject
{
public:
	double nx;   // should be long when using for normal route in this class, but 
	double ny;  // we have to let them be double because we want to use it in GetStraightRoute.
	double dLen;
	CGridRoute(double x,double y,double dL){
		nx=x;
		ny=y;
		dLen=dL;
	}
};

#endif // !defined(AFX_TRACE_H__734D6353_3B6A_11D3_96D6_00C04FCCB957__INCLUDED_)
