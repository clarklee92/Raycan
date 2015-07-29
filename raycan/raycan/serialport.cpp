// serialport.cpp : 实现文件
//

#include "stdafx.h"
#include "raycan.h"
#include "serialport.h"
#include "afxdialogex.h"
#include "ini.h"

// Cserialport 对话框

IMPLEMENT_DYNAMIC(Cserialport, CDialogEx)

Cserialport::Cserialport(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cserialport::IDD, pParent)
	, m_value(0)
	, m_boardnum(_T(""))
	, m_filepath(_T(""))
	, m_percent(_T(""))
	, m_recvcount(_T(""))
{
	flag_Initialization = false;
	m_threshold1 = 0;
	m_threshold2 = 0;
	m_threshold3 = 0;
	m_threshold4 = 0;
	Recv_ZL = "0100000000fe";
	ZL_RT = "0000000000";
	R_ZL_boardnumber ="2600000000";
	R_ZL_IP1 = "2100040000";
	R_ZL_IP2 = "2100030000";
	R_ZL_IP3 = "2100020000";
	R_ZL_IP4 = "2100010000";

	W_ZL_boardnumber = "160000";
	W_ZL_IP1 = "110004";
	W_ZL_IP2 = "110003";
	W_ZL_IP3 = "110002";
	W_ZL_IP4 = "110001";
}

Cserialport::~Cserialport()
{
}


void Cserialport::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BAUDRATE, m_baudrate);
	DDX_Control(pDX, IDC_COMMPORT, m_commport);
	DDX_Control(pDX, IDC_DATABIT, m_databit);
	DDX_Control(pDX, IDC_PARITYBIT, m_paritybit);
	DDX_Control(pDX, IDC_STOPBIT, m_stopbit);
	DDX_Control(pDX, IDC_CHANNEL, m_channel);
	DDX_Control(pDX, IDC_THRESHOLDNUM, m_thresholdnum);
	DDX_Text(pDX, IDC_VALUE, m_value);
	DDX_Control(pDX, IDC_IPADDRESS, m_ipaddress);
	DDX_Text(pDX, IDC_BOARDNUM, m_boardnum);
	DDX_Text(pDX, IDC_THRESHOLD1, m_threshold1);
	DDX_Text(pDX, IDC_THRESHOLD2, m_threshold2);
	DDX_Text(pDX, IDC_THRESHOLD3, m_threshold3);
	DDX_Text(pDX, IDC_THRESHOLD4, m_threshold4);
	DDX_Control(pDX, IDC_NUM, m_numnote);
	DDX_Control(pDX, IDC_IP, m_ipnote);
	DDX_Control(pDX, IDC_RECVIEW, m_recview);
	DDX_Control(pDX, IDC_PROGRESS, m_progressctrl);
	DDX_Text(pDX, IDC_PERCENT, m_percent);
	DDX_Text(pDX, IDC_RECVCOUNT, m_recvcount);
}


BEGIN_MESSAGE_MAP(Cserialport, CDialogEx)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS, &Cserialport::OnIpnFieldchangedIpaddress)
	ON_BN_CLICKED(IDC_OPENCOMM, &Cserialport::OnClickedOpencomm)
	ON_BN_CLICKED(IDC_READBOARDNUM, &Cserialport::OnClickedReadboardnum)
	ON_BN_CLICKED(IDC_WRITEBOARDNUM, &Cserialport::OnBnClickedWriteboardnum)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_WRITEIP, &Cserialport::OnClickedWriteip)
	ON_BN_CLICKED(IDC_READIP, &Cserialport::OnClickedReadip)
	ON_BN_CLICKED(IDC_CLOSECOMM, &Cserialport::OnClickedClosecomm)
	ON_BN_CLICKED(IDC_RESET, &Cserialport::OnClickedReset)
	ON_BN_CLICKED(IDC_READ, &Cserialport::OnClickedRead)
	ON_BN_CLICKED(IDC_SENDDATA, &Cserialport::OnClickedSenddata)
	ON_BN_CLICKED(IDC_WRITE, &Cserialport::OnClickedWrite)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_STOP, &Cserialport::OnBnClickedStopsenddata)
END_MESSAGE_MAP()


// Cserialport 消息处理程序


