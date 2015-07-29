// calibration.cpp : 实现文件
//

#include "stdafx.h"
#include "raycan.h"
#include "calibration.h"
#include "afxdialogex.h"


// Ccalibration 对话框

IMPLEMENT_DYNAMIC(Ccalibration, CDialogEx)

Ccalibration::Ccalibration(CWnd* pParent /*=NULL*/)
: CDialogEx(Ccalibration::IDD, pParent)
, m_percent(_T(""))
, m_packets(_T(""))
, m_timeminute(_T(""))
, m_million(_T(""))
, m_timesecond(_T(""))
, m_timehour(_T(""))
, m_thousand(_T(""))
{
	packetcount = 0;

}

Ccalibration::~Ccalibration()
{
	WSACleanup();
}

void Ccalibration::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPEED, m_speed);
	DDX_Control(pDX, IDC_PROGRESS, m_timegoing);
	DDX_Text(pDX, IDC_PERCENT, m_percent);
	DDX_Control(pDX, IDC_MARQUEE, m_marquee);
	DDX_Text(pDX, IDC_PACKETS, m_packets);
	DDX_Text(pDX, IDC_MINUTE, m_timeminute);
	DDX_Text(pDX, IDC_MILLON, m_million);
	DDX_Text(pDX, IDC_SECOND, m_timesecond);
	DDX_Text(pDX, IDC_HOUR, m_timehour);
	DDX_Text(pDX, IDC_THOUGHD, m_thousand);
}


BEGIN_MESSAGE_MAP(Ccalibration, CDialogEx)
	ON_BN_CLICKED(IDC_BEGAINTIME, &Ccalibration::OnClickedBegaintime)
	ON_BN_CLICKED(IDC_STOPTIME, &Ccalibration::OnClickedStoptime)
	ON_BN_CLICKED(IDC_STOPBYSIZE, &Ccalibration::OnClickedStopbysize)
	ON_BN_CLICKED(IDC_STOPBYTIME, &Ccalibration::OnClickedStopbytime)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// Ccalibration 消息处理程序


BOOL Ccalibration::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CButton *Stoptime = (CButton *)GetDlgItem(IDC_STOPTIME);
	Stoptime->EnableWindow(FALSE);

	//进度条初始化
	m_timegoing.SetRange(0, 1000);//设置进度条范围
	m_timegoing.SetStep(5);
	m_timegoing.SetPos(0);

	//TCHAR FilePath_char[255];
	//GetModuleFileName(NULL, FilePath_char, 255);
	//(wcsrchr(FilePath_char, '\\'))[1] = 0;
	//wcscat_s(FilePath_char, _T("packet.dat"));
	//FilePath = FilePath_char;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}





BOOL Ccalibration::InitSocket()
{
	WSADATA wsadata;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (ret != 0)
	{
		MessageBox(_T("初始化网络协议失败!"));
		return FALSE;
	}
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //基于udp  

	if (INVALID_SOCKET == m_socket)
	{
		MessageBox(_T("套接字创建失败！"));
		return FALSE;
	}
	SOCKADDR_IN addrSock;
	addrSock.sin_family = AF_INET;
	addrSock.sin_port = htons(8000);
	addrSock.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	int retval;
	retval = bind(m_socket, (SOCKADDR*)&addrSock, sizeof(SOCKADDR));

	if (SOCKET_ERROR == retval)
	{
		closesocket(m_socket);
		MessageBox(_T("网络端口绑定失败!"));
		return FALSE;
	}
	return TRUE;
}

bool Ccalibration::OnSaveas()
{
	CFileDialog FileDlg(false, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,
		_T("DATA FILE(*.dat)|*.dat||"), NULL);//第3个参数是指定文件名，第5个参数是文件类型下拉列表
	if (FileDlg.DoModal() == IDOK)
	{
		FilePath = FileDlg.GetPathName();//得到文件完全路径，包括文件名和扩展名
		if (FilePath.Find(_T(".dat")) == -1)
		{
			FilePath += _T(".dat");
		}
		CString strFileName;
		strFileName.Format(_T("\\%s"), FileDlg.GetFileName());
		if (strFileName.Find(_T(".dat")) == -1)
		{
			strFileName += _T(".dat");
		}
		CString g_strProjectPath;
		g_strProjectPath = FilePath;
		g_strProjectPath.Replace(strFileName, _T(""));
		CFileFind m_FileFind;
		if (!m_FileFind.FindFile(g_strProjectPath)) //路径不存在则创建该路径
		{
			if (!CreateDirectory(g_strProjectPath, NULL))//创建路径失败则弹出MessageBox
			{
				AfxMessageBox(_T("路径创建失败，请重试！"));
				return false;
			}
		}
		return true;
	}
	else {
		return false;
	}
}

