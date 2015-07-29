#pragma once
#include "afxwin.h"
#include "afxcmn.h"



// Cserialport 对话框

class Cserialport : public CDialogEx
{
	DECLARE_DYNAMIC(Cserialport)

public:
	Cserialport(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Cserialport();

// 对话框数据
	enum { IDD = IDD_SERIALPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_baudrate;
	CComboBox m_commport;
	CComboBox m_databit;
	CComboBox m_paritybit;
	CComboBox m_stopbit;
	virtual BOOL OnInitDialog();
	afx_msg void OnIpnFieldchangedIpaddress(NMHDR *pNMHDR, LRESULT *pResult);
private:
	HANDLE    hCom;//全局变量,串口句柄 
	bool flag_Initialization;
public:
	int data[72][4];//矩阵存储数据
	CString viewStr,viewcount;//用于显示和计数

	
private:
	//复位指令
	CString ZL_RT;//复位指令
	  

	//读指令集   板子编号   IP  
	CString R_ZL_boardnumber;//板子编号查询指令
	CString R_ZL_IP1;//IP查询指令
	CString R_ZL_IP2;
	CString R_ZL_IP3;
	CString R_ZL_IP4;

	//写指令集  板子编号   IP     
	CString W_ZL_boardnumber;//+板子编号0000
	CString W_ZL_IP1;//+ip1+xx(ip)+00
	CString W_ZL_IP2;//+ip2
	CString W_ZL_IP3;//+ip3
	CString W_ZL_IP4;//+ip4


	//写指令操作正确后的返回数据
	CString Recv_ZL;

public:
	afx_msg void OnClickedOpencomm();
	afx_msg void OnClickedReadboardnum();
	CComboBox m_channel;
	CComboBox m_thresholdnum;
	int m_value;
	CIPAddressCtrl m_ipaddress;
	DWORD COM_read(DWORD dwBytesRead,CString& Buffer);
	DWORD COM_write(DWORD dwBytesWritten,CString&Buffer);
	int HexToDem(CString str);
	CString DemToHex(CString str);
	byte* htb(CString buf);
	CString bth(byte*buf);
	CString Get_paritybit(CString data);
	void comm_DataReceived();
	CString m_boardnum;
	afx_msg void OnBnClickedWriteboardnum();
	int m_threshold1;
	int m_threshold2;
	int m_threshold3;
	int m_threshold4;
	afx_msg void OnDestroy();
	afx_msg void OnClickedWriteip();
	afx_msg void OnClickedReadip();
	CStatic m_numnote;
	afx_msg void OnClickedClosecomm();
	CStatic m_ipnote;
	afx_msg void OnClickedReset();
	afx_msg void OnClickedRead();
	CString m_filepath;
	afx_msg void OnClickedSenddata();
	afx_msg void OnClickedWrite();
	afx_msg void OnEnChangeRecview();
	CEdit m_recview;
	CProgressCtrl m_progressctrl;//进度条
	CString m_percent;
	int processcount;
	bool sendflag=false;
	static DWORD WINAPI MyThread(LPVOID lpParam);//线程函数声明
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void SendData();
	CString m_recvcount;
	afx_msg void OnBnClickedStopsenddata();
};
