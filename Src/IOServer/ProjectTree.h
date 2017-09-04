
#pragma once

#include "ViewTree.h"

class CProjectTreeToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CProjectTree : public CDockablePane
{
// ����
public:
	CProjectTree();

	void		AdjustLayout();
	void		OnChangeVisualStyle();
	void		InitProject();
	int			AddNode(CString szModule, CString szDevice, int nNodeType);
	CTreeCtrl&	GetTreeCtrl();
	HTREEITEM	GetDrvParentNode();//���"�豸����"�ڵ��hr
	HTREEITEM	FindWapInTree(LPCTSTR lpszwapname);
	HTREEITEM	FindDrvInTree(LPCTSTR lpszdrvname, HTREEITEM hwap);
	void		AddDrvToTree(T_DRV_DES &tDrvDes);//�����������
	BOOL		DeleteItem(CString szDevName);
	void		BrowseItem(HTREEITEM hItem);
// ����
protected:

	CViewTree			m_wndProjectTree;
	CImageList			m_ProjectTreeImages;
	CProjectTreeToolBar m_wndToolBar;
	
// ʵ��
public:
	virtual ~CProjectTree();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnProperties();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnProjDbConfig();
	afx_msg void OnProjDbConinfo();
	afx_msg void OnProjDbStart();
	afx_msg void OnProjDbStop();
	afx_msg void OnProjectDbAdd();
	afx_msg void OnProjectDbDel();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int GetNodeStatus(HTREEITEM hItem);
	afx_msg void OnDestroy();
};

