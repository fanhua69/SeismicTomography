
#include "stdafx.h"
#include "NewMount.h"
#include "SvData.h"
#include "stdio.h"

CSvData::CSvData()
{
}

CSvData::CSvData(CString sFile)
{
	Use(sFile);
}

CSvData::Reset()
{
	m_array.RemoveAll();
}

bool  CSvData::Use(CString sFile)
{
	int n=sFile.Find('.');
	CString sExt=sFile.Right(sFile.GetLength()-n-1);
	sExt.MakeUpper();

	//   Read Text File:
	if(sExt=="TXT"){
		FILE *file=fopen(sFile,"rt");
		if(!file){
			AfxMessageBox("Can NOT open file "+sFile);
			return false;
		}

		long RN,n=0;
		float FWJ;
		SSvRecord *pRecord;
		while(!feof(file)){
			n++;
			pRecord=new SSvRecord;
			fscanf(file,"%ld %ld %f %lf %lf %f\n",
				&RN,
				&pRecord->Ph,
				&FWJ,
				&pRecord->XZB,
				&pRecord->YZB,
				&pRecord->HBG
			);  // һ�м�һ���س����з��š�
			/*if(RN!=n){
				CString s;
				s.Format("%ld %ld %f %lf %lf %f",RN,pRecord->Ph,FWJ,pRecord->XZB,pRecord->YZB,pRecord->HBG);
				AfxMessageBox("Survey file error when reading :"+s);
				fclose(file);
				return false;
			}  */
			m_array.Add(pRecord);
		}
		fclose(file);
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////
	//	Read XYH binary file:
	else{
		FILE *file=fopen(sFile,"rb");
		
		// ���ļ�ָ�뵽�ļ�β��
		long n=fseek(file,0,SEEK_END);
		if(n!=0){
			AfxMessageBox("�����ļ�ָ�뵽�ļ�βʱ���ļ�"+sFile+"��������!");
			fclose(file);
			return false;
		}

		// ����ļ����ȣ�// ���ļ����Ȳ���sizof(SvRecord)����������
		float f=(float)ftell(file)/sizeof(SSvRecord);		
		if(int(f*10)!=int(f)*10){
			AfxMessageBox("�ļ�"+sFile+"����!");
			fclose(file);
			return false;
		}	

		// Read the file:
		SSvRecord *pRecord;
		while(!feof(file)){
			pRecord=new SSvRecord;
			n=fread(pRecord,sizeof(SSvRecord),1,file);
			if(n<1){
				AfxMessageBox("����ȡ��������ļ�ʱ��������"+sFile);
				fclose(file);
				return false;
			}
			if(pRecord->Ph<=0){
				AfxMessageBox("����ȡ�����ı��ļ�ʱ��������,׮��С���㣺"+sFile);
				fclose(file);
				return false;
			}
			m_array.Add(pRecord);
		}
	}		
	return true;
}

CSvData::~CSvData()
{
	Reset();
}

SSvRecord *CSvData::GetAt(long nIndex)
{
	if(nIndex>=0&&nIndex<m_array.GetSize()){
		return (SSvRecord*)m_array.GetAt(nIndex);
	}
	else{
		AfxMessageBox("The index  of the survey data you ve given is out of range!");
		return NULL;
	}
}

long CSvData::GetNum()
{
	return m_array.GetSize();
}
bool CSvData::IsEmpty()
{
	if(m_array.GetSize()>0)
		return false;
	else
		return true;
}

double CSvData::GetEleOnPh(long nPh)
{
	SSvRecord *pRecord;
	for(long i=0;i<m_array.GetSize();i++){
		pRecord=(SSvRecord*)m_array[i];		
		if(pRecord->Ph==nPh)
			return pRecord->HBG;
	}
	return -9999;
}
