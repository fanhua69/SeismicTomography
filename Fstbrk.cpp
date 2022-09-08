
#include "stdafx.h"
#include "NewMount.h"
#include "FstBrk.h"
#include "SvSys.h"

CFstBrk::CFstBrk()
{
	m_fpFstBrk=NULL;
	m_lShotNum=0;
}

CFstBrk::CFstBrk(CString UnformFile,CSvSys& SvSys)
{
	m_fpFstBrk=NULL;
	m_lShotNum=0;
	if(!Open(UnformFile,SvSys))Reset();
}

bool CFstBrk::Reset()
{
	if(m_fpFstBrk!=NULL){
		fclose(m_fpFstBrk);
		m_fpFstBrk=NULL;
	}
	m_arrayIndex.RemoveAll();
	m_lShotNum=0;

	return true;
}

bool CFstBrk::IsEmpty()
{
	return !m_fpFstBrk;
}
bool CFstBrk::Open(CString UnformFile,CSvSys& SvSys)
{
	Reset();

	CString sFileFstBrk;
	int n=UnformFile.Find(".");
	if(n==-1)
		sFileFstBrk=UnformFile+".FBK";
	else
		sFileFstBrk=UnformFile.Left(n)+".FBK";
	
	// 保证该文件的存在。
	m_fpFstBrk=fopen(sFileFstBrk,"rb+");
	if(!m_fpFstBrk){
		m_fpFstBrk=fopen(sFileFstBrk,"w");
		fclose(m_fpFstBrk);
		m_fpFstBrk=fopen(sFileFstBrk,"rb+");
	}

	// 计算该文件中包含的总道数, and total shot number:
	fseek(m_fpFstBrk,0,SEEK_END);
	long nLen=ftell(m_fpFstBrk);
	long nTotalTraceNum=(long)(nLen/sizeof(SFstBrkRecord));
	if(nTotalTraceNum*sizeof(SFstBrkRecord)!=nLen){
		AfxMessageBox("First Break file "+sFileFstBrk+" is wrong!");
		return false;
	}

	MakeIndex(SvSys);

	return true;
}

CFstBrk::~CFstBrk()
{
	Reset();
}

long CFstBrk::Get(SFstBrkRecord *room,long pos,long n)
{
	if(!m_fpFstBrk)return -1;
	
	memset(room,0,n*sizeof(SFstBrkRecord));

	fseek(m_fpFstBrk,pos,SEEK_SET);
	return fread(room,sizeof(SFstBrkRecord),n,m_fpFstBrk);	
}

long CFstBrk::Put(SFstBrkRecord *room,long posGroup,long n)
{
	if(!m_fpFstBrk)return -1;

	fseek(m_fpFstBrk,posGroup*sizeof(SFstBrkRecord),SEEK_SET);
	return fwrite(room,sizeof(SFstBrkRecord),n,m_fpFstBrk);
	
}
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CFstBrk::MakeIndex(CSvSys &SvSys)
{
	SFstBrkRecord Fbk;

	long ShotSize=(SvSys.m_iGroupNumBig+SvSys.m_iGroupNumSmall)*sizeof(SFstBrkRecord);

	//  第一个记录用于记录炮点数：
	long n,FilePos=0;
	SFbkIndex *pIndex;
	do{

		fseek(m_fpFstBrk,FilePos,SEEK_SET);
		n=fread(&Fbk,sizeof(SFstBrkRecord),1,m_fpFstBrk);
		if(n==0)break;

		pIndex=new SFbkIndex;
		pIndex->m_lFileNo=Fbk.m_lFileNo;
		pIndex->m_lFilePos=FilePos;

		m_arrayIndex.Add(pIndex);

		FilePos+=ShotSize;
	}while(true);

	return true;
}

long  CFstBrk::GetShotNum()
{
	return m_arrayIndex.GetSize();
}

SFbkIndex* CFstBrk::GetIndex(long nShot)
{
	return (SFbkIndex*)m_arrayIndex.GetAt(nShot);
}
