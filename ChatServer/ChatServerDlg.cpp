
// ChatServerDlg.cpp : implementation file
//

//-----------------------------------------------------------------------------
// File:        ChatServerDlg.cpp
// Author:      Yadunand Kamath
// Date:        2025-03-22
// Description: Implementation file for the CChatServerDlg class, which handles
//              the main dialog of the chat server application.
//-----------------------------------------------------------------------------

#include "pch.h"
#include "framework.h"
#include "ChatServer.h"
#include "ChatServerDlg.h"
#include "DlgProxy.h"
#include "afxdialogex.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <ctime>

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


// CChatServerDlg dialog


IMPLEMENT_DYNAMIC(CChatServerDlg, CDialogEx);

CChatServerDlg::CChatServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHATSERVER_DIALOG, pParent)
{
	// Initialize the member variables in the constructor
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = nullptr;
	m_listenSocket = INVALID_SOCKET;
	m_clientSockets.RemoveAll();
	m_connectedClients.clear();
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
	DDX_Control(pDX, IDC_LIST_CLIENTS, m_clientList);
}

BEGIN_MESSAGE_MAP(CChatServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


//-----------------------------------------------------------------------------
// Method:      OnInitDialog
// Description: This method is called when the dialog is being initialized.
//              It performs setup tasks such as initializing controls,
//              setting the window title, and starting the server.
// Parameters:  None
// Returns:     BOOL - TRUE unless you set the focus to a control.
//-----------------------------------------------------------------------------

BOOL CChatServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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

	// Set the font for the static texts
	CFont font1, font2;
	font1.CreateFont(
		15,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_BOLD,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Tahoma"));                 // lpszFacename

	GetDlgItem(IDC_STATIC_SERVER)->SetFont(&font1);

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
		errorMessage.Format(_T("SERVER: Error binding socket.\n Error code: %d"), WSAGetLastError());
		MessageBox(errorMessage, L"Error", MB_OK | MB_ICONERROR);
		closesocket(listenSocket);
		WSACleanup();
		return FALSE;
	}

	// Listening for incoming connections
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		CString errorMessage;
		errorMessage.Format(_T("SERVER: Error listening on socket.\n Error code: %d"), WSAGetLastError());
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


//-----------------------------------------------------------------------------
// Methods:     OnClose, OnCancel
// Description: Sets behavior when dialog is closed, 
//				Cancel button is clicked,
//				respectively.
// Parameters:  None
// Returns:     void
//-----------------------------------------------------------------------------

void CChatServerDlg::OnClose()
{
	if (CanExit())
		CDialogEx::OnClose();
}

void CChatServerDlg::OnCancel()
{
	if (CanExit())
		CDialogEx::OnCancel();
}

//-----------------------------------------------------------------------------
// Method:      AcceptClientConnectionThreadProc
// Description: This static method is run in a separate thread to accept
//              incoming client connections. It receives the client's username
//              and starts a new thread to handle communication with the client.
// Parameters:  LPVOID pParam - A pointer to the CChatServerDlg instance.
// Returns:     UINT - 0 (thread's exit code)
//-----------------------------------------------------------------------------

UINT CChatServerDlg::AcceptClientConnectionThreadProc(LPVOID pParam)
{
	CChatServerDlg* pThis = static_cast<CChatServerDlg*>(pParam);
	if (pThis == nullptr)
	{
		TRACE(_T("Error: Could not get pointer to main dialog.\n"));
		return 0;
	}
	// Decalre variables to store new client information
	SOCKET clientSocket;
	sockaddr_in clientAddress;
	int clientAddressSize = sizeof(clientAddress);
	char usernameBuffer[256];
	int usernameReceivedBytes;

	// Check for incoming connections continously until the server is shut down
	while (true)
	{
		// Accept a new client connection
		clientSocket = accept(pThis->m_listenSocket, (sockaddr*)&clientAddress, &clientAddressSize);
		if (clientSocket == INVALID_SOCKET)
		{
			TRACE(_T("Error accepting connection: %d\n"), WSAGetLastError());
			Sleep(1000);
			continue;	// Continue to accept new connections
		}
		
		TRACE(_T("Client connected.\n"));

		// Receive the username of the client
		usernameReceivedBytes = recv(clientSocket, usernameBuffer, sizeof(usernameBuffer) - 1, 0);
		if (usernameReceivedBytes > 0)
		{
			usernameBuffer[usernameReceivedBytes] = '\0';	// Terminate the string
			CString username(usernameBuffer);
			TRACE(_T("Received username: %s\n"), username);
			pThis->m_connectedClients[clientSocket] = username;		// Add the client to the list of connected clients
			pThis->m_clientList.AddString(username);	// Add the client to the list box

			CString joinMessage;
			joinMessage.Format(_T("%s has joined the chat!"), username);
			CT2A asciiJoinMessage(joinMessage);
			POSITION pos = pThis->m_clientSockets.GetHeadPosition();
			while (pos != nullptr)
			{
				SOCKET otherClientSocket = pThis->m_clientSockets.GetNext(pos);
				if (otherClientSocket != INVALID_SOCKET && otherClientSocket != clientSocket)
				{
					TRACE(_T("Server is sending to socket %d: %s (length: %d)\n"), otherClientSocket, joinMessage, strlen(asciiJoinMessage));
					send(otherClientSocket, asciiJoinMessage, strlen(asciiJoinMessage), 0);	// send the message to the client
				}
			}
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

//-----------------------------------------------------------------------------
// Method:      HandleClientThreadProc
// Description: This static method is run in a separate thread to handle
//              communication with each client. It receives messages from the
//              client and broadcasts them to all other connected clients.
// Parameters:  LPVOID pParam - Socket of the client to handle.
// Returns:     UINT - 0 (thread's exit code)
//-----------------------------------------------------------------------------

UINT CChatServerDlg::HandleClientThreadProc(LPVOID pParam)
{
	SOCKET clientSocket = (SOCKET)pParam;

	// Declare variables to store received message
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

	// Receive messages from the client continously until the client disconnects
	while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)) > 0)
	{
		buffer[bytesReceived] = '\0';

		CString message(buffer);
		TRACE(_T("Received from client: %s\n"), message);

		// Add timestamp to the message
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
			senderUsername = _T("Unknown"); // if username is not found
		}

		// Format the message with the username and timestamp
		CString formattedMessage;
		formattedMessage.Format(_T("[%s] [%s] : %s"), timeStamp, senderUsername, message);

		// Convert the formatted message to ASCII for sending
		CT2A asciiFormattedMessage(formattedMessage);
		int formattedMessageLength = strlen(asciiFormattedMessage);

		// Iterate through the list of connected clients and send the message to each one
		POSITION pos = pDlg->m_clientSockets.GetHeadPosition();
		while (pos != nullptr)
		{
			SOCKET otherClientSocket = pDlg->m_clientSockets.GetNext(pos);
			if (otherClientSocket != INVALID_SOCKET /*&& otherClientSocket != clientSocket*/)
			{
				TRACE(_T("Server is sending to socket %d: %s (length: %d)\n"), otherClientSocket, formattedMessage, formattedMessageLength);
				send(otherClientSocket, asciiFormattedMessage, formattedMessageLength, 0);	// send the message to the client
			}
		}
	}

	//if (bytesReceived == 0) {
	//	TRACE(_T("Client disconnected.\n"));
	//}
	//else if (bytesReceived == SOCKET_ERROR) {
	//	TRACE(_T("Receive failed with error: %d\n"), WSAGetLastError()); // Added TRACE
	//}

	//TRACE(_T("Client disconnected.\n"));
	//closesocket(clientSocket);

	//
	//POSITION pos = pDlg->m_clientSockets.Find(clientSocket);
	//
	//pDlg->m_clientList.DeleteString(pDlg->m_clientList.FindString(-1, pDlg->m_connectedClients[clientSocket]));  
	//if (pos != nullptr)
	//{
	//	pDlg->m_clientSockets.RemoveAt(pos);
	//	pDlg->m_connectedClients.erase(clientSocket);
	//}

	if (bytesReceived == 0 || bytesReceived == SOCKET_ERROR)
	{
		TRACE(_T("Client disconnected.\n"));
		closesocket(clientSocket);

		// Notify all other connected clients about the disconnected client
		CString leavingUsername = pDlg->m_connectedClients[clientSocket];
		CString leaveMessage;
		leaveMessage.Format(_T("%s left the chat!"), leavingUsername);
		CT2A asciiLeaveMessage(leaveMessage);

		POSITION pos = pDlg->m_clientSockets.GetHeadPosition();
		while (pos != nullptr)
		{
			SOCKET otherClientSocket = pDlg->m_clientSockets.GetNext(pos);
			if (otherClientSocket != INVALID_SOCKET && otherClientSocket != clientSocket)
			{
				send(clientSocket, asciiLeaveMessage, strlen(asciiLeaveMessage), 0);
			}
		}

		pos = pDlg->m_clientSockets.Find(clientSocket);
		// Remove the client from the list box and the list of connected clients
		pDlg->m_clientList.DeleteString(pDlg->m_clientList.FindString(-1, pDlg->m_connectedClients[clientSocket]));
		if (pos != nullptr)
		{
			// Remove the client socket from the list
			pDlg->m_clientSockets.RemoveAt(pos);
			pDlg->m_connectedClients.erase(clientSocket);
		}
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

