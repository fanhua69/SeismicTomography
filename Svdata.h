#ifndef tag_Survey_File
#define tag_Survey_File

struct SSvRecord:public CObject
{
	long Ph;
	double XZB;
	double YZB;
	float HBG;
};
	
class CSvData
{
private:
	Reset();
public:
	double GetEleOnPh(long nPh);
	CSvData();
	CSvData(CString sFile);
	~CSvData();
	bool IsEmpty();

	CObArray m_array;

	bool Use(CString sFile);
	long GetNum();
	SSvRecord *GetAt(long nIndex);

};

#endif