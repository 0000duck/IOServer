// TagItem.h: interface for the CTagItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TAGITEM_H__F3C94662_16CD_48A5_8494_2471F3A9B49A__INCLUDED_)
#define AFX_TAGITEM_H__F3C94662_16CD_48A5_8494_2471F3A9B49A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define FIELD_ID		0	// ��ǩ��
#define FIELD_NAME		1	// ��ǩ��
#define FIELD_DESC		2	// ����
#define FIELD_UNIT		3	// ��λ
#define FIELD_TYPE		4	// ����

#define FIELD_LAST		5	//

#define CUSTOMFIELD_TYPE_LONG		0
#define CUSTOMFIELD_TYPE_DWORD		1
#define CUSTOMFIELD_TYPE_DOUBLE		2
#define CUSTOMFIELD_TYPE_STRING		3

// �Զ����ֶ�������
class CCustomField
{
public:
	CCustomField(int nDataType = 0, int nFieldIndex = 0, LPCTSTR lpszDisplayName = _T(""), 
		LPCTSTR lpszDefaultValue = _T(""), int nColWidth = 100, DWORD dwAlignFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE) {
		m_nDataType = nDataType;
		m_nFieldIndex = nFieldIndex;
		m_nColWidth = nColWidth;
		m_szDefaultValue = lpszDefaultValue;
		m_szDisplayName = lpszDisplayName;
	}
	virtual ~CCustomField(){};

	const CCustomField& operator =( const CCustomField& src ) {
		if (this != &src)
		{
			m_nDataType			= src.m_nDataType;
			m_nFieldIndex		= src.m_nFieldIndex;
			m_nColWidth			= src.m_nColWidth;
			m_szDefaultValue	= src.m_szDefaultValue;
			m_szDisplayName		= src.m_szDisplayName;
		}

		return *this;
	}
	long DefaultValueAsLong() {
		return _ttol(m_szDefaultValue);
	}
	DWORD DefaultValueAsDWORD() {
		return (DWORD)_ttol(m_szDefaultValue);
	}
	double DefaultValueAsDouble() {
		return atof(m_szDefaultValue);
	}
	LPCTSTR DefaultValueAsString() {
		return m_szDefaultValue;
	}
	void GetDefaultValue(long &lValue) {
		lValue = DefaultValueAsLong();
	}
	void GetDefaultValue(DWORD &dwValue) {
		dwValue = DefaultValueAsDWORD();
	}
	void GetDefaultValue(double &dblValue) {
		dblValue = DefaultValueAsDouble();
	}
	void GetDefaultValue(CString &szValue) {
		szValue = m_szDefaultValue;
	}

public:
	int		m_nDataType;		// �Զ����ֶ��������ͣ�0 - long��1 - DWORD��2 - double��3 - string
	int		m_nFieldIndex;		// �Զ����ֶ������ţ�0-3
	int		m_nColWidth;		// �Զ����ֶ���ʾ�п�
	DWORD	m_dwAlignFormat;	// �Զ����ֶ���ʾ���뷽ʽ
	CString m_szDefaultValue;	// �Զ����ֶε�Ĭ��ֵ
	CString	m_szDisplayName;	// �Զ����ֶ���ʾ��
};

typedef CArray<CCustomField, CCustomField&> CCustomFieldArray;

class CTagItem : public CSortItem  
{
public:	
	CTagItem();
	virtual ~CTagItem();

public:
	CString	m_szID;
	CString m_szName; 
	CString m_szDes;
	CString m_szUnit;
	CString m_szNote;	
	WORD 	m_wType;		//��������

	//{{ �����ֶ�
	long	m_lField1;
	long	m_lField2;
	long	m_lField3;
	long	m_lField4;	
	DWORD	m_dwField1;
	DWORD	m_dwField2;
	DWORD	m_dwField3;
	DWORD	m_dwField4;
	double	m_dblField1;
	double	m_dblField2;
	double	m_dblField3;
	double	m_dblField4;
	CString	m_szField1;
	CString m_szField2;
	CString m_szField3;
	CString m_szField4;
	//}} �����ֶ�

	int		m_nError;		//����Rdb��ǩ����,0����,1:�޴˱�ǩ 2:���ʹ���
	BOOL	m_bSel;			//ѡ���־
public:
	CString GetText(int nField);//ȡ�����ַ���
	void	SetCustomField(int nField, LPCSTR lpszValue);// �����Զ����ֶ�ֵ
	void	CopyTo(CTagItem *pt);//����
	void	CopyToDrTag(LPDRVTAG lpdrtag);//���Ƶ��ӿڸ�ʽ��ǩ����
	void    Serialize(CArchive& ar, DWORD dwVersion);
	
	//����Ƚ�����
	virtual		int Compare(CSortItem *pOb);

