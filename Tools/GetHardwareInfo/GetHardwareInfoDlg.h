
// GetHardwareInfoDlg.h : ͷ�ļ�
//

#pragma once


// CGetHardwareInfoDlg �Ի���
class CGetHardwareInfoDlg : public CDialog
{
// ����
public:
	CGetHardwareInfoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_GETHARDWAREINFO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	CFont	m_fntText;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonGet();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonCopy();
	CString m_szMachineCode;
	afx_msg void OnBnClickedOk();
};
