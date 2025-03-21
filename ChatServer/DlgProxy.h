
// DlgProxy.h: header file
//

#pragma once

class CChatServerDlg;


// CChatServerDlgAutoProxy command target

class CChatServerDlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(CChatServerDlgAutoProxy)

	CChatServerDlgAutoProxy();           // protected constructor used by dynamic creation

// Attributes
public:
	CChatServerDlg* m_pDialog;

// Operations
public:

// Overrides
	public:
	virtual void OnFinalRelease();

// Implementation
protected:
	virtual ~CChatServerDlgAutoProxy();

	// Generated message map functions

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CChatServerDlgAutoProxy)

	// Generated OLE dispatch map functions

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

