// DlgHelp.cpp : implementation file
//

#include "stdafx.h"
#include "modbus.h"
#include "DlgHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgHelp dialog


CDlgHelp::CDlgHelp(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgHelp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgHelp)
	//}}AFX_DATA_INIT
}


void CDlgHelp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgHelp)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgHelp, CDialog)
	//{{AFX_MSG_MAP(CDlgHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgHelp message handlers

BOOL CDlgHelp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	HINSTANCE hi = AfxGetResourceHandle( );
	HRSRC hRsrc = FindResource(hi, MAKEINTRESOURCE(IDR_HELP), TEXT("csv"));
	if (NULL == hRsrc)
		return FALSE;
	//��ȡ��Դ�Ĵ�С
	int nsize = (int)SizeofResource(hi, hRsrc); 
	if (0 == nsize)
		return TRUE;
	//������Դ
	HGLOBAL hGlobal = LoadResource(hi, hRsrc); 
	if (NULL == hGlobal)
		return TRUE;
	//������Դ
	LPVOID pBuffer = LockResource(hGlobal); 
	if (NULL == pBuffer)
		return TRUE;

	CString szt = "";
	char *pc= (char*)pBuffer;
	for(int i=0;i<nsize;i++)
	{
		szt+=pc[i];
	}
	SetDlgItemText(IDC_EDIT1,szt);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/*
BOOL UseCustomResource()
{
	//��λ���ǵ��Զ�����Դ��������Ϊ�����Ǵӱ�ģ�鶨λ��Դ�����Խ�����򵥵���ΪNULL����
	HRSRC hRsrc = FindResource(NULL, MAKEINTRESOURCE(ITEMID), TEXT("MyType"));
	if (NULL == hRsrc)
		return FALSE;
	//��ȡ��Դ�Ĵ�С
	DWORD dwSize = SizeofResource(NULL, hRsrc); 
	if (0 == dwSize)
		return FALSE;
	//������Դ
	HGLOBAL hGlobal = LoadResource(NULL, hRsrc); 
	if (NULL == hGlobal)
		return FALSE;
	//������Դ
	LPVOID pBuffer = LockResource(hGlobal); 
	if (NULL == pBuffer)
		return FALSE;
	
	//�����øղŵõ���pBuffer��dwSize����һЩ��Ҫ�����顣����ֱ�����ڴ���ʹ
	//�ã�Ҳ����д�뵽Ӳ���ļ����������Ǽ򵥵�д�뵽Ӳ���ļ���������ǵ��Զ�
	//����Դ����ΪǶ��DLL��Ӧ�ã��������Ҫ����һЩ��
	BOOL bRt = FALSE;
	FILE* fp = _tfopen(_T("demo.exe"), _T("wb"));
	if (fp != NULL)
	{
		if (dwSize == fwrite(pBuffer, sizeof(char), dwSize, fp))
			bRt = TRUE;
		fclose(fp);
	}
	
	//FreeResource(hGlobal);
	return bRt;
}      
  */
