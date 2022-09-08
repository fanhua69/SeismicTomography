
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
			);  // 一行加一个回车换行符号。
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
		
		// 置文件指针到文件尾：
		long n=fseek(file,0,SEEK_END);
		if(n!=0){
			AfxMessageBox("当置文件指针到文件尾时，文件"+sFile+"发生错误!");
			fclose(file);
			return false;
		}

		// 获得文件长度：// 若文件长度不是sizof(SvRecord)的整数倍：
		float f=(float)ftell(file)/sizeof(SSvRecord);		
		if(int(f*10)!=int(f)*10){
			AfxMessageBox("文件"+sFile+"错误!");
			fclose(file);
			return false;
		}	

		// Read the file:
		SSvRecord *pRecord;
		while(!feof(file)){
			pRecord=new SSvRecord;
			n=fread(pRecord,sizeof(SSvRecord),1,file);
			if(n<1){
				AfxMessageBox("当读取测量随机文件时发生错误："+sFile);
				fclose(file);
				return false;
			}
			if(pRecord->Ph<=0){
				AfxMessageBox("当读取测量文本文件时发生错误,桩号小于零："+sFile);
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
