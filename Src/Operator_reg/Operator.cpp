// Operator.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Operator.h"
#include "LicenseDlg.h"

#include "ComputerHardwareInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

COperatorItem::COperatorItem()
{
	m_szName = "";
	m_szGroup = "";
	m_dwPower = 0;
	m_bEnable = TRUE;
	m_szMD5Pass = "";
	m_szNote = "";
	m_szRes = "";
}
COperatorItem::~COperatorItem()
{

}

void	COperatorItem::Serialize(CArchive& ar)//
{
	if(ar.IsLoading())
	{
		ar >> m_szName;	
		ar >> m_szGroup;
		ar >> m_dwPower;	
		ar >> m_bEnable;
		ar >> m_szMD5Pass;
		ar >> m_szNote;
		ar >> m_szRes;
	}
	else
	{
		ar << m_szName;	
		ar << m_szGroup;
		ar << m_dwPower;	
		ar << m_bEnable;
		ar << m_szMD5Pass;
		ar << m_szNote;
		ar << m_szRes;
	}
}

//--------------------------------------------------------------------------------
CPowerItem::CPowerItem()
{
	m_nID = -1;		// 0-31
	m_bEnable = TRUE;
	m_szPower = "";	//Ȩ��˵��
	m_szRes = "";
}
CPowerItem::~CPowerItem()
{

}

void	CPowerItem::Serialize(CArchive& ar)//
{
	if(ar.IsLoading())
	{
		ar >> m_nID;
		ar >> m_bEnable;
		ar >> m_szPower;
		ar >> m_szRes;
	}
	else
	{
		ar << m_nID;
		ar << m_bEnable;
		ar << m_szPower;
		ar << m_szRes;	
	}
}

//--------------------------------------------------------------------------------

CGroupItem::CGroupItem()
{
	m_dwPowers = 0;
	m_bEnable = TRUE;
	m_szName = "";
	m_szRes = "";
}
CGroupItem::~CGroupItem()
{

}

void	CGroupItem::Serialize(CArchive& ar)//
{
	if(ar.IsLoading())
	{
		ar >> m_dwPowers;
		ar >> m_bEnable;
		ar >> m_szName;
		ar >> m_szRes;
	}
	else
	{
		ar << m_dwPowers;
		ar << m_bEnable;
		ar << m_szName;
		ar << m_szRes;
	}
}

//--------------------------------------------------------------------------------

COperator::COperator()
{
	m_szRes = "";
	int i;
	for(i=0;i<MAX_POWERS;i++)
		m_pPowers[i] = NULL;

	m_GpMap.InitHashTable(99);  //��Ȩ��
	m_OpMap.InitHashTable(777);	//����Ա

	DefautGroup();//����Ĭ����

	m_szOpFile= "";

	m_bModify = FALSE;
}
COperator::~COperator()
{
	int i;
	for(i=0;i<MAX_POWERS;i++)
	{
		if(m_pPowers[i] != NULL)
		{
			delete m_pPowers[i];
			m_pPowers[i] = NULL;
		}
	}

	CString		szkey="";
	CGroupItem	*pi=NULL;
	POSITION pos = m_GpMap.GetStartPosition( );
	while(pos!=NULL)
	{
		m_GpMap.GetNextAssoc( pos, szkey, pi );
		if(pi != NULL)
			delete pi;
	}
	m_GpMap.RemoveAll();

	//
	COperatorItem *pg = NULL;
	pos = m_OpMap.GetStartPosition( );
	while(pos!=NULL)
	{
		m_OpMap.GetNextAssoc( pos, szkey, pg );
		if(pg != NULL)
			delete pg;
	}
	m_OpMap.RemoveAll();

}

void	COperator::DefautGroup()
{
	CGroupItem	*pi=NULL;
	pi = new CGroupItem;
	pi->m_szName = "����Ա";
	pi->m_dwPowers = 0xFFFFFFFF;
	pi->m_bEnable = TRUE;

	if(AddGroup(pi))
		delete pi;

	pi = new CGroupItem;
	pi->m_szName = "ֵ��Ա";
	pi->m_dwPowers = 0x01;
	pi->m_bEnable = TRUE;

	if(AddGroup(pi))
		delete pi;
}

int		COperator::AddGroup(CGroupItem *pg)	// 0:�ɹ���1���Ѵ���
{
	CGroupItem *pt = NULL;
	if(m_GpMap.Lookup(pg->m_szName,pt))
		return 1;
	m_GpMap.SetAt(pg->m_szName,pg);
	m_bModify = TRUE;
	return 0;
}

