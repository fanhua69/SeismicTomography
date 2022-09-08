// MountView.cpp : implementation of the CMountView class
//

#include "stdafx.h"
#include "NewMount.h"
#include "MountDoc.h"
#include "MountView.h"
#include "math.h"
#include "Project.h"
#include "SvData.h"
#include "ScData.h"
#include "NewNet.h"
#include "SvSys.h"
#include "DataGroup.h"
#include "Trace.h"
#include "fstbrk.h"
#include "StRcv.h"
#include "ShotPar.h"
#include "Global.h"
#include "MemEqua.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMountView

IMPLEMENT_DYNCREATE(CMountView, CScrollView)

BEGIN_MESSAGE_MAP(CMountView, CScrollView)
	//{{AFX_MSG_MAP(CMountView)
	ON_COMMAND(ID_Test_Survey, OnTestSurvey)
	ON_COMMAND(ID_TEST_FSTBRK, OnTestFstbrk)
	ON_COMMAND(ID_TEST_SvSys, OnTESTSvSys)
	ON_COMMAND(ID_TEST_SR, OnTestSr)
	ON_COMMAND(ID_TEST_ShotPar, OnTESTShotPar)
	ON_COMMAND(ID_TEST_RESOLVE, OnTestResolve)
	ON_COMMAND(ID_Test_Equation, OnTestEquation)
	ON_COMMAND(ID_Test_initial, OnTestinitial)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_Test4_Project, OnTest4Project)
	ON_COMMAND(ID_Test4_Initial, OnTest4Initial)
	ON_COMMAND(ID_Test4_FstBrk, OnTest4FstBrk)
	ON_COMMAND(ID_Test4_Resolve, OnTest4Resolve)
	ON_COMMAND(ID_Test4_ShotPar, OnTest4ShotPar)
	ON_COMMAND(ID_Test4_ShotRecieve, OnTest4ShotRecieve)
	ON_COMMAND(ID_Test4_SvSys, OnTest4SvSys)
	ON_COMMAND(ID_Test_Cac, OnTestCac)
	ON_COMMAND(ID_CALC_STATIC, OnCalcStatic)
	ON_COMMAND(ID_116_Pro, On116Pro)
	ON_COMMAND(ID_116_Sys, On116Sys)
	ON_COMMAND(ID_116_SR, On116Sr)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMountView construction/destruction

CMountView::CMountView()
{
	// TODO: add construction code here
	for(int b=0;b<256;b++){
		brush[b].CreateSolidBrush(RGB(0,b,0));
	}
}

CMountView::~CMountView()
{
}

BOOL CMountView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMountView drawing

void CMountView::OnDraw(CDC* pDC)
{
	CMountDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Draw the SR:
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(sr.GetSize()==0)return;

	CProject pro;
	if(!pro.Open("MZ98116.pro"))return;
	if(!pro.OpenLine ("MZ98116"))return;

	CSvSys svsys;
	if(!svsys.Use(pro.m_sSvSys))return;

	CSvData sd;
	if(!sd.Use (pro.m_sSurvey ))return;


	CMyPoint org(pro.m_nValidBeginPh,sd.GetEleOnPh(pro.m_nValidBeginPh));

	CMyPoint s,r;
	int n=0;
	for(long ph=pro.m_nValidBeginPh ;ph<pro.m_nValidEndPh ;ph+=svsys.m_iShotInterval){
		s.x=ph-org.x;
		s.y=sd.GetEleOnPh(ph)-org.y;
		
		SShotRecieve *pSR=sr.Get((ph-pro.m_nValidBeginPh )/svsys.m_iShotInterval );

		for(int i=0;i<10;i++){
			for(int j=pSR->nBeginGroup[i] ;j<=pSR->nEndGroup [i];j++){
				if(j<=svsys.m_iGroupNumSmall ){
					r.x=ph-svsys.m_iGapSmall -(j-1)*svsys.m_iGroupInterval ;
				}
				else{
					r.x=ph+svsys.m_iGapBig +(j-1)*svsys.m_iGroupInterval ;
				}
				r.y=sd.GetEleOnPh (r.x)-org.y;
				r.x-=org.x;
				
				pDC->MoveTo(s.x/2,s.y*5);
				pDC->LineTo(r.x/2,r.y*5);
			}
		}
		n++;
	}






	if(trace.m_nGridXNumber==0)return;

	org=CMyPoint(50,50);
	double nMaxSlow=0.002;
	int nLargerX=20;
	int nLargerY=20;
	long x1,y1,x2,y2;
	CString sData;       
	  
	CBrush *pOldBrush=pDC->SelectObject(&brush[0]);
	for(int  i=0;i<41;i++){
		for(int j=0;j<21;j++){
			BYTE flag=*(BYTE*)trace.m_dgByteGridFlag.Get(i,j);
			double slow=*(double*)trace.m_dgGridSlow.Get(i,j);
			int n=(int)(slow/nMaxSlow*255);
			
			//if(flag==0)
			//	pDC->SelectObject(&brush[10]);
			//else if(flag==1)
			//	pDC->SelectObject(&brush[100]);
			//else
			//	pDC->SelectObject(&brush[200]);
		   	//
			if(n<0)n=0;
			if(n>255)n=255;

			pDC->SelectObject(&brush[n]);
			
			x1=(i-0.5)*nLargerX+org.x;
			y1=(j-0.5)*nLargerY+org.y;
			x2=(i+0.5)*nLargerX+org.x;
			y2=(j+0.5)*nLargerY+org.y;

			int ss=int(slow*100000);
			sData.Format("%i",ss);
			pDC->Rectangle(x1,y1,x2,y2);
			pDC->TextOut(x1,y1,sData);
		}			    
	}

	CPen pen(PS_SOLID,0,RGB(255,0,0));
	CPen* pOldPen=pDC->SelectObject(&pen);
	
	//trace.Explode(CMyPoint(0,0));
	for(i=0;i<20;i++){
		int j=i;
		CObArray& array =trace.GetRoute(CMyPoint(i,j));
		CMyPoint *p=(CMyPoint*)array[0];
		pDC->MoveTo(org.x+p->x*nLargerX,org.y+p->y*nLargerY);
		for(int k=1;k<array.GetSize();k++){
			p=(CMyPoint*)array[k];
			pDC->LineTo(org.x+p->x*nLargerX,org.y+p->y*nLargerY);
		}
	}
	
	for(i=20;i<=40;i++){
		int j=40-i;
		CObArray& array =trace.GetRoute(CMyPoint(i,j));
		CMyPoint *p=(CMyPoint*)array[0];
		pDC->MoveTo(org.x+p->x*nLargerX,org.y+p->y*nLargerY);
		for(int k=1;k<array.GetSize();k++){
			p=(CMyPoint*)array[k];
			pDC->LineTo(org.x+p->x*nLargerX,org.y+p->y*nLargerY);
		}
	}      

	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	
}

