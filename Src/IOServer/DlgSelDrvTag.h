#if !defined(AFX_DLGSELDRVTAG_H__5D89A754_B23B_40F8_AD7D_8F47C833223F__INCLUDED_)
#define AFX_DLGSELDRVTAG_H__5D89A754_B23B_40F8_AD7D_8F47C833223F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSelDrvTag.h : header file
//

class CDrvTagItem : public CSortItem//��ǩ����
{
public:
	CDrvTagItem();
	virtual ~CDrvTagItem();
public:
	virtual		int Compare(CSortItem *pOb);//����Ƚ�
	void			Serialize(CArchive& ar);	//����
	void			CopyTo(CTagItem *pt);
	static  CString GetTypeStr(int ntype);
	static int		GetTypeVal(LPCTSTR lpszType);

public:
	int		m_ntype;	//��������
	CString m_szName;	//��ǩ��
	CString m_szdes;	//����
	CString m_szunit;	//��λ

	BOOL	m_bSel;		//ѡ��
	//}}
};
class CWinDrvTagList : public CWinList //��ǩ��Ϣ����
{
public:
	CWinDrvTagList();
	virtual ~CWinDrvTagList();
public:
	CSortObArray	m_array;	//��ǩ����CDrvTagItem* ����,���ڱ����ѯ���

public:
	int		GetNextSelTag(int nstart);

	CString GetText(int nr,int nc);

	void	ReDraw();
	void	RedrawList();
protected:
	virtual CString GetCellText(int nrow,int ncol);//ȡ��ʾ�ַ���
	virtual CString GetHeadText(int ncol);//ȡ��ʾ�ַ���
	virtual void	GetRowColorS(int nrow,int ncol,COLORREF &clrTxt,COLORREF &clrBk);//ȡ����ɫ
	virtual void	OnClickHeadCol(int ncol);//�������ͷ

	virtual void	OnCurSelChange(int nrow);
	virtual void	OnSelChange(int nrowstart,int nrowend);
	virtual void	OnCtrlSelChange(int nrow);//CTRL��סʱ��ѡ��


	BOOL	IsSel(int nIndex);
	int		GetItemErrStatus(int nIndex);
	BOOL	SetSelXor(int nIndex); //�෴����
	BOOL	SetSel(int nIndex,BOOL bSel);
};

/////////////////////////////////////////////////////////////////////////////
// CDlgSelDrvTag dialog

class CDlgSelDrvTag : public CDialog
{
// Construction
public:
	CDlgSelDrvTag(CWnd* pParent = NULL);   // standard constructor
	CWinDrvTagList m_wintag;
	CString m_szTagName;
	CString	m_szDrvName;
	void InitTag(CString szDrvName);
// Dialog Data
	//{{AFX_DATA(CDlgSelDrvTag)
	enum { IDD = IDD_DIALOG_SELDRVTAG };
	CStatic	m_stFrm;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSelDrvTag)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSelDrvTag)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	afx_msg LRESULT	OnDblClkWinList(WPARAM wParam, LPARAM lParam); //˫����Ϣ
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSELDRVTAG_H__5D89A754_B23B_40F8_AD7D_8F47C833223F__INCLUDED_)
