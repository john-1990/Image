#include "StdAfx.h"
#include "aaform_class.h"


inline int round(double a) {return (int)(a + 0.5);}
inline BYTE byterange(double a) {int b = round(a); if (b <= 0) return 0; else if (b >= 255) return 255; else return (BYTE)b;}
inline double aaf_min(double & a, double & b) {if (a < b) return a; else return b;}
inline double aaf_max(double & a, double & b) {if (a > b) return a; else return b;}

int roundup(double a){ if (aaf_abs(a - round(a)) < 1e-9) return round(a); else if ((int) a > a) return (int)a; else return (int)a + 1;}
int rounddown(double a){ if (aaf_abs(a - round(a)) < 1e-9) return round(a); else if ((int) a < a) return (int)a; else return (int)a - 1;}

int aaform::ja [] = {1, 2, 3, 0};



void gSaveBitmap(int w, int h,unsigned char *pData,int nDatasize, char* pPath)  
{  

	// Define BMP Size  
	const int height = h;  
	const int width = w;  
	const int size = nDatasize;  
	double x, y;  
	int index;  

	// Part.1 Create Bitmap File Header  
	BITMAPFILEHEADER fileHeader;  

	fileHeader.bfType = 0x4D42;			//bm
	fileHeader.bfReserved1 = 0;  
	fileHeader.bfReserved2 = 0;  
	fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + size;  
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);  

	// Part.2 Create Bitmap Info Header  
	BITMAPINFOHEADER bitmapHeader = { 0 };  

	bitmapHeader.biSize = sizeof(BITMAPINFOHEADER);  
	bitmapHeader.biHeight = -height;  
	bitmapHeader.biWidth = width;  
	bitmapHeader.biPlanes = 3;  
	bitmapHeader.biBitCount = 24;  
	bitmapHeader.biSizeImage = size;  
	bitmapHeader.biCompression = 0; //BI_RGB  


	// Write to file  
	FILE *output = fopen(pPath, "wb");  

	if (output == NULL)  
	{  
		printf("Cannot open file!\n");  
	}  
	else  
	{  
		fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, output);  
		fwrite(&bitmapHeader, sizeof(BITMAPINFOHEADER), 1, output);  
		fwrite(pData, size, 1, output);  
		fclose(output);  
	}  
} 





bool aaform::StartAAForm(int iWdith,int iHeight,RGBQUAD* pBuffer,vector<double> xCorner,vector<double> yCorner,int bgColor /* = BACKGROUNDCOLOR */)
{
	m_bmpWidth = iWdith;
	m_bmpHeight = iHeight;
	m_pBuffer = pBuffer;
	return CreateTransform(xCorner,yCorner,bgColor);
}

bool aaform::CreateTransform(vector<double> xCorner, vector<double> yCorner, int bgColor)
{
	//Check if it's a valid array
	if (xCorner.size() != 4 || yCorner.size() != 4)
		return false;

	//Find the dimensions of the bitmap that needs to be created and offset the corners so that the lowest x value is at 0 and the lowest y is at 0

	rc.left = 0;
	rc.right = 0;
	rc.top = 0;
	rc.bottom = 0;

	double offx = xCorner[0];
	double offy = yCorner[0];
	for (int i = 1; i < 4; i++)
	{
		if (xCorner[i] < offx) offx = xCorner[i];
		if (yCorner[i] < offy) offy = yCorner[i];
	}

	for (int i = 0; i < 4; i++)
	{
		xCorner[i] -= offx;
		yCorner[i] -= offy;
		if (roundup(xCorner[i]) > rc.right) rc.right = roundup(xCorner[i]);
		if (roundup(yCorner[i]) > rc.bottom) rc.bottom = roundup(yCorner[i]);
	}

	return Transform(xCorner, yCorner);
}

