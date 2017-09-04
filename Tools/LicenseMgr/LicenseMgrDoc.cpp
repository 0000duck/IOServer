
// LicenseMgrDoc.cpp : CLicenseMgrDoc ���ʵ��
//

#include "stdafx.h"
#include "LicenseMgr.h"
#include "MainFrm.h"
#include "ViewTree.h"
#include "FileView.h"
#include "LicenseMgrDoc.h"
#include "LicenseMgrView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLicenseMgrDoc

IMPLEMENT_DYNCREATE(CLicenseMgrDoc, CDocument)

BEGIN_MESSAGE_MAP(CLicenseMgrDoc, CDocument)
#ifdef _LICENSE_VIEW_APP
	ON_COMMAND(ID_FILE_IMPORT, &CLicenseMgrDoc::OnFileImport)
	ON_UPDATE_COMMAND_UI(ID_FILE_IMPORT, &CLicenseMgrDoc::OnUpdateFileImport)
	ON_COMMAND(ID_FILE_EXPORT, &CLicenseMgrDoc::OnFileExport)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT, &CLicenseMgrDoc::OnUpdateFileExport)
#endif
END_MESSAGE_MAP()


// CLicenseMgrDoc ����/����

CLicenseMgrDoc::CLicenseMgrDoc()
{
#ifdef _LICENSE_VIEW_APP
	m_szPathName = Utility::GetModulePath( AfxGetInstanceHandle() ).c_str();

	// �����ڵ�ǰĿ¼����
	CString szPathName = m_szPathName + _T("TPRILicense.lic");
	if (Utility::DirectoryExists(szPathName))
		m_szPathName = szPathName;
	else {
		// �ڵ�ǰĿ¼��License��Ŀ¼�в���
		szPathName = m_szPathName + _T("License\\TPRILicense.lic");

		if (Utility::DirectoryExists(szPathName))
			m_szPathName = szPathName;
		else {
			m_szPathName += _T("TPRILicense.lic");
		}
	}
#endif
}

CLicenseMgrDoc::~CLicenseMgrDoc()
{
}

CViewTree* CLicenseMgrDoc::GetTreeCtrl()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if (!pMainFrame)
		return NULL;

	return pMainFrame->GetTreeCtrl();
}

BOOL CLicenseMgrDoc::LoadXmlFromResoure(DWORD dwResID, CString &szXML)
{
	szXML.Empty();

	HINSTANCE hi = AfxGetResourceHandle( );
	HRSRC hRsrc = FindResource(hi, MAKEINTRESOURCE(dwResID), TEXT("XML"));
	if (NULL == hRsrc)
		return FALSE;

	//��ȡ��Դ�Ĵ�С
	int nsize = (int)SizeofResource(hi, hRsrc); 
	if (0 == nsize)		
		return FALSE;

	//������Դ
	HGLOBAL hGlobal = LoadResource(hi, hRsrc); 
	if (NULL == hGlobal)
		return FALSE;

	//������Դ
	LPVOID pBuffer = LockResource(hGlobal); 
	if (NULL == pBuffer)
	{
		FreeResource(hGlobal);
		return FALSE;
	}

	//_tcsncpy(szSQLScript.GetBuffer(nsize + 1), (LPCTSTR)pBuffer, nsize);
	//szSQLScript.ReleaseBuffer();

	char *ptr = new char[nsize + 1];
	memcpy(ptr, pBuffer, nsize);
	ptr[nsize] = '\0';
	szXML = ptr;
	delete[] ptr;

	FreeResource(hGlobal);
	return TRUE;
}

#ifndef _LICENSE_VIEW_APP
BOOL CLicenseMgrDoc::InitLicense(BOOL bSetValue/* = TRUE*/, DWORD dwXmlResoureID/* = 0*/)
{
	TiXmlDocument xmlDoc;

	if (dwXmlResoureID == 0)
	{
		string strFile = Utility::GetModulePath( AfxGetInstanceHandle() );
		strFile += "LicenseConfig.xml";
		xmlDoc.LoadFile(strFile);
	}
	else {
		CString szXML;
		if (!LoadXmlFromResoure(dwXmlResoureID, szXML))
			return FALSE;

		xmlDoc.Parse(szXML);
	}

	if (!xmlDoc.RootElement())
		return FALSE;

	TiXmlNode* pXmlNode = xmlDoc.FirstChild("license");
	if (!pXmlNode)
		return FALSE;

	pXmlNode = pXmlNode->FirstChild("software");
	while(pXmlNode)
	{
		TiXmlElement* pXmlElement = (TiXmlElement*)pXmlNode;

		CLicenseSoftware* pSoftware = theLicenseFile.GetLicenseSoftware( pXmlElement->Attribute("name"), true );

		if (pSoftware)
		{
			pSoftware->SetDescription( pXmlElement->Attribute("description") );
			InitSoftware(pXmlElement, pSoftware, bSetValue);

		}		

		pXmlNode = pXmlNode->NextSibling("software");
	}

	return TRUE;
}