void CMountView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 2900;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

/////////////////////////////////////////////////////////////////////////////
// CMountView printing

BOOL CMountView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMountView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMountView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CMountView diagnostics

#ifdef _DEBUG
void CMountView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CMountView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CMountDoc* CMountView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMountDoc)));
	return (CMountDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMountView message handlers

void CMountView::OnTestSurvey() 
{
	// TODO: Add your command handler code here
	CProject pro;
	pro.m_sSvSys="test2.sys";
	CLineFile *plinefile=new CLineFile;
	plinefile->m_sLineName="test2";
	plinefile->m_sSurvey="test2.txt";
	plinefile->m_sWellDepth="test2.wd";
	plinefile->m_sFstBrk="test2.fbk";
	plinefile->m_sShotPar="test2.spp";
	plinefile->m_sSCData="test2.t";
	plinefile->m_sShotRcv="test2.sr";
	plinefile->m_sTrace="test2.trc";
	plinefile->m_sEndPoint="test2.end";
	plinefile->m_sUnform="test2.unf";
	plinefile->m_sFloat="test2.F";
	plinefile->m_sWellDepth="test2.wd";

	plinefile->m_nValidBeginPh=101000;
	plinefile->m_nValidEndPh=102000;

	pro.AddLine(plinefile);
	pro.Save("Test2");    


	pro.Reset();
	pro.m_sSvSys="test3.sys";
	plinefile=new CLineFile;
	plinefile->m_sLineName="test3";
	plinefile->m_sSurvey="test3.txt";
	plinefile->m_sWellDepth="test3.wd";
	plinefile->m_sFstBrk="test3.fbk";
	plinefile->m_sShotPar="test3.spp";
	plinefile->m_sSCData="test3.t";
	plinefile->m_sShotRcv="test3.sr";
	plinefile->m_sTrace="test3.trc";
	plinefile->m_sEndPoint="test3.end";
	plinefile->m_sUnform="test3.unf";
	plinefile->m_sFloat="test3.F";
	plinefile->m_sWellDepth="test3.wd";

	plinefile->m_nValidBeginPh=102000;
	plinefile->m_nValidEndPh=104000;

	pro.AddLine(plinefile);
	pro.Save("Test3");    





}

void CMountView::OnTestFstbrk() 
{
	CProject pro;
	pro.Open("test3");
	pro.OpenLine( "test3");
					       
	long  PhB=pro.m_nValidBeginPh;
	long  PhE=pro.m_nValidEndPh;

	CNewNet net;
	net.Create(pro.m_sSurvey,PhB,PhE);

	CSize size(20,10);
	trace.Set(net.GetColNum(),net.GetRowNum(),20,1,size);

	double v1=1000,v2=2000,v3=3000,v4=4000,v5=5000;
	
	
	int i,j,k;
	
	for(i=0;i<8;i++){
		for(j=0;j<=i;j++){
			trace.SetVelocity(&v1,i,j);
		}
	}

	for(i=8;i<16;i++){
		for(j=0;j<=i;j++){
			trace.SetVelocity(&v2,i,j);
		}
	}

	for(i=16;i<20;i++){
		for(j=0;j<=i;j++){
			trace.SetVelocity(&v3,i,j);
		}
	}

	for( i=20;i<24;i++){
		int h=40-i;
		for(j=0;j<=h;j++){
			trace.SetVelocity(&v3,i,j);
		}
	}

	for(i=24;i<=32;i++){
		int h=40-i;
		for(j=0;j<=h;j++){
			trace.SetVelocity(&v4,i,j);
		}
	}

	for(i=32;i<=40;i++){
		int h=40-i;
		for(j=0;j<=h;j++){
			trace.SetVelocity(&v5,i,j);
		}
	}
	
	
	CClientDC dc(this);
	CPen pen(PS_SOLID,0,RGB(0,255,0));
	dc.SelectObject(&pen);	   
	
	CFstBrk fstbrk;
	CSvSys svsys(pro.m_sSvSys);
	if(svsys.IsEmpty())return ;

	fstbrk.Open(pro.m_sFstBrk,svsys);

	CSvData sd;
	if(!sd.Use(pro.m_sSurvey))return;

	int nGI=svsys.m_iGroupInterval ;
	int nGroupNumSmall=svsys.m_iGroupNumSmall;
	int nGroupNumBig=svsys.m_iGroupNumBig;
	int nGapSmall=svsys.m_iGapSmall;
	int nGapBig=svsys.m_iGapBig;
	int nShotInterval=svsys.m_iShotInterval;
	
	int nLargerX=20;
	long nBrk=0;
	SFstBrkRecord brk;

	Invalidate();
	UpdateWindow();
	for(long ph=PhB;ph<=PhE;ph+=nShotInterval){
		int nGroupValidSmall=(ph-nGapSmall-PhB)/nGI+1;
		int nGroupPlusSmall=nGroupNumSmall-nGroupValidSmall;
		long nPhBeginSmall=PhB-nGroupPlusSmall*nGI;

		int nGroupValidBig=(PhE-ph-nGapBig)/nGI+1;
		if(nGroupValidBig>nGroupNumBig)
			nGroupValidBig=nGroupNumBig;
		int nGroupPlusBig=nGroupNumBig-nGroupValidBig;
		long nPhEndBig=ph+nGapBig+(nGroupValidBig-1)*nGI+nGroupPlusBig*nGI;

		int HBG=sd.GetEleOnPh(ph);
		if(HBG==-9999)return;

		CMyPoint pointSrc=net.ChgToNetPoint(ph,HBG);
		trace.Explode(pointSrc,&dc);
		
		Invalidate();
		UpdateWindow();
		
		// This section make the first break of the no valid of the first half:
		long nShotFileNo=(ph-PhB)/nShotInterval+1;
		for(long phr=nPhBeginSmall;phr<PhB;phr+=nGI){
			brk.m_fFstBrk =0.001;
			brk.m_iTraceNo =(phr-nPhBeginSmall)/nGI+1;
			brk.m_lFileNo=nShotFileNo;

			fstbrk.Put (&brk,nBrk);
			nBrk++;
		}
		
		// This section  make the first break of the first half of the valid reciever:
		CMyPoint org(50,50);
		for(phr=PhB;phr<=ph-nGapSmall;phr+=nGI){
			CMyPoint pointNet=net.ChgToNetPoint(phr,sd.GetEleOnPh(phr));
			CObArray &array=trace.GetRoute(pointNet);
			if(array.GetSize()==0){
				AfxMessageBox("GetRoute returned 0 nodes!");
				return ;
			}

			CMyPoint *point=(CMyPoint*)array[0];
			dc.MoveTo(point->x*nLargerX+org.x,point->y*nLargerX+org.y);
			for(k=1;k<array.GetSize();k++){
				point=(CMyPoint*)array[k];
				dc.LineTo(point->x*nLargerX+org.x,point->y*nLargerX+org.y);
			}
	
			brk.m_fFstBrk =*(double*)trace.m_dgDoubleTime.Get(pointNet.x,pointNet.y);			
			brk.m_iTraceNo =(phr-PhB)/nGI+1+nGroupPlusSmall;
			brk.m_lFileNo=nShotFileNo;
		
			fstbrk.Put (&brk,nBrk);
			nBrk++;		
		}

		// This setion make the first break of the second half of the valid reciever:
		for(phr=ph+nGapBig;phr<=PhE;phr+=nGI){	      
			CMyPoint pointNet=net.ChgToNetPoint(phr,sd.GetEleOnPh(phr));
			CObArray &array=trace.GetRoute(pointNet);
			if(array.GetSize()==0){
				int m=0;
			}


			CMyPoint *point=(CMyPoint*)array[0];
			dc.MoveTo(point->x*nLargerX+org.x,point->y*nLargerX+org.y);
			for(k=1;k<array.GetSize();k++){
				point=(CMyPoint*)array[k];
				dc.LineTo(point->x*nLargerX+org.x,point->y*nLargerX+org.y);
			}
			
			brk.m_fFstBrk =*(double*)trace.m_dgDoubleTime.Get(pointNet.x,pointNet.y);						
			brk.m_iTraceNo =nGroupNumSmall+(phr-ph-nGapBig)/nGI+1;
			brk.m_lFileNo=nShotFileNo;
		
			fstbrk.Put (&brk,nBrk);
			nBrk++;		
		}

		// this section make the first break of the second half of the notvalid recievers:
		for(phr=PhE;phr<nPhEndBig;phr+=nGI){
			brk.m_fFstBrk =0.001;
			brk.m_iTraceNo =nGroupNumSmall+(phr-ph-nGapBig)/nGI+1;
			brk.m_lFileNo=nShotFileNo;

			fstbrk.Put (&brk,nBrk);
			nBrk++;
		}
	}	

}

void CMountView::OnTESTSvSys() 
{
	// TODO: Add your command handler code here
	CSvSys svsys;
	svsys.m_sArea="test3";
	svsys.m_sCrew="testcrew";
	svsys.m_iGroupInterval=50;
	svsys.m_iShotInterval=50;
	svsys.m_iGroupNumSmall=40;
	svsys.m_iGroupNumBig=40;
	svsys.m_iGapSmall=50;
	svsys.m_iGapBig=50;
	svsys.m_iFillVelocity=2000;

	svsys.Save("Test3.sys");

}

void CMountView::OnTestSr() 
{
	// TODO: Add your command handler code here
	CProject pro;
	if(!pro.Open("test3"))return ;
	if(!pro.OpenLine("test3"))return;

	CShotPar sp;
	if(!sp.Use(pro.m_sShotPar))return;

	CSvSys svsys;
	if(!svsys.Use(pro.m_sSvSys))return;

	CSvData sd;
	if(!sd.Use(pro.m_sSurvey))return ;
		
	CShotRecieve sr;
	
	long tr;
	SShotRecieve *psr;
	for(long ph=pro.m_nValidBeginPh ;
			ph<=pro.m_nValidEndPh;
			ph+=svsys.m_iShotInterval)
			{
				psr=new SShotRecieve;
				psr->ShotPh=ph;

				if(ph==pro.m_nValidBeginPh){
					psr->nBeginGroup[0]=pro.m_nValidBeginPh +svsys.m_iGapBig;
					psr->nEndGroup[0]=pro.m_nValidEndPh ;
					psr->nBeginGroup[0]=sp.GetRecieveNoOnPh(psr->ShotPh,psr->nBeginGroup[0],tr,svsys);
					psr->nEndGroup[0]=sp.GetRecieveNoOnPh(psr->ShotPh,psr->nEndGroup[0],tr,svsys);
				}
				else if(ph==pro.m_nValidEndPh){
					psr->nBeginGroup[0]=pro.m_nValidBeginPh;
					psr->nEndGroup[0]=pro.m_nValidEndPh - svsys.m_iGapSmall;
					psr->nBeginGroup[0]=sp.GetRecieveNoOnPh(psr->ShotPh,psr->nBeginGroup[0],tr,svsys);
					psr->nEndGroup[0]=sp.GetRecieveNoOnPh(psr->ShotPh,psr->nEndGroup[0],tr,svsys);
				}
				else{
					psr->nBeginGroup[0]=pro.m_nValidBeginPh;
					psr->nEndGroup[0]=ph-svsys.m_iGapSmall;
					psr->nBeginGroup[1]=ph+svsys.m_iGapBig;
					psr->nEndGroup[1]=pro.m_nValidEndPh;

					psr->nBeginGroup[0]=sp.GetRecieveNoOnPh(psr->ShotPh,psr->nBeginGroup[0],tr,svsys);
					psr->nBeginGroup[1]=sp.GetRecieveNoOnPh(psr->ShotPh,psr->nBeginGroup[1],tr,svsys);
					psr->nEndGroup[0]=sp.GetRecieveNoOnPh(psr->ShotPh,psr->nEndGroup[0],tr,svsys);
					psr->nEndGroup[1]=sp.GetRecieveNoOnPh(psr->ShotPh,psr->nEndGroup[1],tr,svsys);
				}
				sr.Add(psr);
	}

	sr.Save(pro.m_sShotRcv);

	return;
}

	

void CMountView::OnTESTShotPar() 
{
	// TODO: Add your command handler code here
	CProject pro;
	if(!pro.Open("test3"))return ;
	if(!pro.OpenLine("test3"))return;

	CSvSys svsys;
	if(!svsys.Use(pro.m_sSvSys))return;

	CShotPar sp;
	SShotPar *psp;

	for(long ph=102000;ph<=104000;ph+=50){
		psp=new SShotPar;
		psp->m_iFileNo=(ph-101950)/50;
		psp->m_iGapBig=50;
		psp->m_iGapSmall=50;
		psp->m_iGroupNumBig=40;
		psp->m_iGroupNumSmall=40;
		psp->m_iHP=0;
		psp->m_iZP=0;
		psp->m_lPh=ph;

		sp.Add(psp);
	}

	sp.Save(pro.m_sShotPar);
	return;
}


void CMountView::OnTestResolve() 
{
	// TODO: Add your command handler code here
	CProject pro;
	if(!pro.Open("test3"))return ;
	if(!pro.OpenLine("test3"))return;
	pro.m_sizeGridMerge=CSize(2,2);
	pro.m_nGridSurface=1;

	CClientDC dc(this);
	if(!GResolveEquation(pro,20,1,CSize(20,10),&dc,trace))return;
	
}

void CMountView::OnTestEquation() 
{
	// TODO: Add your command handler code here
	CMemEquation e;
	e.Construct ("test3.A","test3.b",3);
	CObArray array;
	CBlockTrace *pBlock;

	pBlock=new CBlockTrace(0,1);
	array.Add(pBlock);
	pBlock=new CBlockTrace(0,1);
	array.Add(pBlock);
	pBlock=new CBlockTrace(0,1);
	array.Add(pBlock);
	e.AppeEqua(array,3);

	array.RemoveAll();
	pBlock=new CBlockTrace(1,1);
	array.Add(pBlock);
	pBlock=new CBlockTrace(1,1);
	array.Add(pBlock);
	pBlock=new CBlockTrace(1,1);
	array.Add(pBlock);
	e.AppeEqua(array,6);

	array.RemoveAll();
	pBlock=new CBlockTrace(2,1);
	array.Add(pBlock);
	pBlock=new CBlockTrace(2,1);
	array.Add(pBlock);
	pBlock=new CBlockTrace(2,1);
	array.Add(pBlock);
	e.AppeEqua(array,9);

	e.Close();

	e.Use("test3.a","test3.b");
	e.Resolve();

	double *p=e.GetX ();

	int m=0;		

}

void CMountView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	CRect rect=pInfo->m_rectDraw;

	CMyPoint org(150,500);

	if(trace.m_nGridXNumber==0)return;

	double nMaxSlow=0.0013;
	int nLargerX=rect.Width()/45;
	int nLargerY=nLargerX;
	long x1,y1,x2,y2;
	CString s;       

	CBrush *pOldBrush=pDC->SelectObject(&brush[0]);
	for(int  i=0;i<21;i++){
		for(int j=0;j<11;j++){
			BYTE flag=*(BYTE*)trace.m_dgByteGridFlag.Get(i,j);
			double slow=*(double*)trace.m_dgGridSlow.Get(i,j);
			int n=(int)(slow/nMaxSlow*255);
			/*
			if(flag==0)
				pDC->SelectObject(&brush[10]);
			else if(flag==1)
				pDC->SelectObject(&brush[100]);
			else
				pDC->SelectObject(&brush[200]);
		   	   */
			if(n<0)n=0;
			if(n>255)n=255;

			pDC->SelectObject(&brush[n]);
			
			x1=(i-0.5)*nLargerX+org.x;
			y1=(j-0.5)*nLargerY+org.y;
			x2=(i+0.5)*nLargerX+org.x;
			y2=(j+0.5)*nLargerY+org.y;

			int ss=int(slow*100000);
			s.Format("%i",ss);
			pDC->Rectangle(x1,y1,x2,y2);
			pDC->TextOut(x1,y1,s);
		}			    
	}

	CPen pen(PS_SOLID,0,RGB(255,255,255));
	CPen* pOldPen=pDC->SelectObject(&pen);
	
	trace.Explode(CMyPoint(0,0));
	for(i=0;i<10;i++){
		int j=i;
		CObArray& array =trace.GetRoute(CMyPoint(i,j));
		CMyPoint *p=(CMyPoint*)array[0];
		pDC->MoveTo(org.x+p->x*nLargerX,org.y+p->y*nLargerY);
		for(int k=1;k<array.GetSize();k++){
			p=(CMyPoint*)array[k];
			pDC->LineTo(org.x+p->x*nLargerX,org.y+p->y*nLargerY);
		}
	}
	
	for(i=10;i<=20;i++){
		int j=20-i;
		CObArray& array =trace.GetRoute(CMyPoint(i,j));
		CMyPoint *p=(CMyPoint*)array[0];
		pDC->MoveTo(org.x+p->x*nLargerX,org.y+p->y*nLargerY);
		for(int k=1;k<array.GetSize();k++){
			p=(CMyPoint*)array[k];
			pDC->LineTo(org.x+p->x*nLargerX,org.y+p->y*nLargerY);
		}
	}      

	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	
//	CScrollView::OnPrint(pDC, pInfo);
}



