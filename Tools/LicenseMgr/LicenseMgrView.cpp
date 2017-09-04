
// LicenseMgrView.cpp : CLicenseMgrView ���ʵ��
//

#include "stdafx.h"
#include "LicenseMgr.h"

#include "LicenseMgrDoc.h"
#include "LicenseMgrView.h"
#include "CustomGridProperty.h"
#include "ViewTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLicenseMgrView

IMPLEMENT_DYNCREATE(CLicenseMgrView, CView)

BEGIN_MESSAGE_MAP(CLicenseMgrView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

// CLicenseMgrView ����/����

CLicenseMgrView::CLicenseMgrView()
{
#ifdef _LICENSE_VIEW_APP
	m_bFirstOpenView = TRUE;
#endif
}

CLicenseMgrView::~CLicenseMgrView()
{
}

void CLicenseMgrView::UpdateCurSelLicensePropList()
{
	CViewTree *pTreeCtrl = GetDocument()->GetTreeCtrl();

	if (pTreeCtrl->GetRootItem() == NULL)
	{
		m_wndPropList.RemoveAll();
		return;
	}

	CString szPathName = GetDocument()->GetPathName();
	if (szPathName.IsEmpty())
		szPathName = _T("<δ������Ȩ�ļ�>");
	else {
		int pos = szPathName.ReverseFind(_T('\\'));
		if (pos != -1)
			szPathName.Delete(0, pos + 1);
	}

	pTreeCtrl->SetItemText(pTreeCtrl->GetRootItem(), szPathName);

	if (!pTreeCtrl->GetSelectedItem())
	{
		pTreeCtrl->SelectItem( pTreeCtrl->GetRootItem() );
	}
	else {
		if (m_szSoftwareName.IsEmpty())
			UpdateLicensePropList(NULL);
		else
			UpdateLicensePropList(m_szSoftwareName);
	}
}

BOOL CLicenseMgrView::UpdateLicensePropList(LPCTSTR lpszSoftwareName/* = NULL*/)
{
	if (!m_wndPropList.GetSafeHwnd())
		return TRUE;

	m_wndPropList.RemoveAll();

	if (!lpszSoftwareName)
	{
		m_szSoftwareName.Empty();

		CTPRIPropertyGridProperty *pGroup = new CTPRIPropertyGridProperty(_T("������Ϣ"));
		// �ļ���
		CString szPathName = GetDocument()->GetPathName();
		if (szPathName.IsEmpty())
			szPathName = _T("<δ������Ȩ�ļ�>");

		CTPRIPropertyGridProperty* pProp = new CTPRIPropertyGridProperty(_T("��Ȩ�ļ���"), (_variant_t)szPathName, _T("��ǰ��Ȩ�ļ�������ļ���"));
		pProp->AllowEdit(FALSE);
		pGroup->AddSubItem(pProp);

#ifdef _LICENSE_VIEW_APP
		CString szAuthMsg;
		BOOL bMC = theLicenseFile.CheckMachineCode();
		BOOL bTime = (theLicenseFile.CheckExpiredTime() > 0);
		if (bMC && bTime)
		{
			szAuthMsg = _T("��Ȩ��Ч");
		}
		else {
			if (!bMC)
				szAuthMsg = _T("�Ǳ��������Ȩ");
			else
				szAuthMsg = _T("��Ȩ����");
		}

		pProp = new CTPRIPropertyGridProperty(_T("��Ȩ״̬"), (_variant_t)szAuthMsg, _T("��ǰ��Ȩ�ļ��Ա����������Ȩ״̬����Ϊ��Ȩ��Ч���Ǳ��������Ȩ����Ȩ�����������"));
		pProp->AllowEdit(FALSE);
		pGroup->AddSubItem(pProp);
#endif

		m_wndPropList.AddProperty(pGroup);

		pGroup = new CTPRIPropertyGridProperty(_T("��Ȩ��Ϣ"));

		// ��Ȩ����
		pProp = new CTPRIPropertyGridProperty(_T("��Ȩ����"), (_variant_t)theLicenseFile.GetUserName(), _T("��Ȩ��������"));
#ifdef _LICENSE_VIEW_APP
		pProp->AllowEdit(FALSE);
#endif
		pGroup->AddSubItem( pProp );

		// ������
		pProp = new CTPRIPropertyGridMachineCodeProperty(_T("��Ȩ������"), (_variant_t)theLicenseFile.GetMachineCode(), _T("��Ȩ������Ļ�����"));
#ifdef _LICENSE_VIEW_APP
		pProp->AllowEdit(FALSE);
#endif
		pGroup->AddSubItem( pProp );

		// �Ƿ���Ȩ����ʱ��
#ifdef _LICENSE_VIEW_APP
		CString szBoolText = theLicenseFile.IsExpiredTime() ? _T("True") : _T("False");
		pProp = new CTPRIPropertyGridProperty(_T("������Ȩ����"), (_variant_t)szBoolText, 
			_T("�Ƿ�������Ȩ����������Ч�����ΪTRUE���򵽴�ָ����Ȩ�������ں���Ȩ�ļ���ʧЧ"));
		pProp->AllowEdit(FALSE);
#else
		pProp = new CTPRIPropertyGridProperty(_T("������Ȩ����"), (_variant_t)theLicenseFile.IsExpiredTime(), 
			_T("�Ƿ�������Ȩ����������Ч�����ΪTRUE���򵽴�ָ����Ȩ�������ں���Ȩ�ļ���ʧЧ"));
#endif
		pGroup->AddSubItem( pProp );

		// ��Ȩ����ʱ��
#ifdef _LICENSE_VIEW_APP
		CTime tt(theLicenseFile.GetExpiredTime());
		pProp = new CTPRIPropertyGridProperty(_T("��Ȩ��������"), (_variant_t)tt.Format(_T("%Y-%m-%d")), _T("��Ȩ������Ļ�����"), 0);
		pProp->AllowEdit(FALSE);
#else
		pProp = new CTPRIPropertyGridDateProperty(_T("��Ȩ��������"), theLicenseFile.GetExpiredTime(), _T("��Ȩ������Ļ�����"), 0);
#endif
		pGroup->AddSubItem( pProp );

		m_wndPropList.AddProperty(pGroup);
	}
	else {  // �����
		m_szSoftwareName = lpszSoftwareName;

		CLicenseSoftware *pSoftware = theLicenseFile.GetLicenseSoftware(lpszSoftwareName);
		if (!pSoftware)
			return FALSE;

		CTPRIPropertyGridProperty *pGroup;
		CTPRIPropertyGridProperty* pProp;
		vector<CTPRIPropertyGridProperty *> arrGroup;
		string strOptions;

		_LicPropertiesArray	&arrProperties = pSoftware->GetProperties();
		for (size_t i = 0; i < arrProperties.size(); i++)
		{
			CLicenseProperty &pty = arrProperties[i];

			switch(pty.GetDataType())
			{
			case LIC_DT_BOOL:
#ifdef _LICENSE_VIEW_APP
				pProp = new CTPRIPropertyGridProperty(pty.GetDisplayName(), (_variant_t)(pty.AsBool()?_T("True"):_T("False")), pty.GetDescription());
#else
				pProp = new CTPRIPropertyGridProperty(pty.GetDisplayName(), (_variant_t)pty.AsBool(), pty.GetDescription());
#endif
				break;
			case LIC_DT_NUMBER:
				pProp = new CTPRIPropertyGridProperty(pty.GetDisplayName(), (_variant_t)pty.AsLong(), pty.GetDescription());
#ifndef _LICENSE_VIEW_APP
				if (pty.GetMinValue() < pty.GetMaxValue())
				{
					pProp->EnableSpinControl(TRUE, pty.GetMinValue(), pty.GetMaxValue());
				}
#endif
				break;
			case LIC_DT_FLOAT:
				pProp = new CTPRIPropertyGridProperty(pty.GetDisplayName(), (_variant_t)pty.AsDouble(), pty.GetDescription());
				break;
			case LIC_DT_DATE:
#ifdef _LICENSE_VIEW_APP
				{
					CTime tt( pty.AsLong() );
					pProp = new CTPRIPropertyGridProperty(pty.GetDisplayName(), tt.Format(_T("%Y-%m-%d")), pty.GetDescription());
				}
#else
				pProp = new CTPRIPropertyGridDateProperty(pty.GetDisplayName(), pty.AsLong(), pty.GetDescription());
#endif
				break;
			//case LIC_DT_STRING:
			default:
				pProp = new CTPRIPropertyGridProperty(pty.GetDisplayName(), (_variant_t)pty.AsString(), pty.GetDescription());
#ifndef _LICENSE_VIEW_APP
				strOptions = pty.GetOptions();
				if (!strOptions.empty())
				{
					vector<string> arrOptions = stl_string::split(strOptions, ';');
					for (size_t i = 0; i < arrOptions.size(); i++)
					{
						pProp->AddOption( arrOptions[i].c_str() );						
					}
					pProp->AllowEdit(FALSE);
				}
#endif
				break;
			}
#ifdef _LICENSE_VIEW_APP
			pProp->AllowEdit(FALSE);
#endif
			pProp->setPropertyName( pty.GetName() );

			CString strCategory = pty.GetCategory();
			if (strCategory.IsEmpty())
				arrGroup.push_back( pProp );
			else {
				BOOL bFind = FALSE;
				for (size_t i = 0; i < arrGroup.size(); i++)
				{
					if (strCategory.Compare(arrGroup[i]->GetName()) == 0)
					{
						arrGroup[i]->AddSubItem(pProp);
						bFind = TRUE;
						break;
					}
				}

				if (!bFind) {
					pGroup = new CTPRIPropertyGridProperty( strCategory );
					pGroup->AddSubItem(pProp);
					arrGroup.push_back(pGroup);
				}
			}
		}

		for (size_t i = 0; i < arrGroup.size(); i++)
		{
			m_wndPropList.AddProperty(arrGroup[i]);
		}
	}

	m_wndPropList.RedrawWindow();

	return TRUE;
}

void CLicenseMgrView::RemoveAllLicensePropList()
{
	m_wndPropList.RemoveAll();
	m_wndPropList.RedrawWindow();
}

BOOL CLicenseMgrView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CLicenseMgrView ����

void CLicenseMgrView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

#ifdef _LICENSE_VIEW_APP
	if (m_bFirstOpenView)
	{
		if (Utility::DirectoryExists(GetDocument()->m_szPathName))
			GetDocument()->OnOpenDocument(GetDocument()->m_szPathName);
		else
			GetDocument()->SetPathName(GetDocument()->m_szPathName);

		m_bFirstOpenView = FALSE;
	}
#endif

	UpdateCurSelLicensePropList();
}

