
// DlgProxy.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "ChatServer.h"
#include "DlgProxy.h"
#include "ChatServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChatServerDlgAutoProxy

IMPLEMENT_DYNCREATE(CChatServerDlgAutoProxy, CCmdTarget)

CChatServerDlgAutoProxy::CChatServerDlgAutoProxy()
{
	EnableAutomation();

	// To keep the application running as long as an automation
	//	object is active, the constructor calls AfxOleLockApp.
	AfxOleLockApp();

	// Get access to the dialog through the application's
	//  main window pointer.  Set the proxy's internal pointer
	//  to point to the dialog, and set the dialog's back pointer to
	//  this proxy.
	ASSERT_VALID(AfxGetApp()->m_pMainWnd);
	if (AfxGetApp()->m_pMainWnd)
	{
		ASSERT_KINDOF(CChatServerDlg, AfxGetApp()->m_pMainWnd);
		if (AfxGetApp()->m_pMainWnd->IsKindOf(RUNTIME_CLASS(CChatServerDlg)))
		{
			m_pDialog = reinterpret_cast<CChatServerDlg*>(AfxGetApp()->m_pMainWnd);
			m_pDialog->m_pAutoProxy = this;
		}
	}
}

CChatServerDlgAutoProxy::~CChatServerDlgAutoProxy()
{
	// To terminate the application when all objects created with
	// 	with automation, the destructor calls AfxOleUnlockApp.
	//  Among other things, this will destroy the main dialog
	if (m_pDialog != nullptr)
		m_pDialog->m_pAutoProxy = nullptr;
	AfxOleUnlockApp();
}

void CChatServerDlgAutoProxy::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CChatServerDlgAutoProxy, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CChatServerDlgAutoProxy, CCmdTarget)
END_DISPATCH_MAP()

// Note: we add support for IID_IChatServer to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the
//  dispinterface in the .IDL file.

// {f17a1a5a-4189-4302-a6dd-644089d2b34a}
static const IID IID_IChatServer =
{0xf17a1a5a,0x4189,0x4302,{0xa6,0xdd,0x64,0x40,0x89,0xd2,0xb3,0x4a}};

BEGIN_INTERFACE_MAP(CChatServerDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CChatServerDlgAutoProxy, IID_IChatServer, Dispatch)
END_INTERFACE_MAP()

// The IMPLEMENT_OLECREATE2 macro is defined in pch.h of this project
// {5aa4068a-398f-4d49-be44-10673e7c0b72}
IMPLEMENT_OLECREATE2(CChatServerDlgAutoProxy, "ChatServer.Application", 0x5aa4068a,0x398f,0x4d49,0xbe,0x44,0x10,0x67,0x3e,0x7c,0x0b,0x72)


// CChatServerDlgAutoProxy message handlers