void CMountView::OnTestinitial() 
{
	// TODO: Add your command handler code here
	CProject pro;
	if(!pro.Open("test3"))return ;
	if(!pro.OpenLine("test3"))return;
	pro.m_sizeGridMerge=CSize(2,2);
	pro.m_nGridSurface=1;

	if(!GInitEquation(pro))return ;	
	
}

void CMountView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	
	CScrollView::OnLButtonDblClk(nFlags, point);

	trace.Shake(1);
	Invalidate();


}

void CMountView::OnTest4Project() 
{
	CProject pro;
	pro.m_sSvSys="test4.sys";
	CLineFile *plinefile=new CLineFile;
	plinefile->m_sLineName="test4";
	plinefile->m_sSurvey="test4.txt";
	plinefile->m_sWellDepth="test4.wd";
	plinefile->m_sFstBrk="test4.fbk";
	plinefile->m_sShotPar="test4.spp";
	plinefile->m_sSCData="test4.t";
	plinefile->m_sShotRcv="test4.sr";
	plinefile->m_sTrace="test4.trc";
	plinefile->m_sEndPoint="test4.end";
	plinefile->m_sUnform="test4.unf";
	plinefile->m_sFloat="test4.F";
	plinefile->m_sWellDepth="test4.wd";

	plinefile->m_nValidBeginPh=102000;
	plinefile->m_nValidEndPh=104000;

	pro.AddLine(plinefile);
	pro.Save("Test4");    
}

