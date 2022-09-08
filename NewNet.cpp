// RectNet.cpp: implementation of the CNewNet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NewMount.h"
#include "NewNet.h"
#include "Equation.h"
#include "MyPoint.h"
#include "SvData.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNewNet::CNewNet()
{
	m_lLeftX=m_lLowY=m_lRightX=m_lHighY=0;

	m_lColNum=m_lRowNum=m_lNetNum=0;
	m_nRowSpacing=m_nColSpacing=0;
	
	m_pdValue=NULL;
	
	TIME_THRESHOLD=0.00000001;		
}
//
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//
CNewNet::~CNewNet()
{
	Reset();
}
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool CNewNet::Use(CString sSurveyFile)
{
	Reset();

	int n=sSurveyFile.Find('.');
	m_fileSlowness=sSurveyFile.Left(n)+".SLW";   // the file name of the slowness :
	return ReadValue();	
}
////////////////////////////////////////////////////////////////////////////////////////////////
//
void CNewNet::Reset()
{
	m_lLeftX=m_lLowY=m_lRightX=m_lHighY=0;

	m_lColNum=m_lRowNum=m_lNetNum=0;
	m_nRowSpacing=m_nColSpacing=0;

	m_arrayTrace.RemoveAll();
	
	if(m_pdValue){
		delete m_pdValue;
		m_pdValue=NULL;
	}
}
//
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//
bool CNewNet::Create(
		  CString sSurveyFile,
		  long PhBegin,
		  long PhEnd,
		  int ColSpacing,
		  int RowSpacing,
		  int descendDepth)
{
	//////////////////////////////////////////////////////////////////////////////////////////
	//
	int n=sSurveyFile.Find('.');
	m_fileSlowness=sSurveyFile.Left(n)+".SLW";   // the file name of the slowness :

	
	// 获得测量数据,并从中计算获得网格数据.
	CSvData sd;
	if(!sd.Use(sSurveyFile))return false;

	long nData=sd.GetNum();
	if(nData<2){
		AfxMessageBox(" The quantity of the survey data can NOT be less than 2!");
		return false;
	}

	// Calculate the  4 edges of the target NET:
	long x1,x2,y1,y2;
	if(PhBegin==0)
		x1=sd.GetAt(0)->Ph;
	else
		x1=PhBegin;

	if(PhEnd==0)
		x2=sd.GetAt(nData-1)->Ph;
	else
		x2=PhEnd;


	y1=10000000;
	y2=-10000000;

	SSvRecord *pRecord;
	for(long i=0;i<nData;i++){
		pRecord=sd.GetAt(i);
		if(pRecord->Ph>=x1&&pRecord->Ph<=x2){
			if(pRecord->HBG<y1)
				y1=pRecord->HBG;
			if(pRecord->HBG>y2)
				y2=pRecord->HBG;
		}
	}

	// The x, and y spacing of the target net:
	if(ColSpacing==0)
		m_nColSpacing=sd.GetAt(1)->Ph-sd.GetAt(0)->Ph;
	else
		m_nColSpacing=ColSpacing;

	if(RowSpacing==0)
		m_nRowSpacing=1;
	else
		m_nRowSpacing=RowSpacing;

	// 网格的四边坐标:
	if((x1%m_nColSpacing)==0)
		m_lLeftX=x1;
	else
		m_lLeftX=int(x1/m_nColSpacing)*m_nColSpacing;

	if((x2%m_nColSpacing)==0)
		m_lRightX=x2;
	else
		m_lRightX=int(x2/m_nColSpacing)*m_nColSpacing+m_nColSpacing;

	y1-=descendDepth;
	if((y1%m_nRowSpacing)==0)
		m_lLowY=y1;
	else
		m_lLowY=int(y1/m_nRowSpacing)*m_nRowSpacing;

	if((y2%m_nRowSpacing)==0)
		m_lHighY=y2;
	else
		m_lHighY=int(y2/m_nRowSpacing)*m_nRowSpacing+m_nRowSpacing;


	// 网格的数目 
	//  Plus a more suppositional row and column 
	//  to make sure the high and right edge has there net number:
	// or else you do not know what the number of the high edge
	// (the right edge) is.
	m_lColNum=(m_lRightX-m_lLeftX)/m_nColSpacing+1;  
	m_lRowNum=(m_lHighY-m_lLowY)/m_nRowSpacing+1;

	m_lNetNum=m_lColNum*m_lRowNum;

	return true;	
}
//  Create END
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
bool CNewNet::PointInNet(CMyPoint& point)
{
	return (point.x>=m_lLeftX&&
		point.x<=m_lRightX&&
		point.y>=m_lLowY&&
		point.y<=m_lHighY);
}
//        PointInNet
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
long CNewNet::GetXCor(long col)
{
	return (m_lLeftX+col*m_nColSpacing);
}
//
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//
long CNewNet::GetYCor(long row)
{
	return (m_lLowY+row*m_nRowSpacing);
}
//
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//
long CNewNet::GetNetNo(CMyPoint& point)
{
	return GetNetNo(point.x,point.y);
}
//
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//
long CNewNet::GetNetNo(double x,double y)
{														
	double dx,dy;
	dx=x-m_lLeftX;
	dy=y-m_lLowY;
	
	long nx=(long)dx/m_nColSpacing;
	if(nx>m_lColNum)return -1;
	if(nx<0)return -1;

	long ny=(long)dy/m_nRowSpacing;
	if(ny>m_lRowNum)return -1;
	if(ny<0)return -1;

	return ny*m_lColNum+nx;
}
//
////////////////////////////////////////////////////////////////////////////////////////////////
long CNewNet::GetNetNo(long nx,long ny)
{														
	return ny*m_lColNum+nx;
}
//	
bool CNewNet::OnYCor(CMyPoint p)
{
	double dx=p.x-m_lLeftX;
	int  n=int(dx/m_nColSpacing);
	return ((double(n)*m_nColSpacing)==dx);
}

