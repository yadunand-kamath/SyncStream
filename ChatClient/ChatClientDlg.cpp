
// ChatClientDlg.cpp : implementation file
//

//-----------------------------------------------------------------------------
// File:        ChatClientDlg.cpp
// Author:      Yadunand Kamath
// Date:        2025-03-22
// Description: Implementation file for the CChatClientDlg class, which handles
//              the main dialog of the chat client application.
//-----------------------------------------------------------------------------

#include "pch.h"
#include "framework.h"
#include "ChatClient.h"
#include "ChatClientDlg.h"
#include "ConnectDlg.h"
#include "ConnectingDlg.h"
#include "afxdialogex.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CChatClientDlg dialog



CChatClientDlg::CChatClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHATCLIENT_DIALOG, pParent)
{
	// Initialize the member variables
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_clientSocket = INVALID_SOCKET;
}

void CChatClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MESSAGES, m_listBox);
}

BEGIN_MESSAGE_MAP(CChatClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CChatClientDlg::OnBnClickedButtonSend)
	ON_MESSAGE(WM_ADD_MESSAGE, &CChatClientDlg::OnAddMessage)
END_MESSAGE_MAP()


// CChatClientDlg message handlers

//-----------------------------------------------------------------------------
// Method:      OnInitDialog
// Description: This method is called when the dialog is being initialized.
//              It performs setup tasks such as initializing controls,
//              setting the window title, and connecting to the server.
// Parameters:  None
// Returns:     BOOL - TRUE unless you set the focus to a control.
//-----------------------------------------------------------------------------

