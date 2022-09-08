// Trace.cpp: implementation of the CTrace class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Trace.h"
#include "Equation.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTrace::CTrace()
{	  
	m_nGridXNumber=0;
	m_nGridYNumber=0;	
	m_nGridWidth=0;
	m_nGridHeight=0;
	m_sizeScan=CSize(10,10);
	m_pointSource=CMyPoint(0,0);
	m_dEmptySlow=100;
}
 


bool CTrace::Set( int nGridX,  int nGridY, int nGridWidth,int nGridHeight,CSize sizeScan)
{
	// delete all of the pointers, and reallacate the memory.
	Reset();
   
	m_sizeScan=sizeScan;

	m_nGridXNumber=nGridX;
	m_nGridYNumber=nGridY;
	m_nGridWidth=nGridWidth;
	m_nGridHeight=nGridHeight;
	
	m_dgGridSlow.New(sizeof(double),nGridX,nGridY);
	m_dgByteGridFlag.New(sizeof(BYTE),nGridX,nGridY); // 0 , NOT reached, 1 Passed, 2 Just reached.
	m_dgMyPointFrom.New(sizeof(CMyPoint),nGridX,nGridY); // Indicate which point is the wave from.
	m_dgDoubleTime.New(sizeof(double),nGridX,nGridY);     // Record the traveled time .
	m_dgObRoute.New(sizeof(CObArray),m_sizeScan.cx,m_sizeScan.cy);  // containes the route that a radius will travel.
	m_dgBoolFlag.New(sizeof(bool),m_sizeScan.cx,m_sizeScan.cy);  // in the 4 directions (4 quadrants), are same.
	
	// Fill the arrays, some array need not fill:
	CObArray tempArray;
	m_dgObRoute.Fill(&tempArray);

	double timeMax=10000000000;
	m_dgDoubleTime.Fill(&timeMax);

	BYTE byteEmpty=0;
	m_dgByteGridFlag.Fill(&byteEmpty);
	SetEmptySlow(m_dEmptySlow);

	m_dgGridSlow.Fill(&m_dEmptySlow);

	// Set flag on the scanned small area:
	if(!MakeFlags()){
		AfxMessageBox("Error when making the flags!",MB_ICONEXCLAMATION);
		return false;
	}
  
	return true;
}

bool CTrace::SetVelocity(double *pVelocity,long nCol,long nRow)
{
	// Change velocity to slowness:

	double dSlow=1/(*pVelocity);
	m_dgGridSlow.Set(&dSlow,nCol,nRow);
	
	return true;
}     

bool CTrace::SetSlow(double *pSlow,long nCol,long nRow)
{
	m_dgGridSlow.Set(pSlow,nCol,nRow);
	return true;
}     


CTrace::~CTrace()
{
       Reset();
}

	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Note :In the reuturned CDataGroup, the first element CMyPoint.x in every row contains 
