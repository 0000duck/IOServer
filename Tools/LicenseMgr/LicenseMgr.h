
// LicenseMgr.h : LicenseMgr Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif


#ifdef _LICENSE_VIEW_APP
#	include "../LicenseView/resource.h"
#else
#	include "resource.h"       // ������
#endif


// CLicenseMgrApp:
// �йش����ʵ�֣������ LicenseMgr.cpp
//

class CLicenseMgrApp : public CWinAppEx
{
public:
	CLicenseMgrApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CLicenseMgrApp theApp;
extern CLicenseFile theLicenseFile;
