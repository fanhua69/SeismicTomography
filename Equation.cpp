
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
	m_lpNumInCol=NULL;    // 记录方程中每列的非零元素数目。
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
	m_fileAi2=fileA+".Ai2";
	m_fileAiX=fileA+".AiX";
	m_fileFractor=fileA+".FRA";
	
	////////////////////////////////////////////////////////////////////////////////////
	// Get  Some Important Data:
	
	m_fpA=fopen(m_fileA,"rb");
	if(!m_fpA){
		AfxMessageBox("当打开文件"+m_fileA+"时出错！");
		return false;
	}

	m_fpB=fopen(m_fileB,"rb");
	if(!m_fpB){
	   AfxMessageBox("当打开文件"+m_fileB+"时出错！");
		return false;
	}

//  获得方程的个数：
	long n=fseek(m_fpB,0,SEEK_END);
	if(n){
		AfxMessageBox("当置文件指针到"+m_fileB+"文件尾时出错！");
		return false;
	}

	double  f=(double)ftell(m_fpB)/sizeof(double);
	if(int(f*sizeof(double))!=int(f)*sizeof(double)){
		AfxMessageBox("文件"+m_fileB+"错误！");
		return false;
	}

	m_lRowNum=int(f);

	////////////////////////////////////////////////////////////////////////////////////
	//  对方程读取一遍,获得一些数据:
	//  获得未知数的个数：
	fread(&m_lColNum,sizeof(long),1,m_fpA);
		
	m_lpNumInRow=new long[m_lRowNum];
	m_lpNumInCol=new long[m_lColNum];
	memset(m_lpNumInCol,0,sizeof(long)*m_lColNum);
	m_lpPosInRow=new long[m_lRowNum];
	CBlockTrace *pBlockTrace=new CBlockTrace[m_lColNum];

	m_lMaxRowEle=0;
	for(long i=0;i<m_lRowNum;i++){
		// 计算方程中每行所在位置:
		m_lpPosInRow[i]=ftell(m_fpA);
		
		// 计算每行非零元素的个数:
		n=fread(&m_lpNumInRow[i],sizeof(long),1,m_fpA);
		if(n<1){
			AfxMessageBox("方程文件错误!");
			return false;
		}
		
		// 获得每行中非零元素个数的最大者:
		if(m_lpNumInRow[i]>m_lMaxRowEle)m_lMaxRowEle=m_lpNumInRow[i];
				
		//  获得方程各列的元素个数,及最大列元素个数：
		// 计算每列中非零元素的个数:
		fread(pBlockTrace,sizeof(CBlockTrace),m_lpNumInRow[i],m_fpA);
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
	Turn();

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
			m_dpX[i]=10.0;
		}
	}



	// If error, return false;
	if(m_lColNum==0){
                AfxMessageBox("CEquation::ResoEquation 报错：\n获得的方程行数为0，无法继续！");
		Reset();
		return false;
	}

	if(!CalculateAi2()){
                AfxMessageBox("CEquation::ResoEquation 报错：\n计算行向量的内积出错！");
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
//    构造函数。
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//  公共函数：追加方程。
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
bool CEquation::GetEqua(CBlockTrace *room,double *B,long EquaNo)
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

	return TRUE;
}	
//    CEquation::一般性操作结束。
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   计算行向量的内积：
bool CEquation::CalculateAi2()
{
	FILE *file=fopen(m_fileAi2,"wb");	
	
	long  i,j,n,nNum;
	CBlockTrace *pRowData=new CBlockTrace[m_lMaxRowEle];
	double Ai2;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 对所有行向量循环：
	fseek(m_fpA,sizeof(long),SEEK_SET);   //  前Sizeof(long)个字节的是方程的列数
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

		Ai2=0.0;
		for(j=0;j<nNum;j++){
			Ai2+=pRowData[j].m_dLen*pRowData[j].m_dLen;
		}

		fwrite(&Ai2,sizeof(double),1,file);
	}
	// 对所有行向量循环：
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	delete []pRowData;
	
	fclose(file);

	return TRUE;
}
//                                BOOL Equation::CalculateAi2()															 END
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	计算    Ai*X
bool CEquation::CalculateAiX()
{
	FILE* fpAiX=fopen(m_fileAiX,"wb");
	if(!fpAiX){
                AfxMessageBox("CEquation::CalculateAiX报错：\n无法建立中间成果文件："+m_fileAiX );
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
                        AfxMessageBox("CEquation::CalculateAiX报错：\n读入行数目文件出错："+m_fileA);
			return false;
		}		
		k=fread(pRowData,sizeof(CBlockTrace),nNum,m_fpA); // read one row of A.
		if(k<nNum){
                        AfxMessageBox("CEquation::CalculateAiX报错：\n读入行数目文件出错："+m_fileA);
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
                AfxMessageBox("CEquation::CalculateFractor 报错\n无法打开中间成果文件："+m_fileAiX);
		return false;
	}  	
	FILE *fpAi2=fopen(m_fileAi2,"rb");
	if(!fpAi2){
                AfxMessageBox("CEquation::CalculateFractor 报错\n无法打开中间成果文件："+m_fileAi2);
		return false;
	}  	
	FILE *fpFractor=fopen(m_fileFractor,"wb");
	if(!fpFractor){
                AfxMessageBox("CEquation::CalculateFractor 报错\n无法建立中间成果文件："+m_fileFractor); 
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
                        AfxMessageBox("CEquation::CalculateFractor 报错\n文件比预期的要短："+m_fileB);
			return false;
		}
		n=fread(&AiX,sizeof(double),1,fpAiX);
		if(n<1){
                        AfxMessageBox("CEquation::CalculateFractor 报错\n文件比预期的要短："+m_fileAiX);      
			return false;
		}
		n=fread(&Ai2,sizeof(double),1,fpAi2);
		if(n<1){
                        AfxMessageBox("CEquation::CalculateFractor 报错\n文件比预期的要短："+m_fileAi2);
			return false;
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Calculate Fractor.
		Fractor=(B-AiX)/Ai2;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Write Fractor into file.
		n=fwrite(&Fractor,sizeof(double),1,fpFractor);
		if(n<1){
                        AfxMessageBox("CEquation::CalculateFractor 报错\n写入文件时出错："+m_fileFractor);
			return false;
		}
	
	} // 对所有行循环。

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
	// 打开文件：
	FILE *fpAT=fopen(m_fileEquaTurn,"rb");
	if(!fpAT){
                AfxMessageBox("CEquation::CalculateX() 报错\n转置方程没有发现："+m_fileEquaTurn);
		return false;
	}

	FILE *fpFractor=fopen(m_fileFractor,"rb");
	if(!fpFractor){
                AfxMessageBox("CEquation::CalculateX() 报错\n方程中间成果文件没有发现："+m_fileFractor);
		return false;
	}
		
	// 读入分式：
	double *pFractor=new double[m_lRowNum];
	long n=fread(pFractor,sizeof(double),m_lRowNum,fpFractor);
	if(n<m_lRowNum){
                AfxMessageBox("CEquation::CalculateX() 报错\n方程中间文件要比预期短："+m_fileFractor);
		return false;
	}
	
	// 临时变量：
	double sum;
	long i,j;
	
	// 列元素数目最大者：m_lMaxColEle;
	// 存储一行数据的内存：
	CBlockTrace *pColData=new CBlockTrace[m_lMaxColEle];

	// 对所有列、或未知数循环：
	for(i=0;i<m_lColNum;i++){
		
		// 读入一列数据：
		n=fread(pColData,sizeof(CBlockTrace),m_lpNumInCol[i],fpAT);
		if(n<m_lpNumInCol[i]){
                        AfxMessageBox("CEquation::CalculateX() 报错\n转置方程文件错误："+m_fileEquaTurn);
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
//   对方程矩阵A进行转置：
bool CEquation::Turn()
{
		
	//  打开转置方程：
	FILE *fpEquaTurn=fopen(m_fileEquaTurn,"wb");
	if(!fpEquaTurn){
                AfxMessageBox("CEquation::Turn()报错：\n无法建立矩阵转置文件："+m_fileEquaTurn);
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
                        AfxMessageBox("CEquation::Turn()报错：\n从方程文件中读入数据时出错："+m_fileA);
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
                        AfxMessageBox("CEquation::Turn()报错：\n向转置方程中写入数据时出错："+m_fileEquaTurn);
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
//          CEquation::Turn
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  					 求解方程主循环
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
                AfxMessageBox("CEquation::RecordVacant报错：\n未知数个数为零 m_lColNum==0");
		return false;
	}

	ASSERT(m_fileVacant!="");
	FILE *fp=fopen(m_fileVacant,"wt");
	if(!fp){
                AfxMessageBox("CEquation::RecordVacant报错：\n当打开记录空网格的文件"+m_fileVacant+"时，出错！");
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
                                AfxMessageBox("CEquation::FillVacant 报错：\n追加方程时出错。");
				return false;
			}
		}
	}

	return true;
}

			


bool CEquation::Construct(CString fileA, CString fileB,long nColumn)
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

	return TRUE;
}


bool CEquation::ReadAnswer(double *pRoom)
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