void CLicenseMgrView::OnDraw(CDC* /*pDC*/)
{
	CLicenseMgrDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
}

void CLicenseMgrView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CLicenseMgrView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CLicenseMgrView ���

#ifdef _DEBUG
void CLicenseMgrView::AssertValid() const
{
	CView::AssertValid();
}

void CLicenseMgrView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLicenseMgrDoc* CLicenseMgrView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLicenseMgrDoc)));
	return (CLicenseMgrDoc*)m_pDocument;
}
#endif //_DEBUG


// CLicenseMgrView ��Ϣ�������

void CLicenseMgrView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient,rectCombo;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + cyTlb, rectClient.Width(), rectClient.Height() - cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
}

int CLicenseMgrView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("δ�ܴ�����������\n");
		return -1;      // δ�ܴ���
	}

	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* ������*/);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* ����*/);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// �������ͨ���˿ؼ�·�ɣ�������ͨ�������·��:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CLicenseMgrView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CLicenseMgrView::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CLicenseMgrView::OnUpdateExpandAllProperties(CCmdUI* pCmdUI)
{
}

void CLicenseMgrView::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CLicenseMgrView::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CLicenseMgrView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CLicenseMgrView::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CView::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CLicenseMgrView::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

  	LOGFONT lf;
  	afxGlobalData.fontRegular.GetLogFont(&lf);
 
 	NONCLIENTMETRICS info;
 	info.cbSize = sizeof(info);
 
 	afxGlobalData.GetNonClientMetrics(info);
 
 	lf.lfHeight = info.lfMenuFont.lfHeight;
 	lf.lfWeight = info.lfMenuFont.lfWeight;
 	lf.lfItalic = info.lfMenuFont.lfItalic;
 
	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
}


