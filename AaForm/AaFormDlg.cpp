
// AaFormDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AaForm.h"
#include "AaFormDlg.h"
#include <atlimage.h>
#include "aaform_class.h"
#include <vector>
using std::vector;

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


// CAaFormDlg 对话框




CAaFormDlg::CAaFormDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAaFormDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAaFormDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAaFormDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_RUN, &CAaFormDlg::OnBnClickedBtnRun)
END_MESSAGE_MAP()


// CAaFormDlg 消息处理程序

BOOL CAaFormDlg::OnInitDialog()
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

void CAaFormDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CAaFormDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAaFormDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CAaFormDlg::OnBnClickedBtnRun()
{
	// TODO: 在此添加控件通知处理程序代码
	CBitmap srcBmp;
	CImage image;
	image.Load(_T("C:\\Users\\zhangsen9\\Downloads\\Aaform\\C++\\test.bmp"));
	srcBmp.Attach(image.Detach());

	BITMAP bmp;
	srcBmp.GetBitmap(&bmp);

	BYTE* pRGBBuffer = new BYTE[bmp.bmWidthBytes * bmp.bmHeight];
	srcBmp.GetBitmapBits(bmp.bmWidthBytes* bmp.bmHeight,pRGBBuffer);

	RGBQUAD* pRGBABuffer = new RGBQUAD[bmp.bmWidth * bmp.bmHeight];
	for(int i=0; i<bmp.bmWidth*bmp.bmHeight; i++)
	{
		pRGBABuffer[i].rgbBlue = *pRGBBuffer++;
		pRGBABuffer[i].rgbGreen = *pRGBBuffer++;
		pRGBABuffer[i].rgbRed = *pRGBBuffer++;
		pRGBABuffer[i].rgbReserved = 0;
	}

	vector<double> x,y;
	x.push_back(200); y.push_back(30);
	x.push_back(500); y.push_back(0.0);
	x.push_back(300); y.push_back(1500);
	x.push_back(0); y.push_back(1400);

	aaform myForm;
	myForm.StartAAForm(bmp.bmWidth,bmp.bmHeight,pRGBABuffer,x,y);

	// Test 2 
	// 分割成 3 * 3 网格
	// 然后 每个网格 都进行aaForm
	// 最后拼接在一起

}