void Ccalibration::Recv()
{
	InitSocket();
	LARGE_INTEGER litmp;
	LONGLONG QPart1, QPart2;
	double dfMinus, dfFreq, dfTim;
	QueryPerformanceFrequency(&litmp);        //获取CPU时钟频率

	dfFreq = (double)litmp.QuadPart;
	SOCKET sock = m_socket;
	HWND hwnd = m_hWnd;
	SOCKADDR_IN addrFrom;
	int len = sizeof(SOCKADDR);
	struct timeval tv;
	fd_set readfds;
	char recvBuf[1154] = { 0 };//获取客户端发送的消息
	BYTE IP[4] = { 0 };
	CFileException fileException;
	if (myFile.Open(FilePath, CFile::typeBinary | CFile::modeCreate | CFile::modeReadWrite), &fileException)
	{
	}
	else
	{
		TRACE("Can't open file %s,error=%u/n", FilePath, fileException.m_cause);
	}
	while (TRUE)
	{
		if (Threadflag == true)
		{
			myFile.Close();
			KillTimer(1);
			double speed = (packetcount) / (dfTimsum);
			speed = speed * 48 / 1000;
			CString str;
			str.Format(_T("%.lf"), speed);
			m_speed.SetWindowText(str);
			closesocket(m_socket);
			CButton *Begaintime = (CButton *)GetDlgItem(IDC_BEGAINTIME);
			Begaintime->EnableWindow(TRUE);
			CButton *stopbysize = (CButton *)GetDlgItem(IDC_STOPBYSIZE);
			stopbysize->EnableWindow(TRUE);
			CButton *Stopbytime = (CButton *)GetDlgItem(IDC_STOPBYTIME);
			Stopbytime->EnableWindow(TRUE);
			MessageBox(_T("抓包完成!"));
			break;
		}
		QueryPerformanceCounter(&litmp);         //获取开始计数值
		QPart1 = litmp.QuadPart;
		FD_ZERO(&readfds);
		FD_SET(sock, &readfds);
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		if (!select(sock + 1, &readfds, NULL, NULL, &tv))
		{
			myFile.Close();
			closesocket(m_socket);
			KillTimer(1);
			KillTimer(2);
			m_speed.SetWindowText(_T(""));
			MessageBox(_T("请检查网络连接和IP设置！"));
			CButton *Begaintime = (CButton *)GetDlgItem(IDC_BEGAINTIME);
			Begaintime->EnableWindow(TRUE);
			CButton *stopbysize = (CButton *)GetDlgItem(IDC_STOPBYSIZE);
			stopbysize->EnableWindow(TRUE);
			CButton *Stopbytime = (CButton *)GetDlgItem(IDC_STOPBYTIME);
			Stopbytime->EnableWindow(TRUE);
			CButton *Stoptime = (CButton *)GetDlgItem(IDC_STOPTIME);
			Stoptime->EnableWindow(FALSE);
			break;
		}
		if (FD_ISSET(sock, &readfds))
		{
			retval = recvfrom(sock, recvBuf, 1154, 0, (SOCKADDR*)&addrFrom, &len);//获取套接字接收内容	
		}
		if (SOCKET_ERROR == retval)
		{
			break;
		}
		//SourceIp = inet_ntoa(addrFrom.sin_addr);
		IP[0] = addrFrom.sin_addr.S_un.S_un_b.s_b1;
		IP[1] = addrFrom.sin_addr.S_un.S_un_b.s_b2;
		IP[2] = addrFrom.sin_addr.S_un.S_un_b.s_b3;
		IP[3] = addrFrom.sin_addr.S_un.S_un_b.s_b4;
		myFile.Write(IP, 4);
		myFile.Write(recvBuf, 1154);
		packetcount++;

		QueryPerformanceCounter(&litmp);        //获取结束计数值
		QPart2 = litmp.QuadPart;
		dfMinus = (double)(QPart2 - QPart1);
		dfTim = dfMinus / dfFreq;                       //计算持续时间，单位为秒误差不超过1微秒
		dfTimsum += dfTim;
	}
}

DWORD WINAPI Ccalibration::RecvProc(LPVOID lpParameter)
{
	Ccalibration* pDlg = (Ccalibration*)lpParameter;
	pDlg->Recv();
	return 0;
}


DWORD WINAPI Ccalibration::SpeedProc(LPVOID lpParameter)
{
	Ccalibration* pDlg = (Ccalibration*)lpParameter;
	pDlg->SetTimer(1, 200, NULL);
	pDlg->SetTimer(2, 10, NULL);
	return 0;
}

