// Windows ÖÕ¼«¹ÜÀíÆ÷Dlg.h : header file
//

#if !defined(AFX_WINDOWSDLG_H__D789232C_CDF2_43C3_AC4F_3402796DD119__INCLUDED_)
#define AFX_WINDOWSDLG_H__D789232C_CDF2_43C3_AC4F_3402796DD119__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CWindowsDlg dialog

class CWindowsDlg : public CDialog
{
// Construction
public:
	CWindowsDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CWindowsDlg)
	enum { IDD = IDD_WINDOWS_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWindowsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CWindowsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINDOWSDLG_H__D789232C_CDF2_43C3_AC4F_3402796DD119__INCLUDED_)