bool aaform::Transform(vector<double> xCorner, vector<double> yCorner)
{
	//Make sure the coordinates are valid
	if (xCorner.size() != 4 || yCorner.size() != 4)
		return false;


	//create the intial arrays
	pixelgrid = new aaf_pnt[(m_bmpWidth + 1) * (m_bmpHeight + 1)];
	polyoverlap = new aaf_pnt[16];
	polysorted = new aaf_pnt[16];
	corners = new aaf_pnt[4];

	//Load the corners array
	double dx[] = {0.0, 1.0, 1.0, 0.0};
	double dy[] = {0.0, 0.0, 1.0, 1.0};
	for (int i = 0; i < 4; i++)
	{
		corners[i].x = dx[i];
		corners[i].y = dy[i];
	}

	//Find the rectangle of dst to draw to
	outstartx = rounddown(xCorner[0]);
	outstarty = rounddown(yCorner[0]);
	outwidth = 0;
	outheight = 0;
	for (int i = 1; i < 4; i++)
	{
		if (rounddown(xCorner[i]) < outstartx) outstartx = rounddown(xCorner[i]);
		if (rounddown(yCorner[i]) < outstarty) outstarty = rounddown(yCorner[i]);
	}
	for (int i = 0; i < 4; i++)
	{
		if (roundup(xCorner[i] - outstartx) > outwidth) outwidth = roundup(xCorner[i] - outstartx);
		if (roundup(yCorner[i] - outstarty) > outheight) outheight = roundup(yCorner[i] - outstarty);
	}

	bool res;

	//fill out pixelgrid array
	res = CreateGrid( xCorner, yCorner);

	if (res)
	{
		//Do the transformation
		res = DoTransform();
	}

	//clear memory
	delete [] corners;
	delete [] polysorted;
	delete [] polyoverlap;

	//Return if the function completed properly
	return res;
}

bool aaform::CreateGrid(std::vector<double> & xCorner, std::vector<double> & yCorner)
{
	//mmm geometry
	double * sideradius = new double[4];
	double * sidecos = new double[4];
	double * sidesin = new double[4];

	//First we find the radius, cos, and sin of each side of the polygon created by xcorner and ycorner
	int j;
	for (int i = 0; i < 4; i++)
	{
		j = ja[i];
		sideradius[i] = sqrt((xCorner[i] - xCorner[j]) * (xCorner[i] - xCorner[j]) + (yCorner[i] - yCorner[j]) * (yCorner[i] - yCorner[j]));
		sidecos[i] = (xCorner[j] - xCorner[i]) / sideradius[i];
		sidesin[i] = (yCorner[j] - yCorner[i]) / sideradius[i];
	}

	//Next we create two lines in Ax + By = C form
	for (int x = 0; x < m_bmpWidth + 1; x++)
	{
		double topdist = ((double)x / (m_bmpWidth)) * sideradius[0];
		double ptxtop =  xCorner[0] + topdist * sidecos[0];
		double ptytop = yCorner[0] + topdist * sidesin[0];

		double botdist = (1.0 - (double)x / (m_bmpWidth)) * sideradius[2];
		double ptxbot = xCorner[2] + botdist * sidecos[2];
		double ptybot = yCorner[2] + botdist * sidesin[2];

		double Ah = ptybot - ptytop;
		double Bh = ptxtop - ptxbot;
		double Ch = Ah * ptxtop + Bh * ptytop;

		for (int y = 0; y < m_bmpHeight + 1; y++)
		{
			double leftdist = (1.0 - (double)y / (m_bmpHeight)) * sideradius[3];
			double ptxleft =  xCorner[3] + leftdist * sidecos[3];
			double ptyleft = yCorner[3] + leftdist * sidesin[3];

			double rightdist = ((double)y / (m_bmpHeight)) * sideradius[1];
			double ptxright = xCorner[1] + rightdist * sidecos[1];
			double ptyright = yCorner[1] + rightdist * sidesin[1];

			double Av = ptyright - ptyleft;
			double Bv = ptxleft - ptxright;
			double Cv = Av * ptxleft + Bv * ptyleft;

			//Find where the lines intersect and store that point in the pixelgrid array
			double det = Ah * Bv - Av * Bh;
			if (aaf_abs(det) < 1e-9)
			{
				delete [] sidesin;
				delete [] sidecos;
				delete [] sideradius;
				return false;
			}
			else
			{
				int ind = x + y * (m_bmpWidth + 1);
				pixelgrid[ind].x = (Bv * Ch - Bh * Cv) / det;
				pixelgrid[ind].y = (Ah * Cv - Av * Ch) / det;
			}
		}
	}

	delete [] sidesin;
	delete [] sidecos;
	delete [] sideradius;

	//Yayy we didn't fail
	return true;
}

