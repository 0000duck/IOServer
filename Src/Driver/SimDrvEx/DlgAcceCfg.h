#pragma once


// DlgAcceCfg �Ի���

class CDlgAcceCfg : public CDialog
{
	DECLARE_DYNAMIC(CDlgAcceCfg)

public:
	CDlgAcceCfg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgAcceCfg();

// �Ի�������
	enum { IDD = IDD_DIALOG_ACCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_span;
	int m_timeincr;
	COleDateTime m_starttime;
	COleDateTime m_endtime;
	BOOL m_bAllowAcce;
};