void CMountView::OnTest4Initial() 
{
	// TODO: Add your command handler code here
	CProject pro;
	if(!pro.Open("test4"))return ;
	if(!pro.OpenLine("test4"))return;
	pro.m_sizeGridMerge=CSize(2,2);
	pro.m_nGridSurface=1;

	if(!GInitEquation(pro))return ;		
}

void CMountView::OnTest4FstBrk() 
{
	// TODO: Add your command handler code here
	CProject pro;
	pro.Open("test4");
	pro.OpenLine( "test4");
					       
	long  PhB=pro.m_nValidBeginPh;
	long  PhE=pro.m_nValidEndPh;

	CNewNet net;
	net.Create(pro.m_sSurvey,PhB,PhE);

	CSize size(20,10);
	trace.Set(net.GetColNum(),net.GetRowNum(),20,1,size);

	double v1=1000,v2=2000,v3=3000,v4=4000,v5=5000;
	
	
	int i,j,k;

	for(i=0;i<8;i++){
		for(j=0;j<=i;j++){
			trace.SetVelocity(&v1,i,j);
		}
	}

	for(i=8;i<16;i++){
		for(j=0;j<=i;j++){
			trace.SetVelocity(&v2,i,j);
		}
	}
	for(i=16;i<20;i++){
		for(j=0;j<=i;j++){
			trace.SetVelocity(&v3,i,j);
		}
	}
	for(i=20;i<24;i++){
		for(j=0;j<=(40-i);j++){
			trace.SetVelocity(&v3,i,j);
		}
	}
	for(i=24;i<32;i++){
		for(j=0;j<=(40-i);j++){
			trace.SetVelocity(&v4,i,j);
		}
	}
	for(i=32;i<=40;i++){
		for(j=0;j<=(40-i);j++){
			trace.SetVelocity(&v5,i,j);
		}
	}
	
	
/*	
	// Line on the right up is y=40-x
	for(i=0;i<40;i++){
		for(j=0;j<=20;j++){
			if(j>(i-8)*20&&j<=i&&j<=(40-i)){
				trace.SetVelocity(&v1,i,j);
			}
		}
	}

	for(i=0;i<40;i++){
		for(j=0;j<=20;j++){
			if(j>(i-16)*20&&j<=(i-8)*20&&j<=(40-i)){
				trace.SetVelocity(&v2,i,j);
			}
		}
	}

	for(i=0;i<40;i++){
		for(j=0;j<=20;j++){
			if(j>(i-24)*20&&j<=(i-16)*20&&j<=(40-i)){
				trace.SetVelocity(&v3,i,j);
			}
		}
	}

	for(i=0;i<40;i++){
		for(j=0;j<=20;j++){
			if(j>(i-32)*20&&j<=(i-24)*20&&j<=(40-i)){
				trace.SetVelocity(&v4,i,j);
			}
		}
	}

	for(i=0;i<40;i++){
		for(j=0;j<=20;j++){
			if(j>(i-40)*20&&j<=(i-32)*20&&j<=(40-i)){
				trace.SetVelocity(&v4,i,j);
			}
		}
	}	
*/	
	CClientDC dc(this);
	CPen pen(PS_SOLID,0,RGB(0,255,0));
	dc.SelectObject(&pen);

	CFstBrk fstbrk;
	CSvSys svsys(pro.m_sSvSys);
	if(svsys.IsEmpty())return ;

	fstbrk.Open(pro.m_sFstBrk,svsys);

	CSvData sd;
	if(!sd.Use(pro.m_sSurvey))return;

	int nGI=svsys.m_iGroupInterval ;
	int nGroupNumSmall=svsys.m_iGroupNumSmall;
	int nGroupNumBig=svsys.m_iGroupNumBig;
	int nGapSmall=svsys.m_iGapSmall;
	int nGapBig=svsys.m_iGapBig;
	int nShotInterval=svsys.m_iShotInterval;
	
	int nLargerX=20;
	long nBrk=0;
	SFstBrkRecord brk;

	Invalidate();
	UpdateWindow();
	for(long ph=PhB;ph<=PhE;ph+=nShotInterval){
		int nGroupValidSmall=(ph-nGapSmall-PhB)/nGI+1;
		int nGroupPlusSmall=nGroupNumSmall-nGroupValidSmall;
		long nPhBeginSmall=PhB-nGroupPlusSmall*nGI;

		int nGroupValidBig=(PhE-ph-nGapBig)/nGI+1;
		if(nGroupValidBig>nGroupNumBig)
			nGroupValidBig=nGroupNumBig;
		int nGroupPlusBig=nGroupNumBig-nGroupValidBig;
		long nPhEndBig=ph+nGapBig+(nGroupValidBig-1)*nGI+nGroupPlusBig*nGI;

		int HBG=sd.GetEleOnPh(ph);
		if(HBG==-9999)return;

		CMyPoint pointSrc=net.ChgToNetPoint(ph,HBG);
		//trace.Explode(pointSrc,&dc);
		
		Invalidate();
		UpdateWindow();
		
		// This section make the first break of the no valid of the first half:
		long nShotFileNo=(ph-PhB)/nShotInterval+1;
		for(long phr=nPhBeginSmall;phr<PhB;phr+=nGI){
			brk.m_fFstBrk =0.001;
			brk.m_iTraceNo =(phr-nPhBeginSmall)/nGI+1;
			brk.m_lFileNo=nShotFileNo;

			fstbrk.Put (&brk,nBrk);
			nBrk++;
		}
		
		// This section  make the first break of the first half of the valid reciever:
		CMyPoint org(50,50);
		for(phr=PhB;phr<=ph-nGapSmall;phr+=nGI){
			CMyPoint pointNet=net.ChgToNetPoint(phr,sd.GetEleOnPh(phr));
			CObArray &array=trace.GetRoute(pointNet);
			if(array.GetSize()==0){
				AfxMessageBox("GetRoute returned 0 nodes!");
				return ;
			}

			/*CMyPoint *point=(CMyPoint*)array[0];
			dc.MoveTo(point->x*nLargerX+org.x,point->y*nLargerX+org.y);
			for(k=1;k<array.GetSize();k++){
				point=(CMyPoint*)array[k];
				dc.LineTo(point->x*nLargerX+org.x,point->y*nLargerX+org.y);
			}   */
		
			brk.m_fFstBrk =Calculate(pointSrc,pointNet);
			//brk.m_fFstBrk =*(double*)trace.m_dgDoubleTime.Get(pointNet.x,pointNet.y);
			brk.m_iTraceNo =(phr-PhB)/nGI+1+nGroupPlusSmall;
			brk.m_lFileNo=nShotFileNo;

			dc.MoveTo(pointSrc.x*nLargerX+org.x,pointSrc.y*nLargerX+org.y);
			dc.LineTo(m_pointMidRight.x/20*nLargerX+org.x,m_pointMidRight.y*nLargerX+org.y);
			dc.LineTo(pointNet.x*nLargerX+org.x,pointNet.y*nLargerX+org.y);	

		
			fstbrk.Put (&brk,nBrk);
			nBrk++;		
		}

		// This setion make the first break of the second half of the valid reciever:
		for(phr=ph+nGapBig;phr<=PhE;phr+=nGI){	      
			CMyPoint pointNet=net.ChgToNetPoint(phr,sd.GetEleOnPh(phr));
			CObArray &array=trace.GetRoute(pointNet);
			if(array.GetSize()==0){
				int m=0;
			}


			/*CMyPoint *point=(CMyPoint*)array[0];
			dc.MoveTo(point->x*nLargerX+org.x,point->y*nLargerX+org.y);
			for(k=1;k<array.GetSize();k++){
				point=(CMyPoint*)array[k];
				dc.LineTo(point->x*nLargerX+org.x,point->y*nLargerX+org.y);
			}  */
			
			brk.m_fFstBrk =Calculate(pointSrc,pointNet);
			//brk.m_fFstBrk =*(double*)trace.m_dgDoubleTime.Get(pointNet.x,pointNet.y);			
			brk.m_iTraceNo =nGroupNumSmall+(phr-ph-nGapBig)/nGI+1;
			brk.m_lFileNo=nShotFileNo;

			dc.MoveTo(pointSrc.x*nLargerX+org.x,pointSrc.y*nLargerX+org.y);
			dc.LineTo(m_pointMidRight.x/20*nLargerX+org.x,m_pointMidRight.y*nLargerX+org.y);
			dc.LineTo(pointNet.x*nLargerX+org.x,pointNet.y*nLargerX+org.y);	

		
			fstbrk.Put (&brk,nBrk);
			nBrk++;		
		}

		// this section make the first break of the second half of the notvalid recievers:
		for(phr=PhE;phr<nPhEndBig;phr+=nGI){
			brk.m_fFstBrk =0.001;
			brk.m_iTraceNo =nGroupNumSmall+(phr-ph-nGapBig)/nGI+1;
			brk.m_lFileNo=nShotFileNo;

			fstbrk.Put (&brk,nBrk);
			nBrk++;
		}
	}	

}

