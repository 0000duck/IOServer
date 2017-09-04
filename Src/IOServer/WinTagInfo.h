#pragma once


class CWinTagInfo :
	public CWinList
{
public:
	CWinTagInfo(void);
	~CWinTagInfo(void);
	//CDrvItem* m_pdrv;
	CString			m_szDecName;
	//CDecValMan*		m_pvalman;

	CSortObArray	m_array;	//��ǩ����CTagItem* ����,���ڱ����ѯ���

	static CString GetQualityText(CVarVal *pval);
protected:
	CBitmap	m_bmpOK,m_bmpRDB,m_bmpDrv,m_bmpAll;
	CVarVal m_val;//ֵ
public:
	int		GetNextSelTag(int nstart);
	int		GetNextSelected(int nstart);//����ѡ��
	void	ReDraw();
	void	RedrawList();
	void	QueryTag(LPCTSTR stagname,LPCTSTR sproxytagname,LPCTSTR sdes,LPCTSTR sdrvtagname,int ntype);//��ѯ��ǩ

	CString GetText(int nr,int nc);

	BOOL	ExportSelTags(CCSVFile* pf,int *pnExport);//����ѡ���ǩ
	BOOL	ExportRdbSelTags(CCSVFile* pf,int *pnExport,int nsel,LPCTSTR sdbname);//����ѡ���ǩΪʵʱ����ñ�ǩ��
	int		ExportDDSTags(CCSVFile* pf,int *pnExport,int nsel);//����DDS��ǩ,nsel = 0��ʾȫ��

	BOOL	ExportTags(CCSVFile* pf,int *pnExport);//������ǩ

	BOOL	GetTag(int nIndex,CTagItem *pi); //���Ʒ�ʽ
	BOOL	UpdateTag(int nIndex,CTagItem *pi);

protected:
	virtual CString GetCellText(int nrow,int ncol);//ȡ��ʾ�ַ���
	virtual CString GetHeadText(int ncol);//ȡ��ʾ�ַ���
	virtual void	GetRowColorS(int nrow,int ncol,COLORREF &clrTxt,COLORREF &clrBk);//ȡ����ɫ
	virtual void	OnClickHeadCol(int ncol);//�������ͷ

	virtual void	OnCurSelChange(int nrow);
	virtual void	OnSelChange(int nrowstart,int nrowend);
	virtual void	OnCtrlSelChange(int nrow);//CTRL��סʱ��ѡ��

	virtual CBitmap* GetBitMap(int nrow);

	BOOL	IsSel(int nIndex);
	int		GetItemErrStatus(int nIndex);
	BOOL	SetSelXor(int nIndex); //�෴����
	BOOL	SetSel(int nIndex,BOOL bSel);
};

