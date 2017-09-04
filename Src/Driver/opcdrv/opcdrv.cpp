// opcdrv.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "opcdrv.h"
#include "myopc.h"

#include "..\sdk\DrvVersion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ͬ������ǩ�������
#define READ_SYNC_TAG_MAX		1000

#define DRVCFG_VERSION		(DRVCFG_VERSION_USER + 2)	// ��һ��
//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

CString g_LoadString(UINT UID)
{
	CString szt="";
	szt.LoadString(UID);
	return szt;
}

/////////////////////////////////////////////////////////////////////////////
// COpcdrvApp

BEGIN_MESSAGE_MAP(COpcdrvApp, CWinApp)
	//{{AFX_MSG_MAP(COpcdrvApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpcdrvApp construction

COpcdrvApp::COpcdrvApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only COpcdrvApp object

COpcdrvApp theApp;

BOOL COpcdrvApp::InitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class

	// Initialize COM:
	//HRESULT hr = CoInitializeEx (NULL, COINIT_MULTITHREADED);
	//m_bComInitialized = SUCCEEDED (hr);
	
	//// If we failed to initialize COM, there isn't much we can do so 
	//// return FALSE.  This will prevent application from starting.
	//if (!m_bComInitialized)
	//	return (FALSE);

	// this is for DCOM:
	// without this, callbacks from the server may get blocked, depending on
	// DCOM configuration settings

	// �������ʼ�˰�ȫ���ƣ���ĳЩ����¿��ܻ���ָ����������

	HRESULT hr =::CoInitializeSecurity(
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

	return CWinApp::InitInstance();
}

int COpcdrvApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_bComInitialized)
		CoUninitialize ();

	return CWinApp::ExitInstance();
}

//////////////////////////////////////////////////////////////////////////////

CItemDef::CItemDef()
{
	m_szName="";
	m_szDes = "";
	m_szUnit = "";
	m_nType = DRT_FLOAT32;
	m_nAccess = 0;
	m_bZeroFilter = FALSE;
	m_szres = "";
//	memset(&m_val, 0, sizeof(m_val));	// ����
}

CItemDef::~CItemDef()
{

}

void CItemDef::Serialize(CArchive& ar, WORD dwVersion)
{
	if(ar.IsStoring())
	{
		ar << m_nType;
		ar << m_nAccess;
		ar << m_szName;
		ar << m_szDes;
		ar << m_szUnit;
		ar << m_szres;
		ar << m_bZeroFilter;
	}
	else
	{
		ar >> m_nType;
		ar >> m_nAccess;
		ar >> m_szName;
		ar >> m_szDes;
		ar >> m_szUnit;
		ar >> m_szres;

		if (dwVersion > 1)
			ar >> m_bZeroFilter;
	}
}


//IMPLEMENT_SERIAL(COpcDrv, CObject, 0)//�汾0
COpcDrv::COpcDrv()
{
	m_nMultiValSize = 0; //���������
	m_ItemArray.SetSize(0,2048);
	m_OpcServer.m_pdrv = this;

	m_cbOnDataOk = NULL;
	m_cbOnLogOk = NULL;
	m_cbOnSoeOk = NULL;

	m_bKillThread = FALSE;				// ɾ���̱߳�־
	m_bRun = FALSE;						// ���б�־
	m_nStatus = DRSTATUS_STOP;		// ״̬
	m_handle=NULL;

	//{{ OPC Server
	m_strProgID="";						//����ID		
	m_strRemoteMachine="";				//���������IP

	//}}
	//{{ OPC Group
	m_strName="decdefgroup";// group name					����
	m_dwUpdateRate=1000;	// update rate in milliseconds	���ݸ������� MS
	m_dwLanguageID=MAKELANGID (LANG_ENGLISH, SUBLANG_ENGLISH_US);;	// language ID,1033 Ӣ��,2052��������	����
	m_bActive=TRUE;			// active state					�״̬
	m_lBias=0;				// time bias in minutes
	m_fDeadband=0;			// percent deadband				����
	m_dwUpdateMethod=OPC_20_DATACHANGE;	// update method used by this group (see globals.h)
	m_nUpdateTimeout = 0;	// ��ʾ��ʹ�����ݸ��³�ʱ
	m_lLastUpdateTime = 0;
	//}}
	m_OpcGroup.SetServer(&m_OpcServer);
	m_OpcGroup.m_pDrv = this;//����ָ��
	m_nReadMode=3;//�첽2.0
	m_nOpcTime = 0; //OPCʱ��
	m_nTimeArea = 8;//ʱ��
}

COpcDrv::~COpcDrv()
{
	int i,n=m_ItemArray.GetSize();
	for(i=0;i<n;i++)
		delete m_ItemArray.GetAt(i);
	m_ItemArray.RemoveAll();
	if(m_handle)
		delete m_handle;
}

void COpcDrv::AddDebug(LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_DEBUG, LOG_OPT_ALL);
}

void COpcDrv::AddDebugFmt(LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_DEBUG, LOG_OPT_ALL);
}

void COpcDrv::AddDebug(DWORD dwLogOptType, LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_DEBUG, dwLogOptType);
}

void COpcDrv::AddDebugFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_DEBUG, dwLogOptType);
}

