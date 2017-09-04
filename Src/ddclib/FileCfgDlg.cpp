// FileCfgDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ddclib.h"
#include "FileCfgDlg.h"
#include "FileExplorer.h"

// CFileCfgDlg �Ի���

IMPLEMENT_DYNAMIC(CFileCfgDlg, CDialog)

CFileCfgDlg::CFileCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileCfgDlg::IDD, pParent)
	, m_szFileName(_T(""))
	, m_szFileParam(_T(""))
	, m_bAutoDeleteFile(FALSE)
{

}

CFileCfgDlg::~CFileCfgDlg()
{
}

void CFileCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_szFileName);
	DDX_Text(pDX, IDC_EDIT_FILEPARM, m_szFileParam);
	DDX_Check(pDX, IDC_CHECK_AUTO_DELETE_FILE, m_bAutoDeleteFile);
}


BEGIN_MESSAGE_MAP(CFileCfgDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_FILE, &CFileCfgDlg::OnBnClickedButtonFile)
END_MESSAGE_MAP()


// CFileCfgDlg ��Ϣ�������

BOOL CFileCfgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_STATIC_PARAM_DISPLAY_NAME)->SetWindowText(CConfigInfo::m_szParamDisplayName);
	GetDlgItem(IDC_STATIC_PARAM_HINT_TEXT)->SetWindowText(CConfigInfo::m_szParamHintText);

	if (CConfigInfo::m_bSelectFileName)
		GetDlgItem(IDC_STATIC_FILE)->SetWindowText(_T("�ļ���"));
	else
		GetDlgItem(IDC_STATIC_FILE)->SetWindowText(_T("�ļ�·��"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

LPITEMIDLIST CFileCfgDlg::ParsePidlFromPath(LPCSTR lpszPath) 
{     
	OLECHAR szOleChar[MAX_PATH];     
	LPSHELLFOLDER IpsfDeskTop;     
	LPITEMIDLIST lpifq;     
	ULONG ulEaten, ulAttribs;     
	HRESULT hres;     
	SHGetDesktopFolder(&IpsfDeskTop);     
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpszPath, -1, szOleChar,sizeof(szOleChar));     
	hres = IpsfDeskTop ->ParseDisplayName(NULL, NULL, szOleChar, &ulEaten, &lpifq, &ulAttribs);     
	hres = IpsfDeskTop->Release( );  

	if(FAILED(hres)) 
		return NULL; 

	return lpifq; 
}

void CFileCfgDlg::OnBnClickedButtonFile()
{
	if (CConfigInfo::m_bSelectFileName)
	{
		CFileExplorer dlgFile(TRUE, NULL, m_szFileName, OFN_HIDEREADONLY, CConfigInfo::m_szSelectFileFilter, NULL);

		dlgFile.m_ofn.lpstrTitle = CConfigInfo::m_szSelectFileHint;
		if (IDOK != dlgFile.DoModal())
			return;

		m_szFileName = dlgFile.GetPathName();
		UpdateData(FALSE);
	}
	else {
		TCHAR szPath[1024];
		ZeroMemory(szPath, sizeof(TCHAR) * MAX_PATH);
		_tcscpy(szPath, m_szFileName);

		BROWSEINFO bi;   
		bi.hwndOwner = m_hWnd;   
		bi.pidlRoot = NULL;//ParsePidlFromPath((LPCSTR)(LPCTSTR)m_szFileName);   
		bi.pszDisplayName = szPath;   
		bi.lpszTitle = CConfigInfo::m_szSelectFileHint;   
		bi.ulFlags = 0;   
		bi.lpfn = NULL;   
		bi.lParam = 0;   
		bi.iImage = 0;

		//����ѡ��Ŀ¼�Ի���
		LPITEMIDLIST lp = SHBrowseForFolder(&bi);   

		if (lp && SHGetPathFromIDList(lp, szPath))   
		{
			m_szFileName = szPath;
			UpdateData(FALSE);
		}
	}
}