void CMountView::OnTest4Resolve() 
{
	// TODO: Add your command handler code here
	CProject pro;
	if(!pro.Open("test4"))return ;
	if(!pro.OpenLine("test4"))return;
	pro.m_sizeGridMerge=CSize(2,2);
	pro.m_nGridSurface=1;

	CClientDC dc(this);
	if(!GResolveEquation(pro,20,1,CSize(20,10),&dc,trace))return;
}

void CMountView::OnTest4ShotPar() 
{
	// TODO: Add your command handler code here
	CProject pro;
	if(!pro.Open("test4"))return ;
	if(!pro.OpenLine("test4"))return;

	CSvSys svsys;
	if(!svsys.Use(pro.m_sSvSys))return;

	CShotPar sp;
	SShotPar *psp;

	for(long ph=102000;ph<=104000;ph+=50){
		psp=new SShotPar;
		psp->m_iFileNo=(ph-101950)/50;
		psp->m_iGapBig=50;
		psp->m_iGapSmall=50;
		psp->m_iGroupNumBig=40;
		psp->m_iGroupNumSmall=40;
		psp->m_iHP=0;
		psp->m_iZP=0;
		psp->m_lPh=ph;

		sp.Add(psp);
	}

	sp.Save(pro.m_sShotPar);
	return;	
}

