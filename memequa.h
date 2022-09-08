
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




#ifndef tag_MemEuaqtion
#define tag_MemEuaqtion

//////////////////////////////////////////////////////////////////////////////////
//   "Equation" ���ض�Ϊ "Equa" , �������⡣ 
//  To Resolve   AX=B.
class CMemEquation{
private:

	int m_nUsing;    //  Indicating what is doing, Making or Resolving  ,Reading or Writing.
								// 0, NOT using ,  1 	Making ,  2  Resolving .
	CString m_fileA;
	CString m_fileB;
	CString m_fileX;
	CString m_fileEquaTurn;
	CString m_fileVacant;	

	FILE *m_fpA;
	FILE *m_fpAT;
	FILE *m_fpB;

	double *m_pAi2;
	double *m_pAiX;
	double *m_pFractor;
	double *m_pB;
	double *m_dpX;
	double *m_dpLastX;

	long *m_lpNumInRow;
	long *m_lpNumInCol;
	long *m_lpPosInRow;

	double m_dErrorLimit;
	
	long m_lRowNum;
	long m_lColNum;
	long m_lMaxColEle;
	long m_lMaxRowEle;
		
	bool CalculateAi2();
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
	AFX_EXT_CLASS CMemEquation();
	AFX_EXT_CLASS ~CMemEquation();

	bool AFX_EXT_CLASS Use(CString fileA,CString fileB,CString fileX="",int nInitialMethod=2,double *pInitialvalue=NULL);								// ��ʼ���÷���ʱʹ�á�
	bool AFX_EXT_CLASS Construct(CString fileA, CString fileB,long nColumn);
	bool AFX_EXT_CLASS Close();  // Ϊ�����������֮��,�����������֮��ʹ��.

	// ׷�ӷ��̣�
	bool AFX_EXT_CLASS AppeEqua(CObArray& array,double B);

	// ת�÷��̣�
	bool Turn(); 

	//  ��ⷽ�̣�
	bool AFX_EXT_CLASS Resolve();

	// ��ý⣺
	double AFX_EXT_CLASS *ReadX();

	//  ���ӹ��ܣ�������ɸ�������
	bool AFX_EXT_CLASS GetEqua(CBlockTrace *room,double *B,long EquaNo);
	long AFX_EXT_CLASS GetXNum(){return m_lColNum;}
protected:
	bool Reset();
};

#endif