
#include "stdafx.h"
#include "NewMount.h"
#include "FltData.h"

CFloatData::CFloatData()
{
}

CFloatData::CFloatData(CString sFile)
{
	Use(sFile);
}

bool CFloatData::Use(CString sFile)
{
	Reset();
	if(!Read(sFile))Reset();
	return true;
}

bool CFloatData::Read(CString sFile)
{
	FILE *fp=fopen(sFile,"rt");
	if(!fp){  // May be the user is constructing this file.
		return false;
	}

	SFloatRecord *pRecord;	
	while(!feof(fp)){
		fscanf(fp,"%ld %f\n",&pRecord->Ph,&pRecord->F);
		pRecord=new SFloatRecord ;
		m_array.Add(pRecord);
	}

	fclose(fp);

	return true;
}

bool CFloatData::Save(CString sFile)
{
	FILE *fp=fopen(sFile,"wt");
	if(!fp){
		AfxMessageBox("当建立浮动基准面文件时出错:"+sFile);
		return false;
	}

	SFloatRecord *pRecord;
	for(long i=0;i<m_array.GetSize();i++){
		pRecord=(SFloatRecord*)m_array.GetAt(i);
		fprintf(fp,"%ld %f\n",pRecord->Ph,pRecord->F);
	}

	fclose(fp);

	return true;
}

float CFloatData::GetOnPh(long& Ph)
{
	long nBiggerIndex=-1;
	SFloatRecord  *pRecord1, *pRecord2;
	for(long i=0;i<m_array.GetSize();i++){
		pRecord2=(SFloatRecord*)m_array.GetAt(i);
		if(pRecord2->Ph>Ph){
			nBiggerIndex=i;
			break;
		}
	}

	if(nBiggerIndex<2){
		CString s;
		s.Format("%d",Ph);
		AfxMessageBox("CFloatData::GetOnPh 报错:"
					"读取浮动基准面时所给桩号不在文件范围之内:"+s);
		return -9999;
	}

	pRecord1=(SFloatRecord*)m_array.GetAt(nBiggerIndex-1);
	float tl=pRecord2->Ph-pRecord1->Ph;
	float dl=Ph-pRecord1->Ph;
	float tf=pRecord2->F-pRecord1->F;
	return pRecord1->F+dl/tl*tf;
}

SFloatRecord * CFloatData::GetAt(int nIndex)
{
	return (SFloatRecord* )(m_array.GetAt(nIndex));
}

int CFloatData::Add(SFloatRecord * pRecord)
{
	return (m_array.Add(pRecord));
}

void CFloatData::Reset()
{
	m_array.RemoveAll();
}