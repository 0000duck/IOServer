#pragma once
#include "Resource.h"

// CDlgExit �Ի���

class CDlgExit : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgExit)

public:
	CDlgExit(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgExit();

// �Ի�������
	enum { IDD = IDD_DIALOG_EXIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	virtual BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam = NULL);
};
