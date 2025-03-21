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
}

BOOL CConnectingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CenterWindow();

	return TRUE;
}

void CConnectingDlg::OnCancel()
{
	m_bCancelled = true;
	CDialogEx::OnCancel();
}


BEGIN_MESSAGE_MAP(CConnectingDlg, CDialogEx)
END_MESSAGE_MAP()


// ConnectionProgress message handlers
