
#pragma once

#define NODE_LICENSE_FILE		0
#define NODE_LICENSE_SOFTWARE	1

typedef struct
{
	int nNodeType;		// 0 - License File, 1 - software
	char szDescr[80];
}TREENODEDATA, *LPTREENODEDATA;

/////////////////////////////////////////////////////////////////////////////
// CViewTree ����

class CViewTree : public CTreeCtrl
{
// ����
public:
	CViewTree();

// ��д
protected:
	virtual void PreSubclassWindow();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO * pTI) const;
	BOOL	OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult );

// ʵ��
public:
	virtual ~CViewTree();
	afx_msg void OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);

protected:
	DECLARE_MESSAGE_MAP()
};
