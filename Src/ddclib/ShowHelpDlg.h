#pragma once

#include "resource.h"

// CShowHelpDlg �Ի���

class CShowHelpDlg : public CDialog
{
	DECLARE_DYNAMIC(CShowHelpDlg)

public:
	CShowHelpDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CShowHelpDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_HELP };

protected:
	void LoadHelpInfo(CString &szHelpInfo);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
