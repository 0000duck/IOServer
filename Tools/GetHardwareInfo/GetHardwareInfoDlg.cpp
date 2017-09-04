
// GetHardwareInfoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GetHardwareInfo.h"
#include "GetHardwareInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CGetHardwareInfoDlg �Ի���




CGetHardwareInfoDlg::CGetHardwareInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGetHardwareInfoDlg::IDD, pParent)
	, m_szMachineCode(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGetHardwareInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CODE, m_szMachineCode);
}

BEGIN_MESSAGE_MAP(CGetHardwareInfoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_GET, &CGetHardwareInfoDlg::OnBnClickedButtonGet)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CGetHardwareInfoDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_COPY, &CGetHardwareInfoDlg::OnBnClickedButtonCopy)
	ON_BN_CLICKED(IDOK, &CGetHardwareInfoDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CGetHardwareInfoDlg ��Ϣ�������

BOOL CGetHardwareInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	m_fntText.CreateFont(16, 0, 0, 0, 400, 0, 0, 0, GB2312_CHARSET, 0, 0, PROOF_QUALITY, VARIABLE_PITCH | FF_ROMAN, "����");

	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT_CODE);
	pEdit->SetFont(&m_fntText);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CGetHardwareInfoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CGetHardwareInfoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CGetHardwareInfoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CGetHardwareInfoDlg::OnBnClickedButtonGet()
{
	CMachineCodeHelper helper(TRUE);
	std::string strMachineCode = helper.GetMachineBase64Encode();

	m_szMachineCode = strMachineCode.c_str();

	UpdateData(FALSE);
}

void CGetHardwareInfoDlg::OnBnClickedButtonSave()
{
	CFileDialog dlg(FALSE, _T(".txt"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("�ı���ʽ(*.txt)|*.txt|"));

	if (dlg.DoModal() != IDOK)
		return;

	CFile file;

	try
	{
		file.Open(dlg.GetPathName(), CFile::modeWrite | CFile::modeCreate);
		file.Write((void *)(LPCTSTR)m_szMachineCode, m_szMachineCode.GetLength());
		file.Close();
	}
	catch (CFileException* e)
	{
		delete e;
		AfxMessageBox(_T("���������ʱ����д�ļ�ʧ�ܣ�"));
	}
}

void CGetHardwareInfoDlg::OnBnClickedButtonCopy()
{
	UpdateData();

	DWORD dwsize = (m_szMachineCode.GetLength() + 1) * sizeof(TCHAR);
	HGLOBAL hGlobal = GlobalAlloc(GHND | GMEM_SHARE, dwsize);
	if (hGlobal == NULL)
		return;

	TCHAR *pGlobal = (TCHAR *)GlobalLock(hGlobal);
	_tcscpy_s(pGlobal, (dwsize / sizeof(TCHAR)), m_szMachineCode);

	GlobalUnlock(hGlobal);
	OpenClipboard();
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hGlobal);
	CloseClipboard();
}

void CGetHardwareInfoDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnOK();
}
