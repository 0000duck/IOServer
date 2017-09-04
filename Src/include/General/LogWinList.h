#ifndef __LOG_WINLIST_H__
#define __LOG_WINLIST_H__

TRUNK_NAMESPACE_BEGIN

class _GENERAL_CLASS CLogItemView
{
public:
	CLogItemView();
	virtual ~CLogItemView();

	BOOL IsSelected() { return m_bSel; }
	void SetSelected(BOOL bSel = TRUE) { m_bSel = bSel; }
	int GetLogLevel() const { return m_nLogLevel; }
	LPCTSTR GetText() const { return m_szText; }
	void SetText(LPCTSTR lpszText);

private:
	CString m_szText;
	int		m_nLogLevel;
	BOOL	m_bSel;		//ѡ��
};

#define LOG_LIST_MAX	512

class _GENERAL_CLASS CLogWinList : public CWinList //��ǩ��Ϣ����
{
public:
	CLogWinList(LPCTSTR lpszTitle = NULL);
	~CLogWinList();

public:
	int GetCount() const { return m_nCount; }
	void AddLog(LPCTSTR lpszLogText);
	CLogItemView *GetLogItem(int nIndex);
	void Resize(CRect &rc);
	void SetSel(int nIndex);
	LPCTSTR GetTitle() const { return m_szTitle; }
	void SetTitle(LPCTSTR lpszTitle) { m_szTitle = lpszTitle; }

public:
	int		GetNextSelected(int nstart);//����ѡ��
	void	ReDraw(BOOL bSelLast = FALSE);
	CString	GetItemText(int nrow,int ncol);
	void	SortCol(int ncol,BOOL bAsc);

protected:
	virtual CString GetCellText(int nrow,int ncol);//ȡ��ʾ�ַ���
	virtual CString GetHeadText(int ncol);//ȡ��ʾ�ַ���
	virtual void	GetRowColorS(int nrow,int ncol,COLORREF &clrTxt,COLORREF &clrBk);//ȡ����ɫ
	virtual void	OnClickHeadCol(int ncol);//�������ͷ

	virtual void	OnCurSelChange(int nrow);
	virtual void	OnSelChange(int nrowstart,int nrowend);
	virtual void	OnCtrlSelChange(int nrow);//CTRL��סʱ��ѡ��

	virtual CBitmap* GetBitMap(int nrow);
protected:
	BOOL IsSel(int row);
	void SetSel(int row,BOOL bsel);
	BOOL SetSelXor(int row);

private:
	CString m_szTitle;
	CBitmap	m_bmpError;
	CBitmap m_bmpWarning;
	CBitmap m_bmpInfo;
	CBitmap m_bmpDebug;

	CLogItemView *m_array[LOG_LIST_MAX];
	int m_nStart;
	int m_nCount;
};

TRUNK_NAMESPACE_END

#endif // end of __LOG_WINLIST_H__