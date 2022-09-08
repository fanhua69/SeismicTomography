#ifndef tag_MyPoint
#define tag_MyPoint

#include "math.h"

class CMyPoint :public CObject
{
public:
	double x;
	double y;

	CMyPoint(){x=y=0;}
	CMyPoint(double xx,double yy){x=xx;y=yy;}
	CMyPoint(CMyPoint& point){x=point.x;y=point.y;}
	CMyPoint(CMyPoint* point){x=point->x;y=point->y;}

	void Mid(CMyPoint&p1,CMyPoint&p2){
		x=p1.x+(p2.x-p1.x)/2;
		y=p1.y+(p2.y-p1.y)/2;
	}

	void Mid(CMyPoint* p1,CMyPoint  *p2){
		x=p1->x+(p2->x-p1->x)/2;
		y=p1->y+(p2->y-p1->y)/2;
	}

	void operator =(CMyPoint &point){
		x=point.x;
		y=point.y;
	}
	void operator =(CMyPoint *point){
		x=point->x;
		y=point->y;
	}

	bool operator >(CMyPoint &point){
		if(x>point.x)
			return true;
		else if(x==point.x){
			if(y>point.y)
				return true;
			else
				return false;
		}
		return false;					
	}

	bool operator >(CMyPoint *point){
		if(x>point->x)
			return true;
		else if(x==point->x){
			if(y>point->y)
				return true;
			else
				return false;
		}
		return false;					
	}


	bool operator <(CMyPoint &point){
		return !(*this>point);
	}

	bool operator <(CMyPoint *point){
		return !(*this>point);
	}

	
	bool operator ==(CMyPoint &point){
		return(x==point.x&&y==point.y);
	}
	bool operator !=(CMyPoint &point){
		return(x!=point.x||y!=point.y);
	} 
	double Distance(CMyPoint &other){
		double dx=x-other.x;
		double dy=y-other.y;
		return sqrt(dx*dx+dy*dy);
	}

	double Distance(CMyPoint *other){
		double dx=x-other->x;
		double dy=y-other->y;
		return sqrt(dx*dx+dy*dy);
	}


	static Sort(CMyPoint *pPoint,long nPoint)
	{
		CMyPoint pointSwap;
		long i,j;

		for(i=0;i<nPoint;i++){
			pointSwap=pPoint[i];

			for(j=i;j>0;j--){
				if(pPoint[j-1]>pointSwap)
					pPoint[j]=pPoint[j-1];
				else 
					break;
			}
			pPoint[j]=pointSwap;
		}
	};

	static Sort(CObArray& array)
	{
		CMyPoint *pPoint;
		int nPoint=array.GetSize();
		long i,j;
		
		for(i=0;i<nPoint;i++){
			pPoint=(CMyPoint*)array[i];

			for(j=i;j>0;j--){
				if(*(CMyPoint*)array[j-1]>*pPoint)
					array[j]=array[j-1];
				else 
					break;
			}	  
			array[j]=pPoint;
		}
	};


	static DeleteSame(CMyPoint *pPoint,long &nPoint)
	{
		// delete same points:
		for(long i=1;i<nPoint;i++){
			if(pPoint[i-1]==pPoint[i]){
				for(int j=i;j<nPoint;j++){
					pPoint[j-1]=pPoint[j];
				}
				nPoint--;
				i--;				
			}
		}
	};

	static DeleteSame(CObArray& array)
	{
		// delete same points:
		int nPoint=array.GetSize();
		for(long i=1;i<nPoint;i++){
			if(*(CMyPoint*)array[i-1]==*(CMyPoint*)array[i]){
				for(int j=i;j<nPoint;j++){
					array[j-1]=array[j];
				}
				array.RemoveAt(nPoint-1);
				nPoint--;
				i--;				
			}
		}
	};

};

#endif