#pragma once


// Coperation �Ի���

class Coperation : public CDialogEx
{
	DECLARE_DYNAMIC(Coperation)

public:
	Coperation(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~Coperation();

// �Ի�������
	enum { IDD = IDD_OPERATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