BOOL CLicenseMgrDoc::InitSoftware(TiXmlElement* pXmlSoftware, CLicenseSoftware *pSoftware, BOOL bSetValue/* = TRUE*/)
{
	TiXmlNode* pPropertyNode = pXmlSoftware->FirstChild("property");
	while(pPropertyNode)
	{
		TiXmlElement* pPropertyElement = (TiXmlElement*)pPropertyNode;

		string strCategory = pPropertyElement->Attribute( "category" );
		string strName = pPropertyElement->Attribute( "name" );
		string strDisplayName = pPropertyElement->Attribute( "displayname" );
		string strDataType = pPropertyElement->Attribute( "datatype" );
		string strDefault = pPropertyElement->Attribute( "default" );
		string strDescr = pPropertyElement->Attribute( "description" );
		string strMin = pPropertyElement->Attribute( "min" );
		string strMax = pPropertyElement->Attribute( "max" );
		string strOptions = pPropertyElement->Attribute( "options" );

		if (strMin.empty() || strMax.empty())
		{
			strMin = "0";
			strMax = "0";
		}

		CLicenseProperty* pPty = pSoftware->AddProperty(strName.c_str(), strDisplayName.c_str(), strDataType.c_str(), bSetValue ? strDefault.c_str() : NULL, strDescr.c_str(), strCategory.c_str(),
			atoi(strMin.c_str()), atoi(strMax.c_str()), strOptions.c_str());

		pPropertyNode = pPropertyNode->NextSibling("property");
	}

	return TRUE;
}
#endif

void CLicenseMgrDoc::InitTreeView()
{
	CViewTree* pTreeCtrl = GetTreeCtrl();
	if (!pTreeCtrl)
		return;

	pTreeCtrl->DeleteAllItems();


#ifdef _LICENSE_VIEW_APP
	if (theLicenseFile.GetLicenseSoftwares().size() == 0)
		return;
#endif

	CString szPathName = GetPathName();
	if (szPathName.IsEmpty())
		szPathName = _T("<δ������Ȩ�ļ�>");
	else {
		int pos = szPathName.ReverseFind(_T('\\'));
		if (pos != -1)
			szPathName.Delete(0, pos + 1);
	}


	HTREEITEM hRoot = pTreeCtrl->InsertItem(szPathName, 2, 2);
	pTreeCtrl->SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	LPTREENODEDATA pNodeData = new TREENODEDATA;
	if (!pNodeData)
		return;
	pNodeData->nNodeType = NODE_LICENSE_FILE;
	pNodeData->szDescr[0] = '\0';

	pTreeCtrl->SetItemData(hRoot, reinterpret_cast<DWORD_PTR>(pNodeData));

	_LicenseSoftwaresArray &mapSoftwares = theLicenseFile.GetLicenseSoftwares();

	for (size_t i = 0; i < mapSoftwares.size(); i++)
	{
		HTREEITEM hTree = pTreeCtrl->InsertItem(mapSoftwares[i].GetSoftwareName(), 1, 1, hRoot);

		LPTREENODEDATA pNodeData = new TREENODEDATA;
		if (!pNodeData)
			continue;

		pNodeData->nNodeType = NODE_LICENSE_SOFTWARE;
		strcpy_s(pNodeData->szDescr, 80, mapSoftwares[i].GetDescription());
		pTreeCtrl->SetItemData(hTree, reinterpret_cast<DWORD_PTR>(pNodeData));
	}

	pTreeCtrl->Expand(hRoot, TVE_EXPAND);
}

BOOL CLicenseMgrDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	theLicenseFile.Clear();
#ifndef _LICENSE_VIEW_APP
	InitLicense();
#endif
	InitTreeView();
	SetModifiedFlag(FALSE);

	return TRUE;
}


BOOL CLicenseMgrDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
// 	if (!CDocument::OnOpenDocument(lpszPathName))
// 		return FALSE;
// 
// 	// TODO:  �ڴ������ר�õĴ�������
// 
// 	return TRUE;

	BOOL bOK = theLicenseFile.Load(lpszPathName);
#ifndef _LICENSE_VIEW_APP
	InitLicense(FALSE); // ����XML�����ļ����й����ԵĶ�����Ϣ������min/max/options��
#endif

	POSITION pos = GetFirstViewPosition();
	CLicenseMgrView *pView = (CLicenseMgrView *)GetNextView(pos);
	pView->RemoveAllLicensePropList();

	GetTreeCtrl()->DeleteAllItems();

	SetPathName(lpszPathName);
	InitTreeView();

	GetTreeCtrl()->SelectItem(GetTreeCtrl()->GetRootItem());

	SetModifiedFlag(FALSE);     // back to unmodified	

	return bOK;
}

BOOL CLicenseMgrDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	ENSURE(lpszPathName);

