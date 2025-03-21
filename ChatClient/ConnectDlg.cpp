// CConnectDlg.cpp : implementation file
//

#include "pch.h"
#include "ChatClient.h"
#include "afxdialogex.h"
#include "ConnectDlg.h"


// ConnectDlg dialog

IMPLEMENT_DYNAMIC(CConnectDlg, CDialogEx)

CConnectDlg::CConnectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CONNECT, pParent)
	, m_serverIP(0x7F000001)	// set default IP to Loopback adddress (127.0.0.1)
{

}

CConnectDlg::~CConnectDlg()
{
}

void CConnectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_username);
	DDX_IPAddress(pDX, IDC_IPADDRESS, m_serverIP);
}


BEGIN_MESSAGE_MAP(CConnectDlg, CDialogEx)
END_MESSAGE_MAP()


// CUsernameDlg message handlers
