
// IOServer.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "IOServer.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "IOServerDoc.h"
#include "IOServerView.h"
#include "DbpArgConfigDlg.h"
#include "DlgAddDec.h"
#include "IniTool.h"
#include "DlgIniSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIOServerApp
//const CString gCfgFileName = _T("system\\Server.ini");

BEGIN_MESSAGE_MAP(CIOServerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CIOServerApp::OnAppAbout)
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// ��׼��ӡ��������
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(ID_SYS_LOGON, &CIOServerApp::OnSysLogon)
	ON_COMMAND(ID_SYS_LOGOFF, &CIOServerApp::OnSysLogoff)
	ON_COMMAND(ID_PWD_MODIFY, &CIOServerApp::OnPwdModify)
	ON_COMMAND(ID_IDMGR, &CIOServerApp::OnIdmgr)
	ON_COMMAND(ID_SERVER_PARA, &CIOServerApp::OnServerPara)
	ON_COMMAND(ID_SERVER_LOCAL, &CIOServerApp::OnServerLocal)
END_MESSAGE_MAP()


// CIOServerApp ����

CIOServerApp::CIOServerApp()
	:m_pDriverMgr(NULL)
	,m_szAppDir("")
	,m_hMutex(NULL)
	,m_pDecVarTemplate(NULL)
	,m_pAuthorityMgr(NULL)
	,m_pLog(NULL)
	,m_bAuthority(TRUE)
{
	m_bHiColorIcons = TRUE;

	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// ���Ӧ�ó��������ù�����������ʱ֧��(/clr)�����ģ���:
	//     1) �����д˸������ã�������������������֧�ֲ�������������
	//     2) ��������Ŀ�У������밴������˳���� System.Windows.Forms ������á�
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("IOServer.AppID.NoVersion"));

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CIOServerApp ����

CIOServerApp theApp;


// CIOServerApp ��ʼ��

BOOL CIOServerApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// ÿ����װĿ¼����ֻ����һ������ʵ��������ͬĿ¼�ķ��񣬿���ͬʱ����
	CString szAppDir = Utility::GetModulePath();
	m_szAppDir = szAppDir;

	CString szAppDirMd5;
	MakeMD5Txt32(szAppDir, szAppDirMd5);
	CString szInstanceMutexName;
	szInstanceMutexName.Format(_T("_KIMI_IOSERVER_%s"), szAppDirMd5);

	//��ʵ�� wzq 20101202
	HANDLE hMutex; 
	hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, szInstanceMutexName);
	if (hMutex)
	{
		CloseHandle(hMutex); 
		hMutex = NULL;
		return FALSE;
	}
	m_hMutex= CreateMutex(NULL, FALSE, szInstanceMutexName);

	//���뱾���˺�
	CString szErr = "";



	// ��ʼ�� OLE ��
