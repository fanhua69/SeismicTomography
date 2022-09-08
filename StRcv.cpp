
#include "stdafx.h"
#include "NewMount.h"
#include "StRcv.h"
#include "SvSys.h"
#include "ShotPar.h"
#include "SvData.h"
#include "ScData.h"
#include "StRcv.h"
#include "math.h"

CShotRecieve::CShotRecieve()
{
	m_nSectionLimit=100;
}

CShotRecieve::CShotRecieve(CString sFile,CShotPar& ShotPar,CSvSys &SvSys)
{
	m_nSectionLimit=100;
	if(!Open(sFile,ShotPar,SvSys)){
		Reset();
	}
}

int CShotRecieve::Add(SShotRecieve *psr)
{
	return m_array.Add(psr);
}

CShotRecieve::~CShotRecieve()
{
	Reset();
}

void CShotRecieve::Reset()
{
	m_array.RemoveAll();
}

bool CShotRecieve::Open(CString sFile,CShotPar& ShotPar,CSvSys &SvSys)
{
	FILE *file=fopen(sFile,"rt");
	if(!file){
		AfxMessageBox("当打开炮检点对文件时出错："+sFile);
		return false;
	}

	long i,j,n;
	SShotRecieve *psr;
	while(!feof(file)){
		psr=new SShotRecieve;

		fscanf(file,"%ld \n",&psr->ShotPh);
		for(j=0;j<100;j++){
			n=fscanf(file,"%ld %ld \n",&psr->nBeginGroup[j],&psr->nEndGroup[j]);
			if(n<2){
				AfxMessageBox("炮检对文件数据个数不对,请检查!");
				return false;
			}
		}
		m_array.Add(psr);
	}
	fclose(file);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 对所有炮循环:
	long TotalRecieveNum=SvSys.m_iGroupNumSmall+SvSys.m_iGroupNumBig;
	for(i=0;i<m_array.GetSize();i++){
		for(j=0;j<100;j++){
			psr=(SShotRecieve*)m_array[j];

			// 检查起止检波点是否颠倒,若否,则改正之:
			if(psr->nEndGroup[j]<psr->nBeginGroup[j]){
				long temp=psr->nEndGroup[j];
				psr->nEndGroup[j]=psr->nBeginGroup[j];
				psr->nBeginGroup[j]=temp;
			}

			// 判断起始-终止接收道是否在实际范围之内,若否,则改正之:
			if(psr->nEndGroup[j]<1){
				psr->nBeginGroup[j]=-1;
				psr->nEndGroup[j]=-1;
			}
			else if(psr->nBeginGroup[j]>TotalRecieveNum){
				psr->nBeginGroup[j]=-1;
				psr->nEndGroup[j]=-1;
			}
			else {
				if(psr->nBeginGroup[j]<1)
					psr->nBeginGroup[j]=1;
				if(psr->nEndGroup[j]>TotalRecieveNum)
					psr->nEndGroup[j]=TotalRecieveNum;
			}
		}  // 对100个炮检对循环.
	} // 对所有炮循环.
	
	return true;
}
//  Read(), End
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CShotRecieve::Save(CString sFile)
{
	FILE *file=fopen(sFile,"wb");
	if(!file){
		AfxMessageBox("无法建立炮检对文件:"+sFile);
		return false;
	}

	long i,j;
	SShotRecieve *psr;
	for(i=0;i<m_array.GetSize();i++){
		fprintf(file,"%ld \n",((SShotRecieve*)m_array[i])->ShotPh);
		psr=(SShotRecieve*)m_array[i];
		for(j=0;j<100;j++){

			// If the begin and end point no is reversed, then reverse them again:
			if(psr->nEndGroup[j]<psr->nBeginGroup[j]){
				long temp=psr->nEndGroup[j];
				psr->nEndGroup[j]=psr->nBeginGroup[j];
				psr->nBeginGroup[j]=temp;
			}

			fprintf(file,"%ld %ld\n",psr->nBeginGroup[j],psr->nEndGroup[j]);
		}
	}
	fclose(file);

	return true;
}

bool CShotRecieve::IsEmpty()
{
	if(m_array.GetSize()==0)
		return true;
	return false;
}

int CShotRecieve::GetSize()
{
	return m_array.GetSize();
}

SShotRecieve* CShotRecieve::Get(int nIndex)
{
	return (SShotRecieve* )m_array.GetAt(nIndex);
}

