#pragma once


// Coperation 对话框

class Coperation : public CDialogEx
{
	DECLARE_DYNAMIC(Coperation)

public:
	Coperation(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Coperation();

// 对话框数据
	enum { IDD = IDD_OPERATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