void CMountView::OnTest4ShotRecieve() 
{
	// TODO: Add your command handler code here
	CProject pro;
	if(!pro.Open("test4"))return ;
	if(!pro.OpenLine("test4"))return;

	CShotPar sp;
	if(!sp.Use(pro.m_sShotPar))return;

	CSvSys svsys;
	if(!svsys.Use(pro.m_sSvSys))return;

	CSvData sd;
	if(!sd.Use(pro.m_sSurvey))return ;
		
	CShotRecieve sr;
	
	long tr;
	SShotRecieve *psr;
	for(long ph=pro.m_nValidBeginPh ;
			ph<=pro.m_nValidEndPh;
			ph+=svsys.m_iShotInterval)
			{
				psr=new SShotRecieve;
				psr->ShotPh=ph;

				if(ph==pro.m_nValidBeginPh){
					psr->nBeginGroup[0]=pro.m_nValidBeginPh +svsys.m_iGapBig;
					psr->nEndGroup[0]=pro.m_nValidEndPh ;
					psr->nBeginGroup[0]=sp.GetRecieveNoOnPh(psr->ShotPh,psr->nBeginGroup[0],tr,svsys);
					psr->nEndGroup[0]=sp.GetRecieveNoOnPh(psr->ShotPh,psr->nEndGroup[0],tr,svsys);
				}
				else if(ph==pro.m_nValidEndPh){
					psr->nBeginGroup[0]=pro.m_nValidBeginPh;
					psr->nEndGroup[0]=pro.m_nValidEndPh - svsys.m_iGapSmall;
					psr->nBeginGroup[0]=sp.GetRecieveNoOnPh(psr->ShotPh,psr->nBeginGroup[0],tr,svsys);
					psr->nEndGroup[0]=sp.GetRecieveNoOnPh(psr->ShotPh,psr->nEndGroup[0],tr,svsys);
				}
				else{
					psr->nBeginGroup[0]=pro.m_nValidBeginPh;
					psr->nEndGroup[0]=ph-svsys.m_iGapSmall;
					psr->nBeginGroup[1]=ph+svsys.m_iGapBig;
					psr->nEndGroup[1]=pro.m_nValidEndPh;

					psr->nBeginGroup[0]=sp.GetRecieveNoOnPh(psr->ShotPh,psr->nBeginGroup[0],tr,svsys);
					psr->nBeginGroup[1]=sp.GetRecieveNoOnPh(psr->ShotPh,psr->nBeginGroup[1],tr,svsys);
					psr->nEndGroup[0]=sp.GetRecieveNoOnPh(psr->ShotPh,psr->nEndGroup[0],tr,svsys);
					psr->nEndGroup[1]=sp.GetRecieveNoOnPh(psr->ShotPh,psr->nEndGroup[1],tr,svsys);
				}
				sr.Add(psr);
	}

	sr.Save(pro.m_sShotRcv);

	return;	
}

