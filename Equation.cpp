
#include "stdafx.h"
#include "NewMount.h"
#include "Equation.h"
#include "Progdlg.h"
#include "math.h"



CEquation::CEquation()
{
	m_nUsing=0;

	m_fileA="";
	m_fileB="";
	m_fileEquaTurn="";
	m_fileVacant="";

	m_fileAi2="";
	m_fileAiX="";
	m_fileFractor="";
	m_fileX="";
	
	m_fpA=NULL;
	m_fpB=NULL;
	
	m_dpX=NULL;
	m_dpLastX=NULL;
	m_lpNumInRow=NULL;
	m_lpNumInCol=NULL;    // ��¼������ÿ�еķ���Ԫ����Ŀ��
	m_lpPosInRow=NULL;

	m_lRowNum=0;
	m_lColNum=0;
	m_lMaxColEle=0;
	m_lMaxRowEle=0;
	m_dVacantValue=9999;
	m_dErrorLimit=0.0000000000001;
}				


bool CEquation::Use(CString fileA, CString fileB, CString fileX, int nInitialMethod, double *pInitialvalue)                                           
{
	if(m_nUsing==1){
		AfxMessageBox( "������ⷽ��,���ܽ����·��̵����!");
		return false;
	}
	else if(m_nUsing==2){
		AfxMessageBox( "������δ�������,���ܽ��з��̵����!");
		return false;
	}
	
	Reset();	

	
	m_nUsing=2;
		
	// Set the name for A and B:
	m_fileA=fileA;
	m_fileB=fileB;

	// set the name for X:
	if(fileX!=""){
		m_fileX=fileX;
	}
	else{
		m_fileX=fileA+".X";
	}

	m_fileEquaTurn=fileA+".TUR";
	m_fileAi2=fileA+".Ai2";
	m_fileAiX=fileA+".AiX";
	m_fileFractor=fileA+".FRA";
	
	////////////////////////////////////////////////////////////////////////////////////
	// Get  Some Important Data:
	
	m_fpA=fopen(m_fileA,"rb");
	if(!m_fpA){
		AfxMessageBox("�����ļ�"+m_fileA+"ʱ����");
		return false;
	}

	m_fpB=fopen(m_fileB,"rb");
	if(!m_fpB){
	   AfxMessageBox("�����ļ�"+m_fileB+"ʱ����");
		return false;
	}

//  ��÷��̵ĸ�����
	long n=fseek(m_fpB,0,SEEK_END);
	if(n){
		AfxMessageBox("�����ļ�ָ�뵽"+m_fileB+"�ļ�βʱ����");
		return false;
	}

	double  f=(double)ftell(m_fpB)/sizeof(double);
	if(int(f*sizeof(double))!=int(f)*sizeof(double)){
		AfxMessageBox("�ļ�"+m_fileB+"����");
		return false;
	}

	m_lRowNum=int(f);

	////////////////////////////////////////////////////////////////////////////////////
	//  �Է��̶�ȡһ��,���һЩ����:
	//  ���δ֪���ĸ�����
	fread(&m_lColNum,sizeof(long),1,m_fpA);
		
	m_lpNumInRow=new long[m_lRowNum];
	m_lpNumInCol=new long[m_lColNum];
	memset(m_lpNumInCol,0,sizeof(long)*m_lColNum);
	m_lpPosInRow=new long[m_lRowNum];
	CBlockTrace *pBlockTrace=new CBlockTrace[m_lColNum];

	m_lMaxRowEle=0;
	for(long i=0;i<m_lRowNum;i++){
		// ���㷽����ÿ������λ��:
		m_lpPosInRow[i]=ftell(m_fpA);
		
		// ����ÿ�з���Ԫ�صĸ���:
		n=fread(&m_lpNumInRow[i],sizeof(long),1,m_fpA);
		if(n<1){
			AfxMessageBox("�����ļ�����!");
			return false;
		}
		
		// ���ÿ���з���Ԫ�ظ����������:
		if(m_lpNumInRow[i]>m_lMaxRowEle)m_lMaxRowEle=m_lpNumInRow[i];
				
		//  ��÷��̸��е�Ԫ�ظ���,�������Ԫ�ظ�����
		// ����ÿ���з���Ԫ�صĸ���:
		fread(pBlockTrace,sizeof(CBlockTrace),m_lpNumInRow[i],m_fpA);
		for(long j=0;j<m_lpNumInRow[i];j++){
			m_lpNumInCol[pBlockTrace[j].m_nBlock]++;
		}
	}
	delete []pBlockTrace;

	// ����з���Ԫ�ظ�����������:
	m_lMaxColEle=0;
	for(i=0;i<m_lColNum;i++){
		if(m_lpNumInCol[i]>m_lMaxColEle){
			m_lMaxColEle=m_lpNumInCol[i];
		}
	}

	// ת�÷���
	Turn();

	// ��ʼ��δ֪��:
	m_dpX=new double[m_lColNum];
	m_dpLastX=new double[m_lColNum];
		
	if(nInitialMethod==0){
		for(long i=0;i<m_lColNum;i++){
			m_dpX[i]=pInitialvalue[i];
		}
	}
	else if(nInitialMethod==1){
		ReadX();
	}
	else{
		for(long i=0;i<m_lColNum;i++){
			m_dpX[i]=10.0;
		}
	}



	// If error, return false;
	if(m_lColNum==0){
                AfxMessageBox("CEquation::ResoEquation ����\n��õķ�������Ϊ0���޷�������");
		Reset();
		return false;
	}

	if(!CalculateAi2()){
                AfxMessageBox("CEquation::ResoEquation ����\n�������������ڻ�����");
		Reset();
		return false;		
	}      
  
	return true;
}

