// ShowHelpDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ddclib.h"
#include "ShowHelpDlg.h"


// CShowHelpDlg �Ի���

IMPLEMENT_DYNAMIC(CShowHelpDlg, CDialog)

CShowHelpDlg::CShowHelpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShowHelpDlg::IDD, pParent)
{

}

CShowHelpDlg::~CShowHelpDlg()
{
}

void CShowHelpDlg::LoadHelpInfo(CString &szHelpInfo)
{
	szHelpInfo.Empty();

	CString szResID = CConfigInfo::m_szHelpFile;
	szResID.Delete(0, 6); // ɾ��_T("res://")
	DWORD dwResID = (DWORD)atol(szResID);

	HINSTANCE hi = AfxGetResourceHandle( );
	HRSRC hRsrc = FindResource(hi, MAKEINTRESOURCE(dwResID), TEXT("TEXT"));

	if (NULL == hRsrc)
		return;

	//��ȡ��Դ�Ĵ�С
	int nsize = (int)SizeofResource(hi, hRsrc); 
	if (0 == nsize)		
		return;

	//������Դ
	HGLOBAL hGlobal = LoadResource(hi, hRsrc); 
	if (NULL == hGlobal)
		return;

	//������Դ
	LPVOID pBuffer = LockResource(hGlobal); 
	if (NULL == pBuffer)
	{
		FreeResource(hGlobal);
		return;
	}

	char *szText = new char[nsize + 1];
	memcpy(szText, pBuffer, nsize);
	szText[nsize] = '\0';

	szHelpInfo = szText;

	delete[] szText;

	FreeResource(hGlobal);
}

void CShowHelpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CShowHelpDlg, CDialog)
END_MESSAGE_MAP()


// CShowHelpDlg ��Ϣ�������

BOOL CShowHelpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString szHelpInfo;
	LoadHelpInfo(szHelpInfo);
	
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_HELP);
	if (pEdit)
	{
		pEdit->SetWindowText(szHelpInfo);
		pEdit->SetReadOnly(TRUE);
		pEdit->SetSel(0, 0);
		pEdit->SetFocus();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
