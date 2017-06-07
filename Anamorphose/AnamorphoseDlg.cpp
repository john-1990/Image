
// AnamorphoseDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Anamorphose.h"
#include "AnamorphoseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAnamorphoseDlg 对话框




CAnamorphoseDlg::CAnamorphoseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAnamorphoseDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAnamorphoseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAnamorphoseDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CAnamorphoseDlg 消息处理程序

BOOL CAnamorphoseDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAnamorphoseDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。



void CAnamorphoseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//CDialog::OnPaint();

		CPaintDC dc(this);


		CPoint ptHead(50,50);

		CPoint pt[const_count][const_count];

		for(int i=0; i<const_count; i++)
		{
			for(int j=0; j<const_count; j++)
			{
				pt[i][j].x = ptHead.x + j * const_size;
				pt[i][j].y = ptHead.y + i * const_size;
				DrawPoint(&dc, pt[i][j]);
			}
		}

		CPen fixedPen;
		fixedPen.CreatePen(PS_SOLID,1,RGB(255,0,0));
		CPen* oldPen = dc.SelectObject(&fixedPen);

		//映射到坐标  A(80,10)   B(400,170)   C(60,320)   D(200,420)

		TextureBlock(pt[0][0], pt[0][const_count-1], pt[const_count-1][0], pt[const_count][const_count-1],
			CPoint(80,10), CPoint(400,170), CPoint(60,320), CPoint(200,420),
			pt);


		for(int i=0; i<const_count; i++)
		{
			for(int j=0; j<const_count; j++)
			{
				DrawPoint(&dc, pt[i][j]);
			}
		}
	}
}

/*
*	来源 http://blog.csdn.net/pizi0475/article/details/12770813
*	但是通过文档说明你可能并不能懂代码是怎么使用的
*	下面我对代码进行更深一步的解释，并用在自己的功能中
*	对原作者的分享表示感谢
*
*	正文：
*	这个函数的作用是进行变换
*	输入的xo，yo是 块类似编号的东西
*	经过
*	int xi=(xo<<4), yi=(yo<<4);
*	得到的是矩形块 四个端点中的一个 具体坐标 （左上角？这个要看如何定义坐标系）
*	m_awBuf1 是 图像的RGB块的 起始地址（一维数组）
*	m_offset 是 一个二维数据[A][B]，存储的是块的坐标信息（约定某个端点）
*	m_nMul640 是 一位数组，存储的信息是块B 对应的 行（起始行） 对应的 RGB块的起始地址偏移量
*	现在开始对源程序改造
*	约定坐标系左上角 是0 0 
*	四边形的 样子 
*	A----------B
*	|		   |
*	|		   |
*	C----------D
*
*/
void CAnamorphoseDlg::TextureBlock(CPoint oldPtA, CPoint oldPtB, CPoint oldPtC, CPoint oldPtD, CPoint newPtA, CPoint newPtB, CPoint newPtC, CPoint newPtD, CPoint (&ptArray)[const_count][const_count])  
{  
	float fLeftOffX, fLeftOffY;     // 各行左端点相对于上一行左端点的偏移  
	float fRightOffY, fRightOffX;   // 各行右端点相对于上一行右端点的偏移  
	float TX1, TY1, TX2, TY2;       // 当前行左、右端点的坐标  
	float HDx, Hdy;                 // 当前行各点间的平均偏移量  
	float tx, ty;                   // 当前投影点坐标  

	int xi = newPtA.x, yi = newPtA.y;

	WORD *Tptr;  
	fLeftOffX = (newPtC.x - newPtA.x) /16;		// 计算平均偏移  
	fLeftOffY = (newPtC.y - newPtA.y) /16;  
	fRightOffX = (newPtD.x - newPtB.x) /16;  
	fRightOffY = (newPtD.y - newPtB.y) /16;		// 计算平均偏移  
	TX1  = newPtA.x;							// 取投影图块第一行左端点坐标  
	TY1  = newPtA.y;  
	TX2  = newPtB.x;							// 取投影图块第一行右端点坐标  
	TY2  = newPtB.y;  
	for (int y = yi; y < (yi+16); y++)  
	{  
		HDx  = (TX2-TX1) / 16;					// 计算投影图块当前行各点的平均偏移  
		Hdy  = ((TY2-TY1) / 16);  
		tx = TX1;								// 投影平面当前行左端点坐标  
		ty = TY1;  
		for (int x = xi; x < (xi+16); x++)  
		{  
			//*Tptr++ = m_awBuf2[int(tx) + m_nMul640[int(ty)] ];		//这一部分其实是颜色映射
			tx += HDx;							// 下一点  
			ty += Hdy;

			ptArray[y-yi][x-xi].x = tx;
			ptArray[y-yi][x-xi].y = ty;
		}    
		TX1 += fLeftOffX;						// 计算投影平面中下一行左、右端点的坐标 
		TY1 += fLeftOffY;  
		TX2 += fRightOffX;  
		TY2 += fRightOffY;  
	}  
}


void CAnamorphoseDlg::DrawPoint(CDC* pDC, CPoint pt)
{
	DrawPoint(pDC, pt.x, pt.y);
}


void CAnamorphoseDlg::DrawPoint(CDC* pDC, int x, int y)
{
	CRect rectEllipse;
	rectEllipse.left = x - 2;
	rectEllipse.right = x + 2;
	rectEllipse.top = y - 2;
	rectEllipse.bottom = y + 2;

	pDC->Ellipse(rectEllipse);
}


//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAnamorphoseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