void Ccalibration::OnClickedBegaintime()
{
	// TODO:  在此添加控件通知处理程序代码
	Saveasret = OnSaveas();
	if (!Saveasret)
	{
		return;
	}
	Threadflag = false;
	Stopflag = true;
	CButton *Stoptime = (CButton *)GetDlgItem(IDC_STOPTIME);
	Stoptime->EnableWindow(TRUE);
	if (Stopbytimeflag == false && Stopbysizeflag == false && Stopflag == true)
	{
		m_timegoing.ShowWindow(FALSE);
		m_marquee.ShowWindow(TRUE);
		m_percent = _T("正在抓包...");
		this->SetDlgItemText(IDC_PERCENT, m_percent);
	}
	//创建一线程接收  
	packetcount = 0;
	dfTimsum = 0;
	CRITICAL_SECTION g_cs;
	InitializeCriticalSection(&g_cs);
	HANDLE hThread_RECV = CreateThread(NULL, 0, RecvProc, this, 0, NULL);
	CloseHandle(hThread_RECV);
	//创建一线程记录采集速率  
	Sleep(500);
	HANDLE hThread_SPEED = CreateThread(NULL, 0, SpeedProc, this, 0, NULL);
	CloseHandle(hThread_SPEED);
	DeleteCriticalSection(&g_cs);

	CButton *stopbysize = (CButton *)GetDlgItem(IDC_STOPBYSIZE);
	stopbysize->EnableWindow(FALSE);
	CButton *Begintime = (CButton *)GetDlgItem(IDC_BEGAINTIME);
	Begintime->EnableWindow(FALSE);
	CButton *Stopbytime = (CButton *)GetDlgItem(IDC_STOPBYTIME);
	Stopbytime->EnableWindow(FALSE);
}


void Ccalibration::OnClickedStoptime()
{
	// TODO:  在此添加控件通知处理程序代码
	Threadflag = true;
	this->SetDlgItemText(IDC_PERCENT, _T(""));
	CButton *Stoptime = (CButton *)GetDlgItem(IDC_STOPTIME);
	Stoptime->EnableWindow(FALSE);

}


CString Ccalibration::byteToHexStr(byte byte_arr[], int arr_len)

{

	CString hexstr;
	for (int i = 0; i<arr_len; i++)
	{
		CString hex1;
		CString hex2;
		int value;
		/*借助C++支持的unsigned和int的强制转换，把unsigned char赋值给int的值，那么系统就会自动完成强制转换*/
		value = byte_arr[i];
		int S = value / 16;
		int Y = value % 16;
		//将C++中unsigned char和int的强制转换得到的商转成字母
		if (S >= 0 && S <= 9)
			hex1 = (char)(48 + S);
		else
			hex1 = (char)(55 + S);
		//将C++中unsigned char和int的强制转换得到的余数转成字母
		if (Y >= 0 && Y <= 9)
			hex2 = (char)(48 + Y);
		else
			hex2 = (char)(55 + Y);
		//最后一步的代码实现，将所得到的两个字母连接成字符串达到目的
		//*hexstr = *hexstr + hex1 + hex2;
		hexstr += hex1 + hex2;
	}
	return hexstr;
}


void Ccalibration::OnClickedStopbysize()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	int x = _ttoi(m_million);
	if (_ttoi(m_packets) >999 || _ttoi(m_thousand)  > 999 || _ttoi(m_million) > 3)
	{
		MessageBox(_T("超过范围！"));
		return;
	}
	if (_ttoi(m_packets) + _ttoi(m_thousand) * 1000 + _ttoi(m_million) * 1000000 == 0)
	{
		MessageBox(_T("请输入抓包数！"));
		return;
	}
	Stopbysizeflag = true;
	OnClickedBegaintime();
	if (!Saveasret)
	{
		return;
	}
	CButton *stopbysize = (CButton *)GetDlgItem(IDC_STOPBYSIZE);
	stopbysize->EnableWindow(FALSE);
	CButton *Begintime = (CButton *)GetDlgItem(IDC_BEGAINTIME);
	Begintime->EnableWindow(FALSE);
	CButton *Stopbytime = (CButton *)GetDlgItem(IDC_STOPBYTIME);
	Stopbytime->EnableWindow(FALSE);
	CRITICAL_SECTION g_cs;
	InitializeCriticalSection(&g_cs);
	HANDLE hThread_s = CreateThread(NULL, 0, Stopbysize, this, 0, NULL);
	CloseHandle(hThread_s);
	DeleteCriticalSection(&g_cs);

}

DWORD WINAPI Ccalibration::Stopbysize(LPVOID lpParameter)
{
	Ccalibration* pDlg = (Ccalibration*)lpParameter;
	while (TRUE)
	{
		if (pDlg->packetcount >= (unsigned long)(_ttoi(pDlg->m_packets) + _ttoi(pDlg->m_thousand) * 1000 + _ttoi(pDlg->m_million) * 1000000) || pDlg->Threadflag == true)
		{
			pDlg->Threadflag = true;
			break;
		}
	}
	return 0;
}