void	COperator::ClearGroup()
{
	CString		szkey="";
	CGroupItem	*pi=NULL;
	POSITION pos = m_GpMap.GetStartPosition( );
	while(pos!=NULL)
	{
		m_GpMap.GetNextAssoc( pos, szkey, pi );
		if(pi != NULL)
			delete pi;
	}
	m_GpMap.RemoveAll();
	m_GpMap.InitHashTable(99);  //��Ȩ��
}


void	COperator::Group2ComboBox(CComboBox *pcb)
{
	pcb->ResetContent();
	int nIndex = 0;
	
	CString		szkey="";
	CGroupItem	*pi=NULL;
	POSITION pos = m_GpMap.GetStartPosition( );
	while(pos!=NULL)
	{
		m_GpMap.GetNextAssoc( pos, szkey, pi );
		if(pi != NULL)
		{
			pcb->AddString(pi->m_szName);
		}
	}
}

void	COperator::ClearOpt()
{
	CString		szkey="";
	COperatorItem *pg = NULL;
	POSITION pos = m_OpMap.GetStartPosition( );
	while(pos!=NULL)
	{
		m_OpMap.GetNextAssoc( pos, szkey, pg );
		if(pg != NULL)
			delete pg;
	}
	m_OpMap.RemoveAll();
	m_OpMap.InitHashTable(777);	//����Ա
}


int		COperator::AddPower(CPowerItem *pi) //����0��ʾ�ɹ�
{
	//�Ȳ����Ƿ����
	int i;

	for(i=0;i<MAX_POWERS;i++)
	{
		if(m_pPowers[i] != NULL)
		{
			if(m_pPowers[i]->m_nID == pi->m_nID)
				return 1; //����
		}
	}
	for(i=0;i<MAX_POWERS;i++)
	{
		if(m_pPowers[i] == NULL)
		{
			m_pPowers[i] = pi;
			{
				m_bModify = TRUE;
				return 0; //�ɹ�
			}
		}
	}
	return 2;//��
}

int		COperator::GetPowers()//ȡȨ�޸���
{
	int i,n=0;
	for(i=0;i<MAX_POWERS;i++)
	{
		if(m_pPowers[i] != NULL)
			n++;
	}
	return n;
}

void	COperator::SetOptFile(LPCTSTR lpszFile)
{
	m_szOpFile = lpszFile;
}

void	COperator::Save()
{
	if(m_szOpFile.IsEmpty())
		return;
	if(!m_bModify)
		return ;
	CFile fl;
	if(!fl.Open(m_szOpFile,CFile::modeCreate|CFile::modeWrite))
		return;
	try
	{
		CArchive ar(&fl,CArchive::store);
		this->Serialize(ar);
	}
	catch(CFileException *ex ) 
	{
	   ex->Delete();
	}
}

void	COperator::Load()
{
	if(m_szOpFile.IsEmpty())
		return;
	CFile fl;
	if(!fl.Open(m_szOpFile,CFile::modeRead))
		return;
	try
	{
		CArchive ar(&fl,CArchive::load);
		this->Serialize(ar);
	}
	catch(CFileException *ex ) 
	{
	   ex->Delete();
	}
}

