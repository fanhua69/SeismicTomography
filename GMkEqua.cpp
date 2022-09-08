
#include "stdafx.h"
#include "NewMount.h"
#include "MemEqua.h"
#include "Equation.h"
#include "FstBrk.h"
#include "ShotPar.h"
#include "StRcv.h"
#include "SvSys.h"
#include "SvData.h"	
#include "MyPoint.h"
#include "NewNet.h"
#include "math.h"
#include "DataGroup.h"
#include "Trace.h"


bool GInitEquation(CProject &project, CDC *pDC=NULL)
{

	/////////////////////////////////////////////////////////////////////////////////////
	//  一、 获得所需参数数据。
	/////////////////////////////////////////////////////////////////////////////////////
	//  获得观测系统：
	CSvSys SvSys(project.m_sSvSys);
	if(SvSys.IsEmpty())return false;		   	

	// 获得炮点参数：
	CShotPar ShotPar(project.m_sShotPar);
	if(!ShotPar.IsEmpty())return false;
	
	////////////////////////////////////////////////////////////////////////////////////////
	//  在使用炮检对时,必须检查其合法性,即是否为零.
	//  获得有效炮检对：
	CNewNet net;
	net.Create(project.m_sSurvey,project.m_nValidBeginPh,project.m_nValidEndPh);

	CShotRecieve SR(project.m_sShotRcv,ShotPar,SvSys);
	if(SR.IsEmpty())return false;

	// 获得测量数据：
	CSvData sd(project.m_sSurvey);
	if(sd.IsEmpty())return false;

	// 获得初至数据：
	CFstBrk fstbrk(project.m_sFstBrk,SvSys);
	if(fstbrk.IsEmpty())return false;
	
	long nFbkShot=fstbrk.GetShotNum();
	if(nFbkShot==-1)return false;

	// Containes the first breaks of one shot.
	int nShotGroup=SvSys.m_iGroupNumSmall+SvSys.m_iGroupNumBig;
	SFstBrkRecord *pShotFbk=new SFstBrkRecord[nShotGroup]; 
	SFbkIndex *pFbkIndex;  // to get the address of the target fbk index.
	
	// 建立一个文件，以存储获得的射线段：
	CString sEndPoint="EndPoint.$$$";
	
	// make the file of the trac:
	if(project.m_sTrace==""){
		AfxMessageBox("You must give me the name of the trace file!");
		return false;
	}
	FILE *fpTrace=fopen(project.m_sTrace,"wb");
	if(!fpTrace){
		AfxMessageBox("Can NOT create file "+project.m_sTrace);
		return false;
	}

	// Make the end point file:
	if(project.m_sEndPoint==""){
		AfxMessageBox("You must give me the name of the trace end point file!");
		return false;
	}
	FILE *fpEndPoint=fopen(project.m_sEndPoint,"wb");
	if(!fpTrace){
		AfxMessageBox("Can NOT create file "+project.m_sEndPoint);
		return false;
	}


	int Shot,i,j,k,n;
	long RPh;
	long FoundShot,FoundRecieve;
	float ShotHBG;
	CString ErrorString;


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	二、 计算炮检射线段起止坐标。
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// these 4 data is for the trace-end file:
	CMyPoint point;
	long nLenWind;
	double dFirstBreak;

	long LineSegNo=0;
	SSvRecord *pSvData;
	SShotRecieve *pShotRecieve;
	long nRevPoint;

	//  对有效炮循环	
	for(Shot=0;Shot<SR.GetSize();Shot++){

		//  在测量数据中寻找这一炮的桩号，以获得炮点的高程：
		pShotRecieve=SR.Get(Shot);
		ShotHBG=-1;
		for(i=0;i<sd.GetNum();i++){
			pSvData=sd.GetAt(i);
			if(pSvData->Ph==pShotRecieve->ShotPh){
				ShotHBG=pSvData->HBG;
				break;
			}
		}

		if(ShotHBG==-1){
			ErrorString.Format("%d",pShotRecieve[Shot].ShotPh);
			int n=AfxMessageBox("有效炮检对的炮点桩号在测量文件中没有发现！"+ErrorString,MB_ABORTRETRYIGNORE|MB_ICONEXCLAMATION);
			if(n==IDABORT){
				delete pShotFbk;
				return false;
			}
			continue;
		}
		
		//在炮点参数中查找该炮的桩号,以获得该炮的文件号：
		FoundShot=-1;
		for(i=0;i<ShotPar.GetNum();i++){
			if(ShotPar.GetAt(i)->m_lPh==pShotRecieve->ShotPh){
				FoundShot=i;
				break;
			}
		}
		if(FoundShot==-1){
			ErrorString.Format("%d",pShotRecieve[Shot].ShotPh);
			int n=AfxMessageBox("有效炮点"+ErrorString+"在你的炮点参数文件中没有发现！",MB_ABORTRETRYIGNORE|MB_ICONEXCLAMATION);
			if(n==IDABORT)	{
				delete pShotFbk;
				return false;
			}
			continue;
		}

		//  获得该炮的所有道的初至：
		pShotFbk[0].m_lFileNo=-1;
		for(i=0;i<fstbrk.GetShotNum();i++){
			pFbkIndex=fstbrk.GetIndex(i);
			if(pFbkIndex->m_lFileNo==ShotPar.GetAt(FoundShot)->m_iFileNo){
				fstbrk.Get(pShotFbk,pFbkIndex->m_lFilePos,nShotGroup);
				break;
			}
		}

		if(pShotFbk[0].m_lFileNo==-1){
			ErrorString.Format("%d", ShotPar.GetAt(FoundShot)->m_iFileNo);
			n=AfxMessageBox("与炮检对相对应的初至没有发现 , 炮点文件号为:"+ErrorString,MB_ABORTRETRYIGNORE|MB_ICONEXCLAMATION);
			if(n==IDABORT){
				delete pShotFbk;
				return false;
			}
			continue;
		}

		// Write the shot point into the trave end file 
		// which include the shot point and the rev points 
		// that recieve its front wave.
		point.x=pShotRecieve->ShotPh;   // the Shot point Ph.
		point.y=ShotHBG;   // the shot point's HBG.
		net.ChgToNetPoint (point);
		fwrite(&point,sizeof(CMyPoint),1,fpEndPoint);
		nRevPoint=0;
		fwrite(&nRevPoint,sizeof(long),1,fpEndPoint);
		nLenWind=0;  // to record how many bytes were winded after the recieve points of one shot have been recorded into the End file.

		//  对每个有效炮检对的10个部分循环：
		for(i=0;i<10;i++){
			if(pShotRecieve->nBeginGroup[i]==-1)break;

			// 对每个部分中的所有道循环：由于道号编制是从1到道数，所以均减一。
			for(j=pShotRecieve->nBeginGroup[i]-1;j<=pShotRecieve->nEndGroup[i]-1;j++)
			{
				//  计算该检波点的桩号：
				RPh=ShotPar.GetPhOnRecieveNo(pShotRecieve->ShotPh,j+1,SvSys);
				if(RPh==-1){
					int n=AfxMessageBox("GMakeEquation 报错：检波点序号非法！",MB_ABORTRETRYIGNORE|MB_ICONEXCLAMATION);
					if(n==IDABORT){
						delete pShotFbk;
						return false;
					}
					continue;
				}

				//  在测量数据中寻找这一道：
				FoundRecieve=-1;
				for(k=0;k<sd.GetNum();k++){
					if(sd.GetAt(k)->Ph==RPh){
						FoundRecieve=k;
						break;
					}
				}
				if(FoundRecieve==-1)continue;

				// 将该射线段存入文件：
				point.x=2.0;   // the point number in this trace .
				point.y=pShotFbk[j].m_fFstBrk;   // the first break of this trace.
				fwrite(&point,sizeof(CMyPoint),1,fpTrace);
				
				point.x=pShotRecieve->ShotPh;   // the Shot point Ph.
				point.y=ShotHBG;   // the shot point's HBG.
				net.ChgToNetPoint (point);
				fwrite(&point,sizeof(CMyPoint),1,fpTrace);

				point.x=sd.GetAt(FoundRecieve)->Ph;   // the recieve point Ph.
				point.y=sd.GetAt(FoundRecieve)->HBG;  
				net.ChgToNetPoint (point);
				fwrite(&point,sizeof(CMyPoint),1,fpTrace);

				// Write the data to the end point file:
				nRevPoint++;
				dFirstBreak=pShotFbk[j].m_fFstBrk;
				fwrite(&point,sizeof(CMyPoint),1,fpEndPoint);
				fwrite(&dFirstBreak,sizeof(double),1,fpEndPoint);
				nLenWind+=sizeof(CMyPoint)+sizeof(double);
			}  //  对道循环。
		}  // 对10个炮检对循环。
		
		// rewind to the position that record the number of recieve points, 
		// write the recieve point number and wind forward to now here:
		fseek(fpEndPoint,-nLenWind-sizeof(long),SEEK_CUR);   // the sizeof(long) is for the empty recieve point number that has been writen into the fiel.
		fwrite(&nRevPoint,sizeof(long),1,fpEndPoint);
		fseek(fpEndPoint,nLenWind,SEEK_CUR);
	} // 对有效炮循环。

	fclose(fpTrace);        // The trace file and the end point file are same in the beginning
	fclose(fpEndPoint);   // 关闭存储炮检射线的文件。
	delete pShotFbk;

	return true;
}


