
// ChatServerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "ChatServer.h"
#include "ChatServerDlg.h"
#include "DlgProxy.h"
#include "afxdialogex.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <ctime>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "Ws2_32.lib")

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


// CChatServerDlg dialog


IMPLEMENT_DYNAMIC(CChatServerDlg, CDialogEx);

CChatServerDlg::CChatServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHATSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = nullptr;
	m_listenSocket = INVALID_SOCKET;
}

CChatServerDlg::~CChatServerDlg()
{
	// If there is an automation proxy for this dialog, set
	//  its back pointer to this dialog to null, so it knows
	//  the dialog has been deleted.
	if (m_pAutoProxy != nullptr)
		m_pAutoProxy->m_pDialog = nullptr;
}

void CChatServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CChatServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CChatServerDlg message handlers

BOOL CChatServerDlg::OnInitDialog()
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
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	// Initializing Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		MessageBox(L"WSAStartup Failed", L"Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	// Creating a socket for the server
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET)
	{
		MessageBox(L"Socket Creation Failed", L"Error", MB_OK | MB_ICONERROR);
		WSACleanup();
		return FALSE;
	}

	//Binding the socket to an IP address and port
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;	
	serverAddress.sin_port = htons(8888);
	if (bind(listenSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	{
		CString errorMessage;
		errorMessage.Format(_T("Server: Error binding socket. Error code: %d"), WSAGetLastError());
		AfxMessageBox(errorMessage);
		closesocket(listenSocket);
		WSACleanup();
		return FALSE;
	}

	// Listening for incoming connections
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		CString errorMessage;
		errorMessage.Format(_T("Server: Error listening on socket. Error code: %d"), WSAGetLastError());
		MessageBox(errorMessage, L"Error", MB_OK | MB_ICONERROR);
		closesocket(listenSocket);
		WSACleanup();
		return FALSE;
	}

	m_listenSocket = listenSocket;
	AfxBeginThread(AcceptClientConnectionThreadProc, this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CChatServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CChatServerDlg::OnPaint()
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
HCURSOR CChatServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CChatServerDlg::OnClose()
{
	if (CanExit())
		CDialogEx::OnClose();
}

void CChatServerDlg::OnOK()
{
	if (CanExit())
		CDialogEx::OnOK();
}

void CChatServerDlg::OnCancel()
{
	if (CanExit())
		CDialogEx::OnCancel();
}

UINT CChatServerDlg::AcceptClientConnectionThreadProc(LPVOID pParam)
{
	CChatServerDlg* pThis = static_cast<CChatServerDlg*>(pParam);
	SOCKET clientSocket;
	sockaddr_in clientAddress;
	int clientAddressSize = sizeof(clientAddress);
	char usernameBuffer[256];
	int usernameReceivedBytes;

	while (true)
	{
		clientSocket = accept(pThis->m_listenSocket, (sockaddr*)&clientAddress, &clientAddressSize);
		if (clientSocket == INVALID_SOCKET)
		{
			TRACE(_T("Error accepting connection: %d\n"), WSAGetLastError());
			Sleep(1000);
			continue;
			//break;
		}
		
		TRACE(_T("Client connected.\n"));

		usernameReceivedBytes = recv(clientSocket, usernameBuffer, sizeof(usernameBuffer) - 1, 0);
		if (usernameReceivedBytes > 0)
		{
			usernameBuffer[usernameReceivedBytes] = '\0';
			CString username(usernameBuffer);
			TRACE(_T("Received username: %s\n"), username);
			pThis->m_connectedClients[clientSocket] = username;
		}
		else
		{
			TRACE(_T("Error receiving username: %d\n"), WSAGetLastError());
			closesocket(clientSocket);
			continue;
		}
		// Start a new thread to handle communication with this client
		AfxBeginThread(HandleClientThreadProc, (LPVOID)clientSocket);

		// Add client socket to the list of connected clients
		pThis->m_clientSockets.AddTail(clientSocket);
	}

	return 0;
}

UINT CChatServerDlg::HandleClientThreadProc(LPVOID pParam)
{
	SOCKET clientSocket = (SOCKET)pParam;
	char buffer[4096];
	int bytesReceived;

	// Get the pointer to the CChatServerDlg instance
	CChatServerDlg* pDlg = static_cast<CChatServerDlg*>(AfxGetApp()->m_pMainWnd);
	if (pDlg == nullptr)
	{
		TRACE(_T("Error: Could not get pointer to main dialog.\n"));
		closesocket(clientSocket);
		return 0;
	}

	while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)) > 0)
	{
		buffer[bytesReceived] = '\0';

		CString message(buffer);
		TRACE(_T("Received from client: %s\n"), message);

		std::time_t now = std::time(nullptr);
		std::tm currentTime;
		localtime_s(&currentTime, &now);
		char timeBuffer[32];
		strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", &currentTime);
		CString timeStamp(timeBuffer);

		// Get the username of the client who sent the message
		CString senderUsername;
		auto it = pDlg->m_connectedClients.find(clientSocket);
		if (it != pDlg->m_connectedClients.end())
		{
			senderUsername = it->second;
		}
		else
		{
			senderUsername = _T("Unknown"); // Handle case where username is not found
		}

		// Format the message with the username
		CString formattedMessage;
		formattedMessage.Format(_T("[%s] [%s] : %s"), timeStamp, senderUsername, message);

		// Convert the formatted message to ASCII for sending
		CT2A asciiFormattedMessage(formattedMessage);
		int formattedMessageLength = strlen(asciiFormattedMessage);

		// Iterate through the list of connected clients and send the message to each one
		POSITION pos = pDlg->m_clientSockets.GetHeadPosition();
		while (pos != NULL)
		{
			SOCKET otherClientSocket = pDlg->m_clientSockets.GetNext(pos);
			if (otherClientSocket != INVALID_SOCKET /*&& otherClientSocket != clientSocket*/)
			{
				TRACE(_T("Server is sending to socket %d: %s (length: %d)\n"), otherClientSocket, formattedMessage, formattedMessageLength);
				send(otherClientSocket, asciiFormattedMessage, formattedMessageLength, 0);
			}
		}
	}

	if (bytesReceived == 0) {
		TRACE(_T("Client disconnected.\n"));
	}
	else if (bytesReceived == SOCKET_ERROR) {
		TRACE(_T("Receive failed with error: %d\n"), WSAGetLastError()); // Added TRACE
	}

	TRACE(_T("Client disconnected.\n"));
	closesocket(clientSocket);

	// Remove the client socket from the list
	POSITION pos = pDlg->m_clientSockets.Find(clientSocket);
	if (pos != NULL)
	{
		pDlg->m_clientSockets.RemoveAt(pos);
		pDlg->m_connectedClients.erase(clientSocket);
	}

	return 0; // Thread finished
}

BOOL CChatServerDlg::CanExit()
{
	// If the proxy object is still around, then the automation
	//  controller is still holding on to this application.  Leave
	//  the dialog around, but hide its UI.
	if (m_pAutoProxy != nullptr)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}

