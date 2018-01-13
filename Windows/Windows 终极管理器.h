// Windows 终极管理器.h : main header file for the WINDOWS 终极管理器 application
//

#if !defined(AFX_WINDOWS_H__4056F223_FDC8_4599_A5A9_EB53BA82012D__INCLUDED_)
#define AFX_WINDOWS_H__4056F223_FDC8_4599_A5A9_EB53BA82012D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CWindowsApp:
// See Windows 终极管理器.cpp for the implementation of this class
//

class CWindowsApp : public CWinApp
{
public:
	CWindowsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWindowsApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CWindowsApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINDOWS_H__4056F223_FDC8_4599_A5A9_EB53BA82012D__INCLUDED_)