bool CEquation::Reset()
{
	if(m_fpA)fclose(m_fpA);
	if(m_fpB)fclose(m_fpB);
	if(m_dpX){
		delete []m_dpX;
		m_dpX=NULL;
	}

	if(m_dpLastX){
		delete []m_dpLastX;
		m_dpLastX=NULL;
	}

	if(m_lpNumInRow){
		delete []m_lpNumInRow;
		m_lpNumInRow=NULL;
	}

	if(m_lpNumInCol){
		delete []m_lpNumInCol;
		m_lpNumInRow=NULL;
	}

	if(m_lpPosInRow) {
		delete []m_lpPosInRow;
		m_lpPosInRow=NULL;
	}	

	return true;
}



bool CEquation::Close()
{
	if(m_nUsing==1){
		if(!RecordVacant())return false;
		if(!FillVacant())return false;
		if(m_fpA)fclose(m_fpA);
		if(m_fpB)fclose(m_fpB);
		if(m_lpNumInCol){
			delete m_lpNumInCol;
			m_lpNumInCol=NULL;
		}

		m_nUsing=0;
	}
	else if(m_nUsing==2){
		SaveX();
		if(m_fpA)fclose(m_fpA);
		if(m_fpB)fclose(m_fpB);
		
		if(m_lpPosInRow){
			delete []m_lpPosInRow;
			m_lpPosInRow=NULL;
		}

		if(m_lpNumInCol){
			delete []m_lpNumInCol;	
			m_lpNumInCol=NULL;
		}

		if(m_lpNumInRow){
			delete []m_lpNumInRow;
			m_lpNumInRow=NULL;
		}

		if(m_dpX){
			delete []m_dpX;
			m_dpX=NULL;
		}

		if(m_dpLastX){
			delete []m_dpLastX;
			m_dpLastX=NULL;
		}
		
		TRY{    
			CFile::Remove(m_fileEquaTurn);
			CFile::Remove(m_fileVacant);
			CFile::Remove(m_fileAi2);
			CFile::Remove(m_fileAiX);
			CFile::Remove(m_fileFractor);
		}
		CATCH( CFileException, e )
		{
		    #ifdef _DEBUG
				afxDump << "File " << m_fileEquaTurn<< " cannot be removed\n";    
			#endif
			return false;
		}
		END_CATCH
				
		m_nUsing=0;
	}
	return TRUE;
}

