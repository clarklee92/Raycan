
// raycanDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "raycan.h"
#include "raycanDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CraycanDlg 对话框



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


// CraycanDlg 消息处理程序

BOOL CraycanDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	m_tabctrl.InsertItem(0, _T("系统配置(System Configuration)"));  //添加参数一选项卡 
	m_tabctrl.InsertItem(1, _T("数据采集(Data Acquisition)"));  //添加参数二选项卡
	//m_tabctrl.InsertItem(2, _T("数据处理"));  //添加参数三选项卡 
	m_calib.Create(IDD_CALIBRATION, GetDlgItem(IDC_TAB));
	m_serial.Create(IDD_SERIALPORT, GetDlgItem(IDC_TAB));
	//m_operat.Create(IDD_OPERATION, GetDlgItem(IDC_TAB));

	//获得IDC_TABTEST客户区大小
	CRect rs;
	m_tabctrl.GetClientRect(&rs);
	//调整子对话框在父窗口中的位置
	rs.top += 33;
	//rs.bottom -= 0;
	rs.left -=20;
	//rs.right -= 0;
	//设置子对话框尺寸并移动到指定位置 
	m_calib.MoveWindow(&rs);
	m_serial.MoveWindow(&rs);
	//m_operat.MoveWindow(&rs);
	//分别设置隐藏和显示 
	m_calib.ShowWindow(false);
	//m_operat.ShowWindow(false);
	m_serial.ShowWindow(true);
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CraycanDlg::OnPaint()
{
	if (IsIconic())
	{
		;
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CraycanDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CraycanDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
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

