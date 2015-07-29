
// raycanDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "serialport.h"
#include "calibration.h"
#include "Coperation.h"


// CraycanDlg �Ի���
class CraycanDlg : public CDialogEx
{
// ����
public:
	CraycanDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_RAYCAN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	CTabCtrl m_tabctrl;
	Ccalibration m_calib;
	Cserialport m_serial;
	Coperation m_operat;
};
