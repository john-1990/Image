#ifndef __AAROT_TEST_
#define __AAROT_TEST_

#include <windows.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <vector>
using std::vector;

#define	BACKGROUNDCOLOR 0xFF00FF


#define aaf_abs(a) (((a) < 0)?(-(a)):(a))



struct aaf_pnt
{
	double x,y;
	inline aaf_pnt(){}
	inline aaf_pnt(double x,double y):x(x),y(y){}
};

struct aaf_dblrgbquad
{
	double red, green, blue, alpha;
};

struct aaf_indll
{
	aaf_indll * next;
	int ind;
};

class aaform
{
public:
	aaform()	{};
	~aaform()	{};
	bool StartAAForm(int iWdith,int iHeight,RGBQUAD* pBuffer,vector<double> xCorner,vector<double> yCorner,int bgColor = BACKGROUNDCOLOR);

private:
	bool CreateTransform(vector<double> xCorner, vector<double> yCorner, int bgColor);
	bool Transform(vector<double> xCorner, vector<double> yCorner);
	bool CreateGrid(vector<double> & xCorner, vector<double> & yCorner);
	bool DoTransform();
	double PixOverlap(aaf_pnt * p);
	bool PtInConvexPolygon(aaf_pnt * p, aaf_pnt pt);
	void SortPoints();
	double Area();
	
private:
	int m_bmpWidth;
	int m_bmpHeight;
	RGBQUAD* m_pBuffer;
	RECT rc;

	int outstartx;
	int outstarty;
	int outwidth;
	int outheight;

	aaf_pnt * pixelgrid;
	aaf_pnt * polyoverlap;
	int polyoverlapsize;
	aaf_pnt * polysorted;
	int polysortedsize;
	aaf_pnt * corners;

	static int ja [];
};

#endif  __AAROT_TEST_
