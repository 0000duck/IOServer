#pragma once

#include "resource.h"
// CFileCfgDlg �Ի���

#include <ShlObj.h>

class CFileCfgDlg : public CDialog
{
	DECLARE_DYNAMIC(CFileCfgDlg)

public:
	CFileCfgDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFileCfgDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_FILE };

protected:
	LPITEMIDLIST ParsePidlFromPath(LPCSTR lpszPath);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_szFileName;
	CString m_szFileParam;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonFile();
	BOOL m_bAutoDeleteFile;
};