/*
*	根据源程序的方法，获取的scrBmp像素的顺序是按照
*	从上到下 Row
*	因此需要使用GetBitmapBits，而不是BITAMP
*/
bool aaform::DoTransform()
{
	//Get source bitmap's information
	//BITMAP srcbmp;
	//if (GetObject(src, sizeof(srcbmp), &srcbmp) == 0)
	//	return false;

	////Get the destination bitmap's information
	//BITMAP dstbmp;
	//if (GetObject(dst, sizeof(dstbmp), &dstbmp) == 0)
	//	return false;

	//Create the source dib array and the dstdib array
	RGBQUAD * srcdib = m_pBuffer;
	aaf_dblrgbquad * dbldstdib = new aaf_dblrgbquad[outwidth * outheight];
	memset(dbldstdib, 0, outwidth * outheight * sizeof(aaf_dblrgbquad));

	////Load source bits into srcdib
	//BITMAPINFO srcdibbmap;
	//srcdibbmap.bmiHeader.biSize = sizeof(srcdibbmap.bmiHeader);
	//srcdibbmap.bmiHeader.biWidth = srcbmp.bmWidth;
	//srcdibbmap.bmiHeader.biHeight = -srcbmp.bmHeight;
	//srcdibbmap.bmiHeader.biPlanes = 1;
	//srcdibbmap.bmiHeader.biBitCount = 32;
	//srcdibbmap.bmiHeader.biCompression = BI_RGB;

	//Get the source bits
	//HDC ldc = CreateCompatibleDC(0);
	//GetDIBits(ldc, src, 0, srcbmp.bmHeight, srcdib, &srcdibbmap, DIB_RGB_COLORS);
	//DeleteDC(ldc);

	//Create polygon arrays
	aaf_pnt * p = new aaf_pnt[4];
	aaf_pnt * poffset = new aaf_pnt[4];

	//Loop through the source's pixels
	for (int x = 0; x < m_bmpWidth; x++)
	{
		for (int y = 0; y < m_bmpHeight; y++)
		{
			//Construct the source pixel's rotated polygon from pixelgrid
			p[0] = pixelgrid[x + y * (m_bmpWidth + 1)];
			p[1] = pixelgrid[(x + 1) + y * (m_bmpWidth + 1)];
			p[2] = pixelgrid[(x + 1) + (y + 1) * (m_bmpWidth + 1)];
			p[3] = pixelgrid[x + (y + 1) * (m_bmpWidth + 1)];

			//Find the scan area on the destination's pixels
			int mindx = 0x7FFFFFFF;
			int mindy = 0x7FFFFFFF;
			int maxdx = 0x80000000;
			int maxdy = 0x80000000;
			for (int i = 0; i < 4; i++)
			{
				if (rounddown(p[i].x) < mindx) mindx = rounddown(p[i].x);
				if (roundup(p[i].x) > maxdx) maxdx = roundup(p[i].x);
				if (rounddown(p[i].y) < mindy) mindy = rounddown(p[i].y);
				if (roundup(p[i].y) > maxdy) maxdy = roundup(p[i].y);
			}

			int SrcIndex = x + y * m_bmpWidth;

			//loop through the scan area to find where source(x, y) overlaps with the destination pixels
			for (int xx = mindx - 1; xx <= maxdx; xx++)
			{
				if (xx < 0 || xx >= rc.right)
					continue;
				for (int yy = mindy - 1; yy <= maxdy; yy++)
				{
					if(yy < 0 || yy >= rc.bottom)
						continue;

					//offset p and by (xx,yy) and put that into poffset
					for (int i = 0; i < 4; i++)
					{
						poffset[i].x = p[i].x - xx;
						poffset[i].y = p[i].y - yy;
					}

					//FIND THE OVERLAP *a whole lot of code pays off here*
					double dbloverlap = PixOverlap(poffset);

					if (dbloverlap > 0)
					{
						int dstindex = xx + yy * outwidth;

						//Add the rgb and alpha values in proportion to the overlap area
						dbldstdib[dstindex].red += (double)(srcdib[SrcIndex].rgbRed) * dbloverlap;
						dbldstdib[dstindex].blue += (double)(srcdib[SrcIndex].rgbBlue) * dbloverlap;
						dbldstdib[dstindex].green += (double)(srcdib[SrcIndex].rgbGreen) * dbloverlap;
						dbldstdib[dstindex].alpha += dbloverlap;
					}
				}
			}
		}
	}

	//Free memory no longer needed
	delete [] p;
	delete [] poffset;
	delete [] srcdib;
	srcdib = NULL;

	//Create final destination bits
	RGBQUAD * dstdib = new RGBQUAD[rc.right * rc.bottom];
	memset(dstdib,0,sizeof(RGBQUAD)*rc.right*rc.bottom);
	RGBQUAD* pRGB = dstdib;
	RGBQUAD bgColor;
	bgColor.rgbBlue = 255;
	bgColor.rgbGreen = 0;
	bgColor.rgbRed = 255;
	bgColor.rgbReserved = 0;

	for(int i=0; i<rc.right*rc.bottom; i++)
	{
		pRGB[i] = bgColor;
	}

	//BITMAPINFO dstdibmap;
	//dstdibmap.bmiHeader.biSize = sizeof(dstdibmap.bmiHeader);
	//dstdibmap.bmiHeader.biWidth = dstbmp.bmWidth;
	//dstdibmap.bmiHeader.biHeight = -dstbmp.bmHeight;
	//dstdibmap.bmiHeader.biPlanes = 1;
	//dstdibmap.bmiHeader.biBitCount = 32;
	//dstdibmap.bmiHeader.biCompression = BI_RGB;

	////Load the destination bitmap's bits
	//ldc = CreateCompatibleDC(0);
	//GetDIBits(ldc, dst, 0, dstbmp.bmHeight, dstdib, &dstdibmap, DIB_RGB_COLORS);
	//DeleteDC(ldc);

	//Write to dstdib with the information stored in dbldstdib
	for (int x = 0; x < outwidth; x++)
	{
		if (x + outstartx >= rc.right)
			continue;
		for (int y = 0; y < outheight; y++)
		{
			if (y + outstarty >= rc.bottom)
				continue;
			int offindex = x + y * outwidth;
			int dstindex = x + outstartx + (y + outstarty) * rc.right;

			if (dbldstdib[offindex].alpha > 1)
			{//handles wrap around for non-convex transformations
				dstdib[dstindex].rgbReserved = 0;
				dstdib[dstindex].rgbRed = byterange(dbldstdib[offindex].red / dbldstdib[offindex].alpha);
				dstdib[dstindex].rgbGreen = byterange(dbldstdib[offindex].green / dbldstdib[offindex].alpha);
				dstdib[dstindex].rgbBlue = byterange(dbldstdib[offindex].blue / dbldstdib[offindex].alpha);
			}
			else
			{
				dstdib[dstindex].rgbReserved = 0;
				dstdib[dstindex].rgbRed = byterange(dbldstdib[offindex].red + (1 - dbldstdib[offindex].alpha) * (double)dstdib[dstindex].rgbRed);
				dstdib[dstindex].rgbGreen = byterange(dbldstdib[offindex].green + (1 - dbldstdib[offindex].alpha) * (double)dstdib[dstindex].rgbGreen);
				dstdib[dstindex].rgbBlue = byterange(dbldstdib[offindex].blue + (1 - dbldstdib[offindex].alpha) * (double)dstdib[dstindex].rgbBlue);
			}
		}
	}

	//Clear memory
	delete [] dbldstdib;
	dbldstdib = NULL;


	RGBQUAD* pHead = dstdib;
	BYTE* pRGBBuffer = new BYTE[rc.right * 3 * rc.bottom];
	memset(pRGBBuffer,0,rc.right*3*rc.bottom);
	for(int i=0; i<rc.right*rc.bottom; i++)
	{
		pRGBBuffer[i * 3 + 0] = pHead[i].rgbBlue;
		pRGBBuffer[i * 3 + 1] = pHead[i].rgbGreen;
		pRGBBuffer[i * 3 + 2] = pHead[i].rgbRed;
	}

	gSaveBitmap(rc.right,rc.bottom,pRGBBuffer,rc.right*3*rc.bottom,"E:\\f.bmp");

	//Clear memory
	delete [] dstdib;
	dstdib = NULL;

	//:D
	return true;
}


