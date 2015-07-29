// Coperation.cpp : 实现文件
//

#include "stdafx.h"
#include "raycan.h"
#include "Coperation.h"
#include "afxdialogex.h"


// Coperation 对话框

IMPLEMENT_DYNAMIC(Coperation, CDialogEx)

Coperation::Coperation(CWnd* pParent /*=NULL*/)
	: CDialogEx(Coperation::IDD, pParent)
{

}

Coperation::~Coperation()
{
}

void Coperation::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Coperation, CDialogEx)
END_MESSAGE_MAP()


// Coperation 消息处理程序
