// DataGroup.cpp: implementation of the CDataGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Trace.h"
#include "DataGroup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataGroup::CDataGroup()
{
	m_nTotalSubscript=0;
	m_nSubscript1=0;
	m_nSubscript2=0;
	m_nSubscript3=0;
	m_nSubscript4=0;
	
	m_sizeElement=0;
	m_nTotalSpace=0;
	m_pData=NULL;
	m_nErrorData=-99999;
}

CDataGroup::~CDataGroup()
{
	if(m_pData){
		delete m_pData;
		m_pData=NULL;
	}
}

bool CDataGroup::Reset()
{
	m_nTotalSubscript=0;
	m_nSubscript1=0;
	m_nSubscript2=0;
	m_nSubscript3=0;
	m_nSubscript4=0;
	
	m_sizeElement=0;
	m_nTotalSpace=0;
	m_nErrorData=-99999;

	if(m_pData){
		delete m_pData;
		m_pData=NULL;
	}

	return true;
}


CDataGroup::CDataGroup(
		       int sizeEle, 
		       int nSub1,
		       int nSub2,
		       int nSub3,
		       int nSub4)
{
	New(sizeEle, nSub1,nSub2,nSub3,nSub4);
}

bool CDataGroup::New(int sizeEle, int nSub1,int nSub2,int nSub3,int nSub4)
{
	m_nErrorData=-99999;
	m_nSubscript1=nSub1;
	m_nSubscript2=nSub2;
	m_nSubscript3=nSub3;
	m_nSubscript4=nSub4;

	if(nSub2==0){
		m_nTotalSubscript=1;
		m_nTotalSpace=nSub1;
	}
	else if(nSub3==0){
		m_nTotalSubscript=2;
		m_nTotalSpace=nSub1*nSub2;
	}
	else if(nSub4==0){
		m_nTotalSubscript=3;
		m_nTotalSpace=nSub1*nSub2*nSub3;
	}
	else{
		m_nTotalSubscript=4;
		m_nTotalSpace=nSub1*nSub2*nSub3*nSub4;
	}
	m_nTotalSpace*=sizeEle;

	m_sizeElement=sizeEle;
	m_nEle21=nSub1*nSub2;
	m_nEle321=nSub3*m_nEle21;
	
	m_pData=new char[m_nTotalSpace];
	memset(m_pData,0,m_nTotalSpace);

	if(!m_pData)
		return false;
	else
		return true;
	
}

void * CDataGroup::Set(void *src, int nSub1,int nSub2,int nSub3,int nSub4)
{
	long nDest;
	
	if(nSub2==-99999){
		nDest=nSub1;
	}
	else if(nSub3==-99999){
		nDest=nSub2*m_nSubscript1+nSub1;
	}
	else if(nSub4==-99999){
		nDest=nSub3*m_nEle21+nSub2*m_nSubscript1+nSub1;
	}
	else{
		nDest=nSub4*m_nEle321+nSub3*m_nEle21+nSub2*m_nSubscript1+nSub1;
	}
	nDest*=m_sizeElement;
	
	//nDest=Check(nSub1,nSub2,nSub3,nSub4);
	return memcpy((void*)(&m_pData[nDest]), src, m_sizeElement);
}

void *	CDataGroup::Get(int nSub1,int nSub2,int nSub3,int nSub4)
{
	long nDest;
	
	if(nSub2==-99999){
		nDest=nSub1;
	}
	else if(nSub3==-99999){
		nDest=nSub2*m_nSubscript1+nSub1;
	}
	else if(nSub4==-99999){
		nDest=nSub3*m_nEle21+nSub2*m_nSubscript1+nSub1;
	}
	else{
		nDest=nSub4*m_nEle321+nSub3*m_nEle21+nSub2*m_nSubscript1+nSub1;
	}
	nDest*=m_sizeElement;
	
	//nDest=Check(nSub1,nSub2,nSub3,nSub4);

	
	return &m_pData[nDest];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Now do not use Check, because it cost too much time.
// Now do not use Check, because it cost too much time.
long CDataGroup::Check(long nSub1, long nSub2, long nSub3, long nSub4)
{
	int nSub=0;
	long nDest=0;
	if(nSub2==-99999){
		nSub=1;
		nDest=nSub1;
	}
	else if(nSub3==-99999){
		nSub=2;
		nDest=nSub2*m_nSubscript1+nSub1;
	}
	else if(nSub4==-99999){
		nSub=3;
		nDest=nSub3*m_nEle21+nSub2*m_nSubscript1+nSub1;
	}
	else{
		nSub=4;
		nDest=nSub4*m_nEle321+nSub3*m_nEle21+nSub2*m_nSubscript1+nSub1;
	}

	nDest*=m_sizeElement;

	if(nSub!=m_nTotalSubscript){
		CString s1,s2;
		s1.Format("%d",nSub);
		s2.Format("%d",m_nTotalSubscript);
		AfxMessageBox(" Subscription number for the data group is wrong :"+s1
			+"\n It should be :"+s2);
		return m_nErrorData;
	}


	bool bOut=false;
	
	switch(nSub)
	{
		case 4:
			if(nSub4<0||nSub4>=m_nSubscript4){
				bOut=true;
				CString s;
				s.Format("%ld, which should be between 0(include) and %ld",nSub4 ,m_nSubscript4);
				AfxMessageBox("The 4th subscript out of range :"+s);			
			}
		case 3:
			if(nSub3<0||nSub3>=m_nSubscript3){
				bOut=true;
				CString s;
				s.Format("%ld, which should be between 0(include) and %ld",nSub3 ,m_nSubscript3);
				AfxMessageBox("The 3th subscript out of range :"+s);			
			}
		case 2:
			if(nSub2<0||nSub2>=m_nSubscript2){
				bOut=true;
				CString s;
				s.Format("%ld, which should be between 0(include) and %ld",nSub2 ,m_nSubscript2);
				AfxMessageBox("The 2nd subscript out of range :"+s);			
			}
		case 1:
			if(nSub1<0||nSub1>=m_nSubscript1){
				bOut=true;
				CString s;
				s.Format("%ld, which should be between 0(include) and %ld",nSub1 ,m_nSubscript1);
				AfxMessageBox("The 1st subscript out of range :"+s);			
			}
	}

	if(nDest>m_nTotalSpace||bOut)
		return m_nErrorData;
	else
		return nDest;	
}
// Now do not use Check, because it cost too much time.
// Now do not use Check, because it cost too much time.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CDataGroup::Fill(void *src)
{
	long nDest=0;
	for(long i=0;i<m_nTotalSpace/m_sizeElement;i++){
		memcpy((void*)(&m_pData[nDest]), src, m_sizeElement);
		nDest+=m_sizeElement;
	}

	return true;
}

bool CDataGroup::IsEmpty()
{
	if(m_pData!=NULL)
		return false;
	return true;
}
int CDataGroup::GetSubscript(int n){
	if(n>m_nTotalSubscript){
		CString s;
		s.Format("You wanted the %i subscript , but  the subscript number is %i",n, m_nTotalSubscript	);
		AfxMessageBox(s);
		return -1;
	}
	switch(n){
	case 1:
		return m_nSubscript1;
		break;
	case 2:
		return m_nSubscript1;
		break;
	case 3:
		return m_nSubscript1;
		break;
	case 4:
		return m_nSubscript1;
		break;
	default:
		AfxMessageBox("The maximum subscript number is 4!");
		return -1;
	}
}