double aaform::PixOverlap(aaf_pnt * p)
{
	polyoverlapsize = 0;
	polysortedsize = 0;

	double minx, maxx, miny, maxy;
	int j;

	double z;

	for (int i = 0; i < 4; i++)
	{        
		//Search for source points within the destination quadrolateral
		if (p[i].x >= 0 && p[i].x <= 1 && p[i].y >= 0 && p[i].y <= 1)
			polyoverlap[polyoverlapsize++] = p[i];

		//Search for destination points within the source quadrolateral
		if (PtInConvexPolygon(p, corners[i]))
			polyoverlap[polyoverlapsize++] = corners[i];

		//Search for line intersections
		j = ja[i];
		minx = aaf_min(p[i].x, p[j].x);
		miny = aaf_min(p[i].y, p[j].y);
		maxx = aaf_max(p[i].x, p[j].x);
		maxy = aaf_max(p[i].y, p[j].y);

		if (minx < 0.0 && 0.0 < maxx)
		{//Cross left
			z = p[i].y - p[i].x * (p[i].y - p[j].y) / (p[i].x - p[j].x);
			if (z >= 0.0 && z <= 1.0)
			{
				polyoverlap[polyoverlapsize].x = 0.0;
				polyoverlap[polyoverlapsize++].y = z;
			}
		}
		if (minx < 1.0 && 1.0 < maxx)
		{//Cross right
			z = p[i].y + (1 - p[i].x) * (p[i].y - p[j].y) / (p[i].x - p[j].x);
			if (z >= 0.0 && z <= 1.0)
			{
				polyoverlap[polyoverlapsize].x = 1.0;
				polyoverlap[polyoverlapsize++].y = z;
			}
		}
		if (miny < 0.0 && 0.0 < maxy)
		{//Cross bottom
			z = p[i].x - p[i].y * (p[i].x - p[j].x) / (p[i].y - p[j].y);
			if (z >= 0.0 && z <= 1.0)
			{
				polyoverlap[polyoverlapsize].x = z;
				polyoverlap[polyoverlapsize++].y = 0.0;
			}
		}
		if (miny < 1.0 && 1.0 < maxy)
		{//Cross top
			z = p[i].x + (1 - p[i].y) * (p[i].x - p[j].x) / (p[i].y - p[j].y);
			if (z >= 0.0 && z <= 1.0)
			{
				polyoverlap[polyoverlapsize].x = z;
				polyoverlap[polyoverlapsize++].y = 1.0;
			}
		}
	}        

	//Sort the points and return the area
	SortPoints();
	return Area();
}