void COpcDrv::AddInfo(LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_INFO, LOG_OPT_ALL);
}

void COpcDrv::AddInfoFmt(LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_INFO, LOG_OPT_ALL);
}

void COpcDrv::AddInfo(DWORD dwLogOptType, LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_INFO, dwLogOptType);
}

void COpcDrv::AddInfoFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_INFO, dwLogOptType);
}

void COpcDrv::AddWarning(LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_WARNING, LOG_OPT_ALL);
}

void COpcDrv::AddWarningFmt(LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_INFO, LOG_OPT_ALL);
}

void COpcDrv::AddWarning(DWORD dwLogOptType, LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_WARNING, dwLogOptType);
}

void COpcDrv::AddWarningFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_WARNING, dwLogOptType);
}

void COpcDrv::AddError(LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_ERROR, LOG_OPT_ALL);
}

void COpcDrv::AddErrorFmt(LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_ERROR, LOG_OPT_ALL);
}

void COpcDrv::AddError(DWORD dwLogOptType, LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_ERROR, dwLogOptType);
}

void COpcDrv::AddErrorFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_ERROR, dwLogOptType);
}

void	COpcDrv::Create(void *pconfigdata,int nconfiglen,DRCB_OnDataOk fundata,DRCB_OnLogOk funlog,DRCB_OnSoeOk	cbOnSoeOk)
{
	if((pconfigdata!=NULL)&&(nconfiglen > 0))
		LoadConfig(pconfigdata,nconfiglen);
	m_cbOnDataOk = fundata;
	m_cbOnLogOk = funlog;
	m_cbOnSoeOk = cbOnSoeOk;
}

DRVRET  COpcDrv::GetTagDef(int nindex,LPDRVTAG  lptag)
{
	CSafeLock lock(&m_Lock);//��
	int n = m_ItemArray.GetSize();
	CItemDef *pi;
	if((nindex < 0)|| (nindex >= n))
		return DRERR_NOMORETAG;

	pi=m_ItemArray.GetAt(nindex);
	strncpy(lptag->sname,pi->m_szName,63);
	strncpy(lptag->sdes,pi->m_szDes,79);
	strncpy(lptag->sunit,pi->m_szUnit,15);
	lptag->waccess = (char)pi->m_nAccess;
	lptag->wtype = (char)pi->m_nType;

	lptag->cr[0]=0;
	lptag->cr[1]=0;
	lptag->cr[2]=0;
	lptag->cr[3]=0;
	return DRERR_OK;
}

/*
void COpcDrv::PraseArgs(CString &szargs)//��������
{
	CString szReadMode="",szOpcTime = "",szTimeArea="";
	m_strRemoteMachine = "";
	char c;
	int i,pos=0,n = szargs.GetLength();
	for(i=0;i<n;i++)
	{
		c= szargs[i];
		if(c == '\t')
		{
			pos++;
			continue;
		}
		else
		{
			if(pos == 0)
				m_strRemoteMachine += c;
			else if(pos == 1)
				szReadMode += c;
			else if(pos == 2)
				szOpcTime += c;
			else if(pos == 3)
				szTimeArea += c;
		}
	}
	if(!szReadMode.IsEmpty())
		m_nReadMode = atoi(szReadMode);
	if(!szOpcTime.IsEmpty())
		m_nOpcTime = atoi(szOpcTime);
	if(!szTimeArea.IsEmpty())
		m_nTimeArea = atoi(szTimeArea);
	
}
*/
int		COpcDrv::GetConfigSize()		//ȡ�������ݳ���
{
	CMemFile mf(4096);
	CArchive ar(&mf,CArchive::store);
	SaveConfig(ar);
	ar.Close();
	int nl = (int)mf.GetLength();
	mf.Close();
	return nl;
}
DRVRET	COpcDrv::GetConfigData(void *pbuf,int nbufsize,int *pnactsize) //ȡ��������
{
	*pnactsize = 0;

	//����д���ڴ��ļ�
	CMemFile mfile;
	CArchive ar(&mfile,CArchive::store);
	SaveConfig(ar);
	ar.Close();

	//��������ֽ���
	mfile.SeekToEnd();
	int len=(int)mfile.GetLength();
	if( len > nbufsize)//�ռ䲻��
		return 0;//
		
	//���ڴ��ļ����������ܿ�
	mfile.SeekToBegin();//�ƶ���ͷ��
	len = (int)mfile.Read(pbuf,(DWORD)len);
	mfile.Close();
	*pnactsize = len;
	return 0;
}