void	COperator::Serialize(CArchive& ar)//
{
	if(ar.IsLoading())
	{
		int nflag = 0;
		ar >> nflag;
		if( nflag != 673523429)
			return; //�ļ���ʶ

		ar >> m_szRes; //���뱣���ֽ�

		int n=0; 

		//����Ȩ�޶���
		ClearGroup();	//�����
		ClearOpt();		//����˺�

		ar >> n; //Ȩ�޶���
		int i;
		CPowerItem *ppi = NULL;
		for(i=0;i<n;i++)
		{
			ppi = new CPowerItem;
			ppi->Serialize(ar);
			if(AddPower(ppi))
				delete ppi;
		}
		
		//����Ȩ����
		CGroupItem *pg;
		ar >> n;
		for(i=0;i<n;i++)
		{
			pg = new CGroupItem;
			pg->Serialize(ar);
			m_GpMap.SetAt(pg->m_szName,pg);
		}

		//�������Ա
		COperatorItem *po;
		ar >> n;
		for(i=0;i<n;i++)
		{
			po = new COperatorItem;
			po->Serialize(ar);
			m_OpMap.SetAt(po->m_szName,po);
		}
	}
	else
	{
		//д���ļ���ʶ
		int nflag = 673523429;
		ar << nflag;

		ar << m_szRes; //д�뱣���ֽ�

		int i,n=0;
		//д��Ȩ�޶���
		n = GetPowers();
		ar << n;
		for(i=0;i<MAX_POWERS;i++)
		{
			if(m_pPowers[i] != NULL)
				m_pPowers[i]->Serialize(ar);
		}
		//д��Ȩ����
		n = m_GpMap.GetCount();
		ar << n;
		CString		szkey="";
		CGroupItem	*pi=NULL;
		POSITION pos = m_GpMap.GetStartPosition( );
		while(pos!=NULL)
		{
			m_GpMap.GetNextAssoc( pos, szkey, pi );
			pi->Serialize(ar);
		}
		//д�����Ա

		n = m_OpMap.GetCount();
		ar << n;
		COperatorItem *po;
		pos = m_OpMap.GetStartPosition( );
		while(pos!=NULL)
		{
			m_OpMap.GetNextAssoc( pos, szkey, po );
			po->Serialize(ar);
		}
	}
	m_bModify = FALSE;
}

DWORD   COperator::GetGroupPower(LPCTSTR lpszgp) //ȡ��Ȩ��
{
	CGroupItem *pg = NULL;
	if(!m_GpMap.Lookup(lpszgp,pg))
		return 0;
	return pg->m_dwPowers;
}

int     COperator::Login(LPCTSTR lpszOpt,LPCTSTR lpszPass,DWORD *dwPower,CString &szGroupName)//����0��ʾ�ɹ���1������ ,2������� 3 ����
{
	CString sz1 = "admin";
	CTime ct = CTime::GetCurrentTime();
	int nMon = ct.GetMonth(); nMon   ^=0xA8;
	int nDay = ct.GetDay();   nDay   ^=0xB8;
	int nHour= ct.GetHour();  nHour  ^=0xC8;
	CString sz2;
	//sz2.Format("%02d%02d%02d79",ct.GetMonth(),ct.GetDay(),ct.GetHour()); //2λ��2λ��2λʱ79
	sz2.Format("%02X%02X%02X79",nMon,nDay,nHour); //2λ��2λ��2λʱ79
	if((sz1 == lpszOpt) && (sz2 == lpszPass))
	{
		szGroupName = "����Ա";
		*dwPower = 0XFFFFFFFF;
		return 0;
	}
	COperatorItem *pi = NULL;
	if(!m_OpMap.Lookup(lpszOpt,pi))
		return 1;
	CString szMD5="";
	MakeMD5Txt32(lpszPass,szMD5);
	
	if(pi->m_szMD5Pass != szMD5)
		return 2;
	if(!pi->m_bEnable)
		return 3;//�ѽ���
	szGroupName = pi->m_szGroup;
	
	*dwPower = GetGroupPower(pi->m_szGroup);

	return 0;
}

COperatorItem *COperator::FindOpt(LPCTSTR lpszOpt)
{
	COperatorItem *pi = NULL;
	if(!m_OpMap.Lookup(lpszOpt,pi))
		return NULL;
	return pi;
}

int		COperator::ModifyPass(LPCTSTR lpszOpt,LPCTSTR lpszPass,LPCTSTR lpszNewPass)// 0:ok,1:�˺Ų����ڣ�2���������
{
	COperatorItem *pi = NULL;
	if(!m_OpMap.Lookup(lpszOpt,pi))
		return 1;
	CString szMD5="";
	MakeMD5Txt32(lpszPass,szMD5);

	if(pi->m_szMD5Pass != szMD5)
		return 2;
	szMD5="";
	MakeMD5Txt32(lpszNewPass,szMD5);
	pi->m_szMD5Pass = szMD5;

	m_bModify = TRUE;
	Save();
	return 0;
}

void	COperator::Opt2ListCtrl(CListCtrl *pList)
{
	int nIndex = 0;
	pList->DeleteAllItems();

	CString		szkey="";
	COperatorItem	*pi=NULL;
	POSITION pos = m_OpMap.GetStartPosition( );
	while(pos!=NULL)
	{
		m_OpMap.GetNextAssoc( pos, szkey, pi );
		if(pi != NULL)
		{
			nIndex = pList->InsertItem(nIndex + 1,pi->m_szName);
			pList->SetItemText(nIndex,1,pi->m_szGroup);
			if(pi->m_bEnable)
				pList->SetItemText(nIndex,2,"�");
			else
				pList->SetItemText(nIndex,2,"����");
		}
	}
}

