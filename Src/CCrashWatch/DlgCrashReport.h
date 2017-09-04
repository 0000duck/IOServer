#pragma once

#include "XColorStatic.h"
#include "afxwin.h"
#include "Resource.h"

class CCrashHandler;

// CDlgCrashReport �Ի���

class CDlgCrashReport : public CDialog
{
	DECLARE_DYNAMIC(CDlgCrashReport)

public:
	CDlgCrashReport(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgCrashReport();

	CCrashHandler *m_pCrashHandler;

// �Ի�������
	enum { IDD = IDD_DIALOG_CRASH };

protected:
	UINT		m_nTimer;
	int			m_nSeconds;
	HICON		m_hIcon;
	void InitializeDisplay();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CXColorStatic m_ctrlBanner;
	CXColorStatic m_ctrlIcon;
	CXColorStatic m_ctrlPleaseTellUs;

	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	BOOL m_bRestart;
};
