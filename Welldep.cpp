
#include "stdafx.h"
#include "NewMount.h"
#include "WellDep.h"

CWellDepth::CWellDepth()
{
}

CWellDepth::CWellDepth(CString sFile)
{
	Create(sFile);
}

bool CWellDepth::Create(CString sFile)
{
	Reset();
	m_fileWellDepth=sFile;	
	return true;
}

bool CWellDepth::Use(CString sFile)
{
	Reset();
	m_fileWellDepth=sFile;	
	return Read();
}
int  CWellDepth::Add(SWellDepth *pRecord)
{
	return m_array.Add(pRecord);
}

CWellDepth::~CWellDepth()
{
	Reset();
}
	     
void CWellDepth::Reset()
{
	m_array.RemoveAll();
}
bool CWellDepth::Read()
{
	FILE *fp=fopen(m_fileWellDepth,"rt");
	if(!fp){
		AfxMessageBox("�����ļ�"+m_fileWellDepth+"������: ������CWellDepth::Read()ʱ����");
		return false;
	}

	SWellDepth *pRecord;
	while(!feof(fp)){
		pRecord=new SWellDepth;
		fscanf(fp,"%ld %f\n",pRecord->Ph,pRecord->WD);
		m_array.Add(pRecord);
	}

	fclose(fp);

	return true;
	
}

bool CWellDepth::Save()
{
	FILE *fp=fopen(m_fileWellDepth,"wt");
	if(!fp){
		AfxMessageBox("�����ļ�"+m_fileWellDepth+"�޷���:	������CWellDepth::Save()ʱ����");
		return false;
	}
		
	SWellDepth *pRecord;
	for(long i=0;i<m_array.GetSize();i++){
		pRecord=(SWellDepth*)m_array[i];
		fprintf(fp,"%ld %f\n",pRecord->Ph,pRecord->WD);
	}

	fclose(fp);

	return true;
}

float CWellDepth::GetOnPh(long& Ph)
{
	SWellDepth *pRecord;
	for(long i=0;i<m_array.GetSize();i++){
		pRecord=(SWellDepth*)m_array[i];
		if(pRecord->Ph>=Ph){
			return pRecord->WD;
		}
	}

	CString s;
	s.Format("%ld",Ph);
	AfxMessageBox("�ھ���������,δ���ִ�׮��:"+s+",������CWellDepth::GetOnPh(long& Ph)ʱ����!");

	return -1;
}