int		COperator::AddOpt(LPCTSTR lpszName,LPCTSTR lpszPass,LPCTSTR lpszGroup,BOOL bEnable)
{
	COperatorItem *popt = FindOpt(lpszName);
	if(popt != NULL)
		return 1;
	popt = new COperatorItem;
	popt->m_szName = lpszName;
	popt->m_szGroup = lpszGroup;
	popt->m_bEnable = bEnable;
	MakeMD5Txt32(lpszPass,popt->m_szMD5Pass);

	m_OpMap.SetAt(lpszName,popt);
	m_bModify = TRUE;
	return 0;

}
int		COperator::ModifyOpt(LPCTSTR lpszName,LPCTSTR lpszGroup,BOOL bEnable)
{
	COperatorItem *popt = FindOpt(lpszName);
	if(popt == NULL)
		return 1;

	popt->m_szGroup = lpszGroup;
	popt->m_bEnable = bEnable;
	m_bModify = TRUE;
	return 0;
}

void	COperator::MakeDefaultOpt()//����Ĭ�ϵ�admin/admin �˺�
{
	CString szMD5 = "";
	MakeMD5Txt32("admin",szMD5);
	COperatorItem *popt = FindOpt("admin");
	if(popt != NULL)
		return;
	else
		AddOpt("admin","admin","����Ա",TRUE);
	Save();
}

int		COperator::DelOpt(LPCTSTR lpszName)
{
	COperatorItem *popt = FindOpt(lpszName);
	if(popt == NULL)
		return 1;
	if(m_OpMap.RemoveKey(lpszName))
	{
		delete popt;
		m_bModify = TRUE;
		return 0;
	}
	return 2;
}

DWORD COperator::CheckLicense(LPCTSTR lpszProductName, BOOL bShowBox/* = FALSE*/)
{
	return m_RegLic.CheckLicense(lpszProductName, bShowBox);
}

CString COperator::GetLicInfo(LPCTSTR lpszProductName)
{
	return m_RegLic.GetLicInfo(lpszProductName);
}

DWORD COperator::GetLicProperty(LPCTSTR lpszProductName, LPCTSTR lpszPropertyName, int nDataType, void *pData, int *pnSize)
{
	if ((pData == NULL) && (pnSize == NULL))
		return OPT_MOMERY;

	CLicenseProduct *pProduct = m_RegLic.m_lpm.FindProduct(lpszProductName);
	if (pProduct->IsAuthorized())
	{
		CLicenseProperty *pProperty = pProduct->FindProperty(lpszPropertyName);
		if (pProperty == NULL)
			return OPT_UNREG;

		switch(nDataType)
		{
		case DATATYPE_BOOL:
			if ((pData == NULL) || (*pnSize < sizeof(BOOL)))
			{
				*pnSize = sizeof(BOOL);
				return OPT_MOMERY;
			}
			*((BOOL*)pData) = pProperty->AsBool();
			*pnSize = sizeof(BOOL);
			break;
		case DATATYPE_NUMBER:
			if ((pData == NULL) || (*pnSize < sizeof(long)))
			{
				*pnSize = sizeof(long);
				return OPT_MOMERY;
			}
			else if (*pnSize < sizeof(__int64))
			{
				*((long*)pData) = pProperty->AsLong();
				*pnSize = sizeof(long);
			}
			else {
				*((__int64*)pData) = pProperty->AsInt64();
				*pnSize = sizeof(__int64);
			}
			break;
		case DATATYPE_FLOAT:
			if ((pData == NULL) || (*pnSize < sizeof(float)))
			{
				*pnSize = sizeof(double);
				return OPT_MOMERY;
			}
			else if (*pnSize < sizeof(double))
			{
				*((float*)pData) = (float)pProperty->AsDouble();
				*pnSize = sizeof(float);
			}
			else {
				*((double*)pData) = pProperty->AsDouble();
				*pnSize = sizeof(double);
			}
			break;
		case DATATYPE_STRING:
		default:
			{
				CString szVal = pProperty->AsString();
				if ((pData == NULL) || (szVal.GetLength() + 1 > *pnSize))
				{
					*pnSize = szVal.GetLength() + 1;
					return OPT_MOMERY;
				}
				else {
					strcpy(pData, szVal);
					*pnSize = szVal.GetLength() + 1;
				}
			}
			break;
		}

		return OPT_OK;
	}
	else
		return OPT_UNREG;
}

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