void COpcDrv::LoadConfig(const void *pblk,int nsize)
{
	ClearConfig();

	CMemFile mfile;
	mfile.Write(pblk,nsize);
	mfile.SeekToBegin();
	CArchive ar(&mfile,CArchive::load);

	// ���Ӱ汾����Ϣ
	DWORD dwDrvFlag;
	ar >> dwDrvFlag;
	if (dwDrvFlag != DRVCFG_FLAG_OPCCLIENT) // �Ƿ�ΪOPC��������
		return;
	WORD wVersion;
	ar >> wVersion;
	if (wVersion > DRVCFG_VERSION)
		return;

	//���������µ���:
	//m_ItemList.Serialize(ar);
	//CString szargs="";
	//{{ OPC Server
	ar >> m_strProgID;							//����ID
	ar >> m_strRemoteMachine;
	ar >> m_nReadMode;
	ar >> m_nOpcTime;
	ar >> m_nTimeArea;
	
	//ar >> szargs;								//���������IP
	//PraseArgs(szargs);

	//}}
	//{{ OPC Group
	ar >> m_strName;		// group name					����
	ar >> m_dwUpdateRate;	// update rate in milliseconds	���ݸ������� MS
	ar >> m_dwLanguageID;	// language ID					����
	ar >> m_bActive;			// active state					�״̬
	ar >> m_lBias;			// time bias in minutes
	ar >> m_fDeadband;		// percent deadband				����
	ar >> m_dwUpdateMethod;	// update method used by this group (see globals.h)
	ar >> m_nUpdateTimeout;
	//}}

	int n=0;
	ar >> n;

	CString szt;
	CItemDef *pi;
	for(int i=0;i<n;i++)
	{
		pi=new CItemDef;
		if(pi==NULL)
			continue;
		pi->Serialize(ar, wVersion);
		m_ItemArray.Add(pi);
	}

	ar.Close();
	m_bconfigModif=FALSE;//�����޸ı�־
}

void COpcDrv::SaveConfig(CArchive& ar)
{
	//���������µ���:
	//m_ItemList.Serialize(ar);

	//�����Ϊ���£���Ϊ��DLL�ﲻ���Զ���������

	/*
	CString szReadMode,sztsave= m_strRemoteMachine;	
	
	szReadMode.Format("%d\t",m_nReadMode);
	m_strRemoteMachine += '\t';
	m_strRemoteMachine += szReadMode;
	szReadMode.Format("%d\t",m_nOpcTime);//OPCʱ��
	m_strRemoteMachine += szReadMode;
	szReadMode.Format("%d\t",m_nTimeArea);//ʱ��
	m_strRemoteMachine += szReadMode;
	*/
	
	// ���Ӱ汾����Ϣ
	DWORD dwDrvFlag = DRVCFG_FLAG_OPCCLIENT;
	ar << dwDrvFlag;
	WORD wVersion = DRVCFG_VERSION;
	ar << wVersion;

	//{{ OPC Server
	ar << m_strProgID;							//����ID
	ar << m_strRemoteMachine;
	ar << m_nReadMode;
	ar << m_nOpcTime;
	ar << m_nTimeArea;
	//ar << m_strRemoteMachine;					//���������IP
	//m_strRemoteMachine = sztsave;
	//}}
	//{{ OPC Group
	ar << m_strName;		// group name					����
	ar << m_dwUpdateRate;	// update rate in milliseconds	���ݸ������� MS
	ar << m_dwLanguageID;	// language ID					����
	ar << m_bActive;			// active state					�״̬
	ar << m_lBias;			// time bias in minutes
	ar << m_fDeadband;		// percent deadband				����
	ar << m_dwUpdateMethod;	// update method used by this group (see globals.h)
	ar << m_nUpdateTimeout;
	//}}

	int n=m_ItemArray.GetSize();
	ar << n;

	CItemDef *pi;
	for(int i=0;i<n;i++)
	{
		pi=m_ItemArray.GetAt(i);
		pi->Serialize(ar, wVersion);
	}
}

void COpcDrv::ClearConfig()
{
	int i,n=m_ItemArray.GetSize();
	for(i=0;i<n;i++)
		delete m_ItemArray.GetAt(i);
	m_ItemArray.RemoveAll();
}

BOOL  COpcDrv::ReadSync (bool bDeviceRead)//ͬ����
{
	BOOL bret= FALSE;
	CObArray cItemList;
	int nitems=m_ItemArray.GetSize();
	cItemList.SetSize(READ_SYNC_TAG_MAX);
	int i,n=0;
	CItemDef *pi;
	for(i=0;i<nitems;i++)
	{
		pi = m_ItemArray.GetAt(i);

		// ��ǩ�Ƿ���ڣ���
		if (!pi->m_item.IsValid())
			continue;

		cItemList.SetAt(n % READ_SYNC_TAG_MAX,&(pi->m_item));
		n++;
		if((n % READ_SYNC_TAG_MAX) == 0)
		{
			if(m_OpcGroup.ReadSync (cItemList, READ_SYNC_TAG_MAX, bDeviceRead))
				bret = TRUE;
// 			cItemList.RemoveAll();
// 			cItemList.SetSize(READ_SYNC_TAG_MAX);
		}
	}
	if((n % READ_SYNC_TAG_MAX) != 0)
	{
		if(m_OpcGroup.ReadSync (cItemList, n % READ_SYNC_TAG_MAX, bDeviceRead))
			bret = TRUE;
	}

/*	cdwItems=m_ItemList.GetCount();
	cItemList.SetSize(108);


	BOOL bret= FALSE;
	POSITION pos;
	pos=m_ItemList.GetHeadPosition();
	int n=0;
	CItemDef *pi;
	while(pos!=NULL)
	{
		pi = m_ItemList.GetNext(pos);
		cItemList.SetAt(n%100,&(pi->m_item));
		n++;
		if((n%100) == 0)
		{
			if(m_OpcGroup.ReadSync (cItemList, 100, bDeviceRead))
				bret = TRUE;
			cItemList.RemoveAll();
			cItemList.SetSize(108);
		}
	}
	if((n%100) != 0)
	{
		if(m_OpcGroup.ReadSync (cItemList, n%100, bDeviceRead))
			bret = TRUE;
	}*/
	return bret;
}

