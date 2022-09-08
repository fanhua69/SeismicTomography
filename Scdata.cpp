
#include "stdafx.h"
#include "NewMount.h"
#include "Project.h"
#include "SCData.h"
#include "SvSys.h"
#include "NewNet.h"
#include "WellDep.h"
#include "FltData.h"
#include "SvData.h"
#include "Equation.h"

CSCData::CSCData()
{
	m_pProject=NULL;
}

bool CSCData::Open(CProject *project)
{
	Reset();
	m_pProject=project;
	if(!Read())
		return false;
	else
		return true;
}

CObArray& CSCData::GetData()
{
	return m_array;	
}

CSCData::~CSCData()
{
	Reset();
}

void CSCData::Reset()
{
	m_array.RemoveAll();
	m_pProject=NULL;
}

bool CSCData::Calculate(CDC *pDC)
{
	// 检测线名是否存在：
	if(m_pProject->m_sLineName==""){
		AfxMessageBox("计算静校正量前，你必须调用Create函数给出工程名！");
		return false;
	}
	
	// 打开观测系统参数：需要其中的填充速度：
	CSvSys SS(m_pProject->m_sSvSys);
	if(SS.IsEmpty()){
		AfxMessageBox("CSCData::Calculate Error: You must firstly give me the survey system parameters!");
		return false;
	}

	// 打开浮动基准面文件：
	CFloatData FloatData;
	if(!FloatData.Use(m_pProject->m_sFloat)){
		AfxMessageBox("CSCData::Calculate Error: You must firstly give the float data file!");
		return false;
	}

	// 打开井深文件：
	CWellDepth WellDepth;
	if(!WellDepth.Use(m_pProject->m_sWellDepth)){
		AfxMessageBox("CSCData::Calculate Error: You must ly give me the well depth file!");
		return false;
	}

	// 获取测量数据：
	CSvData SvData;
	if(!SvData.Use(m_pProject->m_sSurvey)){
		AfxMessageBox("CSCData::Calculate Error: You must firstly give me the survey data file!");
		return false;
	}
		
	// 打开方程文件,用于获得速度模型:
	CNewNet net;
	if(!net.Use(m_pProject->m_sSurvey)){
		AfxMessageBox("CSCData::Calculate Error: You must firstly calculate the slowness and save if by CNewNet!");
		return false;
	}
	double *pSlow=net.GetSlow();

	// 大循环以计算静校正量：
	float WD,F,HBG;
	double TS,TR,Slowness;
	int i,j;

	// 高速层顶面：
	float GD;
	float LowLevel=net.GetLowY();

	CMyPoint Point;
	CBlockTrace *pBlock;
	SSvRecord *pSvRecord;
	SSCRecord *pSCRecord;
	CString sError;
	
	long Ph;
	for(i=0;i<SvData.GetNum();i++){
		pSvRecord=SvData.GetAt(i);
		Ph=pSvRecord->Ph;
		HBG=pSvRecord->HBG;
		if(Ph<m_pProject->m_nValidBeginPh||Ph<=m_pProject->m_nValidEndPh){
			continue;
		}
		
		WD = WellDepth.GetOnPh(Ph);
		if(WD==-1){
			sError.Format("%ld",Ph);			
			AfxMessageBox("CSCData::Calculate Error: Can NOT find the well depth data of "+sError);
			return false;
		}

		F=FloatData.GetOnPh(Ph);
		if(F==-1){
			sError.Format("%ld",Ph);			
			AfxMessageBox("CSCData::Calculate Error: Can NOT find the float data of "+sError);
			return false;
		}

		// 求取炮点静校正量,GD暂时用于表示一个最低面：
		GD=LowLevel;
		CObArray&trace=net.GetTrace(CMyPoint(Ph,HBG-WD),CMyPoint(Ph,GD));

		TS=0.0;
		for(j=trace.GetSize()-1;j>=0;j--){
			pBlock=(CBlockTrace*)trace[j];
			if(pBlock->m_nBlock==-1||pSlow[pBlock->m_nBlock]==MOUNT_ERROR){
				GD=net.GetNetPoint(pBlock->m_nBlock).y+net.GetColSpacing();
				break;
			}					
			else{
				Slowness=pSlow[pBlock->m_nBlock];
				TS-=pBlock->m_dLen*Slowness;  // Is this len right?
			}			
		}

		TS+=(F-GD)/(SS.m_iFillVelocity/1000);

		// 求取检波点静校正量:
		GD=LowLevel;
		CObArray& traceRcv=net.GetTrace(CMyPoint(Ph,HBG),CMyPoint(Ph,GD));

		TR=0.0;
		for(j=traceRcv.GetSize()-1;j>=0;j--){
			pBlock=(CBlockTrace*)traceRcv[j];
			if(pBlock->m_nBlock==-1||pSlow[pBlock->m_nBlock]==MOUNT_ERROR){
				GD=net.GetNetPoint(pBlock->m_nBlock).y+net.GetColSpacing();
				break;
			}					
			else{
				Slowness=pSlow[pBlock->m_nBlock];
				TR-=pBlock->m_dLen*Slowness;
			}			
		}

		TR+=(F-GD)/(SS.m_iFillVelocity/1000);


		pSCRecord=new SSCRecord;
		pSCRecord->Set(Ph,
			pSvRecord->XZB,
			pSvRecord->YZB,
			HBG,GD,F,TS,TR);

		m_array.Add(pSCRecord);

	}

	Save();
	
	return true;
}

bool CSCData::Save()
{

	FILE *fp=fopen(m_pProject->m_sSCData,"wt");
	if(!fp){
		AfxMessageBox("CSCData::Save 报错:当建立静校正量文本文件时出错:"+m_pProject->m_sSCData);
		return false;
	}

	SSCRecord *pRecord;
	for(long i=0;i<m_array.GetSize();i++){
		pRecord=(SSCRecord*)m_array[i];
		fprintf(fp,"%7ld%9.1lf%10.1lf%6.1f%6.1f%6.1f%4.0d%4.0d\n",
			pRecord->Ph,pRecord->XZB,pRecord->YZB,
			pRecord->HBG,pRecord->GD,pRecord->F,
			pRecord->TS,pRecord->TR);
	}

	fclose(fp);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
bool CSCData::Read()
{
	// 打开文件:
	FILE *fp=fopen(m_pProject->m_sSCData,"rt");
	if(!fp){
		AfxMessageBox("当读取静校正量文本文件时出错:"+m_pProject->m_sSCData);
		return false;
	}		   

	//  读取文件:
	SSCRecord *pRecord;
	while(!feof(fp)){
		pRecord=new SSCRecord;
		fscanf(fp,"-7.0f-9.1lf-10.1lf-6.1f-6.1f-6.1f-4.0d-4.0d",
			pRecord->Ph,&pRecord->XZB,pRecord->YZB,
			pRecord->HBG,&pRecord->GD,pRecord->F,
			pRecord->TS,pRecord->TR);
	}

	fclose(fp);
 
	return true;
}