	CString GetTypeStr();
	void	SetTypeVal(LPCTSTR lpszType);

public:
	// ��ȡ�Զ����ֶ�����
	static int GetCustomFieldCount() {
		return m_arrCustomFields.GetSize();
	}
	// ���һ���Զ����ֶ�
	static BOOL AddCustomField(
		int nDataType,						// �Զ����ֶ��������ͣ�����ΪCUSTOMFIELD_TYPE_LONG��CUSTOMFIELD_TYPE_DWORD��CUSTOMFIELD_TYPE_DOUBLE��CUSTOMFIELD_TYPE_STRING
		int nFieldIndex,					// �Զ����ֶ��ڸ����������е������ţ���ȡ0-3����ͬһ�����������У������ظ�
		LPCTSTR lpszDisplayName,			// �Զ����ֶ����б��е���ʾ����
		LPCTSTR lpszDefaultValue = _T(""),	// �Զ����ֶ�Ĭ��ֵ
		int nColWidth = 100,				// �Զ����ֶ����б��е���ʾ���
		DWORD dwAlignFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE // �Զ����ֶ����б��е���ʾ���뷽ʽ
		) {
		for(int i = 0; i < m_arrCustomFields.GetSize(); i++)
		{
			CCustomField &field = m_arrCustomFields[i];
			if (field.m_nDataType == nDataType && field.m_nFieldIndex == nFieldIndex)
				return FALSE;
		}
		CCustomField field(nDataType, nFieldIndex, lpszDisplayName, lpszDefaultValue, nColWidth, dwAlignFormat);
		m_arrCustomFields.Add(field);
		return TRUE;
	}
	// ��ȡ�Զ����ֶ�
	static CCustomField &GetCustomField(int nIndex) {
		return m_arrCustomFields[nIndex];
	}
	// ���ñ�ǩ���롢������CSV�ļ��еĸ�ʽ��ǩ����Ĭ����DDCDRV�����ʹ�����Զ����ֶΣ�
	// ���Ըı�����ƣ����ⱻ���������������
	static void SetCSVFormatName(LPCTSTR lpszName) {
		m_szCSVFormatName = lpszName; 
	}
	// ��ȡCSV�ļ��еĸ�ʽ��ǩ��
	static LPCTSTR GetCSVFormatName() {
		return m_szCSVFormatName;
	}

protected:
	static CCustomFieldArray m_arrCustomFields;
	static CString m_szCSVFormatName;
};

class CTagDefTab		//��ǩ�����
{
public:
	typedef CMap<CString, LPCTSTR, CTagItem*,CTagItem*>	CTagItemMap;
	CTagDefTab()
	{
		m_bModified = FALSE;
		m_mapTab.InitHashTable(225307);	//��ʼ��MAP�ռ�
		m_oaTag.SetGrown(8192);			//��8k����
	};
	virtual ~CTagDefTab()
	{
		m_mapTab.RemoveAll();//m_oaTag����ʱ��ɾ������,���Բ������ɾ��m_mapTab�Ķ���
	};
protected:
	//m_mapTab��m_oaTag��洢��ͬ�Ķ���ָ��
	CTagItemMap m_mapTab;	//��ǩMAP
	CSortObArray m_oaTag;									//��ǩ����
	BOOL m_bModified;
	friend class CRunTags;
	friend class CWinTagInfo;
public:
	void	SetModified(BOOL bModified = TRUE) { m_bModified = bModified; }
	BOOL	IsModified() { return m_bModified; }
	int		GetSize(){return m_oaTag.GetSize();};
	CSortObArray &GetSortObArray() { return m_oaTag; }
	CTagItemMap &GetTagItemMap() { return m_mapTab; }
	void	Serialize(CArchive& ar, DWORD dwVersion);
	BOOL	IsSelected(int nIndex);
	void	SetSel(int nIndex,BOOL bSel);
	void    Qsort() { m_oaTag.Qsort(); }
	int		DelSelect();//ɾ��ѡ��ı�ǩ
	int		DelBadTags();
	int		GetBadTagCount();

	void	CopyTo(CTagDefTab &table, BOOL bNewAlloc = TRUE);
	
	CString GetText(int nIndex,int nField);
	int	 GetNextSelected(int nstart)//����ѡ��
	{
		CTagItem *pi;
		int i,ns = nstart,n = m_oaTag.GetSize();
		if(ns < 0) ns = 0;
		for(i=ns;i<n;i++)
		{
			pi = (CTagItem	*)m_oaTag.GetAt(i);
			if(pi->m_bSel)
				return i;
		}
		return -1;
	}
	
	CTagItem*	GetTag(LPCTSTR lpszName); //���ر�ǩ����
	CTagItem*	GetTag(int nIndex); //���ر�ǩ����
	BOOL		ModifyItem(CTagItem* pTagItem, LPCTSTR lpszOldName);
	BOOL		GetAt(int nIndex,CTagItem *ptag); //���Ʒ�ʽȡ
	BOOL		AddTag(CTagItem *pTag); //��ӱ�ǩ���壬���Ʒ�ʽ
	BOOL		IsExist(LPCTSTR lpszName);//�Ƿ����
};

#endif // !defined(AFX_TAGITEM_H__F3C94662_16CD_48A5_8494_2471F3A9B49A__INCLUDED_)
