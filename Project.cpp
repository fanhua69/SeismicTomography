// Project.cpp: implementation of the CProject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NewMount.h"
#include "Project.h"
#include "iostream.h"
#include "fstream.h"
#include "iomanip.h"

#include "stdio.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProject::CProject()
{
	Reset();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///						Open the project file:
bool  CProject::Open(CString sFile)
{
	if(sFile==""){
		AfxMessageBox("You must firstly give me the file name of the project!");		
	}
	Reset();
	 
	ifstream fs(sFile);
	if(!fs)return false;

      
	// Check if the project 's name is same with the name in the saved file: 
	sFile.MakeUpper();
	 fs.getline(m_sProjectName.GetBuffer(100),100);
	 m_sProjectName.ReleaseBuffer();
	 m_sProjectName.MakeUpper();

	 if(m_sProjectName!=sFile){
		 AfxMessageBox("The project name given is diffrent from the name in the file!");
		 return false;
	 }

	 // get the name of the survey system:
	 fs.getline(m_sSvSys.GetBuffer(100),100);
	 m_sSvSys.ReleaseBuffer();
	 CString string[11];
   	 
	// Read:
	bool bSuccess=true;
	do
	{
		CLineFile *pLine=new CLineFile ;
		
		bSuccess=true;
		for(int i=0;i<11;i++){	     
			fs.getline(string[i].GetBuffer(100),100); 
			string[i].ReleaseBuffer();	
			if(fs.eof()){
				bSuccess=false;
				break;
			}
		}
		fs >> pLine->m_nValidBeginPh;
		fs.ignore(200,'\n');
		if(fs.eof()||!bSuccess){
			delete pLine;
			break;
		}
		fs >> pLine->m_nValidEndPh;
		fs.ignore(200,'\n');		

		fs>>pLine->m_sizeGridMerge.cx;
		fs>>pLine->m_sizeGridMerge.cy;
		fs>>pLine->m_nGridSurface;

		pLine->m_sLineName=string[0]  ;
		pLine->m_sShotPar=string[1];
		pLine->m_sShotRcv=string[2];
		pLine->m_sSurvey=string[3];
		pLine->m_sSCData=string[4];
		pLine->m_sFloat=string[5];
		pLine->m_sUnform=string[6];
		pLine->m_sFstBrk=string[7];
		pLine->m_sTrace=string[8];
		pLine->m_sEndPoint=string[9];
		pLine->m_sWellDepth=string[10];

		m_array.Add(pLine);
	}while(!fs.eof());

	fs.close();

	return true;
}

void CProject::Create(CString sFile)
{
	Reset();
	m_sProjectName=sFile;	
}

bool CProject::OpenLine(CString sLineName)
{
	sLineName.MakeUpper();
	CString sLast=m_sLineName;
	
	CLineFile *p;
	for(int i=0;i<m_array.GetSize();i++){
		p=(CLineFile*)m_array[i];
		p->m_sLineName.MakeUpper();
		if(p->m_sLineName==sLineName){
			m_nCurLinePos=i;

			m_sLineName=p->m_sLineName;
			m_sShotPar=p->m_sShotPar;
			m_sShotRcv=p->m_sShotRcv;
			m_sSurvey=p->m_sSurvey;
			m_sSCData=p->m_sSCData;
			m_sFloat=p->m_sFloat;
			m_sUnform=p->m_sUnform;
			m_sFstBrk=p->m_sFstBrk;
			m_sTrace=p->m_sTrace;
			m_sEndPoint=p->m_sEndPoint;
			m_nValidBeginPh=p->m_nValidBeginPh;
			m_nValidEndPh=p->m_nValidEndPh;
			m_sWellDepth=p->m_sWellDepth ;
			m_sizeGridMerge=p->m_sizeGridMerge;
			m_nGridSurface=p->m_nGridSurface;
		}
	}
	if(m_sLineName==sLast){
		AfxMessageBox(" Can NOT find the name of this line, current file did not change!");
		return false;
	}
	return true;
}


bool CProject::SaveLine()
{
	if(m_nCurLinePos==-1){
		return false;
	}

	CString sLast=m_sLineName;
	
	CLineFile *p;
	p=(CLineFile*)m_array[m_nCurLinePos];

	p->m_sLineName=m_sLineName;
	p->m_sShotPar=m_sShotPar;
	p->m_sShotRcv=m_sShotRcv;
	p->m_sSurvey=m_sSurvey;
	p->m_sSCData=m_sSCData;
	p->m_sFloat=m_sFloat;
	p->m_sUnform=m_sUnform;
	p->m_sFstBrk=m_sFstBrk;
	p->m_sTrace=m_sTrace;
	p->m_sEndPoint=m_sEndPoint;
	p->m_nValidBeginPh=m_nValidBeginPh;
	p->m_nValidEndPh=m_nValidEndPh;
	p->m_sWellDepth=m_sWellDepth;
	p->m_sizeGridMerge=m_sizeGridMerge;
	p->m_nGridSurface=m_nGridSurface;
		
	return true;
}




CProject::~CProject()
{

}

int CProject ::AddLine(CLineFile* pFile)
{
	pFile->m_sLineName.MakeUpper();
	CLineFile *p;
	for(int i=0;i<m_array.GetSize();i++){
		p=(CLineFile*)m_array[i];
		if(p->m_sLineName==pFile->m_sLineName){
			int n=AfxMessageBox("The line you want to add has now exist, Do you want to continue?",MB_YESNO);
			if(n==IDNO)
				return -1;
			else
				break;
		}
	}

	return m_array.Add(pFile);
}

bool  CProject::RemoveLine(CString sLineName)
{					  
	bool bRemoved=false;
	for(int i=0;i<m_array.GetSize();i++){
		if(((CLineFile*)m_array[i])->m_sLineName==sLineName){
			if(i==m_nCurLinePos)m_nCurLinePos=-1;
			m_array.RemoveAt(i);
			bRemoved=true;
			break;
		}
	}
	return bRemoved;
}

bool CProject::AddFile(ELineFile nType, CString sFile)
{
	if(m_nCurLinePos==-1)
		return false;

	CLineFile *pLine=(CLineFile *)m_array.GetAt(m_nCurLinePos);
	
	switch(nType){
	case LineName:
		pLine->m_sLineName=sFile;
		break;
	case ShotPar:
		pLine->m_sShotPar=sFile;
	case ShotRcv:
		pLine->m_sShotRcv=sFile;
	case Survey:
		pLine->m_sSurvey=sFile;
	case SCData:
		pLine->m_sSCData=sFile;
	case Float:
		pLine->m_sFloat=sFile;
	case Unform:
		pLine->m_sUnform=sFile;
	case FstBrk:
		pLine->m_sFstBrk=sFile;
	case Trace:
		pLine->m_sTrace=sFile;
	case EndPoint:
		pLine->m_sEndPoint=sFile;
	case WellDepth:
		pLine->m_sWellDepth=sFile;
	}
	return true;
}

bool CProject::RemoveFile(ELineFile nType)
{
	if(m_nCurLinePos==-1)
		return false;
	
	CLineFile *pLine=(CLineFile *)m_array.GetAt(m_nCurLinePos);
	
	switch(nType){
	case LineName:
		pLine->m_sLineName="";
		break;
	case ShotPar:
		pLine->m_sShotPar="";
	case ShotRcv:
		pLine->m_sShotRcv="";
	case Survey:
		pLine->m_sSurvey="";
	case SCData:
		pLine->m_sSCData="";
	case Float:
		pLine->m_sFloat="";
	case Unform:
		pLine->m_sUnform="";
	case FstBrk:
		pLine->m_sFstBrk="";
	case Trace:
		pLine->m_sTrace="";
	case EndPoint:
		pLine->m_sEndPoint="";
	case WellDepth:
		pLine->m_sWellDepth="";
	}
	return true;
}	

void CProject::Reset()
{
	m_array.RemoveAll();
	m_nCurLinePos=-1;

	m_sSvSys="";
	m_sLineName="";
	m_sShotPar="";
	m_sShotRcv="";
	m_sSurvey="";
	m_sSCData="";
	m_sFloat="";
	m_sUnform="";
	m_sFstBrk="";
	m_sTrace="";
	m_sEndPoint="";
	m_sWellDepth="";

	m_nGridSurface=1;
	m_sizeGridMerge=CSize(1,1);
}	

bool CProject::Save(CString sProjectFile)
{
	// If the user gave a new name, then use a new  name
	if(sProjectFile!="")
		m_sProjectName=sProjectFile;
	
	SaveLine();

	ofstream file(m_sProjectName);
	if(!file){
		AfxMessageBox("Can not open the create the file:"+m_sProjectName);
		return false;
	}

	// Write:
	file<<m_sProjectName<<'\n';
	file<<m_sSvSys<<'\n';

	for(int i=0;i<m_array.GetSize();i++){
		CLineFile *pLine=(CLineFile *)m_array.GetAt(i);
		file<<pLine->m_sLineName<<'\n';
		file<<pLine->m_sShotPar<<'\n';
		file<<pLine->m_sShotRcv<<'\n';
		file<<pLine->m_sSurvey<<'\n';
		file<<pLine->m_sSCData<<'\n';
		file<<pLine->m_sFloat<<'\n';
		file<<pLine->m_sUnform<<'\n';
		file<<pLine->m_sFstBrk<<'\n';
		file<<pLine->m_sTrace<<'\n';
		file<<pLine->m_sEndPoint<<'\n';
		file<<pLine->m_sWellDepth<<'\n';
		file<<pLine->m_nValidBeginPh<<'\n';
		file<<pLine->m_nValidEndPh<<'\n';
		file<<m_sizeGridMerge.cx<<'\n';
		file<<m_sizeGridMerge.cy<<'\n';		
		file<<m_nGridSurface<<'\n';
	}
	file.close();

	return true;
}
