
#ifndef tagSvSys
#define tagSvSys

class CSvSys
{
public: 
	CSvSys();
	CSvSys(CString);
	bool Use(CString );
	bool Save(CString sFile);
	bool IsEmpty();

	//  记住：不要在类外部为下面这些成员变量置值：
	CString m_sArea;
	CString m_sCrew;
	int m_iGroupInterval;
	int m_iShotInterval;
	int m_iGroupNumSmall;
	int m_iGroupNumBig;
	int m_iGapSmall;
	int m_iGapBig;
	int m_iFillVelocity;
};

#endif    
		