//  the grid number that the route traveled.
//
bool CTrace::Explode(CMyPoint &pointSrc,CDC *pDC)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//  Firstly reset all data to 0, becasue the explode function can be called 
	//  in cycle.
	m_pointSource=pointSrc;
	double dFiller=10000000;
	CMyPoint pointFiller(0,0);
	BYTE byteFiller=0;
	m_dgDoubleTime.Fill(&dFiller);
	m_dgMyPointFrom.Fill(&pointFiller);
	m_arrayWaveFront.RemoveAll();
	m_dgByteGridFlag.Fill(&byteFiller);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//   Then we can explode the source point:
	CMyPoint pointNewSrc(pointSrc);
	CMyPoint pointMinTime;
	
	double time=0.0;
	BYTE nGridFlag=1;
	m_dgDoubleTime.Set(&time,(int)pointNewSrc.x,(int)pointNewSrc.y);
	m_dgMyPointFrom.Set(&pointNewSrc,(int)pointNewSrc.x,(int)pointNewSrc.y);
	m_dgByteGridFlag.Set(&nGridFlag,(int)pointNewSrc.x,(int)pointNewSrc.y);

	CMyPoint pointEmpty(-1,-1),pointLastSrc=pointNewSrc;	
	int pn=0;


	do{ 	
		ScanGrid(pointLastSrc,pointNewSrc,pointMinTime);

		if(pointMinTime==pointNewSrc)break;
		pointNewSrc=pointMinTime;
		m_dgByteGridFlag.Set(&nGridFlag,(int)pointNewSrc.x,(int)pointNewSrc.y);	     

/*		int nMaxSlow=3000;
		int nLargerX=12;
		int nLargerY=12;	       
		for(int i=0;i<100;i++){
			for(int j=0;j<50;j++){
				BYTE flag=*(BYTE*)m_dgByteGridFlag.Get(i,j);
				double slow=*(double*)m_dgGridSlow.Get(i,j);
				int n=(int)((1/slow)/nMaxSlow*255);
				if(flag==0)
					pDC->SelectObject(&brush[10]);
				else if(flag==1)
					pDC->SelectObject(&brush[100]);
				else
					pDC->SelectObject(&brush[200]);
			   
				pDC->SelectObject(&brush[n]);
				pDC->Rectangle((i-0.5)*nLargerX,(j-0.5)*nLargerY,(i+0.5)*nLargerX,(j+0.5)*nLargerY);
			}			    
		}
   */
		pn++;
	}while(true);
	
	return true;
	
}

bool CTrace::MakeFlags()
{
	int i,j,k,max,i1,i2;
	bool flag=true;
	
	double GridWidth2=m_nGridWidth*m_nGridWidth;
	double GridHeight2=m_nGridHeight*m_nGridHeight;
	CMyPoint *pPointRoute=new CMyPoint[m_sizeScan.cx*m_sizeScan.cy];
	CMyPoint pointMid;

	for(i=0;i<m_sizeScan.cx;i++){
		for(j=0;j<m_sizeScan.cy;j++){
	
			// Calculate the route that a radius will travel through:
			pPointRoute[0]=CMyPoint(0.5,0.5);
			pPointRoute[1]=CMyPoint(i+0.5,j+0.5);
			long nPoint=2;
			double m,n;
			if(i==0){
				for(n=1;n<=j;n++){
					pPointRoute[nPoint]=CMyPoint(0.5,n);
					nPoint++;
				}
			}
			else if(j==0){
				for(m=1;m<=i;m++){
					pPointRoute[nPoint]=CMyPoint(m,0.5);
					nPoint++;
				}
			}
			else{
				double kRadius=(double)j/(double)i;
				for(m=1;m<=i;m++){
					n=kRadius*(m-0.5)+0.5;
					pPointRoute[nPoint]=CMyPoint(m,n);
					nPoint++;
				}
				kRadius=1/kRadius;
				for(n=1;n<=j;n++){
					m=kRadius*(n-0.5)+0.5;
					pPointRoute[nPoint]=CMyPoint(m,n);
					nPoint++;
				}
			}
			CMyPoint::Sort(pPointRoute,nPoint);
			CMyPoint::DeleteSame(pPointRoute,nPoint);
			
			CObArray *pArray=(CObArray*)m_dgObRoute.Get(i,j);
			CGridRoute *pGridRoute;
			int x,y;
			double dLen,dx,dy;
			double dWidth2=	m_nGridWidth*m_nGridWidth;
			double dHeight2=m_nGridHeight*m_nGridHeight;
			for(k=0;k<nPoint-1;k++){
				x=int(pPointRoute[k].x+(pPointRoute[k+1].x-pPointRoute[k].x)/2);
				y=int(pPointRoute[k].y+(pPointRoute[k+1].y-pPointRoute[k].y)/2);
				dx=pPointRoute[k+1].x-pPointRoute[k].x;
				dy=pPointRoute[k+1].y-pPointRoute[k].y;
				dLen=sqrt(dx*dx*dWidth2+dy*dy*dHeight2);
				pGridRoute=new CGridRoute(x,y,dLen);				
				pArray->Add(pGridRoute);
			}
			
			// Set the flag that if the grid should be traced:
			if(i==0&&j==0){
				flag=false;
			}
			else if(i==1&&j==1){
				flag=true;
			}
			else{
				max=i;
				if(j>i)max=j;
				flag=true;
				k=1;
				do{
					k++;
					i1=i%k;
					i2=j%k;
					if((i1==0)&&(i2==0)){
						flag=false;
						break;
					}
				}while(k<max);
			}
  		
			m_dgBoolFlag.Set(&flag,i,j);
		}
	}

	delete [] pPointRoute;

	return true;
}

