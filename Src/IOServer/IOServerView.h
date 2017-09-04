
// IOServerView.h : CIOServerView ��Ľӿ�
//

#pragma once
#include "WinTagInfo.h"


class CIOServerView : public CView
{
protected: // �������л�����
	CIOServerView();
	DECLARE_DYNCREATE(CIOServerView)

// ����
public:
	CIOServerDoc* GetDocument() const;
	CString		m_szQuery;//��ѯ�ַ���,name\n �����ǩ�� \n ������ǩ��\n ����\n ����\n
	CFont		m_fntText;
	CButton		m_btnQuery;//��ѯ��ť
	CButton		m_btnRefresh;//ˢ�°�ť
	UINT		m_uTimer;
	CWinTagInfo	m_wndTagTable; //����
// ����
public:
	CString		GetQueryInfo();
	void		Query();
	int			DelSelTags(CString szDecName);//ɾ��ѡ��ı�ǩ
// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CIOServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetDeviceTag();
	afx_msg void OnQueryTag();
	afx_msg void OnExportTag();
	afx_msg void OnExportSelectTag();
	afx_msg void OnImportTag();
	afx_msg void OnDelTag();
	afx_msg void OnAddTag();
	afx_msg void OnReFresh();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

#ifndef _DEBUG  // IOServerView.cpp �еĵ��԰汾
inline CIOServerDoc* CIOServerView::GetDocument() const
   { return reinterpret_cast<CIOServerDoc*>(m_pDocument); }
#endif