BOOL COpcDrv::CheckLastUpdateTime()
{
	if(!m_OpcServer.IsConnected())
		return TRUE;

	// ���ݸ��³�ʱ��⣬����ﵽ������ݸ��³�ʱ����Ͽ�OPC����������
	if((m_nReadMode == 2) || (m_nReadMode == 3))//�첽��
	{
		if (m_nUpdateTimeout > 0)
		{
			long lNow = (long)::time(NULL);
			if (lNow - m_lLastUpdateTime > (long)m_nUpdateTimeout)
			{
				CString szInfo;				
				szInfo.Format(_T("OPC������<%s>���ݸ��³�ʱ%u��!"), m_strProgID, m_nUpdateTimeout);
				m_cbOnLogOk(m_handle, szInfo, LOG_LEVEL_WARNING, LOG_OPT_ALL);
				Disconnect();
				return FALSE;
			}
		}
	}

	return TRUE;
}

UINT COpcDrv::RunThread(void *pParam)// �����߳�
{
	if(pParam == NULL)
		return 1;
	COpcDrv *pdrv=(COpcDrv *)pParam;
	int i;
	pdrv->m_bKillThread = FALSE;
	pdrv->m_bRun = TRUE;
	int ni=pdrv->m_dwUpdateRate/100;//��������
	if(ni<1)
		ni=1;
	int errorno=0;
	BOOL bflash = FALSE;
	while(!pdrv->m_bKillThread)
	{
		for(i=0;i<ni;i++)
		{
			if(pdrv->m_bKillThread)
			{
				TRACE(" opc RunThread Exit1\n");
				pdrv->m_bRun=FALSE;
				pdrv->m_bKillThread = FALSE;
				return 0;
			}
			Sleep(100);
		}

		if(!pdrv->Reconnect()) //���û����������
		{
			for(i=0;i<50;i++) //5�������
			{
				if(pdrv->m_bKillThread)//Ҫ���˳�
				{
					pdrv->m_bRun=FALSE;
					pdrv->m_bKillThread = FALSE;
					TRACE("opc RunThread Exit2\n");
					return 0;
				}
				Sleep(100);
			}
			continue; //���Ӳ��ɹ�
		}

		if(pdrv->m_nReadMode == 0)//ͬ����catch
		{
			if(!pdrv->ReadSync(FALSE))
			{
				if(!pdrv->m_OpcServer.IsAlive())
					pdrv->Disconnect();
			}
		}
		else if(pdrv->m_nReadMode == 1)//ͬ����device
		{
			if(!pdrv->ReadSync(TRUE))
			{
				if(!pdrv->m_OpcServer.IsAlive())
					pdrv->Disconnect();
			}
		}
		else if(pdrv->m_nReadMode == 2)//�첽��1.0
		{
			if(pdrv->m_OpcGroup.IsIAsyncIOSupported())
			{ 
				if(!pdrv->m_OpcServer.IsAlive()) //�ж��Ƿ�����
				{
					pdrv->Disconnect();
					bflash = FALSE;
				}
				else
				{
					if(!bflash) //ûRefresh
					{
						if(pdrv->m_OpcGroup.RefreshAsync10(FALSE)) //�����,1.0ֻ��Refreshһ��
							bflash = TRUE;
					}
				}
			}
		}
		else if(pdrv->m_nReadMode == 3)//�첽��2.0
		{
			//pdrv->AddDebug(_T("�첽��2.0���..."));

			if(pdrv->m_OpcGroup.IsIAsyncIO2Supported())
			{
#ifndef _YUEYANG_HCPS_OPCSERVER
				if(!pdrv->m_OpcServer.IsAlive()) //�ж��Ƿ�����
				{
					//pdrv->AddDebug(_T("IsAlive() = false"));
					pdrv->Disconnect();
					bflash = FALSE;
				}
				else
				{
					/* 
					 * �������Ϻ�����HCPS OPC Server����pdrv->m_OpcGroup.RefreshAsync20()�ᱻ����������߳��޷�ֹͣ�����Բ�������øú���
					 */
					if(!bflash) //ûRefresh
					{
						if(pdrv->m_OpcGroup.RefreshAsync20(FALSE)) //�����,2.0ֻ��Refreshһ��
							bflash = TRUE;
					}
				}
#endif
			}
		}

		// ����ˢ�³�ʱ���
		pdrv->CheckLastUpdateTime();
	}
	TRACE("opc RunThread Exit3\n");
	pdrv->m_bRun=FALSE;
	pdrv->m_bKillThread = FALSE;
	return 0;
}