// 	if (!AfxOleInit())
// 	{
// 		AfxMessageBox(IDP_OLE_INIT_FAILED);
// 		return FALSE;
// 	}
	HRESULT hr = S_FALSE;
	//hr = CoInitializeEx (NULL, COINIT_MULTITHREADED);
	//if(!SUCCEEDED(hr))
	//	return FALSE;
	// �������ʼ�˰�ȫ���ƣ���ĳЩ����¿��ܻ���ָ����������

	/*HRESULT */hr =::CoInitializeSecurity(
		NULL,                    // points to security descriptor
		-1,                     // count of entries in asAuthSvc
		NULL,                    // array of names to register
		NULL,                    // reserved for future use
		RPC_C_AUTHN_LEVEL_NONE, // the default authentication level for proxies
		RPC_C_IMP_LEVEL_IMPERSONATE,// the default impersonation level for proxies
		NULL,                    // used only on Windows 2000
		EOAC_NONE,              // additional client or server-side capabilities
		NULL                     // reserved for future use
		);

	if (FAILED(hr))
	{
		TRACE(_T("CoInitializeSecurity error: 0x%02X"), hr);
	}

	m_pAuthorityMgr = CAuthorityMgr::GetInstance();
	if(!m_pAuthorityMgr)
		return FALSE;
	
	CString szPath = Utility::GetModulePath();

	m_pLog = CLog::GetInstance();
	if(!m_pLog)
		return FALSE;
	m_pLog->InitLogPath(szPath, "IOServer");
	
	//��Ȩ����
	CString szLicFileName = m_szAppDir;
	szLicFileName += _T("License.lic");

	if (!m_LicenseMgr.Load(szLicFileName, "RTDBServer"))
	{
		CString szLicInfo;
		KIMILIC_ERRCODE err = m_LicenseMgr.GetLastErrorCode();

		switch(err)
		{
		case KIMI_LIC_ERR_FILE_NOT_EXIST:
			szLicInfo.LoadString(IDS_NOAUTHORITY_FILE);
			break;
		case KIMI_LIC_ERR_SOFTWARE_NOT_EXIST:
			szLicInfo.LoadString(IDS_NOAUTHORITY);
			break;
		case KIMI_LIC_ERR_NOT_AUTHORIZED:
			szLicInfo.LoadStringA(IDS_NOAUTHORITY_SOFT);
			break;
		case KIMI_LIC_ERR_MCODE_NOT_MATCH:
			szLicInfo.LoadStringA(IDS_AUTHORITY_ERR_MEC);
			break;
		case KIMI_LIC_ERR_AUTH_EXPIRED:
			szLicInfo.LoadStringA(IDS_AUTHORITY_EXPIRED);
			break;
		}

		CString szMachineCodeFile = m_szAppDir;
		szMachineCodeFile += _T("SerielCode.txt");

		try
		{
			FILE* pf = fopen(szMachineCodeFile, "wt");
			if (pf)
			{
				std::string szMachineCode = m_LicenseMgr.GetMachineCode();

				fwrite(szMachineCode.c_str(), szMachineCode.size(), 1, pf);
				fclose(pf);
			}
		}
		catch (...)
		{

		}

		AfxMessageBox(szLicInfo, MB_OK | MB_ICONERROR);
		m_bAuthority = FALSE;
	}
	
	if(!m_pAuthorityMgr->LoadDrv(szAppDir + "system\\Operator.dll",szErr)) //�����˺Ź���DLL�ļ�
	{
		AfxMessageBox(szErr);
		return FALSE;
	}
	m_pAuthorityMgr->LoadOptFile(szAppDir + "system\\opt.opt"); //�����˺��ļ�
	//ϵͳ����
	CString szCfgFile = szAppDir + g_szLocalCfgFile;

	// ��������ļ������ڣ��򴴽�һ��Ĭ�ϵ�
	if (!::PathFileExists(szCfgFile))
	{
		CIniTool newIni(szCfgFile);
		newIni.Write();
	}

	if(!m_Config.ReadIniFile(szCfgFile)) 
	{
		AfxMessageBox(IDS_CFGERR);
		return FALSE;
	}

	m_pLog->SetAllowLogLevel(m_Config.m_nLogLevel);

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// ʹ�� RichEdit �ؼ���Ҫ  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_IOServerTYPE,
		RUNTIME_CLASS(CIOServerDoc),
		RUNTIME_CLASS(CChildFrame), // �Զ��� MDI �ӿ��
		RUNTIME_CLASS(CIOServerView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	m_pDecVarTemplate = pDocTemplate;
	// ������ MDI ��ܴ���
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� MDI Ӧ�ó����У���Ӧ������ m_pMainWnd ֮����������
	// ������/��
	m_pMainWnd->DragAcceptFiles();

	// ������׼ shell ���DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	//����ʱ�����ĵ�
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew) 
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	// ���á�DDE ִ�С�
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);


	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// �������ѳ�ʼ���������ʾ����������и���
	pMainFrame->ShowWindow(m_nCmdShow);

	m_pDriverMgr = CDriverMgr::GetInstance();

	InitDriveMgr();
	//���빤��
	m_pDriverMgr->OpenProject(szAppDir,m_Config.m_szProjectName);

	pMainFrame->AddProjectData();

	m_pDriverMgr->Start(m_Config.m_bAutoRun);

	pMainFrame->UpdateWindow();

	return TRUE;
}

