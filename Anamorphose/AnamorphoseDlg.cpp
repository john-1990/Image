
// AnamorphoseDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Anamorphose.h"
#include "AnamorphoseDlg.h"

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


// CAnamorphoseDlg �Ի���




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


// CAnamorphoseDlg ��Ϣ�������

BOOL CAnamorphoseDlg::OnInitDialog()
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�



void CAnamorphoseDlg::OnPaint()
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

		//ӳ�䵽����  A(80,10)   B(400,170)   C(60,320)   D(200,420)

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
*	��Դ http://blog.csdn.net/pizi0475/article/details/12770813
*	����ͨ���ĵ�˵������ܲ����ܶ���������ôʹ�õ�
*	�����ҶԴ�����и���һ���Ľ��ͣ��������Լ��Ĺ�����
*	��ԭ���ߵķ����ʾ��л
*
*	���ģ�
*	��������������ǽ��б任
*	�����xo��yo�� �����Ʊ�ŵĶ���
*	����
*	int xi=(xo<<4), yi=(yo<<4);
*	�õ����Ǿ��ο� �ĸ��˵��е�һ�� �������� �����Ͻǣ����Ҫ����ζ�������ϵ��
*	m_awBuf1 �� ͼ���RGB��� ��ʼ��ַ��һά���飩
*	m_offset �� һ����ά����[A][B]���洢���ǿ��������Ϣ��Լ��ĳ���˵㣩
*	m_nMul640 �� һλ���飬�洢����Ϣ�ǿ�B ��Ӧ�� �У���ʼ�У� ��Ӧ�� RGB�����ʼ��ַƫ����
*	���ڿ�ʼ��Դ�������
*	Լ������ϵ���Ͻ� ��0 0 
*	�ı��ε� ���� 
*	A----------B
*	|		   |
*	|		   |
*	C----------D
*
*/
void CAnamorphoseDlg::TextureBlock(CPoint oldPtA, CPoint oldPtB, CPoint oldPtC, CPoint oldPtD, CPoint newPtA, CPoint newPtB, CPoint newPtC, CPoint newPtD, CPoint (&ptArray)[const_count][const_count])  
{  
	float fLeftOffX, fLeftOffY;     // ������˵��������һ����˵��ƫ��  
	float fRightOffY, fRightOffX;   // �����Ҷ˵��������һ���Ҷ˵��ƫ��  
	float TX1, TY1, TX2, TY2;       // ��ǰ�����Ҷ˵������  
	float HDx, Hdy;                 // ��ǰ�и�����ƽ��ƫ����  
	float tx, ty;                   // ��ǰͶӰ������  

	int xi = newPtA.x, yi = newPtA.y;

	WORD *Tptr;  
	fLeftOffX = (newPtC.x - newPtA.x) /16;		// ����ƽ��ƫ��  
	fLeftOffY = (newPtC.y - newPtA.y) /16;  
	fRightOffX = (newPtD.x - newPtB.x) /16;  
	fRightOffY = (newPtD.y - newPtB.y) /16;		// ����ƽ��ƫ��  
	TX1  = newPtA.x;							// ȡͶӰͼ���һ����˵�����  
	TY1  = newPtA.y;  
	TX2  = newPtB.x;							// ȡͶӰͼ���һ���Ҷ˵�����  
	TY2  = newPtB.y;  
	for (int y = yi; y < (yi+16); y++)  
	{  
		HDx  = (TX2-TX1) / 16;					// ����ͶӰͼ�鵱ǰ�и����ƽ��ƫ��  
		Hdy  = ((TY2-TY1) / 16);  
		tx = TX1;								// ͶӰƽ�浱ǰ����˵�����  
		ty = TY1;  
		for (int x = xi; x < (xi+16); x++)  
		{  
			//*Tptr++ = m_awBuf2[int(tx) + m_nMul640[int(ty)] ];		//��һ������ʵ����ɫӳ��
			tx += HDx;							// ��һ��  
			ty += Hdy;

			ptArray[y-yi][x-xi].x = tx;
			ptArray[y-yi][x-xi].y = ty;
		}    
		TX1 += fLeftOffX;						// ����ͶӰƽ������һ�����Ҷ˵������ 
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


//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CAnamorphoseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

