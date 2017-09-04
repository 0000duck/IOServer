// ItemWinList.h: interface for the CLeafItemWinList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMWINLIST_H__E9D67A74_CA30_42B4_BE02_1EF582496B92__INCLUDED_)
#define AFX_ITEMWINLIST_H__E9D67A74_CA30_42B4_BE02_1EF582496B92__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h> // ģ��

class CLeafItem : public CSortItem
{	
public:
	CLeafItem();
	virtual ~CLeafItem();

	BOOL m_bSel;
	CString m_szItemName;

public:
	virtual		int Compare(CSortItem *pOb);//����Ƚ�
};

class CLeafItemWinList : public CWinList  
{
public:
	CLeafItemWinList();
	virtual ~CLeafItemWinList();

public:	
	BOOL	IsSel(int nIndex);
	BOOL	SetSel(int nIndex,BOOL bSel);	
	BOOL	SetSelXor(int nIndex); //�෴����
	CLeafItem *GetAt(int nIndex);
	CLeafItem *AddItem(LPCTSTR lpszItemName);
	void	RemoveAt(int nIndex);
	void	RemoveAll();
	void	ReDraw();
	CString GetText(int nrow, int ncol);
	int		GetCount() { return m_array.GetSize(); }

protected:
	virtual CString GetCellText(int nrow,int ncol);//ȡ��ʾ�ַ���
	virtual CString GetHeadText(int ncol);//ȡ��ʾ�ַ���
	virtual void	GetRowColorS(int nrow,int ncol,COLORREF &clrTxt,COLORREF &clrBk);//ȡ����ɫ
	virtual void	OnClickHeadCol(int ncol);//�������ͷ
	
	virtual void	OnCurSelChange(int nrow);
	virtual void	OnSelChange(int nrowstart,int nrowend);
	virtual void	OnCtrlSelChange(int nrow);//CTRL��סʱ��ѡ��
	
	virtual CBitmap* GetBitMap(int nrow);
private:
	CSortObArray	m_array;
	CBitmap	m_biImage;
};

class CTagItem : public CSortItem
{	
public:
	CTagItem();
	virtual ~CTagItem();
	
	BOOL m_bSel;
	CString m_szItemName;
	CString m_szDataType;
	CString m_szDesc;
	CString m_szUnit;
	BOOL	m_bZeroFilter;
	
public:
	virtual		int Compare(CSortItem *pOb);//����Ƚ�
};

typedef CMap<CString, LPCTSTR, CTagItem*, CTagItem*>	CTagItemMap;

class CTagItemWinList : public CWinList  
{
public:
	CTagItemWinList();
	virtual ~CTagItemWinList();
	
public:
	BOOL	IsSel(int nIndex);
	BOOL	SetSel(int nIndex,BOOL bSel);	
	BOOL	SetSelXor(int nIndex); //�෴����
	CTagItem *GetAt(int nIndex);
	CTagItem *AddItem(LPCTSTR lpszItemName, LPCTSTR lpszDesc, LPCTSTR lpszUnit, LPCTSTR lpszDataType,BOOL bZeroFilter);
	void	RemoveAt(int nIndex);
	void	RemoveAll();
	void	ReDraw();
	CString GetText(int nrow, int ncol);
	CTagItem *FindItem(LPCTSTR lpszItemName);
	int		GetCount() { return m_array.GetSize(); }
	void	InitHashSize(int nHashSize) { m_mapTagItems.InitHashTable(nHashSize); }

protected:
	virtual CString GetCellText(int nrow,int ncol);//ȡ��ʾ�ַ���
	virtual CString GetHeadText(int ncol);//ȡ��ʾ�ַ���
	virtual void	GetRowColorS(int nrow,int ncol,COLORREF &clrTxt,COLORREF &clrBk);//ȡ����ɫ
	virtual void	OnClickHeadCol(int ncol);//�������ͷ
	
	virtual void	OnCurSelChange(int nrow);
	virtual void	OnSelChange(int nrowstart,int nrowend);
	virtual void	OnCtrlSelChange(int nrow);//CTRL��סʱ��ѡ��
	virtual CBitmap* GetBitMap(int nrow);

private:
	CSortObArray	m_array;
	CTagItemMap		m_mapTagItems;
	CBitmap	m_biImage;
};

#endif // !defined(AFX_ITEMWINLIST_H__E9D67A74_CA30_42B4_BE02_1EF582496B92__INCLUDED_)
