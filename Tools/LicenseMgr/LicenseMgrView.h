
// LicenseMgrView.h : CLicenseMgrView ��Ľӿ�
//


#pragma once


class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CLicenseMgrView : public CView
{
protected: // �������л�����
	CLicenseMgrView();
	DECLARE_DYNCREATE(CLicenseMgrView)

// ����
public:
	CLicenseMgrDoc* GetDocument() const;

	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}

// ����
public:
	void AdjustLayout();
	void RemoveAllLicensePropList();
	void UpdateCurSelLicensePropList();
	BOOL UpdateLicensePropList(LPCTSTR lpszSoftwareName = NULL);

// ��д
public:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// ʵ��
public:
	virtual ~CLicenseMgrView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CFont m_fntPropList;
	CPropertiesToolBar m_wndToolBar;
	CMFCPropertyGridCtrl m_wndPropList;

	CString m_szSoftwareName;
#ifdef _LICENSE_VIEW_APP
	BOOL	m_bFirstOpenView;
#endif

	void SetPropListFont();

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpandAllProperties();
	afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
	afx_msg void OnSortProperties();
	afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnPropertyChanged(WPARAM,LPARAM);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // LicenseMgrView.cpp �еĵ��԰汾
inline CLicenseMgrDoc* CLicenseMgrView::GetDocument() const
   { return reinterpret_cast<CLicenseMgrDoc*>(m_pDocument); }
#endif