void CMountView::OnTest4SvSys() 
{
	// TODO: Add your command handler code here
	CSvSys svsys;
	svsys.m_sArea="test4";
	svsys.m_sCrew="testcrew";
	svsys.m_iGroupInterval=50;
	svsys.m_iShotInterval=50;
	svsys.m_iGroupNumSmall=40;
	svsys.m_iGroupNumBig=40;
	svsys.m_iGapSmall=50;
	svsys.m_iGapBig=50;
	svsys.m_iFillVelocity=2000;

	CProject pro;
	pro.Open ("test4");
	pro.OpenLine ("test4");
	svsys.Save(pro.m_sSvSys);
}

double CMountView::Calculate(CMyPoint pointSrc, CMyPoint pointTar)
{
	pointSrc.x*=20;
	pointTar.x*=20;

	// TestLayer
	CMyPoint pointMid;

	double dTime=1000000;
	double dTimeTemp=0;
	
	dTime=1000000;	
	m_pointMidRight=CMyPoint(0,0);
	
	
	
	if(pointSrc.x<400){
		if(pointTar.x>400){
			for(double i=-1000;i<=20;i+=0.1){
				pointMid.x=500-5*i;
				pointMid.y=i;

				dTimeTemp=pointSrc.Distance(pointMid)/3000;
				dTimeTemp+=pointMid.Distance(pointTar)/2500;

				if(dTimeTemp<dTime){
					m_pointMidRight=pointMid;
					dTime= dTimeTemp;
				}
			}
		}
		else{
			m_pointMidRight=pointTar;
			dTime=pointSrc.Distance(pointTar)/3000;
		}
	}
	else{
		if(pointTar.x<400){
			for(double i=-1000;i<=20;i+=0.1){
				pointMid.x=460-3*i;
				pointMid.y=i;

				dTimeTemp=pointSrc.Distance(pointMid)/2500;
				dTimeTemp+=pointMid.Distance(pointTar)/3000;

				if(dTimeTemp<dTime){
					m_pointMidRight=pointMid;
					dTime= dTimeTemp;
				}
			}
		}
		else{
			m_pointMidRight=pointTar;
			dTime=pointSrc.Distance(pointTar)/2500;
		}
	}


	return dTime;
}