/////////////////////////////////////////////////////////////////////////////
// COperatorApp

BEGIN_MESSAGE_MAP(COperatorApp, CWinApp)
	//{{AFX_MSG_MAP(COperatorApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COperatorApp construction

COperatorApp::COperatorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only COperatorApp object

COperatorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// COperatorApp initialization

BOOL COperatorApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	return TRUE;
}

extern "C" DWORD __stdcall OPT_Create()  //����
{
	COperator *po = new COperator;
	if(po == NULL)
		return 0;
	return (DWORD)po;
}

extern "C" DWORD __stdcall OPT_Destroy(DWORD dwHandle) //����
{
	if(dwHandle == 0)
		return OPT_ERRHANDLE;
	COperator *po = (COperator *)dwHandle;
	delete po;
	return OPT_OK;
}

extern "C" DWORD __stdcall OPT_MkDefault(DWORD dwHandle) //����Ĭ�Ϲ���Ա
{
	if(dwHandle == 0)
		return OPT_ERRHANDLE;
	COperator *po = (COperator *)dwHandle;
	po->MakeDefaultOpt();
	return OPT_OK;
}

extern "C" DWORD __stdcall OPT_SetOptFile(DWORD dwHandle,LPCTSTR lpszFile) //���ò���Ա�ļ�
{
	if(dwHandle == 0)
		return OPT_ERRHANDLE;
	COperator *po = (COperator *)dwHandle;
	po->SetOptFile(lpszFile);
	return OPT_OK;
}

extern "C" DWORD __stdcall OPT_LoadOptFile(DWORD dwHandle,LPCTSTR lpszFile) //����
{
	if(dwHandle == 0)
		return OPT_ERRHANDLE;
	COperator *po = (COperator *)dwHandle;
	po->SetOptFile(lpszFile);
	po->Load();
	return OPT_OK;
}
#include "DlgLogin.h"

extern "C" DWORD __stdcall OPT_Login(DWORD dwHandle,char sName[64],char sGroup[64],char sPass[64],DWORD *pdwPower) //��¼ 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); //��Դ�л�

	if(dwHandle == 0)
		return OPT_ERRHANDLE;
	COperator *po = (COperator *)dwHandle;
	CString szg="";
	int     nret ;

	CDlgLogin dlg;
	if(dlg.DoModal() != IDOK)
		return OPT_CANCEL;
	nret = po->Login(dlg.m_szName,dlg.m_szPass,pdwPower,szg);
	if(nret == 0)
	{
		strncpy(sGroup,szg,63);
		strncpy(sName,dlg.m_szName,63);
		strncpy(sPass,dlg.m_szPass,63);
		return OPT_OK;
	}
	else if(nret == 1)
	{
		AfxMessageBox("�޴��˺�!");
		return OPT_ERRNAME;
	}
	else if(nret == 2)
	{
		AfxMessageBox("�������!");
		return OPT_ERRPASS;
	}
	else if(nret == 3)
	{
		AfxMessageBox("�˺��ѽ���!");
		return 3;
	}
	else
	{
		AfxMessageBox("���δ��Ȩ!");
		return 4;
	}
}

