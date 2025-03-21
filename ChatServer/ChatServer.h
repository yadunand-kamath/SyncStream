
// ChatServer.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CChatServerApp:
// See ChatServer.cpp for the implementation of this class
//

class CChatServerApp : public CWinApp
{
public:
	CChatServerApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CChatServerApp theApp;
