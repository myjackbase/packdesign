
// SqunGuardDlg.h : header file
//

#include "Protect.h"
#pragma once


// CSqunGuardDlg dialog
class CSqunGuardDlg : public CDialogEx
{
// Construction
public:
	CSqunGuardDlg(CWnd* pParent = NULL);	// standard constructor
	CProtect m_Pro;

// Dialog Data
	enum { IDD = IDD_SQUNGUARD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_FilePath;
	afx_msg void OnBnClickedBtnopen();
	afx_msg void OnBnClickedBtnexit();
	afx_msg void OnBnClickedBtnabout();
	afx_msg void OnBnClickedBtnpack();
};
