// ConnectionProgress.cpp : implementation file
//

#include "pch.h"
#include "ChatClient.h"
#include "afxdialogex.h"
#include "ConnectingDlg.h"


// ConnectionProgress dialog

IMPLEMENT_DYNAMIC(CConnectingDlg, CDialogEx)

CConnectingDlg::CConnectingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CONNECTING, pParent)
	, m_bCancelled(false)
{

}

CConnectingDlg::~CConnectingDlg()
{
}

void CConnectingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CONNECTING, m_staticConnecting);
}

BOOL CConnectingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CenterWindow();

	return TRUE;
}


BEGIN_MESSAGE_MAP(CConnectingDlg, CDialogEx)
END_MESSAGE_MAP()


// ConnectionProgress message handlers