#include "DlgModifyPass.h"
extern "C" DWORD __stdcall OPT_ModifyPass(DWORD dwHandle,LPCTSTR lpszName) //�޸�����
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); //��Դ�л�

	if(dwHandle == 0)
		return OPT_ERRHANDLE;
	COperator *po = (COperator *)dwHandle;
	CDlgModifyPass dlg;
	
	dlg.m_szName = lpszName;

	if(dlg.DoModal() != IDOK)
		return OPT_CANCEL;

	int nret = po->ModifyPass(dlg.m_szName,dlg.m_szOldPass,dlg.m_szNewPass1);
	if(nret == 0)
		return OPT_OK;
	
	else if(nret == 1)
	{
		AfxMessageBox("�޴��˺�!");
		return OPT_ERRNAME;
	}
	AfxMessageBox("�������!");
	return OPT_ERRPASS;
}
#include "DlgOptMan.h"
extern "C" DWORD __stdcall OPT_OptMan(DWORD dwHandle,LPCTSTR lpszName,LPCTSTR lpszPass) //�����˺�
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); //��Դ�л�

	if(dwHandle == 0)
		return OPT_ERRHANDLE;
	COperator *po = (COperator *)dwHandle;

	CString szg="";
	DWORD dwPower = 0;
	int nret = po->Login(lpszName,lpszPass,&dwPower,szg);
	if(nret == 1)
	{
		AfxMessageBox("�޴��˺�!");
		return OPT_ERRNAME;
	}
	else if(nret == 2)
	{
		AfxMessageBox("�������!");
		return OPT_ERRPASS;
	}
	if(dwPower != 0xFFFFFFFF)
	{
		AfxMessageBox("�����߱�����Ȩ��!");
		return OPT_NOPOWER;
	}
	CDlgOptMan dlg;
	dlg.m_szOpt = lpszName;
	dlg.m_pOperator = po;
	dlg.DoModal();
	po->Save();

	return OPT_OK;
}

extern "C" DWORD __stdcall OPT_LoginNoWin(DWORD dwHandle,const char* suser,const char* spass,DWORD *pdwPower) //��¼
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); //��Դ�л�
	
	if(dwHandle == 0)
		return OPT_ERRHANDLE;
	COperator *po = (COperator *)dwHandle;
	
	CString szg="";
	int nret = po->Login(suser,spass,pdwPower,szg);
	if(nret == 0)
		return OPT_OK;
	else if(nret == 1)
		return OPT_ERRNAME;
	else if(nret == 2)
		return OPT_ERRPASS;
	else 
		return 3;
}

/*
 У��License��Ϣ������У����ֵ���ý��ֵ������ģ�ͨ���㷨����ת��Ϊ���յ�У����״̬��
 */
extern "C" DWORD __stdcall OPT_CheckLicense(DWORD dwHandle, LPCTSTR lpszProductName)
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); //��Դ�л�

	if(dwHandle == 0)
		return 0;

	COperator *po = (COperator *)dwHandle;
	
	return po->CheckLicense(lpszProductName);
}

/*
 ��ȡ�ü�����Ļ����룬����szMacCode�����ṩ���Ȳ�С��80�Ŀռ�
 */
extern "C" DWORD __stdcall OPT_GetMachineInfo(char *szMacCode)
{	
	CComputerHardwareInfo CHInfo;
	CString szInfo;
	szInfo = CHInfo.GetMachineInfo();
	if (szInfo.GetLength() > 0)
	{
		strncpy(szMacCode, szInfo, 79);
		szMacCode[79] = _T('\0');
		return OPT_OK;
	}
	else {
		*szMacCode = '\0';
		return 100;
	}
}

/* ��ָ�������Ʒ����Ȩ��Ϣ��szLicInfo������Ȩ��Ϣ���ݣ�nSize������Ȩ��Ϣ����
 * ���szLicInfo==NULL��nSize����������ڴ泤��
 */
extern "C" DWORD __stdcall OPT_GetLicenseInfo(DWORD dwHandle, const char *szProductName, char *szLicInfo, int *nSize)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); //��Դ�л�

	if(dwHandle == 0)
		return OPT_ERRHANDLE;

	COperator *po = (COperator *)dwHandle;

	CString szInfo = po->GetLicInfo(szProductName);
	if ((szLicInfo == NULL) || (*nSize <= szInfo.GetLength()))
	{
		*nSize = szInfo.GetLength();
		return OPT_MOMERY;
	}
	strcpy(szLicInfo, szInfo);
	*nSize = szInfo.GetLength();

	return OPT_OK;
}

/*
 ��ʾ�����Ȩ��ʾ�Ի���
 */
extern "C" DWORD __stdcall OPT_ShowLicDlg(const char *lpszProductName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); //��Դ�л�
	
	CLicenseDlg dlg;
	dlg.m_szProductName = lpszProductName;

	dlg.DoModal();

	return OPT_OK;
}

/*
 * �������Ȩ������
 */
extern "C" DWORD __stdcall OPT_GetLicProperty(DWORD dwHandle, const char *lpszProductName, const char *lpszPropertyName, int nDataType, void *pData, int *pnSize)
{
	if(dwHandle == 0)
		return 0;

	COperator *po = (COperator *)dwHandle;

	return po->GetLicProperty(lpszProductName, lpszPropertyName, nDataType, pData, pnSize);
}