LRESULT CLicenseMgrView::OnPropertyChanged(WPARAM, LPARAM lParam)
{
#ifndef _LICENSE_VIEW_APP
	CTPRIPropertyGridProperty* pProp = reinterpret_cast<CTPRIPropertyGridProperty*>(lParam);
	
	COleVariant varValue = pProp->GetValue(); // �ı�֮���ֵ
	// pProp->GetOriginalValue();  // �ı�֮ǰ��ֵ

	CString strValue;

	if (m_szSoftwareName.IsEmpty()) // ��ǰ�༭������Ȩ�ļ�����
	{
		CString strName = pProp->GetName();  // ���ı�Ĳ�����

		if (strName == _T("��Ȩ����"))
		{
			strValue = varValue.bstrVal;

			theLicenseFile.SetUserName( strValue );
		}
		else if (strName == _T("��Ȩ������"))
		{
			strValue = varValue.bstrVal;

			theLicenseFile.SetMachineCode( strValue );
		}
		else if (strName == _T("������Ȩ����"))
		{
			theLicenseFile.EnabledExpiredTime( varValue.boolVal != VARIANT_FALSE );
		}
		else if (strName == _T("��Ȩ��������"))
		{
			theLicenseFile.SetExpiredTime( varValue.lVal );
		}
	}
	else { // ��ǰ�༭���������Ȩ������
		CLicenseSoftware *pSoftware = theLicenseFile.GetLicenseSoftware(m_szSoftwareName);
		if (!pSoftware)
			return 0;

		CString strName = pProp->GetPropertyName();  // ���ı�Ĳ�����

		CLicenseProperty *pPty = pSoftware->GetProperty(strName);
		if (!pPty)
			return 0;

		switch(pPty->GetDataType())
		{
		case LIC_DT_BOOL:
			pPty->SetValue( (bool)(varValue.boolVal != VARIANT_FALSE) );
			break;
		case LIC_DT_NUMBER:
		case LIC_DT_DATE:
			pPty->SetValue( varValue.lVal );
			break;
		case LIC_DT_FLOAT:
			pPty->SetValue( varValue.dblVal );
			break;
			//case LIC_DT_STRING:
		default:
			strValue = varValue.bstrVal;
			pPty->SetValue( strValue );
			break;
		}
	}

	if (pProp->IsModified())
		GetDocument()->SetModifiedFlag(TRUE);
#endif

	return 0;
}




