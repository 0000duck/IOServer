/*
  ��Ȩ����������������޹�˾
  ���룺����
  ���ڣ�2009.4.5

  */

#ifndef __WINLIST_H__
#define __WINLIST_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WinList.h : header file
//

#define WINLIST_LBDBLCLK WM_USER + 21

TRUNK_NAMESPACE_BEGIN

/////////////////////////////////////////////////////////////////////////////
// CWinList window
#define MAXWINLISTCOLS  32
#define AT_NONE			0
#define AT_INCELL		1
#define AT_ADJUSTW		2
#define AT_ADJUSTH		3
#define AT_SELROW		4
#define AT_SELCOL		5
#define AT_HEAD			6

class _GENERAL_CLASS CWinList : public CWnd
{
// Construction
public:
	CWinList();

protected:
	int m_nRows;//����
	int m_nPreRows;//�ϴ�����
	int m_nCols;//����
	int m_nColW[MAXWINLISTCOLS];
	int	m_nRowH;//�и�
	int m_nColFmt[MAXWINLISTCOLS];//����ʾ��ʽ
	CFont m_fntText;
	CPen  m_pengrid,m_pendrog;
	LOGPEN m_logpen;

	//�����п��п���
	CPoint  m_ptFirstCell; //��һ��ѡ�еĵ�Ԫ������,����
	CPoint	m_ptAdjustOrg;
	CPoint	m_ptAdjust;

	int		m_nMinColW;//��С���гߴ�,��ֵ,twip
	int		m_LastSelType;//�ϴ�ѡ������

	CPoint m_scrollPos;	//������λ��

	COLORREF m_clrHighLight;//������ɫ

	int		m_nCurSel;
	COLORREF	m_bkclr;//������ɫ

	BOOL	m_bGridLineH;//ˮƽ��
	int		m_nTxtSize;
	
	int		m_nSortCol;//������,-1��ʾ��
	BOOL	m_bSortAsc;
protected:
	virtual CString GetCellText(int nrow,int ncol);//ȡ��ʾ�ַ���
	virtual CBitmap* GetBitMap(int nrow){return NULL;};
	virtual CString GetHeadText(int ncol);//ȡ��ʾ�ַ���
	virtual void	GetRowColorS(int nrow,int ncol,COLORREF &clrTxt,COLORREF &clrBk);//ȡ����ɫ
	virtual void	OnCurSelChange(int nrow);
	virtual void	OnSelChange(int nrowstart,int nrowend){};
	virtual void	OnCtrlSelChange(int nrow){};//CTRL��סʱ��ѡ��
	virtual void	OnClickHeadCol(int ncol){};//�������ͷ
	virtual void	OnClickRowCheck(int nrow){};//�������һ��
	
	virtual void	OnViewToolBar(int x,int y,int nrow,int ncol,BOOL bView){};//��ʾ������toolbar
	virtual void	OnMouseAt(int x,int y,int nrow,int ncol){}; //�����������,�������toolbar

	void	DrawHead(CDC *pDC,CRect &rtdraw);
	void	DrawCell(CDC *pDC,CRect &rtdraw);
	void	DrawSortFlag(CDC *pDC,CRect &rtpos);//���������־
	int		HitTest(CPoint point,int &row,int &col);
	BOOL	ClearSelected(BOOL bReset);
	void	DrawAdJust(CDC *pDC,CRect &rtdraw);//���Ƶ���

	void	GetScrollBarSizes(CSize& sizeSb);
	BOOL	GetTrueClientSize(CSize& size, CSize& sizeSb);
	void	CalScrolBar(void);//���������
	int		GetAllColsW();//ȡ�����п�
	int		GetScrollPos32(int nBar, BOOL bGetTrackPos = FALSE);
	BOOL	SetScrollPos32(int nBar, int nPos, BOOL bRedraw  = TRUE );


// Attributes
public:
	

	void SetCols(int ncols){
		if(ncols<MAXWINLISTCOLS)
			m_nCols = ncols;
		else
			m_nCols = MAXWINLISTCOLS;
	};
	int GetCols(){return m_nCols;};

	void	SetRows(int nrow){m_nRows = nrow;};
	int		GetRows(){return m_nRows;};

	void	SetColW(int ncol,int nw){
		if(ncol < MAXWINLISTCOLS)
			m_nColW[ncol] = nw;
	};
	DWORD GetCellFmt(int ncol){
		if((ncol <0)||(ncol >= MAXWINLISTCOLS))
			return DT_LEFT|DT_TOP;
		return m_nColFmt[ncol];
	};

	void SetCellFmt(int ncol,DWORD dwfmt){
		if((ncol <0)||(ncol >= MAXWINLISTCOLS))
			return ;
		m_nColFmt[ncol] = dwfmt;
	};
	void	SetTxtFontSize(int nsize){m_nTxtSize = nsize;};
	int     GetTxtFontSize(){return m_nTxtSize;};

	void SetBkColor(COLORREF bkclr){m_bkclr = bkclr;};
	void SetGridLineH(BOOL bLine){m_bGridLineH = bLine;};
	BOOL HasGridLineH(){return m_bGridLineH;};

	void	SetRowScrollTop(int nrow);//����������
	void	SetSortCol(int ncol,BOOL bAsc)
	{
		m_nSortCol = ncol;
		m_bSortAsc = bAsc;
	};
	int		GetCurSel(){return m_nCurSel;};
	void	SetCurSel(int nrow){m_nCurSel = nrow;};

	void	GetViewRows(int &nstart,int &nend);//ȡ�ɼ�������ֹ��
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinList)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWinList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWinList)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg long   OnMouseLeave(WPARAM   wParam,   LPARAM   lParam);
	afx_msg long   OnMouseHover(WPARAM   wParam,   LPARAM   lParam); 
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

TRUNK_NAMESPACE_END

#endif // __WINLIST_H__