BOOL CChatClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, FALSE);	// Set big icon
	SetIcon(m_hIcon, TRUE);		// Set small icon

	// TODO: Add extra initialization here

	// Set the window title
	CString windowTitle;
	windowTitle.Format(_T("SyncStream Client - User: %s"), GetUsername());
	SetWindowText(windowTitle);
	
	// Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) 
	{
		MessageBox(L"WSAStartup failed", L"Error", MB_OK | MB_ICONERROR);
		EndDialog(IDCANCEL);
		return FALSE;
	}

	// Create a socket for the client
	m_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_clientSocket == INVALID_SOCKET)
	{
		MessageBox(L"CLIENT: Error creating socket", L"Error", MB_OK | MB_ICONERROR);
		EndDialog(IDCANCEL);
		return FALSE;
	}

	// Convert IP address string to binary
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(8888);
	CT2A asciiServerAddr(GetServerAddr());
	if (inet_pton(AF_INET, asciiServerAddr, &serverAddress.sin_addr) <= 0) {
		MessageBox(L"CLIENT: Invalid server IP address", L"Warning", MB_OK | MB_ICONWARNING);
		closesocket(m_clientSocket);
		WSACleanup();
		EndDialog(IDCANCEL);
		return FALSE;
	}

	// Show connecting dialog if connection is delayed
	CConnectingDlg connectingDlg;
	connectingDlg.Create(IDD_DIALOG_CONNECTING, this); // Create the dialog
	TRACE(_T("IP: "), GetServerAddr());
	windowTitle.Format(_T("Connecting to SERVER at IP: %s"), GetServerAddr());
	connectingDlg.m_staticConnecting.SetWindowText(windowTitle); // Set the text
	connectingDlg.ShowWindow(SW_SHOW); // Show it
	TRACE(_T("Attempting connect() to IP: %s\n"), GetServerAddr());

	// Connect to the server
	if (connect(m_clientSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
		connectingDlg.EndDialog(IDCANCEL); // Close loading dialog
		TRACE(_T("connect() returned SOCKET_ERROR.\n"));
		int error = WSAGetLastError();
		TRACE(_T("WSAGetLastError() returned: %d\n"), error);
		MessageBox(L"CLIENT: Connection to SERVER failed. Check IP Address.", L"Error", MB_OK | MB_ICONWARNING);
		closesocket(m_clientSocket);
		WSACleanup();
		EndDialog(IDCANCEL);
		return FALSE;
	}
	else {
		TRACE(_T("connect() succeeded.\n"));
		connectingDlg.EndDialog(IDCANCEL); // Close loading dialog
	}

	MessageBox(_T("CLIENT: Connected to server!"), L"Success", MB_ICONINFORMATION); // Confirmation message

	// Send the username to the server
	if (!GetUsername().IsEmpty())
	{
		CT2A asciiUsername(GetUsername());
		send(m_clientSocket, asciiUsername, strlen(asciiUsername), 0);
	}

	// Launch a thread to receive messages from the server
	AfxBeginThread(ReceiveMessagesThreadProc, this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CChatClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CChatClientDlg::OnPaint()
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
HCURSOR CChatClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// Getters and Setters

CString CChatClientDlg::GetUsername()
{
		return m_username;
}

CString CChatClientDlg::GetServerAddr()
{
	return m_serverAddr;
}

void CChatClientDlg::SetUsername(CString username)
{
	m_username = username;
}

void CChatClientDlg::SetServerAddr(CString serverAddr)
{
	m_serverAddr = serverAddr;
}

//-----------------------------------------------------------------------------
// Method:      OnBnClickedButtonSend
// Description: This method is called when the send button is clicked.
//              It sends the message in the message box to the server.
// Parameters:  None
// Returns:     void
//-----------------------------------------------------------------------------

void CChatClientDlg::OnBnClickedButtonSend()
{
	// Capture the message from the message box
	CString messageToSend;
	GetDlgItemText(IDC_EDIT_MESSAGE, messageToSend);

	// Send the message to the server
	if (!messageToSend.IsEmpty())
	{
		CT2A asciiMsg(messageToSend);
		int bytesSent = send(m_clientSocket, asciiMsg, strlen(asciiMsg), 0);
		if (bytesSent == SOCKET_ERROR) {
			MessageBox(_T("CLIENT: Error sending message."), L"Error");
		}
		SetDlgItemText(IDC_EDIT_MESSAGE, L""); // Clear the message box after sending the message
	}
	else
	{
		MessageBox(L"Please enter a message to send", L"Warning", MB_OK | MB_ICONWARNING);
	}
}

//-----------------------------------------------------------------------------
// Method:      ReceiveMessagesThreadProc
// Description: This static method is the entry point for the thread that
//              receives messages from the server. It runs in an infinite loop
//              and receives messages from the server until the connection is
//              closed or an error occurs.
// Parameters:  LPVOID pParam - A pointer to the CChatClientDlg object.
// Returns:     UINT - 0 (thread's exit code)
//-----------------------------------------------------------------------------

UINT CChatClientDlg::ReceiveMessagesThreadProc(LPVOID pParam)
{
	CChatClientDlg* pDlg = static_cast<CChatClientDlg*>(pParam);
	// Create a buffer to receive messages
	char buffer[4096];
	int bytesReceived;

	// Receive messages from the server
	while ((bytesReceived = recv(pDlg->m_clientSocket, buffer, sizeof(buffer), 0)) > 0) {
		buffer[bytesReceived] = '\0';
		CString receivedMessage(buffer);
		TRACE(_T("CLIENT Received: %s\n"), receivedMessage);

		if (receivedMessage.Left(5) == _T("ALERT"))
		{
			BOOL bResult = pDlg->PostMessage(CChatClientDlg::WM_ADD_MESSAGE, 0, (LPARAM)new CString(receivedMessage));
			TRACE(_T("PostMessage1 result: %d\n"), bResult);
		}
		else
		{
			CString formattedMessage;
			formattedMessage.Format(_T("[%s]: %s"), pDlg->GetUsername(), receivedMessage);
			// Post a message to the main thread to update the list box
			BOOL bResult = pDlg->PostMessage(CChatClientDlg::WM_ADD_MESSAGE, 0, (LPARAM)new CString(receivedMessage));
			TRACE(_T("PostMessage2 result: %d\n"), bResult);
		}
	}

	if (bytesReceived == 0) {
		TRACE(_T("SERVER Disconnected.\n"));
		AfxMessageBox(_T("SERVER Disconnected."), MB_OK | MB_ICONERROR);
	}
	else if (bytesReceived == SOCKET_ERROR) {
		TRACE(_T("Receive failed with error: %d\n"), WSAGetLastError());
		// Handle receive error
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Method:      OnAddMessage
// Description: This method is called when a message is received from 
//				the server. It adds the message to the list box.
// Parameters:  WPARAM wParam - The WPARAM parameter of the message.
//              LPARAM lParam - The LPARAM parameter of the message.
// Returns:     LRESULT - The result of the message processing.
//-----------------------------------------------------------------------------

LRESULT CChatClientDlg::OnAddMessage(WPARAM wParam, LPARAM lParam)
{
	CString* pMessage = reinterpret_cast<CString*>(lParam);
	if (pMessage != nullptr)
	{
		CListBox* pListBox = static_cast<CListBox*>(GetDlgItem(IDC_LIST_MESSAGES));
		if (pListBox != nullptr)
			pListBox->AddString(*pMessage);	// Add the message to the list box
		delete pMessage; // Clean up the allocated CString
	}
	return LRESULT();
}