BOOL COpcDrv::Reconnect()//��������
{
	if(m_OpcServer.IsConnected())
		return TRUE;

	TRACE("Reconnect...\n");
	if(!m_OpcServer.IsConnected())
	{
		if(!m_OpcServer.Connect(m_strProgID,m_strRemoteMachine))
		{
			TRACE("Opc <%s> Connect Err!\n",m_OpcServer.GetProgID());
			return FALSE;
		}
	}
	if(!m_OpcGroup.IsValid())//û�м�����
	{
		CString szError="";
		if(m_nReadMode == 2)
			m_OpcGroup.SetUpdateMethod(OPC_10_DATATIMECHANGE);
		else
			m_OpcGroup.SetUpdateMethod(OPC_20_DATACHANGE);
//		m_OpcGroup.SetLanguageID(m_dwLanguageID);
		if(!m_OpcServer.AddGroup(&m_OpcGroup,szError))
		{
			CString szErr;
			szErr.Format("Err:AddGroup,%s",szError);
			TRACE(szErr);
			TRACE("\n");
			return FALSE;
		}
		AddDebugFmt(_T("OPC���������Group<%s>�ɹ�"), m_OpcGroup.GetName());
	}
	//�����Ա
	CObArray cItemList;
	
/*	DWORD cdwItems;
	cdwItems=this->m_ItemList.GetCount();
	cItemList.SetSize(cdwItems);
	POSITION pos;
	pos=m_ItemList.GetHeadPosition();
	int n=0;
	CItemDef *pi;
	int nEcdDataType;
	while(pos!=NULL)
	{
		pi = m_ItemList.GetNext(pos);
		pi->m_item.SetParam(pi);//����CItemDef*
		pi->m_item.SetParam2(this);//����COpcDrv*
		pi->m_item.SetItemID(pi->m_szName);
		pi->m_item.SetGroup(&m_OpcGroup);
		nEcdDataType = EcdApi::GetEcdDataType(pi->m_val.vt);
		pi->m_item.SetDataType(EcdApi::EcdDataType2OleDataType(nEcdDataType));
		pi->m_item.SetActive(TRUE);
		cItemList.SetAt(n,&(pi->m_item));
		n++;
	}
	*/
	int i,nitems = m_ItemArray.GetSize();
	cItemList.SetSize(nitems);
	CItemDef *pi;
	for(i=0;i<nitems;i++)
	{
		pi = m_ItemArray.GetAt(i);
		pi->m_item.SetParam(pi);//����CItemDef*
		pi->m_item.SetParam2(this);//����COpcDrv*
		pi->m_item.SetItemID(pi->m_szName);
		pi->m_item.SetGroup(&m_OpcGroup);

#ifdef _DABA_OUT_OF_SERVICE_FILTER
		pi->m_item.m_bOutOfServiceFilter = FALSE;
#endif
	
		//nEcdDataType = EcdApi::GetEcdDataType(pi->m_val.vt);
		//pi->m_item.SetDataType(EcdApi::EcdDataType2OleDataType(nEcdDataType));
		pi->ToOleDataType(pi->m_nType); //����ת��
		pi->m_item.SetActive(TRUE);
		cItemList.SetAt(i,&(pi->m_item));
	}

	CString szErr;
	if(!m_OpcGroup.AddItems(cItemList,nitems,szErr))
	{
		CString szError;
		szError.Format("Err:AddItems,%s",szErr);
		TRACE(szError);
		TRACE("\n");
		return FALSE;
	}
	AddDebugFmt(_T("OPCGroup<%s>���%d����ǩ�ɹ�"), m_OpcGroup.GetName(), nitems);

	m_lLastUpdateTime = (long)::time(NULL);
	return TRUE;
}

