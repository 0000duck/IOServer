//SimdrvExobj.h
#pragma once

#include <afxmt.h>
#include <afxtempl.h> //ģ��֧��
#include "..\sdk\drvsdk.h"
#include "..\sdk\sdktool.h"
 
#define SIMTYPE_LINE	0 //����ģ�� line
#define SIMTYPE_SIN		1 //����ģ�� sin
#define SIMTYPE_RAND    2 //���ģ�� rand wzq

#define FIELD_NAME		0
#define FIELD_DES		1
#define FIELD_UNIT		2
#define FIELD_TYPE		3
#define FIELD_SIMTYPE	4
#define FIELD_BASEVAL	5
#define FIELD_PERCENT	6

class CTagItem : public CSortItem //��ǩ������
{
public:
	CTagItem()
	{	
		m_nType = DRT_FLOAT32;
		m_fPercent = 20;
		m_bSel = FALSE;
	};
	virtual ~CTagItem(){};
public:
	CString m_szName; 
	CString m_szDes;
	CString m_szUnit;

	int 	m_nType;		//��������
	int 	m_nSimtype;		//ģ�ⷽʽ0��ݲ���1������
	CString m_szBaseVal;	//��׼ֵ
	float	m_fPercent;//�仯�ٷֱȷ���(����ڻ�׼ֵ)

	BOOL	m_bSel;			//ѡ���־
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
	static CString	GetSimTypeStr(int ntype);
	static int		GetSimTypeVal(LPCTSTR lpszType);
};
class CWinTagInfo;
class CRunTags;
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
	friend class CRunTags;
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

	CTagItem*	GetTag(LPCTSTR lpszName); //���ر�ǩ����
	BOOL		GetAt(int nIndex,CTagItem *ptag); //���Ʒ�ʽȡ
	BOOL		AddTag(CTagItem *pTag); //��ӱ�ǩ���壬���Ʒ�ʽ
	BOOL		UpdateTag(CTagItem *pTag); //���±�ǩ�����δ���ڣ������
	BOOL		IsExist(LPCTSTR lpszName);//�Ƿ����
};

class CTagVal : public CSortItem //��ǩֵ
{
public:
	CTagVal();
	CTagVal(CTagItem *pTag);
	~CTagVal();
public:
	void Simulate(long ltime,CRdRec* lprv);//ģ�⣬������Ƶ�lprv��
public:
	CRdRec m_val;	//ʵʱֵ 
	//{{������������ʱģ��
	union U_BASEVAL
	{
		long  lval;
		float fval;
	}bv; //��׼ֵ
	int 	m_nSimtype;		//ģ�ⷽʽ0��ݲ���1������
	float   m_fPercent;		//
	//}}
public:
	CVarVal *GetVarVal(){return (CVarVal *)&m_val.val;};
};	

class CRunTags //��ǩ���й���
{
public:
	CRunTags(){
		m_oaItems.SetGrown(8192);			//��8k����
	};
	~CRunTags(){};

public:
	void	Init(CTagDefTab *pTagDefTag); //��ʼ��
	void	Simulate(long ltime,CRdRec rec[],int nsize,DRCB_OnDataOk FunOnDataOk,DRHANDLE h); //ģ��
	void	Clear()
	{
		m_oaItems.DeleteObjects();
	};
	
protected:
	CSortObArray	m_oaItems;	//��ǩֵ����

};
class CSimCfg // ���ò���
{
public:
	CSimCfg()
	{
		m_bAllowAcce = FALSE;
		m_span = 1000;
		m_timeincr = 1;
		m_starttime = COleDateTime::GetCurrentTime();
		COleDateTime et(m_starttime.GetYear() , 
			m_starttime.GetMonth(),
			m_starttime.GetDay(), 
			m_starttime.GetHour(), 
			m_starttime.GetMinute(), 
			m_starttime.GetSecond() -1);
		m_endtime   = et;

	};
	~CSimCfg(){};

	void SetModify(BOOL bModify){m_bModify = bModify;};
	BOOL IsModify(){return m_bModify;};

	void	Serialize(CArchive& ar, long lVersion)
	{
		if(ar.IsLoading())
		{

			ar >> m_span;
			ar >> m_timeincr;
			//ar >> m_starttime;
			//ar >> m_endtime;

			if (lVersion == 71022802)
				ar >> m_bAllowAcce;
			else
				m_bAllowAcce = FALSE;
		}
		else
		{
			ar << m_span;
			ar << m_timeincr;
			//ar << m_starttime;
			//ar << m_endtime;

			ar << m_bAllowAcce;
		}
	};

public:
	BOOL m_bModify;
	BOOL m_bAllowAcce;
	int m_span;//��������
	int m_timeincr;//ʱ������
	COleDateTime m_starttime;//��ʼʱ��
	COleDateTime m_endtime; //����ʱ��
};

#define UPDATEVALTAGS		1000 //ÿ�θ��µı�ǩֵ����
class CSimDrvObj : public CDrvObj
{
public:
	CSimDrvObj();
	virtual ~CSimDrvObj();

public:
	CTagDefTab		m_tagtab;	//��ǩ��
	CSimCfg         m_simcfg;//���ò���
protected:
	CRunTags		m_runtags;	//��ǩ����
	CRdRec			m_recs[UPDATEVALTAGS];
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
	CSimDrvObj* m_pdrv;
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