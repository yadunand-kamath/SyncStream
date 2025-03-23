#pragma once
#include "afxdialogex.h"


// CConnectingDlg dialog

class CConnectingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CConnectingDlg)

public:
	bool m_bCancelled;
	CConnectingDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CConnectingDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CONNECTING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_staticConnecting;
};
