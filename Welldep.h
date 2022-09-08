
#ifndef tag_WellDepth
#define tag_WellDepth

class SWellDepth :public CObject
{
public:
	long Ph;
	float WD;
};

class CWellDepth 
{	
private:
	CString m_fileWellDepth;
	void Reset();

public:
	CWellDepth();
	CWellDepth(CString sFile);
	bool Create(CString sFile);
	bool Use(CString sFile);

	int Add(SWellDepth *);

	long GetNum(){return m_array.GetSize();}

	~CWellDepth();
	CObArray m_array;

	float GetOnPh(long& Ph);
	bool Read();
	bool Save(); 
};

#endif