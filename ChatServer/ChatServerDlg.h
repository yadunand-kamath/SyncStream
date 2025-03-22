
// ChatServerDlg.h : header file
//

#pragma once

#include <map>

class CChatServerDlgAutoProxy;


// CChatServerDlg dialog
class CChatServerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CChatServerDlg);
	friend class CChatServerDlgAutoProxy;

// Construction
public:
	CChatServerDlg(CWnd* pParent = nullptr);	// standard constructor
	virtual ~CChatServerDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	CChatServerDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()

	static UINT AcceptClientConnectionThreadProc(LPVOID pParam);
	static UINT HandleClientThreadProc(LPVOID pParam);

	SOCKET m_listenSocket;
	CList<SOCKET, SOCKET&> m_clientSockets;
	CListBox m_clientList;

private:
	std::map<SOCKET, CString> m_connectedClients;
};
