
#include "stdafx.h"
#include "NewMount.h"
#include "SvSys.h"

CSvSys::CSvSys(CString sFile)
{
	m_sArea="";
	m_sCrew="";
	m_iGroupInterval=0;
	m_iShotInterval=0;
	m_iGroupNumSmall=0;
	m_iGroupNumBig=0;
	m_iGapSmall=0;
	m_iGapBig=0;
	m_iFillVelocity=0;

	Use(sFile);
}

CSvSys::CSvSys()
{
	m_sArea="";
	m_sCrew="";
	m_iGroupInterval=0;
	m_iShotInterval=0;
	m_iGroupNumSmall=0;
	m_iGroupNumBig=0;
	m_iGapSmall=0;
	m_iGapBig=0;
	m_iFillVelocity=0;
}

bool CSvSys::Use(CString sFile)
{
	if(sFile==""){
		AfxMessageBox("The survey system file's name is empty!");
		return false;
	}

	FILE *file=fopen(sFile,"rt");
	if(!file){
		AfxMessageBox("当打开观测系统参数文件时出错:"+sFile);
		return false;
	}

	fscanf(file,"%s",m_sArea.GetBuffer(100));
	m_sArea.ReleaseBuffer();
	fscanf(file,"%s",m_sCrew.GetBuffer(100));
	m_sCrew.ReleaseBuffer();
	fscanf(file,"%d\n",&m_iGroupInterval);
	fscanf(file,"%d\n",&m_iShotInterval);
	fscanf(file,"%d\n",&m_iGroupNumSmall);
	fscanf(file,"%d\n",&m_iGroupNumBig);
	fscanf(file,"%d\n",&m_iGapSmall);
	fscanf(file,"%d\n",&m_iGapBig);
	fscanf(file,"%d\n",&m_iFillVelocity);

	fclose(file);

	return true;
}

bool CSvSys::Save(CString sFile)
{
	FILE *file=fopen(sFile,"wt");
	if(!file){
		AfxMessageBox("当建立观测系统参数文件时出错:"+sFile);
		return FALSE;
	}

	fprintf(file,"%s\n",m_sArea);
	fprintf(file,"%s\n",m_sCrew);
	fprintf(file,"%d\n",m_iGroupInterval);
	fprintf(file,"%d\n",m_iShotInterval);
	fprintf(file,"%d\n",m_iGroupNumSmall);
	fprintf(file,"%d\n",m_iGroupNumBig);
	fprintf(file,"%d\n",m_iGapSmall);
	fprintf(file,"%d\n",m_iGapBig);
	fprintf(file,"%d\n",m_iFillVelocity);	

	fclose(file);

	return true;
}

bool CSvSys::IsEmpty(){
	return (m_iFillVelocity==0);
}