//This funciton works for any convex polygon
bool aaform::PtInConvexPolygon(aaf_pnt * p, aaf_pnt pt)
{
	int dir = 0;
	int j;

	//Basically what we are doing is seeing if pt is on the same side of each face of the polygon through cross multiplication
	for (int i = 0; i < 4; i++)
	{
		j = ja[i];
		double cross = (p[i].x - pt.x) * (p[j].y - pt.y) - (p[j].x - pt.x) * (p[i].y - pt.y);

		if (cross == 0)
			continue;

		if (cross > 0)
		{
			if (dir == -1)
				return false;

			dir = 1;
		}
		else
		{
			if (dir == 1)
				return false;

			dir = -1;
		}
	}
	return true;
}

void aaform::SortPoints()
{
	//Why even bother?
	if (polyoverlapsize < 3)
		return;

	//polyoverlap is a triangle, points cannot be out of order
	if (polyoverlapsize == 3)
	{
		polysortedsize = polyoverlapsize - 1;
		polysorted[0].x = polyoverlap[1].x - polyoverlap[0].x;
		polysorted[0].y = polyoverlap[1].y - polyoverlap[0].y;
		polysorted[1].x = polyoverlap[2].x - polyoverlap[0].x;
		polysorted[1].y = polyoverlap[2].y - polyoverlap[0].y;
		return;
	}


	aaf_indll * root = new aaf_indll;
	root->next = NULL;

	//begin sorting the points.  Note that the first element is left out and all other elements are offset by it's values
	for (int i = 1; i < polyoverlapsize; i++)
	{
		polyoverlap[i].x = polyoverlap[i].x - polyoverlap[0].x;
		polyoverlap[i].y = polyoverlap[i].y - polyoverlap[0].y;

		aaf_indll * node = root;
		//Loop until the point polyoverlap[i] is can be sorted (counter) clockwiswe (I'm not sure which way it's sorted)
		while (true)
		{
			if (node->next)
			{
				if (polyoverlap[i].x * polyoverlap[node->next->ind].y - polyoverlap[node->next->ind].x * polyoverlap[i].y < 0)
				{
					//Insert point before this element
					aaf_indll * temp = node->next;
					node->next = new aaf_indll;
					node->next->ind = i;
					node->next->next = temp;
					break;
				}
			}
			else
			{
				//Add point to the end of list
				node->next = new aaf_indll;
				node->next->ind = i;
				node->next->next = NULL;
				break;
			}
			node = node->next;
		}
	}

	//We can leave out the first point because it's offset position is going to be (0, 0)
	polysortedsize = 0;

	aaf_indll * node = root;
	aaf_indll * temp;

	//Add the sorted points to polysorted and clean up memory
	while (node)
	{
		temp = node;
		node = node->next;
		if (node)
			polysorted[polysortedsize++] = polyoverlap[node->ind];
		delete temp;
	}
}

//Note that this function is specific to Aaform to save time
double aaform::Area()
{
	double ret = 0.0;
	//Loop through each triangle with respect to (0, 0) and add the cross multiplication
	for (int i = 0; i + 1 < polysortedsize; i++)
		ret += polysorted[i].x * polysorted[i + 1].y - polysorted[i + 1].x * polysorted[i].y;
	//Take the absolute value over 2
	return aaf_abs(ret) / 2.0;
}