bool CTrace::Reset()
{
	
	if(!m_dgGridSlow.Reset())return false;
	if(!m_dgByteGridFlag.Reset())return false;
	if(!m_dgMyPointFrom.Reset())return false;		
	if(!m_dgDoubleTime.Reset())return false;     // Record the traveled time .
	if(!m_dgBoolFlag.Reset())return false;
	
	if(!m_dgObRoute.IsEmpty()){
		CObArray *p;
		for(int i=0;i<m_sizeScan.cx;i++){
			for(int j=0;j<m_sizeScan.cy;j++){
				p=(CObArray*)m_dgObRoute.Get(i,j);
				p->RemoveAll();
			}
		}								     
		m_dgObRoute.Reset();
	}

	m_arrayWaveFront.RemoveAll();
	m_arrayTrace.RemoveAll();

	m_nGridXNumber=0;
	m_nGridYNumber=0;	
	m_nGridWidth=0;
	m_nGridHeight=0;
	
	return true;
}

bool CTrace::ScanGrid(CMyPoint &pointLastSrc,CMyPoint& pointSrc, CMyPoint &pointMinTime)
{
	// define the area to be scaned:
	double xfrom=pointSrc.x-m_sizeScan.cx+1;
	if(xfrom<0)xfrom=0;
	double yfrom=pointSrc.y-m_sizeScan.cy+1;
	if(yfrom<0)yfrom=0;

	// Should not minus 1 , because in the loop below, 
	// the grid of xto will not be reached it is "<", not "<=".
	double xto=pointSrc.x+m_sizeScan.cx-1;
	if(xto>m_nGridXNumber)xto=m_nGridXNumber;
	double yto=pointSrc.y+m_sizeScan.cy-1;
	if(yto>m_nGridYNumber)yto=m_nGridYNumber;

	double i,x,y;
	BYTE flag,flag2=2;
	bool bScan;
	double timeSrc=*(double*)m_dgDoubleTime.Get(pointSrc.x,pointSrc.y);
	double timeTravel;	     
	double timeExist;
	CMyPoint pointDest;
	CMyPoint *pPointFront;
	
	for( pointDest.x=xfrom;pointDest.x<xto;pointDest.x++){
		for( pointDest.y=yfrom;pointDest.y<yto;pointDest.y++){
			x=fabs(pointDest.x-pointSrc.x);
			y=fabs(pointDest.y-pointSrc.y);

			flag=*(BYTE*)m_dgByteGridFlag.Get(pointDest.x,pointDest.y);
			bScan=*(bool*)m_dgBoolFlag.Get(x,y);	
		
			if(flag!=1&&bScan){
		
				// Calculate the time needed to travel from the source point to the target point:
				timeTravel=CalcTime(pointSrc,pointDest)+timeSrc;
				

				// the just got time is smaller than before :
				timeExist=*(double*)m_dgDoubleTime.Get(pointDest.x,pointDest.y);
				
				if(timeTravel<=timeExist){
					m_dgDoubleTime.Set(&timeTravel,pointDest.x,pointDest.y);
					m_dgMyPointFrom.Set(&pointSrc,pointDest.x,pointDest.y);
					m_dgByteGridFlag.Set(&flag2,pointDest.x,pointDest.y);
				}  

				if(flag==0){
					pPointFront=new CMyPoint;
					*pPointFront=pointDest;
					m_arrayWaveFront.Add(pPointFront);
				}


			} //NOT passed and Can be scanned
		} // Scan Column
	}// Scan Row

	
	// Search for the quickest reached point:
	double dMin=100000000000,timeTemp;
	long nMin=-1;

	for(i=0;i<m_arrayWaveFront.GetSize();i++){
		pPointFront=(CMyPoint*)m_arrayWaveFront[i];
		timeTemp=*(double*)m_dgDoubleTime.Get(pPointFront->x,pPointFront->y);
		if(timeTemp<dMin){
			pointMinTime=*pPointFront;
			nMin=i;
			dMin=timeTemp;
		}
	}
	  					
	// because nextstep will use this point as source , and set this flag to 1.
	// So delete it from the wave front:
	if(nMin!=-1)m_arrayWaveFront.RemoveAt(nMin);
	return true;
}
					

