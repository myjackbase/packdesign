
// HitGuardDlg.h : header file
//

#pragma once
#include "Protect.h"

// CHitGuardDlg dialog
class CHitGuardDlg : public CDialogEx
{
// Construction
public:
	CHitGuardDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_HITGUARD_DIALOG };

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
	CString m_filePath;
	CProtect m_pro;
	afx_msg void OnBnClickedBtnopen();
	afx_msg void OnBnClickedBtnexit();
	afx_msg void OnBnClickedBtnabout();
	afx_msg void OnBnClickedBtnpack();
};