void COpcDrv::StartRun()
{
	if(m_bRun)
		return;
	
	if(!m_OpcServer.IsConnected())
	{
		if(!m_OpcServer.Connect(m_strProgID,m_strRemoteMachine))
		{
			TRACE("Opc <%s> Connect Err!\n",m_OpcServer.GetProgID());
			return;
		}
	}
	if(!m_OpcGroup.IsValid())//û�м�����
	{
		// hq2011/11/11 ԭ����û������������ˢ����
		m_OpcGroup.SetDeadband(m_fDeadband);
		m_OpcGroup.SetUpdateRate(m_dwUpdateRate);
		//m_OpcGroup.SetBias(m_lBias);

		CString szError="";
		if(m_nReadMode == 2)
			m_OpcGroup.SetUpdateMethod(OPC_10_DATATIMECHANGE);
		else
			m_OpcGroup.SetUpdateMethod(OPC_20_DATACHANGE);
		
		m_OpcGroup.SetLanguageID(m_dwLanguageID);
		if(!m_OpcServer.AddGroup(&m_OpcGroup,szError))
		{
			CString szErr;
			szErr.Format("Err:AddGroup,%s",szError);
			TRACE(szErr);
			TRACE("\n");
			return ;
		}
		AddDebugFmt(_T("OPC���������Group<%s>�ɹ�"), m_OpcGroup.GetName());
	}
	//�����Ա
	/*
	CObArray cItemList;
	DWORD cdwItems;
	cdwItems=this->m_ItemList.GetCount();
	cItemList.SetSize(cdwItems);
	POSITION pos;
	pos=m_ItemList.GetHeadPosition();
	int n=0;
	CItemDef *pi;
	int nEcdDataType;
	while(pos!=NULL)
	{
		pi = m_ItemList.GetNext(pos);
		pi->m_item.SetParam(pi);//����CItemDef*
		pi->m_item.SetParam2(this);//����COpcDrv*
		pi->m_item.SetItemID(pi->m_szName);
		pi->m_item.SetGroup(&m_OpcGroup);
		nEcdDataType = EcdApi::GetEcdDataType(pi->m_val.vt);
		pi->m_item.SetDataType(EcdApi::EcdDataType2OleDataType(nEcdDataType));
		pi->m_item.SetActive(TRUE);
		cItemList.SetAt(n,&(pi->m_item));
		n++;
	}
	CString szErr;
	if(!m_OpcGroup.AddItems(cItemList,n,szErr))
	{
		CString szError;
		szError.Format("Err:AddItems,%s",szErr);
		TRACE(szError);
		TRACE("\n");
	}*/
	CObArray cItemList;
	int i,nitems = m_ItemArray.GetSize();
	cItemList.SetSize(nitems);
	CItemDef *pi;
	for(i=0;i<nitems;i++)
	{
		pi = m_ItemArray.GetAt(i);
		pi->m_item.SetParam(pi);//����CItemDef*
		pi->m_item.SetParam2(this);//����COpcDrv*
		pi->m_item.SetItemID(pi->m_szName);
		pi->m_item.SetGroup(&m_OpcGroup);

#ifdef _DABA_OUT_OF_SERVICE_FILTER
		pi->m_item.m_bOutOfServiceFilter = FALSE;
#endif
		//nEcdDataType = EcdApi::GetEcdDataType(pi->m_val.vt);
		//pi->m_item.SetDataType(EcdApi::EcdDataType2OleDataType(nEcdDataType));
		pi->ToOleDataType(pi->m_nType); //����ת��
		pi->m_item.SetActive(TRUE);
		cItemList.SetAt(i,&(pi->m_item));
	}
	CString szErr;
	if(!m_OpcGroup.AddItems(cItemList,nitems,szErr))
	{
		CString szError;
		szError.Format("Err:AddItems,%s",szErr);
		TRACE(szError);
		TRACE("\n");
	}
	AddDebugFmt(_T("OPCGroup<%s>���%d����ǩ�ɹ�"), m_OpcGroup.GetName(), nitems);

#ifdef _YUEYANG_HCPS_OPCSERVER
	AddInfo(_T("�������Ϻ�����HCPS OPC Serverר��"));
#endif

#ifdef _DABA_OUT_OF_SERVICE_FILTER
	AddInfo(_T("��ӵ�OPC Serverר��"));
#endif

//	if(OnLog) //ʹ������
//		OnLog(m_handle,0,"Opc Strat From Drv Log!");
	cItemList.RemoveAll();// needcode �¼ӵĿ�������ǰ��һ��Сbug
	::AfxBeginThread(RunThread,this);
}
void COpcDrv::StopRun()
{
	m_bKillThread=TRUE;
	while(m_bRun)
		Sleep(100);

	if(!m_OpcServer.IsConnected())
		return;
	if(!m_OpcGroup.IsValid())
	{
		m_OpcServer.Disconnect();
		return;
	}

	// �Ƴ�items
	CObArray cItemList;
	int i,nItems;
	nItems=m_ItemArray.GetSize();
	cItemList.SetSize(nItems);
/*	POSITION pos;
	pos=m_ItemList.GetHeadPosition();
	int n=0;
	CItemDef *pi;
	while(pos!=NULL)
	{
		pi = m_ItemList.GetNext(pos);
		cItemList.SetAt(n,&(pi->m_item));
		n++;
	}*/
	CItemDef *pi;
	for(i =0 ;i < nItems;i++)
	{
		pi = m_ItemArray.GetAt(i);
		cItemList.SetAt(i,&(pi->m_item));
	}

	CString szErr="";
	if(!m_OpcGroup.RemoveItems(cItemList,nItems,szErr))
		TRACE(szErr);
	cItemList.RemoveAll();
	// �Ƴ�group
	m_OpcServer.RemoveGroup(&m_OpcGroup);
	// �Ͽ�����
	m_OpcServer.Disconnect();
}

BOOL COpcDrv::Disconnect()
{
	// �Ƴ�items
	CObArray cItemList;
	int  nItems;
	nItems=m_ItemArray.GetSize();
	cItemList.SetSize(nItems);

	CItemDef *pi;
	for(int i=0;i<nItems;i++)
	{
		pi = m_ItemArray.GetAt(i);
		cItemList.SetAt(i,&(pi->m_item));

	}
	CString szErr="";
	if(!m_OpcGroup.RemoveItems(cItemList,nItems,szErr))
		TRACE(szErr);
	cItemList.RemoveAll();
	// �Ƴ�group
	m_OpcServer.RemoveGroup(&m_OpcGroup);
	// �Ͽ�����
	m_OpcServer.Disconnect();
	return TRUE;
}

CItemDef *COpcDrv::FindItem(LPCTSTR lpszName)
{
	int  nItems;
	nItems=m_ItemArray.GetSize();


	CItemDef *pi;
	for(int i=0;i<nItems;i++)
	{
		pi = m_ItemArray.GetAt(i);
		if(pi->m_szName == lpszName)
			return pi;
	}
	return NULL;
}