bool CShotRecieve::Make(CProject &pro)
{
	CSvSys svsys;
	if(!svsys.Use(pro.m_sSvSys))return false;

	CSvData sd;
	if(!sd.Use(pro.m_sSurvey))return false;	  
	
	// Search out the lowest and highest h and 
	//  make the threshold for valid S-R couple:
	double threshold=5; // define 5 meters is the threshold for not valid group:
	double threshold_this;

	// Make S-R couple according to if there is HBG 
	// that is too low (lower than the theshold) .		
	long rph,phmid,RB,RE;
	int nValidSection=0;
	SShotRecieve *pSR;
	double ShotHBG,RHBG,HBG_this;

	long nGroup,nTotalGroup=svsys.m_iGroupNumBig +svsys.m_iGroupNumSmall ;
	bool *bValid=new bool[nTotalGroup+1];
	bValid[nTotalGroup]=false;  // for the last section.

	for(long ph=pro.m_nValidBeginPh ;ph<pro.m_nValidEndPh ;ph+=svsys.m_iShotInterval ){
		
		ShotHBG=sd.GetEleOnPh (ph);
		pSR=new SShotRecieve;
		pSR->ShotPh =ph;
		
		// The front half:
		RB=ph-svsys.m_iGapSmall -(svsys.m_iGroupNumSmall -1)*svsys.m_iGroupInterval ;		
		RE=ph-svsys.m_iGapSmall ;

		for(rph=RB;rph<RE;rph+=svsys.m_iGroupInterval ){
			nGroup=(rph-RB)/svsys.m_iGroupInterval ;
			bValid[nGroup]=true;
			
			// The ph is out of the range eg. not on the mountain :
			if(rph<pro.m_nValidBeginPh ||rph>pro.m_nValidEndPh ){
				bValid[nGroup]=false;
				continue;
			}

			// The HBG on this Ph is lower than expected eg. blocked the trace:
			RHBG=sd.GetEleOnPh (rph);
			for(phmid=rph;phmid<ph;phmid+=svsys.m_iGroupInterval){
				threshold_this=RHBG+(ShotHBG-RHBG)/(ph-rph)*(phmid-rph)-threshold;
				HBG_this=sd.GetEleOnPh (phmid);
				if(HBG_this<threshold_this){
					bValid[nGroup]=false;
					break;
				}
			}
		}

		/// Check the back half:
		RB=ph+svsys.m_iGapBig;
		RE=ph+svsys.m_iGapBig+(svsys.m_iGroupNumBig -1)*svsys.m_iGroupInterval ;		

		for(rph=RB;rph<RE;rph+=svsys.m_iGroupInterval ){
			RHBG=sd.GetEleOnPh (rph);						
			nGroup=svsys.m_iGroupNumSmall+(rph-RB)/svsys.m_iGroupInterval+1;

			bValid[nGroup]=true;
			
			// The Ph is not on the mountain:
			if(rph<pro.m_nValidBeginPh ||rph>pro.m_nValidEndPh ){
				bValid[nGroup]=false;
				continue;
			}
			
			// The HBG on this Ph is lower than expected eg. blocked the trace:
			for(phmid=ph;phmid<rph;phmid+=svsys.m_iGroupInterval ){
				threshold_this=RHBG+(ShotHBG-RHBG)/(ph-rph)*(phmid-rph)-threshold;
				HBG_this=sd.GetEleOnPh (phmid);
				if(HBG_this<threshold_this){
					bValid[nGroup]=false;
					break;
				}
			}
		}

		// Turn the valid group into from what to what:
		int nValidSection=0,nStart=0,nEnd=0;
		while(true){			
			for(int i=nEnd;i<nTotalGroup+1;i++){
				nStart=i;
				if(bValid[i]){
					pSR->nBeginGroup[nValidSection]=i;
					break;
				}
			}
			
			for(i=nStart+1;i<nTotalGroup+1;i++){
				nEnd=i+1;
				if(!bValid[i]){
					pSR->nEndGroup[nValidSection]=i-1;
					nValidSection++;					
					break;
				}
			}
			if(nStart>=(nTotalGroup-1)||nEnd>=(nTotalGroup-1)){
				break;
			}
			if(nValidSection>=m_nSectionLimit){
				AfxMessageBox(" CShotRecieve Error: the quantity for valid section is too small!");
				return false;
			}
		} // while(true)		

		Add(pSR);
	}// The shot Ph : for(ph=pro.m_nValidBeginPh ;ph<pro.m_nValidEndPh ;ph+=svsys.m_iShotInterval ){					
	
	 delete []bValid;

	 return true;

}
