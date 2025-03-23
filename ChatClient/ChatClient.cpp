
// ChatClient.cpp : Defines the class behaviors for the application.
//

//-----------------------------------------------------------------------------
// File:        ChatClient.cpp
// Author:      Yadunand Kamath
// Date:        2025-03-22
// Description: Implementation file for the CChatClient class, which is the
// 				main application class for the ChatClient application.
//-----------------------------------------------------------------------------

#include "pch.h"
#include "framework.h"
#include "ChatClient.h"
#include "ChatClientDlg.h"
#include "ConnectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChatClientApp

BEGIN_MESSAGE_MAP(CChatClientApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CChatClientApp construction

CChatClientApp::CChatClientApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CChatClientApp object

CChatClientApp theApp;


// CChatClientApp initialization

//-----------------------------------------------------------------------------
// Method:      InitInstance
// Description: This method perform application-level initialization 
//				tasks that need to be done only once when the application 
//				starts.
// Parameters:  None
// Returns:     BOOL (Application terminates if FALSE)
//-----------------------------------------------------------------------------

BOOL CChatClientApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	// Display the login dialog
	CConnectDlg connectDlg;
	INT_PTR nResponse{ connectDlg.DoModal() };
	CString trimmedUsername;
	bool flag{ true };
	
	// Check if user enters a username
	while (flag)
	{
		if (nResponse == IDCANCEL)
		{
			return FALSE;
		}

		trimmedUsername = connectDlg.m_username;
		trimmedUsername.Trim();
		if (!trimmedUsername.IsEmpty())
		{
			flag = false;
		}
		else
		{
			connectDlg.m_username.Trim();
			do
			{
				if (nResponse == IDCANCEL)
				{
					return FALSE;
				}
				if (connectDlg.m_username.IsEmpty())
				{
					AfxMessageBox(_T("Please enter username."));
				}
				nResponse = connectDlg.DoModal();
			} while (connectDlg.m_username.IsEmpty());
		}
	}

	CChatClientDlg chatClientDlg;
	
	// Set the username from client input
	chatClientDlg.SetUsername(trimmedUsername);

	// Get server IP from client input and convert DWORD to CString
	DWORD ipAddress = connectDlg.m_serverIP; 
	BYTE byte1 = (ipAddress >> 24) & 0xFF;
	BYTE byte2 = (ipAddress >> 16) & 0xFF;
	BYTE byte3 = (ipAddress >> 8) & 0xFF;
	BYTE byte4 = ipAddress & 0xFF;
	CString serverIP;
	serverIP.Format(_T("%u.%u.%u.%u"), byte1, byte2, byte3, byte4);

	// Set the formatted server IP address
	chatClientDlg.SetServerAddr(serverIP);

	m_pMainWnd = &chatClientDlg;
	nResponse = chatClientDlg.DoModal();
	if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

