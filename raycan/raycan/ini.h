// ini.h: interface for the Cini class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INI_H__97F3D6CA_04FF_4E2F_8364_ACDEE0E73F32__INCLUDED_)
#define AFX_INI_H__97F3D6CA_04FF_4E2F_8364_ACDEE0E73F32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxwin.h> 

class Cini  
{
public:
	Cini();
	virtual ~Cini();
	static DWORD ReadString (TCHAR *section, TCHAR * key,  TCHAR* stringtoread,  TCHAR * filename);
	static UINT ReadInt(TCHAR *section, TCHAR * key,  TCHAR * filename);
    static BOOL	 WriteString(LPCTSTR section, LPCTSTR key, CString stringtoadd, TCHAR *filename);
};

#endif // !defined(AFX_INI_H__97F3D6CA_04FF_4E2F_8364_ACDEE0E73F32__INCLUDED_)
