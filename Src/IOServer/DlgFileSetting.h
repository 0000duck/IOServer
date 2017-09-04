#pragma once


// CDlgFileSetting �Ի���

class CDlgFileSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgFileSetting)

public:
	CDlgFileSetting(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgFileSetting();

// �Ի�������
	enum { IDD = IDD_DLG_FILE_SETTING };

	CString m_szFilePath;
	CString m_szFileName;
	BOOL m_bReplace;
	BOOL m_bUseFileOutput;
	long m_lInterval;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg void OnBnClickedBtnFileSetting();

	DECLARE_MESSAGE_MAP()
};