#ifndef _LICENSE_VIEW_APP

	theLicenseFile.Save(lpszPathName);	

	SetPathName(lpszPathName);

	SetModifiedFlag(FALSE);     // back to unmodified

	POSITION pos = GetFirstViewPosition();
	CLicenseMgrView *pView = (CLicenseMgrView *)GetNextView(pos);
	pView->UpdateCurSelLicensePropList();
#endif

	return TRUE;        // success

	//return CDocument::OnSaveDocument(lpszPathName);
}

void CLicenseMgrDoc::OnCloseDocument()
{
	POSITION pos = GetFirstViewPosition();
	CLicenseMgrView *pView = (CLicenseMgrView *)GetNextView(pos);

	pView->RemoveAllLicensePropList();
	GetTreeCtrl()->DeleteAllItems();

	CDocument::OnCloseDocument();
}


// CLicenseMgrDoc ���л�

void CLicenseMgrDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CLicenseMgrDoc ���

#ifdef _DEBUG
void CLicenseMgrDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLicenseMgrDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CLicenseMgrDoc ����
#ifdef _LICENSE_VIEW_APP

void CLicenseMgrDoc::OnFileImport()
{
	CString szPathName;
	
	CFileDialog dlgFile(TRUE, _T(".lic"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST, 
		_T("��Ȩ�ļ�(*.lic)|*.lic||"), NULL, 0);

	dlgFile.m_ofn.lpstrTitle = _T("������Ȩ�ļ�");
	dlgFile.m_ofn.lpstrFile = szPathName.GetBuffer(_MAX_PATH);

	INT_PTR nResult = dlgFile.DoModal();
	szPathName.ReleaseBuffer();
	
	if (nResult != IDOK)
		return;

	if (szPathName.CompareNoCase(GetPathName()) == 0)
	{
		AfxMessageBox(_T("��������Դ��Ȩ�ļ���������ѡ�����ļ���"), MB_OK | MB_ICONWARNING);
		return;
	}

	CString szMsg;
	CLicenseFile file;

	if ( !file.Load( szPathName ) )
	{
		szMsg.Format(_T("������Ȩ�ļ���%s��ʧ�ܣ����ܲ�����Ч����Ȩ�ļ���ʽ��"), szPathName);
		AfxMessageBox(szMsg, MB_OK | MB_ICONERROR);
		return;
	}

	if (!file.CheckMachineCode())
	{
		if (AfxMessageBox(_T("���ü��������Ч��Ȩ�ļ����Ƿ�ȷ��Ҫ���룡"), MB_YESNO | MB_ICONQUESTION) == IDNO)
			return;
	}
	else if (file.CheckExpiredTime() <= 0)
	{
		if (AfxMessageBox(_T("����Ȩ�ļ��Ѿ�����ʧЧ���Ƿ�ȷ��Ҫ���룡"), MB_YESNO | MB_ICONQUESTION) == IDNO)
			return;
	}

	if (!file.Save( GetPathName() ))
	{
		szMsg.Format(_T("������Ȩ�ļ���%s��ʧ�ܣ�"), GetPathName());
		AfxMessageBox(szMsg, MB_OK | MB_ICONERROR);
		return;
	}

	theLicenseFile = file;

	POSITION pos = GetFirstViewPosition();
	CLicenseMgrView *pView = (CLicenseMgrView *)GetNextView(pos);
	pView->RemoveAllLicensePropList();

	GetTreeCtrl()->DeleteAllItems();

	InitTreeView();

	GetTreeCtrl()->SelectItem(GetTreeCtrl()->GetRootItem());

	SetModifiedFlag(FALSE);

	szMsg.Format(_T("������Ȩ�ļ���%s���ɹ���"), szPathName);
	AfxMessageBox(szMsg, MB_OK | MB_ICONINFORMATION);
}

void CLicenseMgrDoc::OnUpdateFileImport(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
}

void CLicenseMgrDoc::OnFileExport()
{
	CString szPathName;

	CFileDialog dlgFile(FALSE, _T(".lic"), _T("TPRILicense"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		_T("��Ȩ�ļ�(*.lic)|*.lic||"), NULL, 0);

	dlgFile.m_ofn.lpstrTitle = _T("������Ȩ�ļ�");
	dlgFile.m_ofn.lpstrFile = szPathName.GetBuffer(_MAX_PATH);

	INT_PTR nResult = dlgFile.DoModal();
	szPathName.ReleaseBuffer();

	if (nResult != IDOK)
		return;

	if (szPathName.CompareNoCase(GetPathName()) == 0)
	{
		AfxMessageBox(_T("����������Դ��Ȩ�ļ���������ѡ�񵼳��ļ���"), MB_OK | MB_ICONWARNING);
		return;
	}

	theLicenseFile.Save( szPathName );	

	CString szMsg;
	szMsg.Format(_T("������Ȩ�ļ���%s���ɹ���"), szPathName);
	AfxMessageBox(szMsg, MB_OK | MB_ICONINFORMATION);
}

void CLicenseMgrDoc::OnUpdateFileExport(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (theLicenseFile.GetLicenseSoftwares().size() != 0) );
}

#endif