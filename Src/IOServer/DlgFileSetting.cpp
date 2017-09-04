// DlgFileSetting.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "IOServer.h"
#include "DlgFileSetting.h"
#include "afxdialogex.h"


// CDlgFileSetting �Ի���

IMPLEMENT_DYNAMIC(CDlgFileSetting, CDialogEx)

CDlgFileSetting::CDlgFileSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgFileSetting::IDD, pParent)
	, m_szFilePath(_T(""))
	, m_szFileName(_T(""))
	, m_bReplace(FALSE)
	, m_bUseFileOutput(FALSE)
	, m_lInterval(0)
{

}

CDlgFileSetting::~CDlgFileSetting()
{
}

void CDlgFileSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FILE_PATH, m_szFilePath);
	DDX_Text(pDX, IDC_FILE_NAME, m_szFileName);
	DDX_Check(pDX, IDC_CHK_FILE_REPLACE, m_bReplace);
	DDX_Check(pDX, IDC_CHK_USE_FILEOUTPUT, m_bUseFileOutput);
	DDX_Text(pDX, IDC_SAVE_INTERVAL, m_lInterval);
}


BEGIN_MESSAGE_MAP(CDlgFileSetting, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_FILE_SETTING, &CDlgFileSetting::OnBnClickedBtnFileSetting)
END_MESSAGE_MAP()


// CDlgFileSetting ��Ϣ�������


void CDlgFileSetting::OnBnClickedBtnFileSetting()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TCHAR szFolderPath[MAX_PATH] = {0}; 
	CString strFolderPath = ""; 
	BROWSEINFO sInfo; 
	ZeroMemory(&sInfo, sizeof(BROWSEINFO));

	sInfo.pidlRoot = 0; 
	sInfo.lpszTitle = "ѡ��洢·��:"; 
	sInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_EDITBOX;
	sInfo.lpfn = NULL; 

	// ��ʾ�ļ���ѡ��Ի��� 
	LPITEMIDLIST lpidlBrowse = SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL) 
	{ 
		// ��ȡ�ļ����� 
		if (SHGetPathFromIDList(lpidlBrowse,szFolderPath)) 
		{ 
			m_szFilePath = szFolderPath; 
		} 
	} 
	if(lpidlBrowse != NULL) 
	{ 
		CoTaskMemFree(lpidlBrowse); 
	}
}
