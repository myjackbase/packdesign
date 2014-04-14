
// HitGuardDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HitGuard.h"
#include "HitGuardDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHitGuardDlg dialog



CHitGuardDlg::CHitGuardDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHitGuardDlg::IDD, pParent)
	, m_filePath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHitGuardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_filePath);
}

BEGIN_MESSAGE_MAP(CHitGuardDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTNOPEN, &CHitGuardDlg::OnBnClickedBtnopen)
	ON_BN_CLICKED(IDC_BTNEXIT, &CHitGuardDlg::OnBnClickedBtnexit)
	ON_BN_CLICKED(IDC_BTNABOUT, &CHitGuardDlg::OnBnClickedBtnabout)
	ON_BN_CLICKED(IDC_BTNPACK, &CHitGuardDlg::OnBnClickedBtnpack)
END_MESSAGE_MAP()


// CHitGuardDlg message handlers

BOOL CHitGuardDlg::OnInitDialog()
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

void CHitGuardDlg::OnPaint()
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
HCURSOR CHitGuardDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CHitGuardDlg::OnBnClickedBtnopen()
{
	CFileDialog dlg(TRUE);
	if(dlg.DoModal() == IDOK)
	{
		m_filePath = dlg.GetPathName();
		UpdateData(FALSE);
	}
}


void CHitGuardDlg::OnBnClickedBtnexit()
{
	CHitGuardDlg::OnCancel();
}


void CHitGuardDlg::OnBnClickedBtnabout()
{
	MessageBox(_T("HitGuard by Jackbase!"));
}


void CHitGuardDlg::OnBnClickedBtnpack()
{
	if(m_pro.Init(m_filePath) != 0)
	{
		MessageBox(_T("加壳失败！"));
	}
	else if (m_pro.Protect() != 0)
	{
		MessageBox(_T("加壳失败！"));
	}
	else
	{
		MessageBox(_T("加壳成功！"));
	}
}