BOOL Cserialport::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CButton *CloseComm = (CButton *)GetDlgItem(IDC_CLOSECOMM);
	CloseComm->EnableWindow(FALSE);
	//进度条初始化
	m_progressctrl.SetRange(0,100);//设置进度条范围
	m_progressctrl.SetStep(5);
	m_progressctrl.SetPos(0);
	processcount = 0;
	//读取各项ini值
	UpdateData(TRUE);
	this->m_threshold1 = Cini::ReadInt(_T("Threshold"), _T("Threshold1"), _T("raycan.ini"));
	this->m_threshold2 = Cini::ReadInt(_T("Threshold"), _T("Threshold2"), _T("raycan.ini"));
	this->m_threshold3 = Cini::ReadInt(_T("Threshold"), _T("Threshold3"), _T("raycan.ini"));
	this->m_threshold4 = Cini::ReadInt(_T("Threshold"), _T("Threshold4"), _T("raycan.ini"));
	UpdateData(FALSE);
	for (int i = 0; i < 72;i++)
	{
		data[i][0] = m_threshold1;
		data[i][1] = m_threshold2;
		data[i][2] = m_threshold3;
		data[i][3] = m_threshold4;	
	}
	//获取可用串口
	CString  strCom, strComOpen;
	int  nCom = 0;
	int  count = 0;
	HANDLE    hCom_;

	m_commport.ResetContent();
	do
	{
		nCom++;
		strCom.Format(_T("COM%d"), nCom);
		strComOpen.Format(_T("\\\\.\\COM%d"), nCom);
		hCom_ = CreateFile(strComOpen, GENERIC_READ | GENERIC_WRITE, 0, NULL,
			OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
		if (INVALID_HANDLE_VALUE == hCom_)
		{
			DWORD error = ::GetLastError();//取得错误信息 
		}
		else
		{
			m_commport.AddString(strCom);
			count++;
		}
		CloseHandle(hCom_);
	} while (nCom<256);
	m_commport.SetCurSel(0);

	//设置波特率选择列表
	m_baudrate.ResetContent(); 
	m_baudrate.AddString(_T("1200"));
	m_baudrate.SetItemData(0, 1200);
	m_baudrate.AddString(_T("2400"));
	m_baudrate.SetItemData(1, 2400);
	m_baudrate.AddString(_T("4800"));
	m_baudrate.SetItemData(2, 4800);
	m_baudrate.AddString(_T("9600"));
	m_baudrate.SetItemData(3, 9600);
	m_baudrate.AddString(_T("14400"));
	m_baudrate.SetItemData(4, 14400);
	m_baudrate.AddString(_T("19200"));
	m_baudrate.SetItemData(5, 19200);
	m_baudrate.AddString(_T("28800"));
	m_baudrate.SetItemData(6, 28800);
	m_baudrate.AddString(_T("38400"));
	m_baudrate.SetItemData(7, 38400);
	m_baudrate.AddString(_T("57600"));
	m_baudrate.SetItemData(8, 57600);
	m_baudrate.AddString(_T("115200"));	
	m_baudrate.SetItemData(9, 115200);
	m_baudrate.SetCurSel(5);

	//设置校验位下拉列表
	m_paritybit.ResetContent();
	m_paritybit.AddString(_T("None"));
	m_paritybit.SetItemData(0,NOPARITY);
	m_paritybit.AddString(_T("Odd"));
	m_paritybit.SetItemData(1,ODDPARITY);
	m_paritybit.AddString(_T("Even"));
	m_paritybit.SetItemData(2,EVENPARITY);
	m_paritybit.AddString(_T("Mark"));
	m_paritybit.SetItemData(3,MARKPARITY);
	m_paritybit.AddString(_T("Space"));
	m_paritybit.SetItemData(4,SPACEPARITY);
	m_paritybit.SetCurSel(0);

	//设置数据位
	m_databit.ResetContent();
	m_databit.AddString(_T("8"));
	m_databit.SetItemData(0,8);
	m_databit.AddString(_T("7"));
	m_databit.SetItemData(1,7);
	m_databit.AddString(_T("6"));
	m_databit.SetItemData(2,6);
	m_databit.SetCurSel(0);

	//设置停止位
	m_stopbit.ResetContent();
	m_stopbit.AddString(_T("1"));
	m_stopbit.SetItemData(0, ONESTOPBIT);
	m_stopbit.AddString(_T("1.5"));
	m_stopbit.SetItemData(1, ONE5STOPBITS);
	m_stopbit.AddString(_T("2"));
	m_stopbit.SetItemData(2, TWOSTOPBITS);
	m_stopbit.SetCurSel(0);

	//设置通道号
	m_channel.ResetContent();
	CString str;
	for (int i=0; i<72;i++)
	{
		str.Format(_T("%d"), i+1);
		m_channel.AddString(str);
		m_channel.SetItemData(i,i+1);
	}
	m_channel.SetCurSel(0);

	//设置阈值号
	m_thresholdnum.ResetContent();
	m_thresholdnum.AddString(_T("1"));
	m_thresholdnum.SetItemData(0,1);
	m_thresholdnum.AddString(_T("2"));
	m_thresholdnum.SetItemData(1, 2);
	m_thresholdnum.AddString(_T("3"));
	m_thresholdnum.SetItemData(2,3);
	m_thresholdnum.AddString(_T("4"));
	m_thresholdnum.SetItemData(3,4);
	m_thresholdnum.SetCurSel(0);

	CButton *Stopsenddata = (CButton *)GetDlgItem(IDC_STOP);
	Stopsenddata->EnableWindow(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void Cserialport::OnIpnFieldchangedIpaddress(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 0;
}

void Cserialport::OnClickedOpencomm()
{
	// TODO:  在此添加控件通知处理程序代码
	//Get_paritybit(R_ZL_boardnumber);
	CButton *CloseComm = (CButton *)GetDlgItem(IDC_CLOSECOMM);
	CloseComm->EnableWindow(TRUE);
	CButton *OpenComm = (CButton *)GetDlgItem(IDC_OPENCOMM);
	OpenComm->EnableWindow(FALSE);
	CString Str;
	if (m_commport.GetCount() == 0)
	{
		MessageBox(_T("没有可用的串口!"));
		return;
	}
	m_commport.GetLBText(m_commport.GetCurSel(), Str);
	hCom = CreateFile(Str,//串口名称 
		GENERIC_READ | GENERIC_WRITE,//允许读和写              
		0,//独占方式                
		NULL,
		OPEN_EXISTING,//打开而不是创建 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,//重叠方式               
		NULL);
	if (hCom == INVALID_HANDLE_VALUE)
	{
		MessageBox(_T("串口打开失败!"));
		return;
	}
	else{
		flag_Initialization = true;
		MessageBox(_T("串口打开成功!"));
		SetupComm(hCom,1024,1024);
		COMMTIMEOUTS TimeOuts; //设定读超时 
		TimeOuts.ReadIntervalTimeout=1000;  
		TimeOuts.ReadTotalTimeoutMultiplier=500;
		TimeOuts.ReadTotalTimeoutConstant=5000; //设定写超时  
		TimeOuts.WriteTotalTimeoutMultiplier=500; 
		TimeOuts.WriteTotalTimeoutConstant=2000; 
	    SetCommTimeouts(hCom,&TimeOuts);//设置超时
		DCB dcb;
		GetCommState(hCom, &dcb);
		dcb.BaudRate = m_baudrate.GetItemData(m_baudrate.GetCurSel());//波特率
		dcb.ByteSize = m_databit.GetItemData(m_databit.GetCurSel());//数据位
		dcb.Parity = m_paritybit.GetItemData(m_paritybit.GetCurSel());//奇偶校验位 
		dcb.StopBits = m_stopbit.GetItemData(m_stopbit.GetCurSel());//停止位 
		SetCommState(hCom, &dcb);
		PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
	}
}


DWORD Cserialport::COM_read(DWORD dwBytesRead,CString & Buffer)
{
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	OVERLAPPED m_osRead;
	memset(&m_osRead,0,sizeof(OVERLAPPED));
	m_osRead.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	ClearCommError(hCom, &dwErrorFlags, &ComStat);
	dwBytesRead = min(dwBytesRead,(DWORD)ComStat.cbInQue);
	if (!dwBytesRead)
		return FALSE;
	BOOL bReadStatus;
	byte* Rbuffer=new byte [dwBytesRead];
	bReadStatus = ReadFile(hCom,Rbuffer,dwBytesRead, &dwBytesRead, &m_osRead);
	Buffer = bth(Rbuffer);
	delete[] Rbuffer;
	if (!bReadStatus)//如果ReadFile函数返回FALSE 
	{
		if (GetLastError()==ERROR_IO_PENDING)
		//GetLastError()函数返回ERROR_IO_PENDING,表明串口正在进行读操作 
		{
			WaitForSingleObject(m_osRead.hEvent, 500);
	//使用WaitForSingleObject函数等待,直到读操作完成或延时已达到0.5秒钟 
	//当串口读操作进行完毕后,m_osRead的hEvent事件会变为有信号 
			PurgeComm(hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
			return dwBytesRead;
    }
    return 0;
	}
	PurgeComm(hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	return dwBytesRead;
}

DWORD Cserialport::COM_write(DWORD dwBytesWritten,CString&Buffer)
{
	OVERLAPPED m_osWrite;
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	memset(&m_osWrite, 0, sizeof(OVERLAPPED));
	m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	ClearCommError(hCom, &dwErrorFlags, &ComStat);
	BOOL bWriteStat;
	byte* Wbuffer = htb(Buffer);
	bWriteStat = WriteFile(hCom,Wbuffer,dwBytesWritten, &dwBytesWritten, &m_osWrite);
	if (!bWriteStat){
		int error = GetLastError();
		if (GetLastError() == ERROR_IO_PENDING){
			WaitForSingleObject(m_osWrite.hEvent,1000);
			PurgeComm(hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
			return dwBytesWritten;
		}
		return 0;
	}
	PurgeComm(hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	return dwBytesWritten;
}

int Cserialport::HexToDem(CString str) //16进制转化为10进制int
{
	int dem = 0;
	for (int i = 0; i < str.GetLength(); i++)
	{
		dem = dem * 16;
		if ((str[i] <= '9') && (str[i] >= '0'))        //0~9之间的字符
			dem += str[i] - '0';
		else if ((str[i] <= 'F') && (str[i] >= 'A'))   //A~F之间的字符
			dem += str[i] - 'A' + 10;
		else if ((str[i] <= 'f') && (str[i] >= 'a'))   //a~f之间的字符
			dem += str[i] - 'a' + 10;
		else
			return -1;                          //出错时返回-1
	}
	return dem;
}


CString Cserialport::DemToHex(CString str) //10进制字符转化为16进制字符
{
		int ihex, i, j = 0;
		int buffer[50];
		int iint = _ttoi(str);
		char hOut[50];
		while (iint / 16)
		{
			ihex = iint % 16;
			buffer[j++] = ihex;
			iint /= 16;
		}
		buffer[j++] = iint;
		i = 0;
		while (j--)
		{

			if (buffer[j] >= 0 && buffer[j] <= 9)
				hOut[i++] = buffer[j] + 48;
			else
			{
				switch (buffer[j])
				{
				case 10:
					hOut[i++] = 'a';
					break;
				case 11:
					hOut[i++] = 'b';
					break;
				case 12:
					hOut[i++] = 'c';
					break;
				case 13:
					hOut[i++] = 'd';
					break;
				case 14:
					hOut[i++] = 'e';
					break;
				case 15:
					hOut[i++] = 'f';
					break;
				default:
					break;
				}
			}
		}
		hOut[i] = '\0';
		CString temp,r;
		temp=hOut;
		r.Format(_T("%04s"),temp);
		return r;	 
}

byte* Cserialport::htb(CString buf) //16进制字符转byte
{
	byte *m_byte = new byte[buf.GetLength()/2];
	for (int i = 0; i < buf.GetLength(); i += 2){
		m_byte[i / 2] = HexToDem(buf.Mid(i, 2));
	}
	return m_byte;
}

CString Cserialport::bth(byte *buf) //byte转16进制字符
{
	CString m_Str; 
	for (int m = 0; m < 6;m++)
	{
		int ihex, i, j = 0;
		int buffer[50];
		char hOut[50];
		while (buf[m] / 16)
		{
			ihex = buf[m] % 16;
			buffer[j++] = ihex;
			buf[m] /= 16;
		}
		buffer[j++] = buf[m];
		i = 0;
		while (j--)
		{

			if (buffer[j] >= 0 && buffer[j] <= 9)
				hOut[i++] = buffer[j] + 48;
			else
			{
				switch (buffer[j])
				{
				case 10:
					hOut[i++] = 'a';
					break;
				case 11:
					hOut[i++] = 'b';
					break;
				case 12:
					hOut[i++] = 'c';
					break;
				case 13:
					hOut[i++] = 'd';
					break;
				case 14:
					hOut[i++] = 'e';
					break;
				case 15:
					hOut[i++] = 'f';
					break;
				default:
					break;
				}
			}
		}
		hOut[i] = '\0';
		CString temp,r;
		temp = hOut;
		r.Format(_T("%02s"),temp);
		m_Str += r;
	}
	return m_Str;
}


CString Cserialport::Get_paritybit(CString data)   //获取校验位，算法：前5个bytes直接加和，进位部分加低位，求补
{
	//获取前5bytes
	CString OP = data.Left(2);
	CString AD_H = data.Mid(2,2);
	CString AD_L = data.Mid(4,2);
	CString DATA_H = data.Mid(6,2);
	CString DATA_L = data.Right(2);
	//前5bytes转成10进制然后求和
	int value = HexToDem(OP) + HexToDem(AD_H) + HexToDem(AD_L)
		+ HexToDem(DATA_H) + HexToDem(DATA_L);
	//和先转成4位16进制数,然后舍弃前2位，即进位
	int SUM = 255 -  value % (16 * 16);
	char a[2];
if (0 <= (SUM / 16 )&& (SUM / 16 )<= 9)
	{
		a[0] = SUM / 16+'0';
	}
	else{
		a[0] = 'a' - 10 + SUM / 16;
	}
	if (0 <= (SUM %16) && (SUM %16) <= 9)
	{
		a[1] = SUM % 16+'0';
	}
	else{
		a[1] = 'a' - 10 + SUM % 16;
	}
	CString r(a);
	return r.Left(2);
}

void Cserialport::OnClickedReadboardnum()
{
	// TODO:  在此添加控件通知处理程序代码
	if (hCom == INVALID_HANDLE_VALUE || m_commport.GetCount() == 0||flag_Initialization==false)
	{
		MessageBox(_T("串口未初始化!"));
		return;
	}
	else{
		if (processcount != 0 && processcount != 288)
		{
			MessageBox(_T("串口正忙!"));
			return;
		}
		else{
			m_numnote.SetWindowText(_T(""));
			CString ZL;
			ZL = R_ZL_boardnumber + Get_paritybit(R_ZL_boardnumber);
			COM_write(ZL.GetLength(), ZL);
			OVERLAPPED de_lay;
			memset(&de_lay, 0, sizeof(OVERLAPPED));
			de_lay.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			WaitForSingleObject(de_lay.hEvent, 100);
			CString Recv;
			COM_read(100, Recv);
			m_recview.SetWindowText(_T(""));
			m_recview.SetSel(-1, -1); //自动滚屏 
			m_recview.ReplaceSel(Recv + "\r\n"); //自动换行 
			UpdateData(true);
			m_recvcount="";
			m_recvcount.Format(_T("%d"), Recv.GetLength() / 12);
			m_recvcount += "/1";
			UpdateData(FALSE);
			if (Recv.GetLength() == 12 && Recv.Left(2) == "01" && Recv.Mid(2, 4) == "0000")
			{
				CString Recv_ZL_CZ = Recv.Left(6);
				CString Recv_ZL_JY = Get_paritybit(Recv.Left(10));
				if (Recv_ZL_CZ == "010000" && Recv_ZL_JY == Recv.Mid(10, 2))
				{
					//板子编号
					m_boardnum.Format(_T("%d"), HexToDem(Recv.Mid(6, 4)));
					UpdateData(FALSE);
					m_numnote.SetWindowText(_T("读取板号成功"));
				}
				else
				{
					m_boardnum = "null";
					UpdateData(FALSE);
					MessageBox(_T("接收的解析指令异常，请核查硬件或串口号!"));
					return;
				}
			}
			else
			{
				m_boardnum = "null";
				UpdateData(FALSE);
				MessageBox(_T("接收的解析指令异常，请核查硬件或串口号!"));
				return;
			}

		}
	}
}


void Cserialport::OnBnClickedWriteboardnum()
{
	// TODO:  在此添加控件通知处理程序代码
	if (hCom == INVALID_HANDLE_VALUE || m_commport.GetCount() == 0 || flag_Initialization == false)
	{
		MessageBox(_T("串口未初始化!"));
		return;
	}
	else{
		if (processcount != 0 && processcount != 288)
		{
			MessageBox(_T("串口正忙!"));
			return;
		}
		else{
			UpdateData(TRUE);
			if (m_boardnum.IsEmpty())
			{
				MessageBox(_T("请输入板号!"));
				return;
			}
			else{
				int flag = 0;
				for (int i = 0; i < m_boardnum.GetLength(); i++)
				{
					if (m_boardnum.GetAt(i) > ('0' - 1) && m_boardnum.GetAt(i) < ('9' + 1))
					{
						flag++;
					}
					else{
						MessageBox(_T("请输入正确的板号!"));
						return;
					}
				}
				if (flag == m_boardnum.GetLength())
				{
					int boardrange = _ttoi(m_boardnum);
					if (boardrange >= 0 && boardrange < 10000)
					{
						CString ZL;
						UpdateData(TRUE);
						ZL = W_ZL_boardnumber + DemToHex(m_boardnum) + Get_paritybit(W_ZL_boardnumber + DemToHex(m_boardnum));
						COM_write(ZL.GetLength(), ZL);
						OVERLAPPED de_lay;
						memset(&de_lay, 0, sizeof(OVERLAPPED));
						de_lay.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
						WaitForSingleObject(de_lay.hEvent, 100);
						CString Recv;
						COM_read(100, Recv);
						m_recview.SetWindowText(_T(""));
						m_recview.SetSel(-1, -1); //自动滚屏 
						m_recview.ReplaceSel(Recv + "\r\n"); //自动换行 
						UpdateData(true);
						m_recvcount="";
						m_recvcount.Format(_T("%d"), Recv.GetLength() / 12);
						m_recvcount += "/1";
						UpdateData(FALSE);
						if (Recv == Recv_ZL)
						{
							//m_boardnum = _T("已写入");
							//UpdateData(FALSE);
							m_numnote.SetWindowText(_T("写入板号成功"));
						}
						else{
							m_numnote.SetWindowText(_T("失败，请重试!"));
							return;
						}
					}
					else{
						MessageBox(_T("超出范围"));
						return;
					}
				}
			}
		}
	}
}

	void Cserialport::OnClickedWriteip()
	{
		// TODO:  在此添加控件通知处理程序代码
		if (hCom == INVALID_HANDLE_VALUE || m_commport.GetCount() == 0 || flag_Initialization == false)
		{
			MessageBox(_T("串口未初始化!"));
			return;
		}
		else{
			if (processcount != 0 && processcount != 288)
			{
				MessageBox(_T("串口正忙!"));
				return;
			}
			else{
				UpdateData(TRUE);
				if (m_ipaddress.IsBlank())
				{
					MessageBox(_T("请输入IP!"));
					return;
				}
				else{
					CString ZL, IP_Str, IP1, IP2, IP3, IP4;
					CString Recv1, Recv2, Recv3, Recv4;
					UpdateData(TRUE);
					byte ip1, ip2, ip3, ip4;
					m_ipaddress.GetAddress(ip1, ip2, ip3, ip4);
					byte IP[6] = { ip1, ip2, ip3, ip4, 0, 0 };
					IP_Str = bth(IP);
					IP_Str = IP_Str.Mid(0, 8);
					OVERLAPPED de_lay;
					memset(&de_lay, 0, sizeof(OVERLAPPED));
					de_lay.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
					IP1 = IP_Str.Mid(0, 2);
					ZL = W_ZL_IP1 + "00" + IP1 + Get_paritybit(W_ZL_IP1 + IP1 + _T("00"));
					COM_write(ZL.GetLength(), ZL);
					WaitForSingleObject(de_lay.hEvent, 100);
					COM_read(100, Recv1);
					IP2 = IP_Str.Mid(2, 2);
					ZL = W_ZL_IP2 + "00" + IP2 + Get_paritybit(W_ZL_IP2 + IP2 + _T("00"));
					COM_write(ZL.GetLength(), ZL);
					WaitForSingleObject(de_lay.hEvent, 100);
					COM_read(100, Recv2);
					IP3 = IP_Str.Mid(4, 2);
					ZL = W_ZL_IP3 + "00" + IP3 + Get_paritybit(W_ZL_IP3 + IP3 + _T("00"));
					COM_write(ZL.GetLength(), ZL);
					WaitForSingleObject(de_lay.hEvent, 100);
					COM_read(100, Recv3);
					IP4 = IP_Str.Mid(6, 8);
					ZL = W_ZL_IP4 + "00" + IP4 + Get_paritybit(W_ZL_IP4 + IP4 + _T("00"));
					COM_write(ZL.GetLength(), ZL);
					WaitForSingleObject(de_lay.hEvent, 100);
					COM_read(100, Recv4);
					CString Recv;
					Recv = Recv1 + _T("       ") + Recv2 + _T("       ") + Recv3 + _T("       ") + Recv4 + _T("       ");
					m_recview.SetWindowText(_T(""));
					m_recview.SetSel(-1, -1); //自动滚屏 
					m_recview.ReplaceSel(Recv + "\r\n"); //自动换行 
					UpdateData(true);
					m_recvcount = "";
					m_recvcount.Format(_T("%d"), (Recv1 + Recv2 + Recv3 + Recv4).GetLength() / 12);
					m_recvcount += "/4";
					UpdateData(FALSE);
					if (Recv1 == Recv_ZL&&Recv2 == Recv_ZL&&Recv3 == Recv_ZL&&Recv4 == Recv_ZL)
					{
						m_ipnote.SetWindowText(_T("写入IP成功"));
						//m_ipaddress.SetAddress(0, 0, 0, 0);
					}
					else{
						m_ipnote.SetWindowText(_T("失败，请重试!"));
						return;
					}
				}
			}
		}
	}


	void Cserialport::OnClickedReadip()
	{
		// TODO:  在此添加控件通知处理程序代码
		if (hCom == INVALID_HANDLE_VALUE || m_commport.GetCount() == 0 || flag_Initialization == false)
		{
			MessageBox(_T("串口未初始化!"));
			return;
		}
		else{
			if (processcount != 0 && processcount != 288)
			{
				MessageBox(_T("串口正忙!"));
				return;
			}
			else{
				CString ZL, IP1, IP2, IP3, IP4;
				CString Recv1, Recv2, Recv3, Recv4;

				OVERLAPPED de_lay;
				memset(&de_lay, 0, sizeof(OVERLAPPED));
				de_lay.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
				ZL = R_ZL_IP1 + Get_paritybit(R_ZL_IP1);
				COM_write(ZL.GetLength(), ZL);
				WaitForSingleObject(de_lay.hEvent, 100);
				COM_read(100, Recv1);
				m_recview.SetWindowText(_T(""));
				m_recview.SetSel(-1, -1); //自动滚屏 
				m_recview.ReplaceSel(Recv1 + _T("       ") + "\r\n"); //自动换行 
				UpdateData(true);
				m_recvcount = "";
				m_recvcount.Format(_T("%d"), Recv1.GetLength() / 12);
				m_recvcount += "/1";
				UpdateData(FALSE);
				if (Recv1.GetLength() == 12 && Recv1.Left(2) == "01" && Recv1.Mid(2, 4) == "0000")
				{
					CString Recv_ZL_CZ = Recv1.Left(6);
					CString Recv_ZL_JY = Get_paritybit(Recv1.Left(10));
					if (Recv_ZL_CZ == "010000" && Recv_ZL_JY == Recv1.Mid(10, 2))
					{
						//IP1
						IP1.Format(_T("%d"), HexToDem(Recv1.Mid(8, 2)));
						ZL = R_ZL_IP2 + Get_paritybit(R_ZL_IP2);
						COM_write(ZL.GetLength(), ZL);
						WaitForSingleObject(de_lay.hEvent, 100);
						COM_read(100, Recv2);
						m_recview.SetWindowText(_T(""));
						m_recview.ReplaceSel(Recv1 + _T("       ") + Recv2 + _T("       ") + "\r\n"); //自动换行 
						UpdateData(true);
						m_recvcount = "";
						m_recvcount.Format(_T("%d"), (Recv1+Recv2).GetLength() / 12);
						m_recvcount += "/2";
						UpdateData(FALSE);
						if (Recv2.GetLength() == 12 && Recv2.Left(2) == "01" && Recv2.Mid(2, 4) == "0000")
						{
							Recv_ZL_CZ = Recv2.Left(6);
							Recv_ZL_JY = Get_paritybit(Recv2.Left(10));
							if (Recv_ZL_CZ == "010000" && Recv_ZL_JY == Recv2.Mid(10, 2))
							{
								//IP2
								IP2.Format(_T("%d"), HexToDem(Recv2.Mid(8, 2)));
								ZL = R_ZL_IP3 + Get_paritybit(R_ZL_IP3);
								COM_write(ZL.GetLength(), ZL);
								WaitForSingleObject(de_lay.hEvent, 100);
								COM_read(100, Recv3);
								m_recview.SetWindowText(_T(""));
								m_recview.ReplaceSel(Recv1 + _T("       ") + Recv2 + _T("       ") + Recv3 + _T("       ") + "\r\n"); //自动换行 
								UpdateData(true);
								m_recvcount = "";
								m_recvcount.Format(_T("%d"), (Recv1 + Recv2+Recv3).GetLength() / 12);
								m_recvcount += "/3";
								UpdateData(FALSE);
								if (Recv3.GetLength() == 12 && Recv3.Left(2) == "01" && Recv3.Mid(2, 4) == "0000")
								{
									Recv_ZL_CZ = Recv3.Left(6);
									Recv_ZL_JY = Get_paritybit(Recv3.Left(10));
									if (Recv_ZL_CZ == "010000" && Recv_ZL_JY == Recv3.Mid(10, 2))
									{
										//IP3
										IP3.Format(_T("%d"), HexToDem(Recv3.Mid(8, 2)));
										ZL = R_ZL_IP4 + Get_paritybit(R_ZL_IP4);
										COM_write(ZL.GetLength(), ZL);
										WaitForSingleObject(de_lay.hEvent, 100);
										COM_read(100, Recv4);
										m_recview.SetWindowText(_T(""));
										m_recview.ReplaceSel(Recv1 + _T("       ") + Recv2 + _T("       ") + Recv3 + _T("       ") + Recv4 + _T("       ") + "\r\n"); //自动换行 
										UpdateData(true);
										m_recvcount = "";
										m_recvcount.Format(_T("%d"), (Recv1 + Recv2 + Recv3+Recv4).GetLength() / 12);
										m_recvcount += "/4";
										UpdateData(FALSE);
										if (Recv4.GetLength() == 12 && Recv4.Left(2) == "01" && Recv4.Mid(2, 4) == "0000")
										{
											Recv_ZL_CZ = Recv4.Left(6);
											Recv_ZL_JY = Get_paritybit(Recv4.Left(10));
											if (Recv_ZL_CZ == "010000" && Recv_ZL_JY == Recv4.Mid(10, 2))
											{
												//IP4
												IP4.Format(_T("%d"), HexToDem(Recv4.Mid(8, 2)));
												m_ipaddress.SetAddress(_ttoi(IP1), _ttoi(IP2), _ttoi(IP3), _ttoi(IP4));
												m_ipnote.SetWindowText(_T("读取IP成功"));
											}
											else
											{
												MessageBox(_T("接收的解析指令异常，请核查硬件或串口号!"));
												return;
											}
										}
										else
										{
											MessageBox(_T("接收的解析指令异常，请核查硬件或串口号!"));
											return;
										}
									}
									else
									{
										MessageBox(_T("接收的解析指令异常，请核查硬件或串口号!"));
										return;
									}
								}
								else
								{
									MessageBox(_T("接收的解析指令异常，请核查硬件或串口号!"));
									return;
								}
							}
							else
							{
								MessageBox(_T("接收的解析指令异常，请核查硬件或串口号!"));
								return;
							}
						}
						else
						{
							MessageBox(_T("接收的解析指令异常，请核查硬件或串口号!"));
							return;
						}
					}
					else
					{
						MessageBox(_T("接收的解析指令异常，请核查硬件或串口号!"));
						return;
					}
				}
				else
				{
					MessageBox(_T("接收的解析指令异常，请核查硬件或串口号!"));
					return;
				}
			}
		}
	}

	void Cserialport::OnDestroy()
	{
		CDialogEx::OnDestroy();

		// TODO:  在此处添加消息处理程序代码
		/*	if (hCom != NULL && hCom != INVALID_HANDLE_VALUE)
			{
			CloseHandle(hCom);
			}	*/
		//保存ini各项值
		UpdateData(TRUE);
		CString str;
		str.Format(_T("%d"), this->m_threshold1);
		Cini::WriteString(_T("Threshold"), _T("Threshold1"), str, _T("raycan.ini"));

		str.Format(_T("%d"), this->m_threshold2);
		Cini::WriteString(_T("Threshold"), _T("Threshold2"), str, _T("raycan.ini"));

		str.Format(_T("%d"), this->m_threshold3);
		Cini::WriteString(_T("Threshold"), _T("Threshold3"), str, _T("raycan.ini"));

		str.Format(_T("%d"), this->m_threshold4);
		Cini::WriteString(_T("Threshold"), _T("Threshold4"), str, _T("raycan.ini"));
	}

	void Cserialport::OnClickedClosecomm()
	{
		// TODO:  在此添加控件通知处理程序代码
		if (processcount != 0 && processcount != 288)
		{
			MessageBox(_T("串口正忙,请等待!"));
			return;
		}
		else{
			CloseHandle(hCom);
			hCom = NULL;
			flag_Initialization = false;
			CButton *CloseComm = (CButton *)GetDlgItem(IDC_CLOSECOMM);
			CloseComm->EnableWindow(FALSE);
			CButton *OpenComm = (CButton *)GetDlgItem(IDC_OPENCOMM);
			OpenComm->EnableWindow(TRUE);
		}
	}


	void Cserialport::OnClickedReset()
	{
		// TODO:  在此添加控件通知处理程序代码
		if (hCom == INVALID_HANDLE_VALUE || m_commport.GetCount() == 0 || flag_Initialization == false)
		{
			MessageBox(_T("串口未初始化!"));
			return;
		}
		else{
			if (processcount != 0 && processcount != 288)
			{
				MessageBox(_T("串口正忙!"));
				return;
			}
			else{
				CString ZL;
				ZL = ZL_RT + Get_paritybit(ZL_RT);
				COM_write(ZL.GetLength(), ZL);
				OVERLAPPED de_lay;
				memset(&de_lay, 0, sizeof(OVERLAPPED));
				de_lay.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
				WaitForSingleObject(de_lay.hEvent, 100);
				CString Recv;
				COM_read(100, Recv);
				m_recview.SetWindowText(_T(""));
				m_recview.SetSel(-1, -1); //自动滚屏 
				m_recview.ReplaceSel(Recv + "\r\n"); //自动换行 
				UpdateData(true);
				m_recvcount = "";
				m_recvcount.Format(_T("%d"), Recv.GetLength() / 12);
				m_recvcount += "/1";
				UpdateData(FALSE);
				if (Recv == Recv_ZL)
				{
					MessageBox(_T("复位成功!"));
				}
				else
				{
					MessageBox(_T("接收的解析指令异常，请核查硬件或串口号!"));
					return;
				}
			}
		}

	}


	void Cserialport::OnClickedRead()//单一阈值读取
	{
		// TODO:  在此添加控件通知处理程序代码
		if (hCom == INVALID_HANDLE_VALUE || m_commport.GetCount() == 0 || flag_Initialization == false)
		{
			MessageBox(_T("串口未初始化!"));
			return;
		}
	
		else{
			if (processcount != 0 && processcount != 288)
			{
				MessageBox(_T("串口正忙!"));
				return;
			}
			else{
				CString ZL, channel, thresholdnum;
				m_channel.GetLBText(m_channel.GetCurSel(), channel);
				m_thresholdnum.GetLBText(m_thresholdnum.GetCurSel(), thresholdnum);
				ZL = _T("20") + DemToHex(channel).Right(2) + DemToHex(thresholdnum).Right(2) + _T("0000");
				ZL += Get_paritybit(ZL);
				COM_write(ZL.GetLength(), ZL);
				OVERLAPPED de_lay;
				memset(&de_lay, 0, sizeof(OVERLAPPED));
				de_lay.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
				WaitForSingleObject(de_lay.hEvent, 100);
				CString Recv;
				COM_read(100, Recv);
				m_recview.SetWindowText(_T(""));
				m_recview.SetSel(-1, -1); //自动滚屏 
				m_recview.ReplaceSel(Recv +  "\r\n"); //自动换行 
				UpdateData(true);
				m_recvcount = "";
				m_recvcount.Format(_T("%d"), Recv.GetLength() / 12);
				m_recvcount += "/1";
				UpdateData(FALSE);
				if (!Recv.IsEmpty())
				{
					CString Recv_ZL_CZ = Recv.Left(6);
					CString Recv_ZL_JY = Get_paritybit(Recv.Left(10));
					if (Recv_ZL_CZ == "010000" && Recv_ZL_JY == Recv.Mid(10, 2)){
						CString A = Recv.Mid(6, 4);
						double B = (double)(1250.0 *(double)(HexToDem(A)) / 4096.0);
						m_value = (int)(B + 0.5);//结果四舍五入
						UpdateData(FALSE);
					}
					else
					{
						MessageBox(_T("接收的解析指令异常，请核查硬件或串口号!"));
						return;
					}
				}
				else
				{
					MessageBox(_T("接收的解析指令异常，请核查硬件或串口号!"));
					return;
				}
			}
		}
	}


	void Cserialport::OnClickedWrite()//单一阈值写入
	{
		// TODO:  在此添加控件通知处理程序代码
		if (hCom == INVALID_HANDLE_VALUE || m_commport.GetCount() == 0 || flag_Initialization == false)
		{
			MessageBox(_T("串口未初始化!"));
			return;
		}
		else{
			if (processcount!=0&&processcount!=288)
			{
				MessageBox(_T("串口正忙!"));
				return;
			}
			else{
				CString ZL, channel, thresholdnum;
				m_channel.GetLBText(m_channel.GetCurSel(), channel);
				m_thresholdnum.GetLBText(m_thresholdnum.GetCurSel(), thresholdnum);
				CString m_thresholdvalue;
				UpdateData(TRUE);
				int A = (int)((4096.0 *(double)(m_value) / 1250.0) + 0.5);
				m_thresholdvalue.Format(_T("%d"), A);
				ZL = _T("10") + DemToHex(channel).Right(2) + DemToHex(thresholdnum).Right(2) + DemToHex(m_thresholdvalue);
				ZL += Get_paritybit(ZL);
				COM_write(ZL.GetLength(), ZL);
				OVERLAPPED de_lay;
				memset(&de_lay, 0, sizeof(OVERLAPPED));
				de_lay.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
				WaitForSingleObject(de_lay.hEvent, 100);
				CString Recv;
				COM_read(100, Recv);
				m_recview.SetWindowText(_T(""));
				m_recview.SetSel(-1, -1); //自动滚屏 
				m_recview.ReplaceSel(Recv +"\r\n"); //自动换行 
				UpdateData(true);
				m_recvcount = "";
				m_recvcount.Format(_T("%d"), Recv.GetLength() / 12);
				m_recvcount += "/1";
				UpdateData(FALSE);
				if (Recv == Recv_ZL)
				{
					MessageBox(_T("阈值写入成功!"));
				}
				else
				{
					MessageBox(_T("接收的解析指令异常，请核查硬件或串口号!"));
					return;
				}
			}
		}
	}


	void Cserialport::OnClickedSenddata()
	{
		// TODO:  在此添加控件通知处理程序代码
		if (hCom == INVALID_HANDLE_VALUE || m_commport.GetCount() == 0 || flag_Initialization == false)
		{
			MessageBox(_T("串口未初始化!"));
			return;
		}
		else{	
			CButton *SendData = (CButton *)GetDlgItem(IDC_SENDDATA);
			SendData->EnableWindow(FALSE);
			UpdateData(TRUE);
			for (int i = 0; i < 72; i++)
			{
				data[i][0] = m_threshold1;
				data[i][1] = m_threshold2;
				data[i][2] = m_threshold3;
				data[i][3] = m_threshold4;
			}
			CButton *Stopsenddata = (CButton *)GetDlgItem(IDC_STOP);
			Stopsenddata->EnableWindow(TRUE);
			CRITICAL_SECTION g_cs;
			InitializeCriticalSection(&g_cs);
			HANDLE hThread = CreateThread(NULL,0,MyThread,this,0,NULL);
			CloseHandle(hThread);
			Sleep(1000);
			DeleteCriticalSection(&g_cs);
			SetTimer(1, 1000, NULL);//设置进度条更新时钟			
		}
	}

	DWORD WINAPI Cserialport::MyThread(LPVOID lpParam)
	{
		Cserialport* pDlg = (Cserialport*)lpParam;	
		pDlg->SendData();
		return 0;	
	}

	void Cserialport::OnTimer(UINT_PTR nIDEvent)
	{
		// TODO:  在此添加消息处理程序代码和/或调用默认值
		int m_nPos = (int)((processcount * 100) / 288 + 0.5);
		m_progressctrl.SetPos(m_nPos);
		m_percent.Format(_T("%d"), m_nPos);
		m_percent += '%';
		this->SetDlgItemText(IDC_PERCENT, m_percent);
		if (processcount == 288)
		{
			m_recview.SetWindowText(_T(""));
			m_recview.SetSel(-1, -1); //自动滚屏 
			m_recview.ReplaceSel(viewStr + "\r\n"); //自动换行 
			UpdateData(true);
			viewStr = "";
			m_recvcount = "";
			m_recvcount.Format(_T("%d"),viewcount.GetLength()/12);
			viewcount = "";
			m_recvcount += "/288";
			UpdateData(FALSE);
			
			processcount = 0;
			m_percent = _T("完成");
			this->SetDlgItemText(IDC_PERCENT, m_percent);
			KillTimer(1);
			CButton *SendData = (CButton *)GetDlgItem(IDC_SENDDATA);
			SendData->EnableWindow(TRUE);
			CButton *Stopsenddata = (CButton *)GetDlgItem(IDC_STOP);
			Stopsenddata->EnableWindow(FALSE);
		}
		
		CDialogEx::OnTimer(nIDEvent);
	}



	void Cserialport::SendData()
	{
		CString ZL, channel, thresholdnum;
		CString m_thresholdvalue;
		for (int i = 0; i < 72; i++)
		{
			if (sendflag == true)
			{
				sendflag = false;
				processcount = 0;
				m_percent = _T("已中止");
				this->SetDlgItemText(IDC_PERCENT, m_percent);
				m_progressctrl.SetPos(0);
				KillTimer(1);
				viewStr = "";
				m_recvcount = "";
				viewcount = "";
				CButton *SendData = (CButton *)GetDlgItem(IDC_SENDDATA);
				SendData->EnableWindow(TRUE);
				break;
			}
			for (int j = 0; j < 4; j++)
			{
				int A = (int)((4096.0 *(double)(data[i][j]) / 1250.0) + 0.5);
				m_thresholdvalue.Format(_T("%d"), A);
				channel.Format(_T("%d"), i + 1);
				thresholdnum.Format(_T("%d"), j + 1);
				ZL = _T("10") + DemToHex(channel).Right(2) + DemToHex(thresholdnum).Right(2) + DemToHex(m_thresholdvalue);
				ZL += Get_paritybit(ZL);
				COM_write(ZL.GetLength(), ZL);
				OVERLAPPED de_lay;
				memset(&de_lay, 0, sizeof(OVERLAPPED));
				de_lay.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
				WaitForSingleObject(de_lay.hEvent, 50);
				CString Recv;
				COM_read(100, Recv);
				viewStr += Recv + _T("       ");
				viewcount += Recv;
				processcount++;
			}
		}
	}


	void Cserialport::OnBnClickedStopsenddata()
	{
		// TODO:  在此添加控件通知处理程序代码
		sendflag = true;
		CButton *Stopsenddata = (CButton *)GetDlgItem(IDC_STOP);
		Stopsenddata->EnableWindow(FALSE);
	}
