#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#define  BUFFERSIZE 1024*1024
// Ccalibration �Ի���

class Ccalibration : public CDialogEx
{
	DECLARE_DYNAMIC(Ccalibration)

public:
	Ccalibration(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~Ccalibration();

	// �Ի�������
	enum { IDD = IDD_CALIBRATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	SOCKET m_socket;
	void Recv();
	BOOL InitSocket();
	bool OnSaveas();
	CString byteToHexStr(byte byte_arr[], int arr_len);
	static DWORD WINAPI RecvProc(LPVOID lpParameter);//ץ�����̺߳���
	static DWORD WINAPI SpeedProc(LPVOID lpParameter);//�洢���̳߳���
	static DWORD WINAPI Stopbysize(LPVOID lpParameter);//�����������̺߳���
	static DWORD WINAPI Stopbytime(LPVOID lpParameter);//������ʱ���̺߳���
	CListCtrl m_packetlist;
	int retval;
	unsigned long packetcount;
	double	dfTimsum = 0.0;
	CStdioFile myFile;
	CString FilePath;
	bool Threadflag = false;//�߳̿��ر�־λ
	bool Stopbysizeflag = false;
	bool Stopbytimeflag = false;
	bool Stopflag = false;
	bool Saveasret;
	CString SourceIp;
	CString	DestinIp = _T("192.168.1.110");
	CString RelativeTime;
	afx_msg void OnClickedBegaintime();
	afx_msg void OnClickedStoptime();
	afx_msg void OnClickedStopbysize();
	afx_msg void OnClickedStopbytime();

	CEdit m_speed;
	//int num_temp = 0;
	//double t_temp = 0;
	int time; //�ֶ�ץ����ʱ
	unsigned int m_nPos = 0;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CProgressCtrl m_timegoing;
	CString m_percent;
	CProgressCtrl m_marquee;
	CString m_packets;
	CString m_timeminute;
	CString m_million;
	CString m_timesecond;
	CString m_timehour;
	CString m_thousand;
	CEdit m_time;
	CEdit m_size;
};