BOOL COpcDrv::DelItem(LPCTSTR lpszName,CString &szErr)
{
	CSafeLock lock(&m_Lock);//��

	int  nItems;
	nItems=m_ItemArray.GetSize();


	CItemDef *pi;
	for(int i=0;i<nItems;i++)
	{
		pi = m_ItemArray.GetAt(i);
		if(pi->m_szName == lpszName)
		{
			m_ItemArray.RemoveAt(i);
			delete pi;
			m_bconfigModif = TRUE;
			return TRUE;
		}
	}
	/*
	CItemDef *pi;
	POSITION pos,posp;
	pos=m_ItemList.GetHeadPosition();
	while(pos!=NULL)
	{
		posp=pos;
		pi=m_ItemList.GetNext(pos);
		if(pi->m_szName == lpszName)
		{
			m_ItemList.RemoveAt(posp);
			delete pi;
			m_bconfigModif = TRUE;
			return TRUE;
		}
	}*/
	szErr.Format("����:û���ҵ�<%s>",lpszName);
	return FALSE;
}

BOOL COpcDrv::ModifyItem(LPCTSTR lpszOldName,LPCTSTR lpszNewName,
						  int nType,int nAccess,CString &szErr)
{
	CSafeLock lock(&m_Lock);//��

	CItemDef *pi,*pin=NULL;
	BOOL	bReName=FALSE;
	CString szNewName = lpszNewName,szOldName = lpszOldName;
	BOOL   bNeedFindReName=FALSE;
	if(szNewName != lpszOldName)
		bNeedFindReName=TRUE;
	
	int i,n = m_ItemArray.GetSize();
	for(i=0;i<n;i++)
	{
		pi=m_ItemArray.GetAt(i);
	
		if(pi->m_szName == szOldName)
		{
			pin=pi;
			if(!bNeedFindReName)
				break;
		}

		if(bNeedFindReName)//��Ҫ������
		{
			if(pi->m_szName == szNewName)
			{
				szErr.Format("<%s>����!",lpszNewName);
				return FALSE;
			}
		}
		
	}
	if(pin != NULL)
	{
		pin->m_szName = szNewName;
		pin->m_nType = nType;
		pin->m_nAccess = nAccess;
		m_bconfigModif = TRUE;
		return TRUE;
	}
	szErr.Format("����:û���ҵ�<%s>",lpszOldName);
	return FALSE;
}

BOOL COpcDrv::AddItem(LPCTSTR lpszName,
					   int nType,int nAccess,CString &szErr)
{
	CSafeLock lock(&m_Lock);//��
	int i,n=m_ItemArray.GetSize();
	CItemDef *pi;

	for(i=0;i<n;i++)
	{
		pi=m_ItemArray.GetAt(i);
		if(pi->m_szName == lpszName)
		{
			szErr.Format("����:������<%s>����!",lpszName);
			return FALSE;
		}
	}

	pi=new CItemDef;
	if(!pi)
		return FALSE;
	pi->m_szName = lpszName;
	pi->m_nType  = nType;
	pi->m_nAccess = nAccess;

	m_ItemArray.Add(pi);
	m_bconfigModif = TRUE;
	return TRUE;
}
void COpcDrv::ReMoveAll()
{
	CSafeLock lock(&m_Lock);//��
	ClearConfig();
}
void COpcDrv::ItemsToListCtrl(CTagItemWinList *pList)
{
	CSafeLock lock(&m_Lock);//��

	CItemDef *pi;
	int i,n = m_ItemArray.GetSize();
	int nHashSize = (int)(n * 1.2);
	if (nHashSize > 36353)
		pList->InitHashSize(nHashSize);

	for(i=0;i<n;i++)
	{
		pi = m_ItemArray.GetAt(i);
		pList->AddItem(pi->m_szName, pi->m_szDes, pi->m_szUnit, pi->GetTypeStr(pi->m_nType), pi->m_bZeroFilter);
	}
	pList->ReDraw();
}

void COpcDrv::ListCtrlToItems(CTagItemWinList *pList)
{
	CSafeLock lock(&m_Lock);//��
	CItemDef *pi;
	CString szType;
	CTagItem *pItem;

	for(int i = 0; i < pList->GetCount(); i++)
	{
		pItem = pList->GetAt(i);
		if (!pItem)
			continue;

		pi = new CItemDef;
		if(!pi)
			continue;
		pi->m_szName = pItem->m_szItemName;
		szType = pItem->m_szDataType;
		pi->m_szDes = pItem->m_szDesc;
		pi->m_szUnit = pItem->m_szUnit;
		pi->m_bZeroFilter = pItem->m_bZeroFilter;

		pi->m_nAccess = 0;
		pi->m_nType = pi->GetTypeVal(szType);
		m_ItemArray.Add(pi);
	}
}

#include "DlgOpcSet.h"
#include "DlgOpcVar.h"

