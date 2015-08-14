
// raycanDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "raycan.h"
#include "raycanDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CraycanDlg �Ի���



CraycanDlg::CraycanDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CraycanDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CraycanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_tabctrl);
}

BEGIN_MESSAGE_MAP(CraycanDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CraycanDlg::OnTcnSelchangeTab)
END_MESSAGE_MAP()


// CraycanDlg ��Ϣ�������

BOOL CraycanDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	m_tabctrl.InsertItem(0, _T("ϵͳ����(System Configuration)"));  //��Ӳ���һѡ� 
	m_tabctrl.InsertItem(1, _T("���ݲɼ�(Data Acquisition)"));  //��Ӳ�����ѡ�
	//m_tabctrl.InsertItem(2, _T("���ݴ���"));  //��Ӳ�����ѡ� 
	m_calib.Create(IDD_CALIBRATION, GetDlgItem(IDC_TAB));
	m_serial.Create(IDD_SERIALPORT, GetDlgItem(IDC_TAB));
	//m_operat.Create(IDD_OPERATION, GetDlgItem(IDC_TAB));

	//���IDC_TABTEST�ͻ�����С
	CRect rs;
	m_tabctrl.GetClientRect(&rs);
	//�����ӶԻ����ڸ������е�λ��
	rs.top += 33;
	//rs.bottom -= 0;
	rs.left -=20;
	//rs.right -= 0;
	//�����ӶԻ���ߴ粢�ƶ���ָ��λ�� 
	m_calib.MoveWindow(&rs);
	m_serial.MoveWindow(&rs);
	//m_operat.MoveWindow(&rs);
	//�ֱ��������غ���ʾ 
	m_calib.ShowWindow(false);
	//m_operat.ShowWindow(false);
	m_serial.ShowWindow(true);
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CraycanDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CraycanDlg::OnPaint()
{
	if (IsIconic())
	{
		;
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
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CraycanDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CraycanDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int CurSel = m_tabctrl.GetCurSel();
	switch (CurSel)
	{
	case 0:
		m_calib.ShowWindow(false);
		//m_operat.ShowWindow(false);
		m_serial.ShowWindow(true);
		break;
	case 1:
		m_calib.ShowWindow(true);
		//m_operat.ShowWindow(false);
		m_serial.ShowWindow(false);
		break;
	//case 2:
		//m_calib.ShowWindow(false);
		//m_serial.ShowWindow(false);
		//m_operat.ShowWindow(true);
		break;
	default:
		break;
	}
	*pResult = 0;
}

