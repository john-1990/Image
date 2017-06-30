
// AaFormDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CAaFormDlg �Ի���




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


// CAaFormDlg ��Ϣ�������

BOOL CAaFormDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAaFormDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CAaFormDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CAaFormDlg::OnBnClickedBtnRun()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// �ָ�� 3 * 3 ����
	// Ȼ�� ÿ������ ������aaForm
	// ���ƴ����һ��

}