bool GResolveEquation(CProject &project,int nGridWidth,int nGridHeight, CSize sizeScan, CDC *pDC,CTrace &trace)
{
#ifdef _DEBUG
	CMemoryState OriginalState;
	OriginalState.Checkpoint();
#endif

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// A CString that will be used by all of the subroutine.
	CString sError="";

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Check if the user directly used this function:
	if(project.m_sLineName==""){
		AfxMessageBox(" You must open a line of the project before you resolve the equation!");
		return false;
	}		

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////
	///////        This is a big loop, to make equation and to resolve it and to trace it and goto the beginning:
	///////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CNewNet net;
	net.Create(project.m_sSurvey,project.m_nValidBeginPh,project.m_nValidEndPh);

//      CTrace trace;
	trace.Set(net.GetColNum(),net.GetRowNum(),nGridWidth,nGridHeight,sizeScan); // the trace that will be used by the big loop.

	double *pVelocityLast=new double[net.GetNetNum()];  // this is for saving the velocity to compare with the velocity got from next loop.

	long i,j,nBigLoop=0;
	do{
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//  Make the equation according to the trace file project.m_sTrace
		//  which is made by GInitEquation for the first time , and by this 
		//  cycle in the later times.
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		CMemEquation Equation;
		if(!Equation.Construct(
			project.m_sLineName+".A",
			project.m_sLineName+".B",
			net.GetNetNum()))goto tagLoopOver;

		FILE *fpTrace=fopen(project.m_sTrace,"rb");
		CMyPoint pointHead,pointBody,pointBodyLast;

		double emptyTime;
		CObArray arrayTotalRoute;
		CBlockTrace *pBlockTrace;

		//  Do loop  while not feof(fpTrace):
		while(true){
			arrayTotalRoute.RemoveAll();
			
			// pointHead.x , saves the block number of this trace.
			// pointHead.y, saves the travelled time of this trace.
			int n=fread(&pointHead,sizeof(CMyPoint),1,fpTrace);
			if(n==0){
				break;
			}

			fread(&pointBody,sizeof(CMyPoint),1,fpTrace);
			for(i=1;i<pointHead.x;i++){
				pointBodyLast=pointBody;
				fread(&pointBody,sizeof(CMyPoint),1,fpTrace);

				CObArray &traceRoute=trace.GetStraightRoute(pointBodyLast,pointBody,emptyTime);
				for(j=0;j<traceRoute.GetSize();j++){
					pBlockTrace=new CBlockTrace(((CBlockTrace*)traceRoute[j])->m_nBlock,((CBlockTrace*)traceRoute[j])->m_dLen);
					arrayTotalRoute.Add(pBlockTrace);
				}
			}			

			if(!Equation.AppeEqua(arrayTotalRoute,(double)pointHead.y)){
				goto tagLoopOver;
			}			
		}	

		// Append some equations according to the grids 
		// that the user want to merge.
		// Merging  some grids:
		CSize size=project.m_sizeGridMerge;
		int nSur=project.m_nGridSurface;  
		 long nLow,nHigh,m,n,nx,ny;
		 bool bValid;
		 long nGridCanMerge=0;
		 pBlockTrace=new CBlockTrace[size.cx*size.cy+trace.m_nGridXNumber];
		 arrayTotalRoute.SetSize(2);
		/*
		 // Merge the bottom line of grids:
		for(m=1;m<trace.m_nGridXNumber-2;m++){
			pBlockTrace[m].m_nBlock=trace.GetGridNo(m,0);
			pBlockTrace[m].m_dLen=1;

			pBlockTrace[m+1].m_nBlock=trace.GetGridNo(m+1,0);
			pBlockTrace[m+1].m_dLen=-1;

			arrayTotalRoute[0]=&pBlockTrace[m];
			arrayTotalRoute[1]=&pBlockTrace[m+1];
			if(!Equation.AppeEqua(arrayTotalRoute,0)){
				delete []pBlockTrace;
				goto tagLoopOver;
			}
		}

		 // Merge the second bottom line of grids:
		for(m=1;m<trace.m_nGridXNumber-2;m++){
			pBlockTrace[m].m_nBlock=trace.GetGridNo(m,1);
			pBlockTrace[m].m_dLen=1;

			pBlockTrace[m+1].m_nBlock=trace.GetGridNo(m+1,1);
			pBlockTrace[m+1].m_dLen=-1;

			arrayTotalRoute[0]=&pBlockTrace[m];
			arrayTotalRoute[1]=&pBlockTrace[m+1];
			if(!Equation.AppeEqua(arrayTotalRoute,0)){
				delete []pBlockTrace;
				goto tagLoopOver;
			}
		}

		*/
		// Merge for size of grids:
		for(i=0;i<trace.m_nGridXNumber;i+=size.cx){
			for(j=0;j<trace.m_nGridYNumber;j+=size.cy){
				nGridCanMerge=0;
				for(m=0;m<size.cx;m++){
					for(n=0;n<size.cy;n++){
						nx=i+m;
						ny=j+n;
						if(nx<trace.m_nGridXNumber &&ny<trace.m_nGridYNumber){
							pBlockTrace[nGridCanMerge].m_nBlock=trace.GetGridNo(nx,ny);
							pBlockTrace[nGridCanMerge].m_dLen=1;
							nGridCanMerge++;
						}
					}
				}

				// Do NOT care those grids that are useless:
				bValid=true;
				for(m=0;m<nGridCanMerge;m++){
					if(Equation.IsVarVacant (pBlockTrace[m].m_nBlock)){
						bValid=false;
						break;
					}
				}
				if(!bValid)continue;

				// X(m)-X(m+1)=0				
				for(m=0;m<nGridCanMerge-1;m++){
					pBlockTrace[m].m_dLen=1;
					pBlockTrace[m+1].m_dLen=-1;
					arrayTotalRoute[0]=&pBlockTrace[m];
					arrayTotalRoute[1]=&pBlockTrace[m+1];
					if(!Equation.AppeEqua(arrayTotalRoute,0)){
						delete []pBlockTrace;
						goto tagLoopOver;
					}
				}
			}
		}

		delete []pBlockTrace;
		Equation.Close(); 

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Resoleve the equation:
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if(!Equation.Use(project.m_sLineName+".A",project.m_sLineName+".B")){
			AfxMessageBox("Can NOT open the equation file!");
			goto tagLoopOver;
		}
		Equation.ReadX();
		Equation.Resolve();
		if(!Equation.SaveX()){
			if(AfxMessageBox(" Error when saving the resolved data, Continue? !",MB_YESNO|MB_ICONEXCLAMATION)==IDNO){
				goto tagLoopOver;
			}
		} 

		// Check if the precision have reached nessecary:
		double *dVelocity=Equation.GetX();		
		double dError=0;
		for(i=0;i<net.GetColNum();i++){
			dError+=fabs(pVelocityLast[i]-dVelocity[i]);
		}
		dError/=net.GetColNum();
		if(dError<0.000001){
			AfxMessageBox("Resolving equation over!");
			break;
		}
		
		// Record current velocity:
		for(i=0;i<net.GetColNum();i++){
			pVelocityLast[i]=dVelocity[i];
		}
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//  Trace the net and get the new route trace:
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
		//  Give velocity of the trace:
		double dSlow;
		for(i=0;i<net.GetColNum();i++){
			if(i==10||i==11){
				int m=0;
			}
			for(j=0;j<net.GetRowNum();j++){


				dSlow=dVelocity[trace.GetGridNo(i,j)];
				trace.SetSlow(&dSlow,i,j);
			}
		}
		trace.SetEmptySlow(Equation.m_dVacantValue );

		return true ;

		//trace.Shake(2);
		
		// Open two nessesary file;		
		fpTrace=fopen(project.m_sTrace,"wb");
		FILE *fpEnd=fopen(project.m_sEndPoint,"rb");

		// Trace:
		CMyPoint pointSrc,pointDest;
		long nRcvPoint,nRead;
		double dFstbrk;
		
		// Do loop while !feof(fpEnd):
		while(true){
			
			// Read the source point and check it:
			nRead=fread(&pointSrc,sizeof(CMyPoint),1,fpEnd);
			if(nRead<1){
				break;
			}
			if(pointSrc.x<0||pointSrc.x>=trace.m_nGridXNumber||pointSrc.y<0||pointSrc.y>=trace.m_nGridYNumber){
				sError.Format("%lf,%lf",pointSrc.x,pointSrc.y);
				AfxMessageBox("Error when tracing :  the source point is out of scope! "+sError);
				goto tagLoopOver;
			}

			// Read the number of the reciever point:
			nRead=fread(&nRcvPoint,sizeof(long),1,fpEnd);
			if(nRead<1){
				AfxMessageBox(" The End point File error!");
				goto tagLoopOver;
			}
		
			// Explode at the source point:
			trace.Explode(pointSrc);

			//   Get route according to every reciever point:
			for(i=0;i<nRcvPoint;i++){
				
				// Read the recieve point and check it:
				fread(&pointDest,sizeof(CMyPoint),1,fpEnd);
				fread(&dFstbrk,sizeof(double),1,fpEnd);

				if(pointDest.x<0||pointDest.x>=trace.m_nGridXNumber||pointDest.y<0||pointDest.y>=trace.m_nGridYNumber){
					sError.Format("%lf,%lf",pointDest.x,pointDest.y);
					AfxMessageBox("Error when tracing :  the reciever point is out of scope! "+sError);
					goto tagLoopOver;
				}

				// Get the route from the source point to the recieve point:
				CObArray &array=trace.GetRoute(pointDest);
				pointHead.x=array.GetSize();
				pointHead.y=dFstbrk;
				fwrite(&pointHead,sizeof(CMyPoint),1,fpTrace);

				// Draw the point and save the route points into the file:
				CMyPoint *pp;
				CSize size(50,50);
				pDC->MoveTo(pointDest.x+size.cx,pointDest.y+size.cy);

				int nLarger=40;
				for(j=0;j<array.GetSize();j++){
					pp=(CMyPoint*)array[j];
					pDC->LineTo(pp->x*nLarger+size.cx,pp->y*nLarger+size.cy);
					fwrite((CMyPoint*)array[j],sizeof(CMyPoint),1,fpTrace);
				}
			}
		}

		fclose(fpTrace);
		nBigLoop++;

		// Ask the user if he want to exit the big loop:
		CString sLoop;
		sLoop.Format("%i",nBigLoop);
		if(AfxMessageBox("We have done the Big Loop of "+sLoop+", Do you want to continue?",MB_YESNO)==IDNO)
			break;

	}while(true);

tagLoopOver:	
	//  release the memory having been occupied:
	delete []pVelocityLast;


	#ifdef _DEBUG
		CMemoryState FinalState;
		FinalState.Checkpoint();

		CMemoryState Diff;

		if(!Diff.Difference(OriginalState,FinalState)){
			AfxMessageBox("有内存没有回收!");
			return false;
		}
	#endif
	
	//  Resolve the problem over.
	if(sError!="")
		return false;
	else
		return true;
}


	