bool CNewNet::OnXCor(CMyPoint p)
{	
	double dy=p.y-m_lLowY;
	int n=dy/m_nRowSpacing;
	return((double(n)*m_nRowSpacing)==dy);
}


CMyPoint CNewNet::GetNetPoint(long n)
{
	long ny=int(n/m_lColNum);
	long nx=n-ny*m_lColNum;

	return CMyPoint(nx*m_nColSpacing+m_lLeftX,
							  ny*m_nRowSpacing+m_lLowY);
}


CRect CNewNet::GetNetRect(long n)
{
	long ny=long(n/m_lColNum);
	long nx=long(n-ny*m_lColNum);

	int x=nx*m_nColSpacing;
	int y=ny*m_nRowSpacing;

	return CRect(x,y+m_nRowSpacing,x+m_nColSpacing,y);
}

bool CNewNet::InSameGrid(CMyPoint& p1, CMyPoint& p2)
{
	double BX,BY,EX,EY;
	if(p1.x>p2.x){
		BX=p2.x;
		EX=p1.x;
	}		
	else{
		BX=p1.x;
		EX=p2.x;		
	}

	if(p1.y>p2.y){
		BY=p2.y;
		EY=p1.y;
	}
	else{
		BY=p1.y;
		EY=p2.y;
	}

	long nBX=(BX-m_lLeftX)/m_nColSpacing;
	long nEX=(EX-m_lLeftX)/m_nColSpacing+1;
	long nBY=(BY-m_lLowY)/m_nColSpacing;
	long nEY=(EY-m_lLowY)/m_nColSpacing+1;
	
	// Get the cross points:
	double x,y;
	for(long i=nBX;i<nEX;i++){
		x=GetXCor(i);
		if(BX<x&&EX>x)return false;
	}

	for(i=nBY;i<nEY;i++){
		y=GetYCor(i);
		if(BY<y&&EY>y)return false;
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CObArray& CNewNet::GetTrace(CMyPoint& p1,CMyPoint &p2)
{
	//  开始:
	double x,y;

	//  把该射线与网格的交点全加入链表内
	CMyPoint *pPoint;
	CObArray tempTrace;
	
	pPoint=new CMyPoint(p1);
	tempTrace.Add(pPoint);
	pPoint=new CMyPoint(p2);
	tempTrace.Add(pPoint);

	// Find the x ,y coordinate lines that might be cross with the P1-P2;
	double BX,BY,EX,EY;
	if(p1.x>p2.x){
		BX=p2.x;
		EX=p1.x;
	}		
	else{
		BX=p1.x;
		EX=p2.x;		
	}

	if(p1.y>p2.y){
		BY=p2.y;
		EY=p1.y;
	}
	else{
		BY=p1.y;
		EY=p2.y;
	}
	double DY=p2.y-p1.y;
	double DX=p2.x-p1.x;

	long nBX,nBY,nEX,nEY;
	nBX=(BX-m_lLeftX)/m_nColSpacing;
	nEX=(EX-m_lLeftX)/m_nColSpacing+1;
	nBY=(BY-m_lLowY)/m_nColSpacing;
	nEY=(EY-m_lLowY)/m_nColSpacing+1;
	
	// Get the cross points:
	if(BX!=EX){
		for(long i=nBX;i<nEX;i++){
			x=GetXCor(i);
			if(BX<=x&&EX>=x){
				y=DY*(x-p1.x)/DX+p1.y;
				if(y>=m_lLowY&&y<=m_lHighY){
					pPoint=new CMyPoint(x,y);
					tempTrace.Add(pPoint);
				}
			}
		}
	}

	if(BY!=EY){
		for(long i=nBY;i<nEY;i++){
			y=GetYCor(i);
			if(BY<=y&&EY>=y){
				x=DX*(y-p1.y)/DY+p1.x;
				if(x>=m_lLeftX&&x<=m_lRightX){
					pPoint=new CMyPoint(x,y);
					tempTrace.Add(pPoint);
				}
			}
		}
	}

	// Sort  the aray:
	CMyPoint::Sort(tempTrace);

	// 删除相同的交点
	CMyPoint::DeleteSame(tempTrace);

	//  计算速度块的序号,计算速度块的长度 :
	m_arrayTrace.RemoveAll();
	pPoint=(CMyPoint*)tempTrace[0];
	CMyPoint *pPointLast,pointMid;
	CBlockTrace *pBlockAdd;
	for(int i=1;i<tempTrace.GetSize();i++){					      
		pPointLast=pPoint;
		pPoint=(CMyPoint*)tempTrace[i];
		pointMid.Mid(pPointLast,pPoint);

		pBlockAdd=new CBlockTrace;
		pBlockAdd->m_nBlock=GetNetNo(pointMid);
		pBlockAdd->m_dLen=pPointLast->Distance(pPoint);

		m_arrayTrace.Add(pBlockAdd);
	}

	//  返回 .
	return m_arrayTrace;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mainly for saving  the slowness:
bool CNewNet::SaveValue()
{
	// 存成文件:
	if(m_fileSlowness=="")return false;

	FILE *fp=fopen(m_fileSlowness,"wb");
	if(!fp){
		AfxMessageBox("无法建立慢度文件:"+m_fileSlowness);
		return false;
	}

	fwrite(&m_lLeftX,sizeof(long),1,fp);
	fwrite(&m_lLowY,sizeof(long),1,fp);
	fwrite(&m_lRightX,sizeof(long),1,fp);
	fwrite(&m_lHighY,sizeof(long),1,fp);
	fwrite(&m_lColNum,sizeof(long),1,fp);
	fwrite(&m_lRowNum,sizeof(long),1,fp);
	fwrite(&m_lNetNum,sizeof(long),1,fp);
	fwrite(&m_nColSpacing,sizeof(int),1,fp);
	fwrite(&m_nRowSpacing,sizeof(int),1,fp);

	long n=fwrite(m_pdValue,sizeof(double),m_lNetNum,fp);
	fclose(fp);

	if(n==GetNetNum())return true;	
	return false;

}

bool CNewNet::ReadValue()
{
	if(m_fileSlowness=="")return false;

	FILE *fp=fopen(m_fileSlowness,"rb");
	if(!fp){
		AfxMessageBox("无法打开慢度文件:"+m_fileSlowness);
		return false;
	}

	fread(&m_lLeftX,sizeof(long),1,fp);
	fread(&m_lLowY,sizeof(long),1,fp);
	fread(&m_lRightX,sizeof(long),1,fp);
	fread(&m_lHighY,sizeof(long),1,fp);
	fread(&m_lColNum,sizeof(long),1,fp);
	fread(&m_lRowNum,sizeof(long),1,fp);
	fread(&m_lNetNum,sizeof(long),1,fp);
	fread(&m_nColSpacing,sizeof(int),1,fp);
	fread(&m_nRowSpacing,sizeof(int),1,fp);

	if(m_pdValue){
		delete []m_pdValue;
		m_pdValue=new double[m_lNetNum];
	}

	long n=fread(m_pdValue,sizeof(double),m_lNetNum,fp);
	fclose(fp);
	
	m_dMaxValue=-1000000;
	m_dMinValue=1000000;
	for(long i=0;i<n;i++){
		if(m_pdValue[i]>m_dMaxValue&&m_pdValue[i]!=MOUNT_ERROR)
			m_dMaxValue=m_pdValue[i];
		if(m_pdValue[i]<m_dMinValue&&m_pdValue[i]!=MOUNT_ERROR)
			m_dMinValue=m_pdValue[i];
	}

	if(n==m_lNetNum)return true;
	return false;
}



bool CNewNet::SetValue(double *pValue)
{
	for(long i=0;i<m_lNetNum;i++){
		m_pdValue[i]=pValue[i];
	}
	return true;
}

double * CNewNet::GetSlow()
{
	return m_pdValue;
}



CMyPoint CNewNet::ChgToNetPoint(double x,double y)
{														
	double dx,dy;
	dx=x-m_lLeftX;
	dy=y-m_lLowY;
	
	long nx=(long)dx/m_nColSpacing;
	if(nx>m_lColNum)return CMyPoint(-1,-1);

	long ny=(long)dy/m_nRowSpacing;
	if(ny>m_lRowNum)return CMyPoint(-1,-1);

	return CMyPoint(nx,ny);
}
//
////////////////////////////////////////////////////////////////////////////////////////////////


bool CNewNet::ChgToNetPoint(CMyPoint &point)
{
	double dx,dy;
	dx=point.x-m_lLeftX;
	dy=point.y-m_lLowY;
	
	long nx=(long)dx/m_nColSpacing;
	if(nx<0||nx>m_lColNum)return false;

	long ny=(long)dy/m_nRowSpacing;
	if(ny<0||ny>m_lRowNum)return false;

	point.x=nx;
	point.y=ny;

	return true;
}

bool CNewNet::ChgToOrgPoint(CMyPoint &point)
{
	if(point.x<0||point.x>m_lColNum||point.y<0||point.y>m_lRowNum)return false;
	
	point.x=m_lLeftX+point.x*m_nColSpacing;
	point.y=m_lLowY+point.y*m_nRowSpacing;
	
	return true;

}
