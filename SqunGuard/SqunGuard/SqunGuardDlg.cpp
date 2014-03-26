
// SqunGuardDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SqunGuard.h"
#include "SqunGuardDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSqunGuardDlg dialog



CSqunGuardDlg::CSqunGuardDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSqunGuardDlg::IDD, pParent)
	, m_FilePath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSqunGuardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDTPATH, m_FilePath);
}

BEGIN_MESSAGE_MAP(CSqunGuardDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTNOPEN, &CSqunGuardDlg::OnBnClickedBtnopen)
	ON_BN_CLICKED(IDC_BTNEXIT, &CSqunGuardDlg::OnBnClickedBtnexit)
	ON_BN_CLICKED(IDC_BTNABOUT, &CSqunGuardDlg::OnBnClickedBtnabout)
	ON_BN_CLICKED(IDC_BTNPACK, &CSqunGuardDlg::OnBnClickedBtnpack)
END_MESSAGE_MAP()


// CSqunGuardDlg message handlers

BOOL CSqunGuardDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSqunGuardDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSqunGuardDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSqunGuardDlg::OnBnClickedBtnopen()
{
	CFileDialog dlg(TRUE);
	if (dlg.DoModal()==IDOK)
	{
		m_FilePath=dlg.GetPathName();
		UpdateData(FALSE);
	}

}


void CSqunGuardDlg::OnBnClickedBtnexit()
{
	CSqunGuardDlg::OnCancel();
}


void CSqunGuardDlg::OnBnClickedBtnabout()
{
	MessageBox("SqunGuard by Jackbase");
}


void CSqunGuardDlg::OnBnClickedBtnpack()
{
	UpdateData(TRUE);
	if(m_Pro.Init(m_FilePath)!=0)
	{
		MessageBox("Failed!");
	}
	else if (m_Pro.Protect() ==0)
	{
		MessageBox("Pack Successful!");
	}
	else
	{
		MessageBox("Pack Failed!");
	}
}
