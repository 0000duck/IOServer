//odbcobj.h
#pragma once

#include <afxmt.h>
#include <afxtempl.h> //ģ��֧��
#include "../sdk/drvsdk.h"
#include "../sdk/sdktool.h"

class CSafeLock  //�ٽ���
{
public:
	CSafeLock(CCriticalSection *pLock){
		if(pLock != NULL){
			m_pLock = pLock;
			m_pLock->Lock();
		}
	};
	virtual ~CSafeLock(){
		if(m_pLock != NULL)
			m_pLock->Unlock();
	};
private:
	CCriticalSection *m_pLock;
};

#define FIELD_NAME		0
#define FIELD_ALIASNAME	1
#define FIELD_DES		2
#define FIELD_UNIT		3
#define FIELD_TYPE		4


class CTagItem : public CSortItem //��ǩ������
{
public:
	CTagItem()
	{
		m_szName = _T("");
		m_szAliasName = _T("");
		m_nType = DRT_FLOAT32;
		m_bSel = FALSE;
		m_ldatatime = 0;
	};
	virtual ~CTagItem(){};
public:
	CString m_szName;		// ��ǩ���������ֶε�IDֵ
	CString m_szAliasName;	// ��ǩ�������ñ������ϴ���DDC
	CString m_szDes;
	CString m_szUnit;
	int 	m_nType;		//��������

//�����̲���
public:
	BOOL	m_bSel;			//ѡ���־
	long	m_ldatatime;	//����ʱ�꣬������ͬʱ�䲻�ط� 
public:
	CString GetText(int nField);//ȡ�����ַ���
	void	CopyTo(CTagItem *pt);//����
	void	CopyToDrTag(LPDRVTAG lpdrtag);//���Ƶ��ӿڸ�ʽ��ǩ����
	void    Serialize(CArchive& ar);

	//����Ƚ�����
	virtual		int Compare(CSortItem *pOb);
	
public: //static
	static  CString GetTypeStr(int ntype);
	static int		GetTypeVal(LPCTSTR lpszType);
};

class CWinTagInfo;
class CTagDefTab		//��ǩ�����
{
public:
	CTagDefTab()
	{
		m_mapTab.InitHashTable(11111);	//��ʼ��MAP�ռ�
		m_oaTag.SetGrown(8192);			//��8k����
	};
	~CTagDefTab()
	{
		m_mapTab.RemoveAll();//m_oaTag����ʱ��ɾ������,���Բ������ɾ��m_mapTab�Ķ���
	};
protected:
	//m_mapTab��m_oaTag��洢��ͬ�Ķ���ָ��
	CMap<CString ,LPCTSTR ,CTagItem*,CTagItem*> m_mapTab;	//��ǩMAP
	CSortObArray m_oaTag;									//��ǩ����

	friend class CWinTagInfo;
public:
	int		GetSize(){return m_oaTag.GetSize();};
	void	Serialize(CArchive& ar);
	BOOL	IsSelected(int nIndex);
	void	SetSel(int nIndex,BOOL bSel);
	BOOL	DelSelect();//ɾ��ѡ��ı�ǩ
	
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
	void Initdatatime()
	{
		CTagItem *pi;
		int i,n = m_oaTag.GetSize();
	
		for(i=0;i<n;i++)
		{
			pi = (CTagItem	*)m_oaTag.GetAt(i);
			if(pi)
				pi->m_ldatatime = 0;
		}
	};
	CTagItem*	GetTag(LPCTSTR lpszName); //���ر�ǩ����
	BOOL		GetAt(int nIndex,CTagItem *ptag); //���Ʒ�ʽȡ
	BOOL		AddTag(CTagItem *pTag); //��ӱ�ǩ���壬���Ʒ�ʽ
	BOOL		IsExist(LPCTSTR lpszName,long *ptime = NULL);//�Ƿ����
};

#define UPDATEVALTAGS		2000 //ÿ�θ��µı�ǩֵ����
#define	TAB_REAL			0   //ʵʱ��
#define TAB_HIS				1   //��ʷ��
class COdbcDrvObj : public CDrvObj
{
public:
	COdbcDrvObj();
	virtual ~COdbcDrvObj();

public:
	CTagDefTab		m_tagtab;	//��ǩ��
	
	static BOOL IsNeedReConnect(CString &szstatus);//�Ƿ���Ҫ��������
protected:
	//ȫ������
	int		m_ntabstyle;
	int		m_ntimestyle; //δ��
	CString	m_szconstr; //�����ַ���
	CString	m_szfname;	//��ǩ���ֶ���
	CString	m_szqa;		//�����ֶ���
	CString	m_szftime;	//ʱ���ֶ���
	CString	m_szfval;	//ֵ�ֶ���
	CString	m_sztabname;//����

	CString m_szres;//����
	//���в���
	CDatabase	m_db; //��ϵ���ݿ�

	int				m_nrecsize;
	CRdRec			m_recs[UPDATEVALTAGS];
protected:
	CString GetFieldList(LPCTSTR lpszTableName = NULL);
	BOOL			Connectdb(CString &szerr);//�������ݿ�
	void			OnDataOk(CDBVariant &varname,CDBVariant &vartime,CDBVariant &varqa,CDBVariant &varval);
	void			OnUploadData();

//�������ػ���ĺ���
public: //�麯��
	virtual DRVRET  GetErrMsg(CString& szMsg,DRVRET dwcode);			//��ȡ������Ϣ
	virtual DRVRET  WriteVar(LPDRREC lpval);							//д����
	virtual DRVRET  GetTagDef(int nindex,LPDRVTAG  lptag);
					
protected: //���麯��
	virtual BOOL	OnConfig();					//���ú����ý���,����TRUE��ʾ�޸�������
	virtual void	Serialize(CArchive& ar);	//���л����棬��ȡ����
	virtual DRVRET	OnStart();					//׼������
	virtual void	OnStop();					//ֹͣ���к��������
	virtual UINT	DoRun();					//��������й��̣���RunThread�е���
};

class CWinTagInfo : public CWinList //��ǩ��Ϣ����
{
public:
	CWinTagInfo();
	~CWinTagInfo();
public:
	COdbcDrvObj* m_pdrv;
public:
	int		GetNextSelected(int nstart);//����ѡ��
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

};