int CIOServerApp::ExitInstance()
{
	//TODO: �����������ӵĸ�����Դ
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CIOServerApp ��Ϣ�������


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
void CIOServerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CIOServerApp �Զ������/���淽��

void CIOServerApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);

	GetContextMenuManager()->AddMenu("���̹���", IDR_MENU_PROJECT);
}

void CIOServerApp::LoadCustomState()
{
}

void CIOServerApp::SaveCustomState()
{
}

// CIOServerApp ��Ϣ�������



// //��¼����
void CIOServerApp::Logon(void)
{
	m_pAuthorityMgr->Login();
	if(!m_pAuthorityMgr->m_szOpt.IsEmpty())
	{
		m_pLog->AddInfoFmt("<%s>��¼",theApp.m_pAuthorityMgr->m_szOpt);
	}
}


// ע��
void CIOServerApp::LogOff(void)
{
	CString szopt = theApp.m_pAuthorityMgr->m_szOpt;
	m_pAuthorityMgr->Logout();
	if(m_pAuthorityMgr->m_szOpt.IsEmpty())
	{
		m_pLog->AddInfoFmt("<%s>ע��",szopt);
	}
}


void CIOServerApp::UserIDMgr(void)
{
	if(m_pAuthorityMgr->m_szOpt.IsEmpty())
	{
		AfxMessageBox("���ȵ�¼!");
		return;
	}
	if(m_pAuthorityMgr->m_dwPower != 0xFFFFFFFF)
	{
		AfxMessageBox("Ȩ�޲���!");
		return;
	}
	m_pAuthorityMgr->OptMan(m_pAuthorityMgr->m_szOpt,m_pAuthorityMgr->m_szPass);
}


void CIOServerApp::UserIDModify(void)
{
	if(m_pAuthorityMgr->m_szOpt.IsEmpty())
	{
		AfxMessageBox("���ȵ�¼!");
		return;
	}

	if(!m_pAuthorityMgr->ModifyPass(m_pAuthorityMgr->m_szOpt))
	{
		m_pLog->AddInfoFmt(_T("<%s>ע��"), m_pAuthorityMgr->m_szOpt);

		AfxMessageBox("�޸�����ɹ��������µ�¼!");
		m_pAuthorityMgr->Logout();
	}	
}


void CIOServerApp::SetDBConnectInfo(void)
{
	if(m_pAuthorityMgr->m_szOpt.IsEmpty())
	{
		AfxMessageBox("���ȵ�¼!");
		return;
	}
	if(m_pAuthorityMgr->m_dwPower != 0xFFFFFFFF)
	{
		AfxMessageBox("Ȩ�޲���!");
		return;
	}
	CDBPArgs dbpArgs;
	CDbpArgConfigDlg dlg;
	m_pDriverMgr->GetDBPConnectArgs(dbpArgs);
	dlg.SetDbpConArgs(dbpArgs);

	if (dlg.DoModal() != IDOK)
		return;

	if (dlg.GetDbpConArgs(dbpArgs))
	{
		m_pDriverMgr->SetDBPConnectArgs(dbpArgs);//����

		AfxMessageBox("Ŀ����������������Ѹı䣬����������������������Ч!");
	}

}


BOOL CIOServerApp::IsLogin(void)
{
	BOOL bRet = FALSE;
	if (m_pAuthorityMgr->m_szOpt.IsEmpty())
	{
		bRet = TRUE;
	}
	return bRet;
}


DWORD CIOServerApp::GetUserPower(void)
{
	return m_pAuthorityMgr->m_dwPower;
}