double CTrace::CalcTime(CMyPoint &pointSrc, CMyPoint &pointDest)
{
	// Needed Number
	double dx=pointDest.x-pointSrc.x;
	double dy=pointDest.y-pointSrc.y;
	double ax=fabs(dx);
	double ay=fabs(dy);
	double timeTravel=0.0;
	
	CObArray *pArray=(CObArray*)m_dgObRoute.Get((int)ax,(int)ay);
	
	CGridRoute *pSection;
	double m,n;

	if(dx!=0&&dy!=0){
		if(dx==ax){
			if(dy==ay){
				for(int i=0;i<pArray->GetSize();i++){
					pSection=(CGridRoute *)pArray->GetAt(i);
					m=pointSrc.x+pSection->nx;
					n=pointSrc.y+pSection->ny;
					timeTravel+=*(double*)m_dgGridSlow.Get(m,n)*pSection->dLen;
				}
			}
			else{
				for(int i=0;i<pArray->GetSize();i++){
					pSection=(CGridRoute *)pArray->GetAt(i);
					m=pointSrc.x+pSection->nx;
					n=pointSrc.y-pSection->ny;
					timeTravel+=*(double*)m_dgGridSlow.Get(m,n)*pSection->dLen;
				}
			}
		}
		else if(dy==ay){
			for(int i=0;i<pArray->GetSize();i++){
				pSection=(CGridRoute *)pArray->GetAt(i);
				m=pointSrc.x-pSection->nx;
				n=pointSrc.y+pSection->ny;
				timeTravel+=*(double*)m_dgGridSlow.Get(m,n)*pSection->dLen;
			}
		}
		else {
			for(int i=0;i<pArray->GetSize();i++){
				pSection=(CGridRoute *)pArray->GetAt(i);
				m=pointSrc.x-pSection->nx;
				n=pointSrc.y-pSection->ny;
				timeTravel+=*(double*)m_dgGridSlow.Get(m,n)*pSection->dLen;
			}
		}
	}
	else if(dx==0&&dy!=0){
		m=pointSrc.x;
		if(ay==dy){
			for(int i=0;i<pArray->GetSize();i++){
				pSection=(CGridRoute *)pArray->GetAt(i);
				n=pointSrc.y+pSection->ny;
				timeTravel+=*(double*)m_dgGridSlow.Get(m,n)*pSection->dLen;
			}				  
		}
		else{
			for(int i=0;i<pArray->GetSize();i++){
				pSection=(CGridRoute *)pArray->GetAt(i);
				n=pointSrc.y-pSection->ny;
				timeTravel+=*(double*)m_dgGridSlow.Get(m,n)*pSection->dLen;
			}				  
		}
	}
	else if(dx!=0&&dy==0){
		n=pointSrc.y;
		if(ax==dx){
			for(int i=0;i<pArray->GetSize();i++){
				pSection=(CGridRoute *)pArray->GetAt(i);
				m=pointSrc.x+pSection->nx;
				timeTravel+=*(double*)m_dgGridSlow.Get(m,n)*pSection->dLen;
			}
		}
		else{
			for(int i=0;i<pArray->GetSize();i++){
				pSection=(CGridRoute *)pArray->GetAt(i);
				m=pointSrc.x-pSection->nx;
				timeTravel+=*(double*)m_dgGridSlow.Get(m,n)*pSection->dLen;
			}
		}
	}
	else{
		return 0;
	}
	
	return timeTravel;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// From the rev point ,  regressively search to the shot point ,
// So we can get the trace that the shot wave travelled.
CObArray& CTrace::GetRoute(CMyPoint &pointDest)
{
	m_arrayTrace.RemoveAll();

	// Because the grid is based on 0 , so you can not give a number equal or more than m_nGridYNumber.
	if(pointDest.x<0||pointDest.y<0||pointDest.x>=m_nGridXNumber||pointDest.y>=m_nGridYNumber){
		AfxMessageBox("CTrace::GetRoute Report Error: point out of range!");
		return m_arrayTrace;
	}	

	CMyPoint *pointAdd=new CMyPoint(pointDest);
	m_arrayTrace.Add(pointAdd);
	CMyPoint *pPoint=&pointDest;
	int n=0;
	do{
		pPoint=(CMyPoint*)m_dgMyPointFrom.Get(pPoint->x,pPoint->y);
		if(*pPoint==*(CMyPoint*)m_arrayTrace.GetAt(n))break;
		pointAdd=new CMyPoint(*pPoint);
		m_arrayTrace.Add(pointAdd);
		n++;
	}while(true);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Get the straight route from the shot point to the destination point 
	// and compare them , we should return the route that is quicker.
	double timeStraight,timeCurve;
	GetStraightRoute(m_pointSource,pointDest,timeStraight);	 
	timeCurve=*(double*)m_dgDoubleTime.Get(int(pointDest.x),int(pointDest.y));
	
	if(timeStraight<timeCurve){
		m_arrayStraightTrace.RemoveAll();
		CMyPoint *pPoint=new CMyPoint(m_pointSource);
		m_arrayStraightTrace.Add(pPoint);
		pPoint=new CMyPoint(pointDest);
		m_arrayStraightTrace.Add(pPoint);
		
		return m_arrayStraightTrace;
	}
	else
		return m_arrayTrace;
}

bool CTrace::IsEmpty()
{
	if(m_nGridXNumber==0)return true;
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// To accelerate the speed of calculation, 
// we set the flag of the empty grid  to 1, which means 
// that the grid has been scanned.
bool CTrace::SetEmptySlow(double dEmptySlow)
{
	m_dEmptySlow=dEmptySlow;  // this data id to determine if the grid is useless.
	BYTE flag=1;
	for(int i=0;i<m_nGridXNumber;i++){
		for(int j=0;j<m_nGridYNumber;j++){
			if(*(double*)m_dgGridSlow.Get(i,j)==m_dEmptySlow){
				m_dgByteGridFlag.Set(&flag,i,j);
			}
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Return : 
//  The m_arrayStraightTrace , contains the CMypoints, in which is the grid 's number and the legth in the grid:
//  The total time that the trace traveled in the routes:
CObArray& CTrace::GetStraightRoute(CMyPoint p1,CMyPoint p2,double& dStraightTime)
{
	//  开始:
	CObArray arrayTemp;
	
	//  把该射线与网格的交点全加入链表内
	CMyPoint *pPoint;
	
	p1.x+=0.5;
	p1.y+=0.5;
	p2.x+=0.5;
	p2.y+=0.5;	
	
	pPoint=new CMyPoint(p1.x,p1.y);
	arrayTemp.Add(pPoint);
	pPoint=new CMyPoint(p2.x,p2.y);
	arrayTemp.Add(pPoint);

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
	nBX=int(BX);
	nEX=int(EX)+1;
	nBY=int(BY);
	nEY=int(EY)+1;
	
	// Get the cross points:
	double x,y;	
	for(x=nBX;x<nEX;x++){
		if(BX<=x&&EX>=x){
			y=DY*(x-p1.x)/DX+p1.y;
			if(y>=0&&y<=m_nGridYNumber){
				pPoint=new CMyPoint(x,y);
				arrayTemp.Add(pPoint);
				if(x>1000){
					int m=0;
				}
			}
		}
	}

	for(y=nBY;y<nEY;y++){
		if(BY<=y&&EY>=y){
			x=DX*(y-p1.y)/DY+p1.x;
			if(x>=0&&x<=m_nGridXNumber){
				pPoint=new CMyPoint(x,y);
				arrayTemp.Add(pPoint);
			}
		}
	}

	// Sort  the aray:	
	CMyPoint::Sort(arrayTemp);
	
	// 删除相同的交点
	CMyPoint::DeleteSame(arrayTemp);
	
	//  计算速度块的序号,计算速度块的长度 :
	CMyPoint *pPointLast,pointMid;
	
	pPoint=(CMyPoint*)arrayTemp[0];
	dStraightTime=0;
	double dx,dy,dLen;

	CBlockTrace *pBlockTrace;
	m_arrayStraightTrace.RemoveAll();
	for(int i=1;i<arrayTemp.GetSize();i++){					      
		pPointLast=pPoint;
		pPoint=(CMyPoint*)arrayTemp[i];
		pointMid.Mid(pPointLast,pPoint);
		dx=(pPointLast->x-pPoint->x)*m_nGridWidth;
		dy=(pPointLast->y-pPoint->y)*m_nGridHeight;
		dLen=sqrt(dx*dx+dy*dy);
		
		// Change the trace ' s element to " Grid No. and The length in the grid.
		pBlockTrace=new CBlockTrace(GetGridNo(int(pointMid.x),int(pointMid.y)),dLen);
		m_arrayStraightTrace.Add(pBlockTrace);

		// Calculate  the total time in the route:
		dStraightTime+=*(double*)m_dgGridSlow.Get(int(pointMid.x),int(pointMid.y))*dLen;
	}

	//  返回 .
	return m_arrayStraightTrace;
}


bool CTrace::IsGridEmpty(int i, int j)
{
	double d=*(double*)m_dgGridSlow.Get(i,j);
	if((*(double*)m_dgGridSlow.Get(i,j))==m_dEmptySlow )
		return true;
	else
		return false;
}

long CTrace::GetGridNo(int nx, int ny)
{
	return ny*m_nGridXNumber+nx;
}

bool CTrace::Shake(long nLoop)
{
	long i,m,n;
	double dSlow,dSlowSide;

	for(i=0;i<nLoop;i++){

		// Shake to  rigtht:
		for(m=0;m<m_nGridXNumber-1;m++){
			for(n=0;n<m_nGridYNumber;n++){
				dSlow=*(double*)m_dgGridSlow.Get(m,n);
				dSlowSide=*(double*)m_dgGridSlow.Get(m+1,n);
				if(dSlow==m_dEmptySlow||dSlowSide==m_dEmptySlow){
					continue;
				}
				dSlow=(dSlow+dSlowSide)/2;
				m_dgGridSlow.Set(&dSlow,m,n);
				m_dgGridSlow.Set(&dSlow,m+1,n);
			}
		}

		// Shake down:
		for(m=0;m<m_nGridXNumber;m++){
			for(n=0;n<m_nGridYNumber-1;n++){
				dSlow=*(double*)m_dgGridSlow.Get(m,n);
				dSlowSide=*(double*)m_dgGridSlow.Get(m,n+1);
				if(dSlow==m_dEmptySlow||dSlowSide==m_dEmptySlow){
					continue;
				}
				dSlow=(dSlow+dSlowSide)/2;
				m_dgGridSlow.Set(&dSlow,m,n);
				m_dgGridSlow.Set(&dSlow,m,n+1);
			}
		}
	}

	return true;
}
