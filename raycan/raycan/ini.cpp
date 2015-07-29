// ini.cpp: implementation of the Cini class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "raycan.h"
#include "ini.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cini::Cini()
{

}

Cini::~Cini()
{

}
/////////////////////////////////////////////////////////////////////////
//д�ַ�����INI�ļ�,GetLastError()�������ڷ���д��ʧ��
void error(LPSTR lpszFunction) 
{ 
	CString szBuf;
    DWORD dw = GetLastError(); 
	szBuf.Format(_T("%s failed: GetLastError returned %u\n"),lpszFunction, dw);
    MessageBox(NULL, szBuf, _T("Error"), MB_OK); 
    ExitProcess(dw); 
} 

BOOL Cini::WriteString(LPCTSTR section, LPCTSTR key, CString stringtoadd, TCHAR *filename)
{
    TCHAR FilePath[255]; 
    GetModuleFileName(NULL,FilePath,255); 
	(wcsrchr(FilePath,'\\'))[1] = 0;
	wcscat_s(FilePath,filename);
    return ::WritePrivateProfileString(section,key,stringtoadd,FilePath);
} 

/////////////////////////////////////////////////////////////////////////
//��INI�ļ��ж�ȡ�ַ���
DWORD Cini::ReadString(TCHAR *section, TCHAR * key,  TCHAR* stringtoread,  TCHAR * filename)
{
    TCHAR FilePath[255]; 
    GetModuleFileName(NULL,FilePath,255); 
	(wcsrchr(FilePath, '\\'))[1] = 0;
	wcscat_s(FilePath, filename);
    return ::GetPrivateProfileString(section, key,NULL,stringtoread,255,FilePath);
}

//��INI�ļ��ж�ȡint
UINT Cini::ReadInt(TCHAR *section, TCHAR * key,  TCHAR * filename)
{
    TCHAR FilePath[255]; 
    GetModuleFileName(NULL,FilePath,255); 
	(wcsrchr(FilePath,'\\'))[1] = 0;
	wcscat_s(FilePath, filename);
    return ::GetPrivateProfileInt(section,key,10,FilePath);
}