DWORD WINAPI Ccalibration::Stopbytime(LPVOID lpParameter)
{
	Ccalibration* pDlg = (Ccalibration*)lpParameter;
	while (TRUE)
	{
		if (pDlg->dfTimsum >= (_ttoi(pDlg->m_timehour) * 3600 + _ttoi(pDlg->m_timeminute) * 60 + _ttoi(pDlg->m_timesecond)) || pDlg->Threadflag == true)
		{
			pDlg->Threadflag = true;
			break;
		}
	}
	return 0;
}

void Ccalibration::OnClickedStopbytime()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (_ttoi(m_timesecond)>59 || _ttoi(m_timeminute)>59 || _ttoi(m_timehour)>2)
	{
		MessageBox(_T("超出范围！"));
		return;
	}
	if (_ttoi(m_timesecond) + _ttoi(m_timeminute) * 60 + _ttoi(m_timehour) * 3600 == 0)
	{
		MessageBox(_T("请输入抓包时间！"));
		return;
	}
	Stopbytimeflag = true;
	OnClickedBegaintime();
	if (!Saveasret)
	{
		return;
	}
	CButton *stopbysize = (CButton *)GetDlgItem(IDC_STOPBYSIZE);
	stopbysize->EnableWindow(FALSE);
	CButton *Begintime = (CButton *)GetDlgItem(IDC_BEGAINTIME);
	Begintime->EnableWindow(FALSE);
	CButton *stopbytime = (CButton *)GetDlgItem(IDC_STOPBYTIME);
	stopbytime->EnableWindow(FALSE);

	CRITICAL_SECTION g_cs;
	InitializeCriticalSection(&g_cs);
	HANDLE hThread_t = CreateThread(NULL, 0, Stopbytime, this, 0, NULL);
	CloseHandle(hThread_t);
	DeleteCriticalSection(&g_cs);
}


void Ccalibration::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	if (nIDEvent == 1)
	{
		double speed = (packetcount /*- num_temp*/) / (dfTimsum/* - t_temp*/);
		speed = speed * 48 / 1000;
		CString str;
		str.Format(_T("%.lf"), speed);
		m_speed.SetWindowText(str);
		/*num_temp = packetcount;
		t_temp = dfTimsum;*/
	}
	else{
		//进度条


		if (Stopbysizeflag == true && Stopflag == true && Stopbytimeflag == false)
		{
			m_timegoing.ShowWindow(TRUE);
			m_marquee.ShowWindow(FALSE);
			m_nPos = (unsigned long)((packetcount * 1000) / (unsigned long)(_ttoi(m_packets) + _ttoi(m_thousand) * 1000 + _ttoi(m_million) * 1000000) + 0.5);
			if (m_nPos >= 1000)
			{
				m_nPos = 1000;
				Stopflag = false;
				Stopbysizeflag = false;
				Stopbytimeflag = false;
				KillTimer(2);
			}
			else if (Threadflag == true)
			{
				Stopflag = false;
				Stopbysizeflag = false;
				Stopbytimeflag = false;
				KillTimer(2);
			}
			m_timegoing.SetPos(m_nPos);
			m_percent.Format(_T("%d"), m_nPos / 10);
			m_percent += '%';
			this->SetDlgItemText(IDC_PERCENT, m_percent);
		}
		if (Stopbytimeflag == true && Stopflag == true && Stopbysizeflag == false)
		{
			m_timegoing.ShowWindow(TRUE);
			m_marquee.ShowWindow(FALSE);
			m_nPos = (int)((dfTimsum * 1000) / (_ttoi(m_timesecond) + _ttoi(m_timeminute) * 60 + _ttoi(m_timehour) * 3600) + 0.5);
			if (m_nPos >= 1000)
			{
				m_nPos = 1000;
				Stopflag = false;
				Stopbysizeflag = false;
				Stopbytimeflag = false;
				KillTimer(2);
			}
			else if (Threadflag == true)
			{
				Stopflag = false;
				Stopbysizeflag = false;
				Stopbytimeflag = false;
				KillTimer(2);
			}
			m_timegoing.SetPos(m_nPos);
			m_percent.Format(_T("%d"), m_nPos / 10);
			m_percent += '%';
			this->SetDlgItemText(IDC_PERCENT, m_percent);
		}
		if (Stopbytimeflag == false && Stopbysizeflag == false && Stopflag == true)
		{
			m_marquee.OffsetPos(10);
			if (Threadflag == true)
			{
				Stopflag = false;
				Stopbysizeflag = false;
				Stopbytimeflag = false;
				KillTimer(2);
			}
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}