BOOL CIOServerApp::CheckPower(DWORD dwPower)
{
	if(m_pAuthorityMgr->m_szOpt.IsEmpty())
	{
		AfxMessageBox("���ȵ�¼!",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	if((m_pAuthorityMgr->m_dwPower & dwPower) != dwPower)
	{
		AfxMessageBox("Ȩ�޲���!",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}

	return TRUE;
}


void CIOServerApp::AddDrv(void)
{
	if(!theApp.CheckPower(0XFFFFFFFF))
		return ; 
	CDlgAddDec dlg;
	dlg.m_szPath = Utility::GetModulePath() + "drivers/";
	if(dlg.DoModal() != IDOK)
		return;
	CString szDecName = dlg.m_szDecName;
	CString szDrvName = dlg.m_szDrvName;
	int nRet = m_pDriverMgr->AddDrv(dlg.m_szPath,dlg.m_szDrvName,dlg.m_szDecName);
	switch(nRet)
	{
	case  ERR_DRV_EXIST: //�豸����
		AfxMessageBox("�豸���Ѵ���!",MB_OK|MB_ICONERROR);
		break;
	case  ERR_DRV_CREATE: //�豸����
		AfxMessageBox("��������ʵ������!",MB_OK|MB_ICONERROR);
		break;
	case  ERR_DRV_LOAD: //�豸����
		AfxMessageBox("������������",MB_OK|MB_ICONERROR);
		break;
	case  ERR_DRV_NO: //�豸����
		break;
	}
	GetLogger()->AddWarningFmt(LOG_ADD_DEVICE_FORMAT, LOG_LOGIN_USER, dlg.m_szDecName);
	if(nRet == ERR_DRV_OK)
	{
		//m_pDriverMgr->AddDrvToTree(pdrv,m_WorkBar.GetTree());	
		((CMainFrame *)m_pMainWnd)->AddNode2Project(szDrvName, szDecName, NODE_RDB);
		Save();//��������
	}
}


BOOL CIOServerApp::GetDrvInst(CString szDrvName)
{
	CDrvItem* pItem = m_pDriverMgr->GetDrvInst(szDrvName);
	if (pItem)
		return TRUE;
	return FALSE;
}


BOOL CIOServerApp::GetDrvInfo(CString szName, CString szPath, T_DRVINFO1& tDRVINFO)
{
	BOOL bRet = FALSE;
	if (szName.IsEmpty() || szPath.IsEmpty())
		return bRet;
	if(ERR_DRV_OK == m_pDriverMgr->GetDrvInfo(szName, szPath, tDRVINFO))
		bRet = TRUE;
	return bRet;
}


int CIOServerApp::QueryTag(LPCTSTR strDecName, 
	CSortObArray& SortArray,
	LPCTSTR szTagName,
	LPCTSTR szProxyTagName,
	LPCTSTR szDes,
	LPCTSTR szDrvTagName,
	int nType)
{
	return m_pDriverMgr->QueryTag(strDecName, SortArray, szTagName, szProxyTagName, szDes, szDrvTagName, nType);
}


int CIOServerApp::GetDecStatus(CString szDecName)
{
	return m_pDriverMgr->GetDecStatus(szDecName);
}

BOOL CIOServerApp::GetDrvTagDef(CString szDrvName, int iIndex, T_DRVTAG& tDRVTAG)
{
	return m_pDriverMgr->GetDrvTagDef(szDrvName,  iIndex, tDRVTAG);
}

int CIOServerApp::AddTag2Drv(CString szDecName, T_DRVTAG& tDRVTAG, DWORD dwFlag)
{
	return m_pDriverMgr->AddTag2Drv(szDecName,  tDRVTAG, dwFlag);
}
BOOL CIOServerApp::AddTag2Drv(CString szDecName, CTagItem& tTagItem, DWORD dwFlag)
{
	return m_pDriverMgr->AddTag2Drv(szDecName,  tTagItem, dwFlag);
}

int CIOServerApp::Save(void)
{
	int iRet = 0;
	iRet = m_pDriverMgr->SavePrjFile();
	return iRet;
}

int CIOServerApp::CheckExp(const CString& szExp, CString szTagName, CString& szErrMsg, int& nTagCount)
{
	return m_pDriverMgr->CheckExp(szExp, szTagName, szErrMsg, nTagCount);
}

int CIOServerApp::DelDrvTag(CString szDecName, CString szTagName)
{
	return m_pDriverMgr->DelDrvTag(szDecName, szTagName);
}


BOOL CIOServerApp::UpdateTag(CString szDecName, CTagItem& tagItem)
{
	return m_pDriverMgr->UpdateTag(szDecName, tagItem);
}


BOOL CIOServerApp::ImportTags(CString szDecName,CCSVFile& csvFile,int &nUpdate,int &nInsert,int &nAll,DWORD dwflag)
{
	return m_pDriverMgr->ImportTags(szDecName, csvFile, nUpdate, nInsert, nAll, dwflag);
}


int CIOServerApp::InitDriveMgr(void)
{
	if(!m_pDriverMgr)
		return -1;
	m_pDriverMgr->m_bAutoRun	= m_Config.m_bAutoRun;			//����ʱ�Զ���������ʵ��
	m_pDriverMgr->m_bSaveLog	= m_Config.m_bSaveLog;			//������־
	m_pDriverMgr->m_bExitPass	= m_Config.m_bExitPass;			//�˳�������֤
	m_pDriverMgr->m_bCashe		= m_Config.m_bCashe;			//�Ƿ�ʹ��cashe
	m_pDriverMgr->m_bSkipSameTime = m_Config.m_bSkipSameTime;	//������ͬʱ����ո���
	m_pDriverMgr->m_nTimeZero	= m_Config.m_nTimeZero;			//��ʾʱ��-12,12
	m_pDriverMgr->m_wHeartPort	= m_Config.m_wheartport;		//�����˿�
	m_pDriverMgr->m_szHeartIP	= m_Config.m_szHeartIP;			//����IP,�ձ�ʾ�����Ƿ�����
	m_pDriverMgr->m_nLogLevel	= m_Config.m_nLogLevel;			//��־����0-����1-���桢2-��ʾ��3-����
	//m_pDriverMgr->m_szTitle		= m_Config.m_szTitle;			//DDClient������ʾ���⣬������DDClient�Ĺ���
	m_pDriverMgr->m_bWaitTcpAck	= m_Config.m_bWaitTcpAck;		//TCP�Ƿ�����Ӧ����������բ�ǲ���������Ӧ����
	m_pDriverMgr->m_nUdpSendDelay = m_Config.m_nUdpSendDelay;	//UDP������ʱ
	m_pDriverMgr->m_nUpThreads	= m_Config.m_nupthreads;		//�ϴ��߳���
	//������
	m_pDriverMgr->m_lAdjustSec	= m_Config.m_lAdjustSec;		//��ʾ��������,��m_nTimeZero * 3600 + TIME_ZONE_INFORMATION.Bias * 60
	//end
	return 0;
}


int CIOServerApp::GetDrvInstances(vecDrivers& vecDatas)
{
	return m_pDriverMgr->GetDrvInstances(vecDatas);
}


BOOL CIOServerApp::StartDrv(CString szModule, CString szDrvName)
{
	return m_pDriverMgr->Start(szModule, szDrvName);
}


BOOL CIOServerApp::StopDrv(CString szDrvName)
{
	return m_pDriverMgr->Stop(szDrvName);
}


BOOL CIOServerApp::ConfigDrv(CString szDrvName)
{
	return m_pDriverMgr->ConfigDrv(szDrvName);
}


BOOL CIOServerApp::DeleteDrv(CString szDevName)
{
	m_pDriverMgr->DelOneDec(szDevName);
	CloseDecVarView(szDevName);
	return 0;
}



void CIOServerApp::OnSysLogon()
{
	// TODO: �ڴ���������������
	Logon();
}


void CIOServerApp::OnSysLogoff()
{
	// TODO: �ڴ���������������
	LogOff();
}


void CIOServerApp::OnPwdModify()
{
	// TODO: �ڴ���������������
	if(m_pAuthorityMgr->m_szOpt.IsEmpty())
	{
		AfxMessageBox("���ȵ�¼!");
		return;
	}

	if(!m_pAuthorityMgr->ModifyPass(m_pAuthorityMgr->m_szOpt))
	{
		m_pLog->AddInfoFmt(_T("<%s>ע��"), m_pAuthorityMgr->m_szOpt);

		AfxMessageBox("�޸�����ɹ��������µ�¼!");
		m_pAuthorityMgr->Logout();
	}	
}


void CIOServerApp::OnIdmgr()
{
	// TODO: �ڴ���������������
	if(m_pAuthorityMgr->m_szOpt.IsEmpty())
	{
		AfxMessageBox("���ȵ�¼!");
		return;
	}
	if(m_pAuthorityMgr->m_dwPower != 0xFFFFFFFF)
	{
		AfxMessageBox("Ȩ�޲���!");
		return;
	}
	m_pAuthorityMgr->OptMan(m_pAuthorityMgr->m_szOpt,m_pAuthorityMgr->m_szPass);
}


void CIOServerApp::OnServerPara()
{
	// TODO: �ڴ���������������
	if(m_pAuthorityMgr->m_szOpt.IsEmpty())
	{
		AfxMessageBox("���ȵ�¼!");
		return;
	}
	if(m_pAuthorityMgr->m_dwPower != 0xFFFFFFFF)
	{
		AfxMessageBox("Ȩ�޲���!");
		return;
	}
	CDBPArgs dbpArgs;
	m_pDriverMgr->GetDBPConnectArgs(dbpArgs);
	CDbpArgConfigDlg dlg;

	dlg.SetDbpConArgs(dbpArgs);

	if (dlg.DoModal() != IDOK)
		return;

	if (dlg.GetDbpConArgs(dbpArgs))
	{
		m_pDriverMgr->SetDBPConnectArgs(dbpArgs);
		AfxMessageBox("Ŀ����������������Ѹı䣬����������������������Ч!");
	}
}


void CIOServerApp::OnServerLocal()
{
	// TODO: �ڴ���������������
	if(m_pAuthorityMgr->m_szOpt.IsEmpty())
	{
		AfxMessageBox("���ȵ�¼!");
		return;
	}
	if(m_pAuthorityMgr->m_dwPower != 0xFFFFFFFF)
	{
		AfxMessageBox("Ȩ�޲���!");
		return;
	}

	CDlgIniSet dlg;
	CString szFile = Utility::GetModulePath();
	szFile += g_szLocalCfgFile;

	CIniTool ini(szFile);
	ini.Read();

	dlg.m_szprj = ini.m_szProjectName;
	dlg.m_bExitPass = ini.m_bExitPass;
	dlg.m_bSaveLog = ini.m_bSaveLog;
	dlg.m_bAutoRun = ini.m_bAutoRun;
	dlg.m_bCache = ini.m_bCashe;
	dlg.m_wHeartPort = ini.m_wheartport;
	dlg.m_szHeartIp = ini.m_szHeartIP;
	dlg.m_nTimeZero = ini.m_nTimeZero;
	dlg.m_nthread = ini.m_nupthreads;
	dlg.m_nLogLevel = ini.m_nLogLevel;
	dlg.m_szTitle = ini.m_szTitle;
	dlg.m_bSkipSameTime = ini.m_bSkipSameTime;
	dlg.m_bWaitTcpAck = ini.m_bWaitTcpAck;

	if(dlg.DoModal() != IDOK)
		return;
	if(	(dlg.m_szprj != ini.m_szProjectName)||
		(dlg.m_bExitPass != ini.m_bExitPass)||
		(dlg.m_bSaveLog != ini.m_bSaveLog)||
		(dlg.m_bAutoRun != ini.m_bAutoRun)||
		(dlg.m_bCache != ini.m_bCashe)||
		(dlg.m_bSkipSameTime != ini.m_bSkipSameTime)||
		(dlg.m_wHeartPort != ini.m_wheartport)||
		(dlg.m_szHeartIp != ini.m_szHeartIP)||
		(dlg.m_nTimeZero != ini.m_nTimeZero)||
		(dlg.m_nLogLevel != ini.m_nLogLevel)||
		(dlg.m_szTitle != ini.m_szTitle)||
		(dlg.m_nthread != ini.m_nupthreads)||
		(dlg.m_bWaitTcpAck != ini.m_bWaitTcpAck))
	{
		ini.m_szProjectName = dlg.m_szprj;
		ini.m_bExitPass = dlg.m_bExitPass;
		ini.m_bSaveLog = dlg.m_bSaveLog;
		ini.m_bAutoRun = dlg.m_bAutoRun;
		ini.m_bCashe = dlg.m_bCache;
		ini.m_bSkipSameTime = dlg.m_bSkipSameTime;
		ini.m_wheartport = dlg.m_wHeartPort;
		ini.m_szHeartIP = dlg.m_szHeartIp;
		ini.m_nTimeZero = dlg.m_nTimeZero;
		ini.m_nupthreads = dlg.m_nthread;
		ini.m_nLogLevel = dlg.m_nLogLevel;
		ini.m_szTitle = dlg.m_szTitle;
		ini.m_bWaitTcpAck = dlg.m_bWaitTcpAck;

		if(ini.Write())
		{
			// ����Ҫ����
			m_Config.m_bExitPass = dlg.m_bExitPass;
			m_Config.m_nLogLevel = dlg.m_nLogLevel;
			m_pLog->SetAllowLogLevel((DWORD)dlg.m_nLogLevel);

			SetAdditionTitle(ini.m_szTitle);

			AfxMessageBox("��̬�����ļ��Ѹ��ģ���������!");
		}
	}
}

void CIOServerApp::SetAdditionTitle(LPCTSTR lpszTitle)
{
	CString szMainTitle;
	szMainTitle.LoadString(IDR_MAINFRAME);

	CString szTitle;

	// ���ñ���
	if (!lpszTitle || !*lpszTitle)
	{
		szTitle = szMainTitle;
	}
	else 
	{
		szTitle.Format(_T("%s - [%s]"), szMainTitle, lpszTitle);
	}

	CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->SetTitle( szTitle );
	pMainFrame->OnUpdateFrameTitle(TRUE);
}

BOOL CIOServerApp::OpenDecVarView(CString szDecName)
{
	if(NULL == m_pDecVarTemplate)
		return FALSE;
	POSITION posDoc = m_pDecVarTemplate->GetFirstDocPosition();
	while(posDoc!=NULL)
	{
		CIOServerDoc* pDoc=(CIOServerDoc *)m_pDecVarTemplate->GetNextDoc(posDoc);
		if(pDoc->m_szDecName == szDecName)
		{
			POSITION posView=pDoc->GetFirstViewPosition();
			if(posView!=NULL)
			{
				CView* pView=pDoc->GetNextView(posView);
				((CMDIChildWnd *)(pView->GetParent()))->ActivateFrame();
				return TRUE;
			}
		}
	}
	CIOServerDoc* pDoc = (CIOServerDoc *)m_pDecVarTemplate->CreateNewDocument();
	if (pDoc == NULL)
	{
		TRACE0("CDocTemplate::CreateNewDocument returned NULL.\n");
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC,MB_ICONERROR);
		return FALSE;
	}

	pDoc->m_szDecName = szDecName;
	ASSERT_VALID(pDoc);

	pDoc->SetTitle(szDecName);//�ĵ�����

	BOOL bAutoDelete = pDoc->m_bAutoDelete;
	pDoc->m_bAutoDelete = FALSE;   // don't destroy if something goes wrong

	CFrameWnd* pFrame = m_pDecVarTemplate->CreateNewFrame(pDoc, NULL);

	pDoc->m_bAutoDelete = bAutoDelete;
	if (pFrame == NULL)
	{
		TRACE0("Warning: failed to create new frame.\n");
		return FALSE;     // command failed
	}
	m_pDecVarTemplate->InitialUpdateFrame(pFrame, pDoc);
	pFrame->ShowWindow(SW_SHOWMAXIMIZED);
	return TRUE;
}


void  CIOServerApp::CloseDecVarView(CString szDecName)
{
	POSITION posDocument=m_pDecVarTemplate->GetFirstDocPosition();
	while(posDocument!=NULL)
	{
		CIOServerDoc *pDoc=(CIOServerDoc *)m_pDecVarTemplate->GetNextDoc(posDocument);
		if(pDoc->m_szDecName == szDecName)
		{
			POSITION posView = pDoc->GetFirstViewPosition();
			if(posView!=NULL)
			{
				CView* pView=pDoc->GetNextView(posView);
				((CMDIChildWnd *)(pView->GetParent()))->SendMessage(WM_CLOSE);
				return;
			}
		}
	}
}

int CIOServerApp::GetDrvTags(DWORD dwFlag, CString szDrvName, CDrvTags& DrvTags, int &nNew, int& nUpdate)
{
	if(NULL == m_pDriverMgr)
		return -1;
	int nRet = m_pDriverMgr->GetDrvTags(dwFlag, szDrvName, DrvTags, nNew, nUpdate);
	return nRet;
}


BOOL CIOServerApp::DelTag(CString szDecName, CString szTagName)
{
	if(NULL == m_pDriverMgr)
		return -1;
	int nRet = m_pDriverMgr->DelTag(szDecName, szTagName);
	return 0;
}

BOOL CIOServerApp::GetTagDef(LPCTSTR lpszDecName, int i, T_DRVTAG& tDRVTAG)
{
	if(NULL == m_pDriverMgr)
		return FALSE;
	return m_pDriverMgr->GetDrvTagDef(lpszDecName, i, tDRVTAG);
}
CLog*  CIOServerApp::GetLog(void)
{
	return m_pLog;
}

CString CIOServerApp::GetOperator()
{
	return m_pAuthorityMgr->m_szOpt;
}
CLog* GetLogger()
{
	return theApp.GetLog();
}

long g_GetTimeAdjust()
{
	return theApp.m_Config.m_lAdjustSec;
}

BOOL CIOServerApp::IsSaveLog(void)
{
	return m_Config.m_bSaveLog;
}


BOOL CIOServerApp::IsCanExit(void)
{
	if(theApp.m_Config.m_bExitPass)
	{
		if(!theApp.CheckPower(1))
			return FALSE;
	}
	return TRUE;
}


void CIOServerApp::ExitServer(void)
{
	m_pDriverMgr->ExitServer();
}


BOOL CIOServerApp::GetTagValue(LPCTSTR lpszDecName, LPCTSTR lpszTagName, CVarVal& varValue)
{
	if(m_pDriverMgr)
		return m_pDriverMgr->GetTagValue(lpszDecName, lpszTagName, varValue);
	else
		return FALSE;
}

void CIOServerApp::GetOutFileCfig(BOOL& bUseFileOutput, T_OutFileCfg& tOutFileCfg)
{
	if(m_pDriverMgr)
		m_pDriverMgr->GetOutFileCfig(bUseFileOutput, tOutFileCfg);
}

void CIOServerApp::SetOutFileCfig(BOOL bUseFileOutput, const T_OutFileCfg& tOutFileCfg)
{
	if(m_pDriverMgr)
		m_pDriverMgr->SetOutFileCfig(bUseFileOutput, tOutFileCfg);
}