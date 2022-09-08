
#include "stdafx.h"
#include "NewMount.h"
#include "MemEqua.h"
#include "Progdlg.h"
#include "math.h"

CMemEquation::CMemEquation()
{
	m_nUsing=0;

	m_fileA="";
	m_fileB="";
	m_fileEquaTurn="";
	m_fileVacant="";
	m_fileX="";	

	m_fpA=NULL;
	m_fpAT=NULL;
	
	m_dpX=NULL;
	m_dpLastX=NULL;
	m_lpNumInRow=NULL;
	m_lpNumInCol=NULL;    // 记录方程中每列的非零元素数目。
	m_lpPosInRow=NULL;

	m_pAi2=NULL;
	m_pAiX=NULL;
	m_pFractor=NULL;
	m_pB=NULL;

	m_lRowNum=0;
	m_lColNum=0;
	m_lMaxColEle=0;
	m_lMaxRowEle=0;
	m_dVacantValue=9999;
	m_dErrorLimit=0.0000000000001;
}				


bool CMemEquation::Use(CString fileA, CString fileB, CString fileX, int nInitialMethod, double *pInitialvalue)                                              
{
	if(m_nUsing==1){
		AfxMessageBox( "正在求解方程,不能进行新方程的求解!");
		return false;
	}
	else if(m_nUsing==2){
		AfxMessageBox( "方程尚未建立完毕,不能进行方程的求解!");
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
	
	////////////////////////////////////////////////////////////////////////////////////
	// Get  Some Important Data:
	////////////////////////////////////////////////////////////////////////////////////
	m_fpA=fopen(m_fileA,"rb");
	if(!m_fpA){
		AfxMessageBox("当打开文件"+m_fileA+"时出错！");
		return false;
	}

	// temporily open this file , get some data , and then read all of it to m_pB;
	FILE *fpB=fopen(m_fileB,"rb"); 
	if(!fpB){
		AfxMessageBox("当打开文件"+m_fileB+"时出错！");
		return false;
	}

	//  获得方程的个数：
	long n=fseek(fpB,0,SEEK_END);
	if(n){
		AfxMessageBox("当置文件指针到"+m_fileB+"文件尾时出错！");
		return false;
	}

	double  f=(double)ftell(fpB)/sizeof(double);
	if(int(f*sizeof(double))!=int(f)*sizeof(double)){
		AfxMessageBox("文件"+m_fileB+"错误！");
		return false;
	}

	m_lRowNum=int(f);

	////////////////////////////////////////////////////////////////////////////////////////////////
	//   data group that must be used frequently:
	m_pAi2=new double [m_lRowNum];
	m_pAiX=new double [m_lRowNum];
	m_pFractor=new double [m_lRowNum];
	m_pB=new double [m_lRowNum];

	// Read B:
	fseek(fpB,0,SEEK_SET);
	fread(m_pB,sizeof(double),m_lRowNum,fpB);
	fclose(fpB);

	////////////////////////////////////////////////////////////////////////////////////
	//  对方程读取一遍,获得一些数据:
	//  获得未知数的个数：
	n=fread(&m_lColNum,sizeof(long),1,m_fpA);
	if(n<1){
		AfxMessageBox(" The equation file is error!");
		Reset();
		return false;
	}
		
	m_lpNumInRow=new long[m_lRowNum];
	m_lpNumInCol=new long[m_lColNum];
	memset(m_lpNumInCol,0,sizeof(long)*m_lColNum);
	m_lpPosInRow=new long[m_lRowNum];
	CBlockTrace *pBlockTrace=new CBlockTrace[m_lColNum];

	m_lMaxRowEle=0;
	for( long i=0;i<m_lRowNum;i++){
		// 计算方程中每行所在位置:
		m_lpPosInRow[i]=ftell(m_fpA);
		
		// 计算每行非零元素的个数:
		n=fread(&m_lpNumInRow[i],sizeof(long),1,m_fpA);
		if(n<1){
			AfxMessageBox("方程文件错误!");
			Reset();
			return false;
		}
		
		// 获得每行中非零元素个数的最大者:
		if(m_lpNumInRow[i]>m_lMaxRowEle)m_lMaxRowEle=m_lpNumInRow[i];
				
		//  获得方程各列的元素个数,及最大列元素个数：
		// 计算每列中非零元素的个数:
		n=fread(pBlockTrace,sizeof(CBlockTrace),m_lpNumInRow[i],m_fpA);
		if(n<m_lpNumInRow[i]){
			AfxMessageBox("方程文件错误!");
			Reset();
			return false;
		}

		for(long j=0;j<m_lpNumInRow[i];j++){
			m_lpNumInCol[pBlockTrace[j].m_nBlock]++;
		}
	}
	delete []pBlockTrace;

	// 获得中非零元素个数的最大的列:
	m_lMaxColEle=0;
	for(i=0;i<m_lColNum;i++){
		if(m_lpNumInCol[i]>m_lMaxColEle){
			m_lMaxColEle=m_lpNumInCol[i];
		}
	}

	// 转置方程
	if(!Turn()){
		AfxMessageBox("Error when turning the equation!");
		Reset();
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////
	// Open this file through this object:
	m_fpAT=fopen(m_fileEquaTurn,"rb");


	////////////////////////////////////////////////////////////////////////////////////////////////
	// 初始化未知数:
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
			m_dpX[i]=0.001;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////
	// If error, return false;
	if(m_lColNum==0){
                AfxMessageBox("CMemEquation::ResoEquation 报错：\n获得的方程行数为0，无法继续！");
		Reset();
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////
	//  Calculate ai2 and save it into  the data group of m_pAi2:
	if(!CalculateAi2()){
                AfxMessageBox("CMemEquation::ResoEquation 报错：\n计算行向量的内积出错！");
		Reset();
		return false;		
	}      
  
	return true;
}

bool CMemEquation::Reset()
{
	if(m_fpA){
		fclose(m_fpA);
		m_fpA=NULL;

	}

	if(m_fpB){
		fclose(m_fpB);
		m_fpB=NULL;
	}

	if(m_fpAT){
		fclose(m_fpAT);
		m_fpAT=NULL;
	}

	if(m_dpX){
		delete []m_dpX;
		m_dpX=NULL;
	}

	if(m_dpLastX){
		delete []m_dpLastX;
		m_dpLastX=NULL;
	}


	if(m_pAi2){
		delete []m_pAi2;
		m_pAi2=NULL;
	}

	if(m_pAiX){
		delete []m_pAiX;
		m_pAiX=NULL;
	}

	if(m_pFractor){
		delete []m_pFractor;
		m_pFractor=NULL;
	}

	if(m_pB){
		delete []m_pB;
		m_pB=NULL;
	}


	if(m_lpNumInRow){
		delete []m_lpNumInRow;
		m_lpNumInRow=NULL;
	}

	if(m_lpNumInCol){
		delete []m_lpNumInCol;
		m_lpNumInCol=NULL;
	}

	if(m_lpPosInRow) {
		delete []m_lpPosInRow;
		m_lpPosInRow=NULL;
	}	

	return true;
}



bool CMemEquation::Close()
{
	if(m_nUsing==1){
		if(!RecordVacant())return false;
		if(!FillVacant())return false;

		Reset();
		m_nUsing=0;
	}
	else if(m_nUsing==2){
		SaveX();
		Reset();

		TRY{    
			CFile::Remove(m_fileEquaTurn);
			CFile::Remove(m_fileVacant);
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
	return true;
}

CMemEquation::~CMemEquation()
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
//    构造函数。
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//  公共函数：追加方程。
bool CMemEquation::AppeEqua(CObArray& array,double B)
{
	long nBlockNum=array.GetSize();

	CBlockTrace *pBlock;	

	//  Record the element number of every column, NEEDED:
	for(long i=0;i<nBlockNum;i++){
		pBlock=(CBlockTrace*)array[i];
				
		if(pBlock->m_nBlock<0||pBlock->m_nBlock>=m_lColNum){
			CString s;
			s.Format("%ld",pBlock->m_nBlock);
			AfxMessageBox("方程的行向量有错,未知数的序号超出范围!"+s);
			return false;
		}

		m_lpNumInCol[pBlock->m_nBlock]++;
	}

	//  Append to A:
	if(fseek(m_fpA,0,SEEK_END)){
		AfxMessageBox("当置文件指针到方程文件"+m_fileA+"文件尾时出错！");
		return false;
	}

	fwrite(&nBlockNum,sizeof(long),1,m_fpA);  // 将该行的数据数目记入文件.
	for(i=0;i<nBlockNum;i++){
		if(fwrite((CBlockTrace*)array[i],sizeof(CBlockTrace),1,m_fpA)<1){
		AfxMessageBox("当向方程文件"+m_fileA+"尾部追加方程时出错！");
			return false;
		}
	}

	// Append to B:
	if(fseek(m_fpB,0,SEEK_END)){
		AfxMessageBox("当置文件指针到方程文件"+m_fileB+"文件尾时出错！");
		return false;
	}

	if(!fwrite(&B,sizeof(double),1,m_fpB)){
		AfxMessageBox("当向方程文件"+m_fileB+"追加数据时出错！");
		return false;
	}

	return true;

}
//  公共函数：追加方程。
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//  从方程中  读  数据   的私有函数    开始：
bool CMemEquation::GetEqua(CBlockTrace *room,double *B,long EquaNo)
{
	//  错误返回:
	if(m_lpPosInRow[0]==0||!m_fpA){
		AfxMessageBox("必须首先调用Use以后才能调用这个函数!");
		return false;
	}

	//  获得方程位置：
	long Pos=m_lpPosInRow[EquaNo];
	long Num=m_lpNumInRow[EquaNo];
	
	// 读取方程：
	if(fseek(m_fpA,sizeof(CBlockTrace)*Pos,SEEK_SET)!=0){
		AfxMessageBox("当置文件指针到要求位置时，方程文件"+m_fileA+"出错！");
		return false;
	}
	if(fread(room,sizeof(CBlockTrace),Num,m_fpA)<Num){
		AfxMessageBox("当读取数据时，方程文件"+m_fileA+"出错！");
		return false;
	}

	//  读取 B 值：
	if(fseek(m_fpB,sizeof(double)*EquaNo,SEEK_SET)!=0){
		AfxMessageBox("当置文件指针到要求位置时，方程值文件"+m_fileB+"出错！");
		return false;
	}
	if(fread(B,sizeof(double),1,m_fpB)<1){
		AfxMessageBox("当读取数据时，方程值文件"+m_fileB+"出错！");
		return false;
	}	

	return true;
}	
//    CMemEquation::一般性操作结束。
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   计算行向量的内积：
bool CMemEquation::CalculateAi2()
{	
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 对所有行向量循环：
	CBlockTrace *pRowData=new CBlockTrace[m_lMaxRowEle];
	fseek(m_fpA,sizeof(long),SEEK_SET);   //  前Sizeof(long)个字节的是方程的列数
	long  i,j,n,nNum;
	for(i=0;i<m_lRowNum;i++){
		n=fread(&nNum,sizeof(long),1,m_fpA);
		if(n<1){
			AfxMessageBox("方程的主文件出错:"+m_fileA);
			return false;
		}
		n=fread(pRowData,sizeof(CBlockTrace),nNum,m_fpA);
		if(n<nNum){
			AfxMessageBox("方程的主文件出错:"+m_fileA);
			return false;
		}

		m_pAi2[i]=0.0;
		for(j=0;j<nNum;j++){
			m_pAi2[i]+=pRowData[j].m_dLen*pRowData[j].m_dLen;
		}
	}
	// 对所有行向量循环：
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	delete []pRowData;
	
	return true;
}
//                                BOOL Equation::CalculateAi2()															 END
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   对方程矩阵A进行转置：
bool CMemEquation::Turn()
{
		
	//  打开转置方程：
	FILE *fpEquaTurn=fopen(m_fileEquaTurn,"wb");
	if(!fpEquaTurn){
                AfxMessageBox("CMemEquation::Turn()报错：\n无法建立矩阵转置文件："+m_fileEquaTurn);
		return false;
	}	

	//  计算方程转置以后每列的位置：
	//	|   0      |   1   |   2      |   3            |   4   |.... room number
	//  | NULL |  d1  |  d2     |  d3           |  d4  |.... "Num In Col"
	//  |   0      |  d1  | d1+d2|d1+d2+d3 |         ....  Positon  of  matrix element in column sequence.
	long* pPosInCol=new long[m_lColNum];   //Positon  of  matrix element in column sequence.
	pPosInCol[0]=0;
	for(long i=1;i<m_lColNum;i++){
		pPosInCol[i]=pPosInCol[i-1]+m_lpNumInCol[i-1];
	}
			
	//  从原方程中读取一行，用于存储的数组：
	CBlockTrace *pRowData=new CBlockTrace[m_lMaxRowEle];
	
	//临时数据空间，用于存储所有列，
	//当一列的数据存满后，就把该列存入转置方程：
	long RoomOfCol=10,m,n,nNum;
	CBlockTrace (*pBlockTrace)[10]=new CBlockTrace[m_lColNum][10];  

	// 指示临时数据空间中每列的空间已经被使用了多少：
	int *pNumStored=new int[m_lColNum]; 
	memset(pNumStored,0,m_lColNum*sizeof(int));

	bool bSuccess=true;

	//  从原方程中按行读取数据：
	fseek(m_fpA,0,SEEK_SET);
	fread(&nNum,sizeof(long),1,m_fpA);	//读入未知变量的个数
	for(long Row=0;Row<m_lRowNum;Row++){

		//读入一行的数据：
		fread(&nNum,sizeof(long),1,m_fpA);				
		n=fread(pRowData,sizeof(CBlockTrace),nNum,m_fpA);
		if(n<nNum){
                        AfxMessageBox("CMemEquation::Turn()报错：\n从方程文件中读入数据时出错："+m_fileA);
			bSuccess=false;
			goto tag_quit;
		}	

		// 把该行的数据存成列序。
		for(i=0;i<nNum;i++){

			m=pRowData[i].m_nBlock;
			n=pNumStored[m];

			pBlockTrace[m][n].m_nBlock=Row;
			pBlockTrace[m][n].m_dLen=pRowData[i].m_dLen;
			pNumStored[m]++;
			
			//  如果本列的临时空间放满，则存储本列的数据，并置标志为零：
			if(pNumStored[m]==RoomOfCol){
				fseek(fpEquaTurn,pPosInCol[m]*sizeof(CBlockTrace),SEEK_SET); // 找到正确位置：
				fwrite(pBlockTrace[m],sizeof(CBlockTrace),RoomOfCol,fpEquaTurn);  //  写入数据：
				pPosInCol[m]+=RoomOfCol;   // 重新计算列位置。
				pNumStored[m]=0;		
			}// 如果本列的临时空间放满，则存储本列的数据，并置标志为零。
		}//把该行的数据存成列序。
	}  // 从原方程中按行读取数据。

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//  the room was not full, so have not been saved. Save it now.
	for(i=0;i<m_lColNum;i++){
		fseek(fpEquaTurn,pPosInCol[i]*sizeof(CBlockTrace),SEEK_SET);
		n=fwrite(pBlockTrace[i],sizeof(CBlockTrace),pNumStored[i],fpEquaTurn);
		if(n<pNumStored[i]){
                        AfxMessageBox("CMemEquation::Turn()报错：\n向转置方程中写入数据时出错："+m_fileEquaTurn);
			bSuccess=false;
			goto tag_quit;
		}	
	}
		
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 删除指针：
tag_quit:
	delete []pRowData;
	delete []pPosInCol;
	delete []pNumStored;
	delete []pBlockTrace;
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 关闭文件：
	fclose(fpEquaTurn);
	return bSuccess;
}
//          CMemEquation::Turn
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  					 求解方程主循环
bool CMemEquation::Resolve()
{
	if(m_nUsing!=2)return false;

	double x1=0,y1=100,x2=m_lColNum*0.0000001,y2=1,x;
	double a=(x1*y1-x2*y2)/(y2-y1);
	double b=y1*(x1+a);
	CString sStatus;

	CProgressDlg dlg;
	dlg.Create();
	dlg.SetRange(0,100);
	dlg.ShowWindow(SW_SHOW);

	
	long nMaxBlock=m_lMaxRowEle;
	if(m_lMaxColEle>m_lMaxRowEle){
		nMaxBlock=m_lMaxColEle;
	}

	CBlockTrace *pData=new CBlockTrace[nMaxBlock];
	long nLoop=0,i,j,k,nNum,nCheckLoop=50;
	double dSum;				

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// This is the big loop to resolve the problem , generally speaking, 
	// Stop only when the user press cancel key.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	while(true){
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Save the  x data for checking:
		if(nLoop%nCheckLoop==0){
			for(i=0;i<m_lColNum;i++){
				m_dpLastX[i]=m_dpX[i];
			}	
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Calculate AiX and the fractor at the same time:
		fseek(m_fpA,sizeof(long),SEEK_SET);
		for(i=0;i<m_lRowNum;i++){
			fread(&nNum,sizeof(long),1,m_fpA);
			fread(pData,sizeof(CBlockTrace),nNum,m_fpA); // read one row of A.
			
			// calculate AiX of the row, or the equation, or the group.
			m_pAiX[i]=0.0;
			for(k=0;k<nNum;k++){
				m_pAiX[i]+=pData[k].m_dLen*m_dpX[pData[k].m_nBlock];
			}

			// Calculate the fractor:
			m_pFractor[i]=(m_pB[i]-m_pAiX[i])/m_pAi2[i];

		} // loop for all of the shots.

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Calculate X
		// 对所有列、或未知数循环：
		fseek(m_fpAT,0,SEEK_SET);
		for(i=0;i<m_lColNum;i++){
			
			// 读入一列数据：
			fread(pData,sizeof(CBlockTrace),m_lpNumInCol[i],m_fpAT);
			dSum=0;
			for(j=0;j<m_lpNumInCol[i];j++){
				dSum+=pData[j].m_dLen*m_pFractor[pData[j].m_nBlock];
			}
			dSum=dSum/m_lpNumInCol[i];
			m_dpX[i]+=dSum;
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Every some loop, show the data to the user:
		if(nLoop%nCheckLoop==0){
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
double CMemEquation::CheckX()
{
	double Sum=0.0;
	for(long i=0;i<m_lColNum;i++){
		Sum+=fabs(m_dpX[i]-m_dpLastX[i]);
	}
	Sum/=m_lColNum;

	return Sum;
}
bool CMemEquation::SaveX()
{
	FILE *fp=fopen(m_fileX,"wb");
	if(!fp){
		AfxMessageBox("当建立方程解文件时出错！");
		return false;
	}

	fwrite( m_dpX,sizeof(double),m_lColNum, fp);
	fclose(fp);

	// A text file use to check the data:
	fp=fopen("X.TXT","wt");
	if(!fp){
		AfxMessageBox("当建立方程解文件时出错！");
		return false;
	}

	for(long i=0;i<m_lColNum;i++){
		fprintf(fp,"%i, %15.10lf\n",i,m_dpX[i]);
	}
	fclose(fp);

	return true;
}
double *CMemEquation::ReadX()
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

bool CMemEquation::RecordVacant()
{
	if(m_lColNum==0){
                AfxMessageBox("CMemEquation::RecordVacant报错：\n未知数个数为零 m_lColNum==0");
		return false;
	}

	ASSERT(m_fileVacant!="");
	FILE *fp=fopen(m_fileVacant,"wt");
	if(!fp){
                AfxMessageBox("CMemEquation::RecordVacant报错：\n当打开记录空网格的文件"+m_fileVacant+"时，出错！");
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

bool CMemEquation::FillVacant()
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
                                AfxMessageBox("CMemEquation::FillVacant 报错：\n追加方程时出错。");
				return false;
			}
		}
	}

	return true;
}

			


bool CMemEquation::Construct(CString fileA, CString fileB,long nColumn)
{
	if(m_nUsing==1){
		AfxMessageBox( "正在求解方程,不能进行新方程的建立!");
		return false;
	}
	else if(m_nUsing==2){
		AfxMessageBox( "方程尚未建立完毕,不能进行新方程的建立!");
		return false;
	}  	

	m_nUsing=1;

	// set the numbers:
	m_lColNum=nColumn;
		
	m_fpA=fopen(fileA,"wb");
	if(!m_fpA){
		AfxMessageBox("当建立文件"+m_fileA+"时出错！");
		return false;
	}
	fwrite(&nColumn,sizeof(long),1,m_fpA);  // 将矩阵的列数入主文件

	m_fpB=fopen(fileB,"wb");
	if(!m_fpB){
		AfxMessageBox("当建立文件"+m_fileB+"时出错！");
		return false;
	}

	m_fileVacant="EquaVacant.$$$";
	m_lpNumInCol=new long[nColumn];
	memset(m_lpNumInCol,0,sizeof(long)*m_lColNum);

	return true;
}


bool CMemEquation::ReadAnswer(double *pRoom)
{
	FILE *fp=fopen("X.TXT","rt");
	if(!fp){
		AfxMessageBox("当打开方程解文件时出错！");
		return false;
	}

	long n;
	for(long i=0;i<m_lColNum;i++){
		fscanf(fp,"%ld,%lf\n",&n,&(pRoom[i]));
	}

	fclose(fp);
	return true;
}

double CMemEquation::SetErrorLimit(double dErrorLimit)
{
	// Generally speaking , The error can be reached is 
	// Much bigger than the data that CheckX given.
	double dLastErrorLimit=m_dErrorLimit*1000;
	m_dErrorLimit=dErrorLimit/1000;
	return dLastErrorLimit;
}

double * CMemEquation::GetX()
{
	return m_dpX;
}

long CMemEquation::GetX(double *pRoom)
{
	for(int i=0;i<m_lColNum;i++){
		pRoom[i]=m_dpX[i];
	}
	return m_lColNum;
}


bool CMemEquation::IsVarVacant(long nVar)
{
	return (m_lpNumInCol[nVar]==0);
}
