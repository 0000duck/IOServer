// S715xxDrv.h : main header file for the S715xxDrv DLL
//

#if !defined(AFX_S715xxDrv_H__7B1E2BF8_CF88_4BEF_B4F8_50166608C10B__INCLUDED_)
#define AFX_S715xxDrv_H__7B1E2BF8_CF88_4BEF_B4F8_50166608C10B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMyS715xxDrvApp
// See S715xxDrv.cpp for the implementation of this class
//

class CMyS715xxDrvApp : public CWinApp
{
public:
	CMyS715xxDrvApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyS715xxDrvApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMyS715xxDrvApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_S715xxDrv_H__7B1E2BF8_CF88_4BEF_B4F8_50166608C10B__INCLUDED_)
