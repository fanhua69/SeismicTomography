
#ifndef tag_FloatData
#define tag_FloatData

class SFloatRecord:public CObject
{
public:
	long Ph;
	float F;
};

class CFloatData
{
private:
	long GetNum(){return m_array.GetSize();}
	CObArray m_array;

	void Reset();

public:
	CFloatData();
	CFloatData(CString sFile);
	bool Use(CString sFile);

	bool Read(CString sFile);
	bool Save(CString sFile);	
	float GetOnPh(long& Ph);
	SFloatRecord * GetAt(int nIndex);
	int Add(SFloatRecord * pRecord);
};

#endif