void CMountView::OnTestCac() 
{
	for(int i=20;i<40;i++){
		double t=Calculate(CMyPoint(0,0),CMyPoint(i,40-i));
	}
}

void CMountView::OnCalcStatic() 
{
	CSCData scdata;
}

void CMountView::On116Pro() 
{
	// TODO: Add your command handler code here
	CProject pro;
	pro.m_sSvSys="MZ98.SYS";

	CLineFile *pLine=new CLineFile;
	pLine->m_sLineName="MZ98116";
	pLine->m_sShotPar="MZ98116.SPP";
	pLine->m_sShotRcv="MZ98116.SR";
	pLine->m_sSurvey="MZ98116.TXT";
	pLine->m_sSCData="MZ98116.T";
	pLine->m_sFloat="MZ98116.F";   
	pLine->m_sUnform="MZ98116.JB";
	pLine->m_sFstBrk="MZ98116.FBK";
	pLine->m_sTrace="MZ98116.TRC";
	pLine->m_sEndPoint="MZ98116.END";  // Containning the two end points of the trace.
	pLine->m_sWellDepth="MZ98116.WD";
	pLine->m_nValidBeginPh=1640;    //599340;
	pLine->m_nValidEndPh=7960;       // 605660;
	pLine->m_sizeGridMerge=CSize(4,4);
	pLine->m_nGridSurface=1;		  // 

	pro.AddLine(pLine);

	pro.Save("MZ98116.PRO");
	
}

void CMountView::On116Sys() 
{
	// TODO: Add your command handler code here
	// TODO: Add your command handler code here
	CSvSys svsys;
	svsys.m_sArea="MZ";
	svsys.m_sCrew="2151";
	svsys.m_iGroupInterval=40;
	svsys.m_iShotInterval=40;
	
	// These parameters are to written according to the real  unformed data 
	// or the read data:
	svsys.m_iGroupNumSmall=40;
	svsys.m_iGroupNumBig=40;
	svsys.m_iGapSmall=40;
	svsys.m_iGapBig=40;	
	
	svsys.m_iFillVelocity=2000;

	CProject pro;
	pro.Open("MZ98116.pro");
	svsys.Save(pro.m_sSvSys);
	
}

void CMountView::On116Sr() 
{
	// TODO: Add your command handler code here
	CProject pro;
	if(!pro.Open("MZ98116.pro"))return;
	if(!pro.OpenLine ("MZ98116"))return;

	CSvSys svsys;
	if(!svsys.Use(pro.m_sSvSys))return;

	CSvData sd;
	if(!sd.Use (pro.m_sSurvey ))return;

	if(!sr.Make(pro))return;
	sr.Save(pro.m_sShotRcv);

	return;
}
