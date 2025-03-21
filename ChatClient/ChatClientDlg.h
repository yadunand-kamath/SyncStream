
// ChatClientDlg.h : header file
//

#pragma once


// CChatClientDlg dialog
class CChatClientDlg : public CDialogEx
{
// Construction
public:
	CChatClientDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedButtonSend();
	DECLARE_MESSAGE_MAP()

	static UINT ReceiveMessagesThreadProc(LPVOID pParam);
	afx_msg LRESULT OnAddMessage(WPARAM wParam, LPARAM lParam);
	
	static const UINT WM_ADD_MESSAGE = WM_USER + 101;
	SOCKET m_clientSocket;

private:
	CString m_username;
	CString m_serverAddr;

public:
	CString GetUsername();
	CString GetServerAddr();
	void SetUsername(CString username);
	void SetServerAddr(CString serverAddr);

	CListBox m_listBox;
};