CEquation::~CEquation()
{  	
		if(m_fpA)fclose(m_fpA);
		if(m_fpB)fclose(m_fpB);
		if(m_lpPosInRow){
			delete []m_lpPosInRow;
			m_lpPosInRow=NULL;
		}

		if(m_lpNumInCol){
			delete []m_lpNumInCol;	
			m_lpNumInCol=NULL;
		}

		if(m_lpNumInRow){
			delete []m_lpNumInRow;
			m_lpNumInRow=NULL;
		}

		if(m_dpX){
			delete []m_dpX;
			m_dpX=NULL;
		}

		if(m_dpLastX){
			delete []m_dpLastX;
			m_dpLastX=NULL;
		}
		
		m_nUsing=0;
}
//    ���캯����
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//  ����������׷�ӷ��̡�
bool CEquation::AppeEqua(CObArray& array,double B)
{
	long nBlockNum=array.GetSize();

	CBlockTrace *pBlock;	

	//  Record the element number of every column, NEEDED:
	for(long i=0;i<nBlockNum;i++){
		pBlock=(CBlockTrace*)array[i];
				
		if(pBlock->m_nBlock<0||pBlock->m_nBlock>=m_lColNum){
			CString s;
			s.Format("%ld",pBlock->m_nBlock);
			AfxMessageBox("���̵��������д�,δ֪������ų�����Χ!"+s);
			return false;
		}

		m_lpNumInCol[pBlock->m_nBlock]++;
	}

	//  Append to A:
	if(fseek(m_fpA,0,SEEK_END)){
		AfxMessageBox("�����ļ�ָ�뵽�����ļ�"+m_fileA+"�ļ�βʱ����");
		return false;
	}

	fwrite(&nBlockNum,sizeof(long),1,m_fpA);  // �����е�������Ŀ�����ļ�.
	for(i=0;i<nBlockNum;i++){
		if(fwrite((CBlockTrace*)array[i],sizeof(CBlockTrace),1,m_fpA)<1){
		AfxMessageBox("���򷽳��ļ�"+m_fileA+"β��׷�ӷ���ʱ����");
			return false;
		}
	}

	// Append to B:
	if(fseek(m_fpB,0,SEEK_END)){
		AfxMessageBox("�����ļ�ָ�뵽�����ļ�"+m_fileB+"�ļ�βʱ����");
		return false;
	}

	if(!fwrite(&B,sizeof(double),1,m_fpB)){
		AfxMessageBox("���򷽳��ļ�"+m_fileB+"׷������ʱ����");
		return false;
	}

	return true;

}
//  ����������׷�ӷ��̡�
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//  �ӷ�����  ��  ����   ��˽�к���    ��ʼ��
bool CEquation::GetEqua(CBlockTrace *room,double *B,long EquaNo)
{
	//  ���󷵻�:
	if(m_lpPosInRow[0]==0||!m_fpA){
		AfxMessageBox("�������ȵ���Use�Ժ���ܵ����������!");
		return false;
	}

	//  ��÷���λ�ã�
	long Pos=m_lpPosInRow[EquaNo];
	long Num=m_lpNumInRow[EquaNo];
	
	// ��ȡ���̣�
	if(fseek(m_fpA,sizeof(CBlockTrace)*Pos,SEEK_SET)!=0){
		AfxMessageBox("�����ļ�ָ�뵽Ҫ��λ��ʱ�������ļ�"+m_fileA+"����");
		return false;
	}
	if(fread(room,sizeof(CBlockTrace),Num,m_fpA)<Num){
		AfxMessageBox("����ȡ����ʱ�������ļ�"+m_fileA+"����");
		return false;
	}

	//  ��ȡ B ֵ��
	if(fseek(m_fpB,sizeof(double)*EquaNo,SEEK_SET)!=0){
		AfxMessageBox("�����ļ�ָ�뵽Ҫ��λ��ʱ������ֵ�ļ�"+m_fileB+"����");
		return false;
	}
	if(fread(B,sizeof(double),1,m_fpB)<1){
		AfxMessageBox("����ȡ����ʱ������ֵ�ļ�"+m_fileB+"����");
		return false;
	}	

	return TRUE;
}	
//    CEquation::һ���Բ���������
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   �������������ڻ���
bool CEquation::CalculateAi2()
{
	FILE *file=fopen(m_fileAi2,"wb");	
	
	long  i,j,n,nNum;
	CBlockTrace *pRowData=new CBlockTrace[m_lMaxRowEle];
	double Ai2;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ������������ѭ����
	fseek(m_fpA,sizeof(long),SEEK_SET);   //  ǰSizeof(long)���ֽڵ��Ƿ��̵�����
	for(i=0;i<m_lRowNum;i++){
		n=fread(&nNum,sizeof(long),1,m_fpA);
		if(n<1){
			AfxMessageBox("���̵����ļ�����:"+m_fileA);
			return false;
		}
		n=fread(pRowData,sizeof(CBlockTrace),nNum,m_fpA);
		if(n<nNum){
			AfxMessageBox("���̵����ļ�����:"+m_fileA);
			return false;
		}

		Ai2=0.0;
		for(j=0;j<nNum;j++){
			Ai2+=pRowData[j].m_dLen*pRowData[j].m_dLen;
		}

		fwrite(&Ai2,sizeof(double),1,file);
	}
	// ������������ѭ����
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	delete []pRowData;
	
	fclose(file);

	return TRUE;
}
//                                BOOL Equation::CalculateAi2()															 END
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����    Ai*X
bool CEquation::CalculateAiX()
{
	FILE* fpAiX=fopen(m_fileAiX,"wb");
	if(!fpAiX){
                AfxMessageBox("CEquation::CalculateAiX����\n�޷������м�ɹ��ļ���"+m_fileAiX );
		return false;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Initiate data.
	long i,k,nNum;
	double AiX;
	CBlockTrace *pRowData=new CBlockTrace[m_lMaxRowEle];

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Loop for all of the shots, Calculate (Ai,X), and write it into file.
	fseek(m_fpA,sizeof(long),SEEK_SET);
	for(i=0;i<m_lRowNum;i++){
		k=fread(&nNum,sizeof(long),1,m_fpA);
		if(k<1){
                        AfxMessageBox("CEquation::CalculateAiX����\n��������Ŀ�ļ�����"+m_fileA);
			return false;
		}		
		k=fread(pRowData,sizeof(CBlockTrace),nNum,m_fpA); // read one row of A.
		if(k<nNum){
                        AfxMessageBox("CEquation::CalculateAiX����\n��������Ŀ�ļ�����"+m_fileA);
			return false;
		}

		// calculate AiX of the row, or the equation, or the group.
		AiX=0.0;
		for(k=0;k<nNum;k++){
			AiX+=pRowData[k].m_dLen*m_dpX[pRowData[k].m_nBlock];
		}		
		fwrite(&AiX,sizeof(double),1,fpAiX);
		
	} // loop for all of the shots.
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Close all files.
	fclose(fpAiX);
	delete []pRowData;
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// return .
	return TRUE;
}
//											BOOL Equation::CalculateAiX()											END
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                    BOOL ResoEquation::CalculateVelocityFractor()					  BEGIN
bool CEquation::CalculateFractor()  //bi-(ai,X) / ||ai||2
{
	FILE *fpAiX=fopen(m_fileAiX,"rb");
	if(!fpAiX){
                AfxMessageBox("CEquation::CalculateFractor ����\n�޷����м�ɹ��ļ���"+m_fileAiX);
		return false;
	}  	
	FILE *fpAi2=fopen(m_fileAi2,"rb");
	if(!fpAi2){
                AfxMessageBox("CEquation::CalculateFractor ����\n�޷����м�ɹ��ļ���"+m_fileAi2);
		return false;
	}  	
	FILE *fpFractor=fopen(m_fileFractor,"wb");
	if(!fpFractor){
                AfxMessageBox("CEquation::CalculateFractor ����\n�޷������м�ɹ��ļ���"+m_fileFractor); 
		return false;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Initialize rooms.
	double B,Ai2,AiX,Fractor;
	long i,n;

	fseek(m_fpB,0,SEEK_SET);
	for(i=0;i<m_lRowNum;i++){
		//////////////////////////////////////////////////////////////////////////////////////////////
		// read data from 3 files , and check the legality of the files.
		n=fread(&B,sizeof(double),1,m_fpB);
		if(n<1){
                        AfxMessageBox("CEquation::CalculateFractor ����\n�ļ���Ԥ�ڵ�Ҫ�̣�"+m_fileB);
			return false;
		}
		n=fread(&AiX,sizeof(double),1,fpAiX);
		if(n<1){
                        AfxMessageBox("CEquation::CalculateFractor ����\n�ļ���Ԥ�ڵ�Ҫ�̣�"+m_fileAiX);      
			return false;
		}
		n=fread(&Ai2,sizeof(double),1,fpAi2);
		if(n<1){
                        AfxMessageBox("CEquation::CalculateFractor ����\n�ļ���Ԥ�ڵ�Ҫ�̣�"+m_fileAi2);
			return false;
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Calculate Fractor.
		Fractor=(B-AiX)/Ai2;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Write Fractor into file.
		n=fwrite(&Fractor,sizeof(double),1,fpFractor);
		if(n<1){
                        AfxMessageBox("CEquation::CalculateFractor ����\nд���ļ�ʱ����"+m_fileFractor);
			return false;
		}
	
	} // ��������ѭ����

	////////////////////////////////////////////////////////////////////////////////////////////////
	//   Close all of the files.
	fclose(fpAiX);
	fclose(fpAi2);		 	
	fclose(fpFractor);

	////////////////////////////////////////////////////////////////////////////////////////////////
	//  return.
	return TRUE;

}
//  CalculateFractor
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//   CalculateX()                BEGIN
bool CEquation::CalculateX() 
{
	// ���ļ���
	FILE *fpAT=fopen(m_fileEquaTurn,"rb");
	if(!fpAT){
                AfxMessageBox("CEquation::CalculateX() ����\nת�÷���û�з��֣�"+m_fileEquaTurn);
		return false;
	}

	FILE *fpFractor=fopen(m_fileFractor,"rb");
	if(!fpFractor){
                AfxMessageBox("CEquation::CalculateX() ����\n�����м�ɹ��ļ�û�з��֣�"+m_fileFractor);
		return false;
	}
		
	// �����ʽ��
	double *pFractor=new double[m_lRowNum];
	long n=fread(pFractor,sizeof(double),m_lRowNum,fpFractor);
	if(n<m_lRowNum){
                AfxMessageBox("CEquation::CalculateX() ����\n�����м��ļ�Ҫ��Ԥ�ڶ̣�"+m_fileFractor);
		return false;
	}
	
	// ��ʱ������
	double sum;
	long i,j;
	
	// ��Ԫ����Ŀ����ߣ�m_lMaxColEle;
	// �洢һ�����ݵ��ڴ棺
	CBlockTrace *pColData=new CBlockTrace[m_lMaxColEle];

	// �������С���δ֪��ѭ����
	for(i=0;i<m_lColNum;i++){
		
		// ����һ�����ݣ�
		n=fread(pColData,sizeof(CBlockTrace),m_lpNumInCol[i],fpAT);
		if(n<m_lpNumInCol[i]){
                        AfxMessageBox("CEquation::CalculateX() ����\nת�÷����ļ�����"+m_fileEquaTurn);
			return false;
		}	
		
		sum=0;
		for(j=0;j<m_lpNumInCol[i];j++){
			sum+=pColData[j].m_dLen*pFractor[pColData[j].m_nBlock];
		}
		sum=sum/m_lpNumInCol[i];
		m_dpX[i]+=sum;
	}

	
	// Close all Files .
	fclose(fpAT);
	fclose(fpFractor);

	// delete all data pointers.
	delete []pFractor;
	delete []pColData;

	return TRUE;
}
//                             BOOL ResoEquation::CalculateVelocityValue()  //New X!						 END
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   �Է��̾���A����ת�ã�
bool CEquation::Turn()
{
		
	//  ��ת�÷��̣�
	FILE *fpEquaTurn=fopen(m_fileEquaTurn,"wb");
	if(!fpEquaTurn){
                AfxMessageBox("CEquation::Turn()����\n�޷���������ת���ļ���"+m_fileEquaTurn);
		return false;
	}	

	//  ���㷽��ת���Ժ�ÿ�е�λ�ã�
	//	|   0      |   1   |   2      |   3            |   4   |.... room number
	//  | NULL |  d1  |  d2     |  d3           |  d4  |.... "Num In Col"
	//  |   0      |  d1  | d1+d2|d1+d2+d3 |         ....  Positon  of  matrix element in column sequence.
	long* pPosInCol=new long[m_lColNum];   //Positon  of  matrix element in column sequence.
	pPosInCol[0]=0;
	for(long i=1;i<m_lColNum;i++){
		pPosInCol[i]=pPosInCol[i-1]+m_lpNumInCol[i-1];
	}
			
	//  ��ԭ�����ж�ȡһ�У����ڴ洢�����飺
	CBlockTrace *pRowData=new CBlockTrace[m_lMaxRowEle];
	
	//��ʱ���ݿռ䣬���ڴ洢�����У�
	//��һ�е����ݴ����󣬾ͰѸ��д���ת�÷��̣�
	long RoomOfCol=10,m,n,nNum;
	CBlockTrace (*pBlockTrace)[10]=new CBlockTrace[m_lColNum][10];  

	// ָʾ��ʱ���ݿռ���ÿ�еĿռ��Ѿ���ʹ���˶��٣�
	int *pNumStored=new int[m_lColNum]; 
	memset(pNumStored,0,m_lColNum*sizeof(int));

	bool bSuccess=true;

	//  ��ԭ�����а��ж�ȡ���ݣ�
	fseek(m_fpA,0,SEEK_SET);
	fread(&nNum,sizeof(long),1,m_fpA);	//����δ֪�����ĸ���
	for(long Row=0;Row<m_lRowNum;Row++){

		//����һ�е����ݣ�
		fread(&nNum,sizeof(long),1,m_fpA);				
		n=fread(pRowData,sizeof(CBlockTrace),nNum,m_fpA);
		if(n<nNum){
                        AfxMessageBox("CEquation::Turn()����\n�ӷ����ļ��ж�������ʱ����"+m_fileA);
			bSuccess=false;
			goto tag_quit;
		}	

		// �Ѹ��е����ݴ������
		for(i=0;i<nNum;i++){  
			
				m=pRowData[i].m_nBlock;
				n=pNumStored[m];   

				pBlockTrace[m][n].m_nBlock=Row;
				pBlockTrace[m][n].m_dLen=pRowData[i].m_dLen;
				pNumStored[m]++;
				
				//  ������е���ʱ�ռ��������洢���е����ݣ����ñ�־Ϊ�㣺
				if(pNumStored[m]==RoomOfCol){
					fseek(fpEquaTurn,pPosInCol[m]*sizeof(CBlockTrace),SEEK_SET); // �ҵ���ȷλ�ã�
					fwrite(pBlockTrace[m],sizeof(CBlockTrace),RoomOfCol,fpEquaTurn);  //  д�����ݣ�
					pPosInCol[m]+=RoomOfCol;   // ���¼�����λ�á�
					pNumStored[m]=0;		
				}// ������е���ʱ�ռ��������洢���е����ݣ����ñ�־Ϊ�㡣
		}//�Ѹ��е����ݴ������
	}  // ��ԭ�����а��ж�ȡ���ݡ�

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//  the room was not full, so have not been saved. Save it now.
	for(i=0;i<m_lColNum;i++){
		fseek(fpEquaTurn,pPosInCol[i]*sizeof(CBlockTrace),SEEK_SET);
		n=fwrite(pBlockTrace[i],sizeof(CBlockTrace),pNumStored[i],fpEquaTurn);
		if(n<pNumStored[i]){
                        AfxMessageBox("CEquation::Turn()����\n��ת�÷�����д������ʱ����"+m_fileEquaTurn);
			bSuccess=false;
			goto tag_quit;
		}	
	}
		
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ɾ��ָ�룺
tag_quit:
	delete []pRowData;
	delete []pPosInCol;
	delete []pNumStored;
	delete []pBlockTrace;
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// �ر��ļ���
	fclose(fpEquaTurn);
	return bSuccess;
}
//          CEquation::Turn
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  					 ��ⷽ����ѭ��
bool CEquation::Resolve()
{
	double x1=0,y1=100,x2=m_lColNum*0.0000001,y2=1,x;
	double a=(x1*y1-x2*y2)/(y2-y1);
	double b=y1*(x1+a);
	CString sStatus;

	CProgressDlg dlg;
	dlg.Create();
	dlg.SetRange(0,100);
	dlg.ShowWindow(SW_SHOW);

	long nLoop=0,i;
	while(true){		
		for(i=0;i<m_lColNum;i++){
			m_dpLastX[i]=m_dpX[i];
		}
	
		if(!CalculateAiX())return false;
		if(!CalculateFractor())return false;
		if(!CalculateX())return false;

		if(nLoop%100==0){
			sStatus.Format("Resolving %ld th loop",nLoop);
			x=CheckX();
			//if(x<m_dErrorLimit)break;
			dlg.SetWindowText(sStatus);
			dlg.SetPos(int(b/(x+a)));
			dlg.SetMonitor (m_dpX,100);
			if(dlg.CheckCancelButton ())break;
		}

		nLoop++;
	}
	dlg.DestroyWindow ();
	return true;
}
double CEquation::CheckX()
{
	double Sum=0.0;
	for(long i=0;i<m_lColNum;i++){
		Sum+=fabs(m_dpX[i]-m_dpLastX[i]);
	}
	Sum/=m_lColNum;

	return Sum;
}
bool CEquation::SaveX()
{
	FILE *fp=fopen(m_fileX,"wb");
	if(!fp){
		AfxMessageBox("���������̽��ļ�ʱ����");
		return false;
	}

	fwrite( m_dpX,sizeof(double),m_lColNum, fp);
	fclose(fp);

	// A text file use to check the data:
	fp=fopen("X.TXT","wt");
	if(!fp){
		AfxMessageBox("���������̽��ļ�ʱ����");
		return false;
	}

	for(long i=0;i<m_lColNum;i++){
		fprintf(fp,"%i, %15.10lf\n",i,m_dpX[i]);
	}
	fclose(fp);

	return TRUE;
}
double *CEquation::ReadX()
{
	FILE *fp=fopen(m_fileX,"rt");
	if(!fp){
		return NULL;
	}

	long n;
	for(long i=0;i<m_lColNum;i++){
		fscanf(fp,"%ld,%lf\n",&n,&m_dpX[i]);
	}

	fclose(fp);

	return m_dpX;
}

bool CEquation::RecordVacant()
{
	if(m_lColNum==0){
                AfxMessageBox("CEquation::RecordVacant����\nδ֪������Ϊ�� m_lColNum==0");
		return false;
	}

	ASSERT(m_fileVacant!="");
	FILE *fp=fopen(m_fileVacant,"wt");
	if(!fp){
                AfxMessageBox("CEquation::RecordVacant����\n���򿪼�¼��������ļ�"+m_fileVacant+"ʱ������");
		return false;
	}

	for(long i=0;i<m_lColNum;i++){
		if(m_lpNumInCol[i]==0){
			fprintf(fp,"%ld\n",i);
		}
	}

	fclose(fp);
	return true;
}

bool CEquation::FillVacant()
{
	CBlockTrace BlockTrace;
	BlockTrace.m_dLen=1;
	
	CObArray  array;
	array.SetSize(1);

	for(long i=0;i<m_lColNum;i++){
		if(m_lpNumInCol[i]==0){
			BlockTrace.m_nBlock=i;
			array[0]=&BlockTrace;
			if(!AppeEqua(array,m_dVacantValue)){
                                AfxMessageBox("CEquation::FillVacant ����\n׷�ӷ���ʱ����");
				return false;
			}
		}
	}

	return true;
}

			


bool CEquation::Construct(CString fileA, CString fileB,long nColumn)
{
	if(m_nUsing==1){
		AfxMessageBox( "������ⷽ��,���ܽ����·��̵Ľ���!");
		return false;
	}
	else if(m_nUsing==2){
		AfxMessageBox( "������δ�������,���ܽ����·��̵Ľ���!");
		return false;
	}  	

	m_nUsing=1;

	// set the numbers:
	m_lColNum=nColumn;
		
	m_fpA=fopen(fileA,"wb");
	if(!m_fpA){
		AfxMessageBox("�������ļ�"+m_fileA+"ʱ����");
		return false;
	}
	fwrite(&nColumn,sizeof(long),1,m_fpA);  // ����������������ļ�

	m_fpB=fopen(fileB,"wb");
	if(!m_fpB){
		AfxMessageBox("�������ļ�"+m_fileB+"ʱ����");
		return false;
	}

	m_fileVacant="EquaVacant.$$$";
	m_lpNumInCol=new long[nColumn];
	memset(m_lpNumInCol,0,sizeof(long)*m_lColNum);

	return TRUE;
}


bool CEquation::ReadAnswer(double *pRoom)
{
	FILE *fp=fopen("X.TXT","rt");
	if(!fp){
		AfxMessageBox("���򿪷��̽��ļ�ʱ����");
		return false;
	}

	long n;
	for(long i=0;i<m_lColNum;i++){
		fscanf(fp,"%ld,%lf\n",&n,&(pRoom[i]));
	}

	fclose(fp);
	return true;
}

double CEquation::SetErrorLimit(double dErrorLimit)
{
	// Generally speaking , The error can be reached is 
	// Much bigger than the data that CheckX given.
	double dLastErrorLimit=m_dErrorLimit*1000;
	m_dErrorLimit=dErrorLimit/1000;
	return dLastErrorLimit;
}

double * CEquation::GetX()
{
	return m_dpX;
}

long CEquation::GetX(double *pRoom)
{
	for(int i=0;i<m_lColNum;i++){
		pRoom[i]=m_dpX[i];
	}
	return m_lColNum;
}


bool CEquation::IsVarVacant(long nVar)
{
	return (m_lpNumInCol[nVar]==0);
}