BOOL COpcDrv::Config()//����
{
	
	m_bconfigModif = FALSE;

//	if(m_strProgID.IsEmpty())
	{
		CDlgOpcSet dlg;
		dlg.m_szMachine = m_strRemoteMachine;
		dlg.m_szProgID = m_strProgID;
		dlg.m_nTimeArea = m_nTimeArea;
		dlg.m_dwUpdateRate = m_dwUpdateRate;
		dlg.m_fPercentDead = m_fDeadband;
		dlg.m_nUpdateTimeout = m_nUpdateTimeout;
		
		if(IDOK != dlg.DoModal())
			return FALSE;
		m_strRemoteMachine = dlg.m_szMachine;
		m_strProgID = dlg.m_szProgID;
		m_dwUpdateRate=dlg.m_dwUpdateRate;
		m_fDeadband = dlg.m_fPercentDead;
		m_nTimeArea = dlg.m_nTimeArea;
		m_nUpdateTimeout = dlg.m_nUpdateTimeout;
	}
lpconnect:
	if(!m_OpcServer.IsConnected())
	{
		if(!m_OpcServer.Connect(m_strProgID,m_strRemoteMachine))
		{
			CString szErr;
			szErr.Format("����OPC������:Machine=<%s>,ProgID=<%s>ʧ��!",
				m_strRemoteMachine,m_strProgID);
			AfxMessageBox(szErr);
			CDlgOpcSet dlg;
			dlg.m_szMachine = m_strRemoteMachine;
			dlg.m_szProgID = m_strProgID;
			dlg.m_nTimeArea = m_nTimeArea;
			dlg.m_dwUpdateRate = m_dwUpdateRate;
			dlg.m_fPercentDead = m_fDeadband;
			if(IDOK != dlg.DoModal())
				return FALSE;
			m_strRemoteMachine = dlg.m_szMachine;
			m_strProgID = dlg.m_szProgID;
			m_dwUpdateRate=dlg.m_dwUpdateRate;
			m_fDeadband = dlg.m_fPercentDead;
			m_nTimeArea = dlg.m_nTimeArea;
			goto lpconnect;
		
		}
	}

	CDlgOpcVar dlgvar(m_OpcServer.GetProgID(),this->m_OpcGroup.GetIItemMgt(),m_OpcServer.GetIBrowse());
	dlgvar.m_piItemProp = m_OpcServer.GetIItemProps();//addby jy 20091221
	dlgvar.m_nReadMode = this->m_nReadMode;
	dlgvar.m_nOpcTime = m_nOpcTime;
	if(this->m_dwLanguageID == MAKELANGID (LANG_ENGLISH, SUBLANG_ENGLISH_US))
		dlgvar.m_nLangage = 0;
	else
		dlgvar.m_nLangage = 1;
	dlgvar.SetDrv(this);
//	dlgvar.m_sztitle.Format("OPC items����<%s>",lpszDecName);
	dlgvar.m_sztitle = "OPC items����";
	if(IDOK != dlgvar.DoModal())
		return FALSE;
	

	m_nReadMode = dlgvar.m_nReadMode;
	m_nOpcTime = dlgvar.m_nOpcTime;
	if(dlgvar.m_nLangage == 0)
		m_dwLanguageID = MAKELANGID (LANG_ENGLISH, SUBLANG_ENGLISH_US);
	else
		m_dwLanguageID = MAKELANGID (LANG_CHINESE,SUBLANG_CHINESE_SIMPLIFIED);//����
//	return TRUE;
//	if(dlgvar.IsModify())
//		return TRUE;

	m_OpcServer.Disconnect();
	return TRUE;
}
DRVRET COpcDrv::WriteVar(LPDRREC lprec)
{
	//needcode �ݲ�֧��д
	return 0; 
}
/*
DWORD COpcDrv::WriteVar(LPCTSTR lpszName,LPECDITEMVAL pval)
{

	CString szAddr=lpszName;

	CItemDef *pi = this->FindItem(szAddr);
	if(pi == NULL)
		return RESULT_NO_ITEM;				// ��������
	if((EcdApi::GetEcdDataAccess(pi->m_val.vt)) < 2)
		return RESULT_ITEM_READONLY;		// ֻ��
	if(!m_OpcServer.IsConnected())
		return RESULT_FAILURE;
	if(!m_OpcGroup.IsValid())
		return RESULT_FAILURE;
	int ecdtype=EcdApi::GetEcdDataType(pval->vt);
	int oletype=EcdApi::EcdDataType2OleDataType(ecdtype);
	CString szVal="";
	szVal=EcdApi::EcdItemVal2Str(pval);
	TRACE("COpcDrv::WriteVar:szVal = %s\n",szVal);
	m_OpcGroup.WriteOneSync(pi->m_item.GetServerHandle(),szVal,
		oletype);
	
	return RESULT_OK;
}*/

int COpcDrv::GetItemCount()
{
	CSafeLock lock(&m_Lock);//��
	return m_ItemArray.GetSize();
}

BOOL COpcDrv::GetItem(LPCTSTR lpszName,int &nType,int &nAccess,CString &szErr)
{
	CSafeLock lock(&m_Lock);//��
	int i,n = m_ItemArray.GetSize();
	CItemDef *pi;

	for(i=0;i<n;i++)
	{
		pi=m_ItemArray.GetAt(i);
		if(pi->m_szName == lpszName)
		{
			nType = pi->m_nType;
			nAccess = pi->m_nAccess;
			return TRUE;
		}
	}
	szErr.Format("%s<%s>",g_LoadString(IDS_ERRNOFIND),lpszName);//����:û���ҵ�
	return FALSE;
}
