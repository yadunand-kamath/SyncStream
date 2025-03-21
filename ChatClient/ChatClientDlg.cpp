
// ChatClientDlg.cpp : implementation file
//

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

	CString windowTitle;
	windowTitle.Format(_T("Chat Client - User: %s"), GetUsername());
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

	// Connect to the server
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(8888);
	CT2A asciiServerAddr(m_serverAddr);
	if (inet_pton(AF_INET, asciiServerAddr, &serverAddress.sin_addr) <= 0) {
		MessageBox(L"CLIENT: Invalid server IP address", L"Warning", MB_OK | MB_ICONWARNING);
		closesocket(m_clientSocket);
		WSACleanup();
		EndDialog(IDCANCEL);
		return FALSE;
	}

	CConnectingDlg connectingDlg;
	connectingDlg.Create(IDD_DIALOG_CONNECTING, this); // Create the dialog
	connectingDlg.ShowWindow(SW_SHOW); // Show it
	TRACE(_T("Attempting connect() to IP: %s\n"), m_serverAddr);


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

	MessageBox(_T("CLIENT: Connected to server!"), L"Success", MB_ICONINFORMATION); // Optional: Confirmation message

	if (!GetUsername().IsEmpty())
	{
		CT2A asciiUsername(m_username);
		send(m_clientSocket, asciiUsername, strlen(asciiUsername), 0);
	}

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

void CChatClientDlg::OnBnClickedButtonSend()
{
	CString messageToSend;
	GetDlgItemText(IDC_EDIT_MESSAGE, messageToSend);

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

UINT CChatClientDlg::ReceiveMessagesThreadProc(LPVOID pParam)
{
	TRACE(_T("ReceiveMessagesThreadProc is running.\n"));
	CChatClientDlg* pDlg = static_cast<CChatClientDlg*>(pParam);
	char buffer[4096];
	int bytesReceived;

	while ((bytesReceived = recv(pDlg->m_clientSocket, buffer, sizeof(buffer), 0)) > 0) {
		buffer[bytesReceived] = '\0';
		CString receivedMessage(buffer);
		TRACE(_T("CLIENT Received: %s\n"), receivedMessage);

		CString formattedMessage;
		formattedMessage.Format(_T("[%s]: %s"), pDlg->m_username, receivedMessage);
		BOOL bResult = pDlg->PostMessage(CChatClientDlg::WM_ADD_MESSAGE, 0, (LPARAM)new CString(receivedMessage));
		TRACE(_T("PostMessage result: %d\n"), bResult);
	}

	if (bytesReceived == 0) {
		TRACE(_T("SERVER Disconnected.\n"));
		// Handle server disconnection (e.g., display a message, disable sending)
	}
	else if (bytesReceived == SOCKET_ERROR) {
		TRACE(_T("Receive failed with error: %d\n"), WSAGetLastError());
		// Handle receive error
	}

	return 0;
}

LRESULT CChatClientDlg::OnAddMessage(WPARAM wParam, LPARAM lParam)
{
	CString* pMessage = reinterpret_cast<CString*>(lParam);
	if (pMessage != nullptr)
	{
		CListBox* pListBox = static_cast<CListBox*>(GetDlgItem(IDC_LIST_MESSAGES));
		if (pListBox != nullptr)
		{
			//TRACE(_T("Adding message to List Box: %s\n"), *pMessage);
			pListBox->AddString(*pMessage);
		}
		delete pMessage; // Clean up the allocated CString
	}
	return LRESULT();
}

