#ifndef tag_CSCdata
#define tag_CSCData

class SSCRecord:public CObject
{
public:
	long Ph;
	double XZB;
	double YZB;
	float HBG;
	float GD;
	float F;
	int TS;
	int TR;

	SSCRecord()
	{
		Ph=0;
		XZB=0;
		YZB=0;
		HBG=0;
		GD=0;
		F=0;
		TS=0;
		TR=0;
	}

	SSCRecord(long Ph1,
		double XZB1,
		double YZB1,
		float HBG1,
		float GD1,
		float F1,
		int TS1,
		int TR1)
	{
		Ph=Ph1;
		XZB=XZB1;
		YZB=YZB1;
		HBG=HBG1;
		GD=GD1;
		F=F1;
		TS=TS1;
		TR=TR1;
	}

	void Set(long Ph1,
		double XZB1,
		double YZB1,
		float HBG1,
		float GD1,
		float F1,
		int TS1,
		int TR1)
	{
		Ph=Ph1;
		XZB=XZB1;
		YZB=YZB1;
		HBG=HBG1;
		GD=GD1;
		F=F1;
		TS=TS1;
		TR=TR1;
	}

};

class CSCData
{
private:
	CProject *m_pProject;
	CObArray m_array;

public:
	CSCData();
	~CSCData();
	bool Open(CProject *project);
	CObArray& GetData();
	void Reset();

	bool Calculate(CDC *pDC=NULL);
	bool Read();
	bool Save();
};

#endif