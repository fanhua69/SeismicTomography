
#include "stdafx.h"
#include "NewMount.h"
#include "ShotPar.h"
#include "math.h"

CShotPar::CShotPar()
{
}
CShotPar::CShotPar(CString sFile)
{
	Use(sFile);
}

CShotPar::~CShotPar()
{
	m_array.RemoveAll();
}

long CShotPar::GetNum()
{
	return m_array.GetSize();
}

bool CShotPar::IsEmpty()
{
	if(m_array.GetSize()>0)
		return true;
	return false;
}

bool CShotPar::Use(CString sFile)
{
	if(sFile==""){
		AfxMessageBox("CShotPar::Use(CString) -- You gave me a empty file name!");
		return false;
	}
	FILE *file=fopen(sFile,"rt");
	if(!file){
		AfxMessageBox("当打开炮点参数文件时发生错误："+sFile);
		return false;
	}

	int n=0;
	SShotPar *pShotPar;
	while(!feof(file)){
		pShotPar=new SShotPar;
		n=fscanf(file,"%ld  %d %d %d %d %d %d %d\n",
			&(pShotPar->m_lPh),
			&(pShotPar->m_iFileNo),
			&(pShotPar->m_iZP),
			&(pShotPar->m_iHP),
			&(pShotPar->m_iGroupNumSmall),
			&(pShotPar->m_iGapSmall),
			&(pShotPar->m_iGapBig),
			&(pShotPar->m_iGroupNumBig)
		);
		if(n<6){
			AfxMessageBox("当从炮点参数文件中读入数据时，数据数目不对："+sFile);
			return false;
		}
		m_array.Add(pShotPar);
	}

	return TRUE;

}

bool CShotPar::Save(CString  sFile)
{
	FILE *file=fopen(sFile,"wt");
	if(!file){
		AfxMessageBox("当建立炮点参数文件时发生错误："+sFile);
		return false;
	}

	SShotPar *pShotPar;
	int n=0;
	for(long i=0;i<m_array.GetSize();i++){
		pShotPar=(SShotPar*)m_array.GetAt(i);
		n=fprintf(file,"%ld  %d %d %d %d %d %d %d\n",
			pShotPar->m_lPh,
			pShotPar->m_iFileNo,
			pShotPar->m_iZP,
			pShotPar->m_iHP,
			pShotPar->m_iGroupNumSmall,
			pShotPar->m_iGapSmall,
			pShotPar->m_iGapBig,
			pShotPar->m_iGroupNumBig
		);
		
		if(n<6){
			AfxMessageBox("当向炮点参数文件中存入数据时出错:"+sFile);
			return false;
		}
	}

	fclose(file);

	return true;
}
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    	CShotPar::GetPhOnRecieveNo									 
//     在涉及到检波点的函数中，检波点被认为从1到检波点总数，
//     如从1到240， 而不是0到239， 以便与习惯相符。
long CShotPar::GetPhOnRecieveNo(long ShotPh,int RecieveNo,CSvSys &SvSys)
{
	// 在炮点参数中寻找炮检对的炮点桩号:
	long nShot=-1;
	for(long j=0;j<m_array.GetSize();j++){
		if(((SShotPar*)m_array.GetAt(j))->m_lPh==ShotPh){
			nShot=j;
			break;
		}
	}
	if(nShot==-1){
		CString s;
		s.Format("%d",ShotPh);
		AfxMessageBox("CShotPar::GetPhOnRecieveNo报错："
				"炮点桩号在炮点参数中没有找到:"+s);
		return -1;
	}
	
	//  若检波点序号非法，则返回-1。
	SShotPar *pShotPar=(SShotPar *)m_array.GetAt(nShot);
	if(RecieveNo>(pShotPar->m_iGroupNumBig+pShotPar->m_iGroupNumSmall)||RecieveNo<1){
		CString s;
		s.Format("%d",RecieveNo);
		AfxMessageBox("CShotPar::GetPhOnRecieveNo 报错："
				"所给检波点序号非法："+s);
		return -1;
	}

	int GroupSmall=pShotPar->m_iGroupNumSmall;
	int GroupBig=pShotPar->m_iGroupNumBig;
	int GapSmall=pShotPar->m_iGapSmall;
	int GapBig=pShotPar->m_iGapBig;
	int GI=SvSys.m_iGroupInterval;

	//  分三种情况求检波点的桩号：
	long Ph;
	if(GroupSmall==0)
		Ph=ShotPh+GapBig+(RecieveNo-1)*GI;
	else if(GroupBig==0)
		Ph=ShotPh-GapSmall-(GroupSmall-RecieveNo)*GI;
	else{
		if(RecieveNo<=GroupSmall)
			Ph=ShotPh-GapSmall-(GroupSmall-RecieveNo)*GI;
		else
			Ph=ShotPh+GapBig+(RecieveNo-GroupSmall-1)*GI;
	}

	return Ph;
}

long CShotPar::GetRecieveNoOnPh(long ShotPh,long RecievePh,long& TotalRecieveNum,CSvSys &SvSys)
{
	
	//  Test if the survey system is valid:
	if(SvSys.IsEmpty()){
		AfxMessageBox("The Survey system must be constructed firstly!");
		return -1;
	}

	// 在炮点参数中寻找炮检对的炮点桩号:
	long nShot=-1;
	SShotPar* pRecord;
	for(long j=0;j<m_array.GetSize();j++){
		pRecord=(SShotPar* )m_array[j];
		if(pRecord->m_lPh==ShotPh){
			nShot=j;
			break;
		}
	}
	if(nShot==-1){
		CString s;
		s.Format("%d",ShotPh);
		AfxMessageBox("CShotPar::GetPhOnRecieveNo报错："
				"炮点桩号在炮点参数中没有找到:"+s);
		return -1;
	}
	  
	SShotPar *pShotPar=(SShotPar *)m_array.GetAt(nShot);
	int GroupSmall=pShotPar->m_iGroupNumSmall;
	int GroupBig=pShotPar->m_iGroupNumBig;
	int GapSmall=pShotPar->m_iGapSmall;
	int GapBig=pShotPar->m_iGapBig;
	int GI=SvSys.m_iGroupInterval;

	TotalRecieveNum=GroupSmall+GroupBig;

	//  不检查检波点号的合法性，因为有起止检波点的问题，
	//   由调用者检查合法性：
	long DPh=RecievePh-ShotPh;
	long RecieveNo;
	if(DPh<0){
		RecieveNo=GroupSmall-(fabs(DPh)-GapSmall)/GI;
	}
	else{
		RecieveNo=GroupSmall+1+(DPh-GapBig)/GI;
	}

	return RecieveNo;
}

int  CShotPar::Add(SShotPar* pShotPar)
{
	return m_array.Add(pShotPar);
}
