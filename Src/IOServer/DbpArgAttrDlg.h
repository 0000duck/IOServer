#pragma once
#include "afxcmn.h"


// CDbpArgAttrDlg �Ի���

class CDbpArgAttrDlg : public CDialog
{
	DECLARE_DYNAMIC(CDbpArgAttrDlg)

public:
	CDbpArgAttrDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDbpArgAttrDlg();

	BOOL	m_bAdd;
	CString m_szip;

// �Ի�������
	enum { IDD = IDD_DIALOG_DBPARG_ATTE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CIPAddressCtrl m_ip;
	UINT m_wport;
	CString m_szuser;
	CString m_szpass;
	int m_nNetMode;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
