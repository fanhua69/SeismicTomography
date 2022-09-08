
#ifndef tag_BlockTrace
#define tag_BlockTrace
class CBlockTrace:public CObject
{
public:
	long m_nBlock;
	double m_dLen;

	CBlockTrace(){
		m_nBlock=0;
		m_dLen=0.0;
	}

	CBlockTrace(long nBlock,double dLen){
		m_nBlock=nBlock;
		m_dLen=dLen;
	}

	void Set(long nBlock,double dLen){
		m_nBlock=nBlock;
		m_dLen=dLen;
	}
};
#endif



#ifndef tag_Euaqtion
#define tag_Euaqtion


//////////////////////////////////////////////////////////////////////////////////
//   "Equation" 被截断为 "Equa" , 类名除外。 
//  To Resolve   AX=B.
class CEquation{
private:

	int m_nUsing;    //  Indicating what is doing, Making or Resolving  ,Reading or Writing.
								// 0, NOT using ,  1 	Making ,  2  Resolving .
	CString m_fileA;
	CString m_fileB;
	CString m_fileEquaTurn;
	CString m_fileVacant;
	
	CString m_fileAi2;
	CString m_fileAiX;
	CString m_fileFractor;
	CString m_fileX;

	FILE *m_fpA;
	FILE *m_fpB;

	double m_dErrorLimit;
	
	long m_lRowNum;
	long m_lColNum;
	long m_lMaxColEle;
	long m_lMaxRowEle;
	long *m_lpNumInRow;
	long *m_lpNumInCol;
	long *m_lpPosInRow;
	double *m_dpX;
	double *m_dpLastX;
		
	bool CalculateAi2();
	bool CalculateAiX();
	bool CalculateFractor();
	bool CalculateX();
	double CheckX();

	bool RecordVacant();
	bool FillVacant();
	
public:
	bool IsVarVacant(long nVar);
	double m_dVacantValue;
	long GetX(double *);
	bool SaveX();
	double * GetX();
	double AFX_EXT_CLASS SetErrorLimit(double dErrorLimit);
	bool AFX_EXT_CLASS ReadAnswer(double *pRoom);
	AFX_EXT_CLASS CEquation();
	AFX_EXT_CLASS ~CEquation();

	bool AFX_EXT_CLASS Use(CString fileA,CString fileB,CString fileX="",int nInitialMethod=2,double *pInitialvalue=NULL);								// 开始调用方程时使用。
	bool AFX_EXT_CLASS Construct(CString fileA, CString fileB,long nColumn);
	bool AFX_EXT_CLASS Close();  // 为建立方程完毕之后,建立方程完毕之后使用.

	// 追加方程：
	bool AFX_EXT_CLASS AppeEqua(CObArray& array,double B);

	// 转置方程：
	bool Turn(); 

	//  求解方程：
	bool AFX_EXT_CLASS Resolve();

	// 获得解：
	double AFX_EXT_CLASS *ReadX();

	//  附加功能：获得若干个方程行
	bool AFX_EXT_CLASS GetEqua(CBlockTrace *room,double *B,long EquaNo);
	long AFX_EXT_CLASS GetXNum(){return m_lColNum;}
protected:
	bool Reset();
};

#endif