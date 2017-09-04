//cashe.h
#pragma once
#include <afxtempl.h>
#include "../sdk/sdktool.h"
#include "pagefile/pagefile.h"

//ѹ��
#define  TA_COMPNO		0	//��ѹ��
#define  TA_COMPPER		1	//�ٷֱȾ���ѹ��
#define  TA_COMPVAL		2	//����ֵ����ѹ��


///////////////////////////////////////////////////////////////////////////////
class CBaseVal //����ֵ
{
public:
	CBaseVal(){
		m_szName = "";
	};
	~CBaseVal(){};
public:
	CString m_szName; //��ǩ��
	CVarVal m_val;	//ֵ
public:
	void Clear()
	{
		m_szName = "";
		m_val.SetVoid();
	};
};
//��ǩֵʹ�õĻ���
class CTvObj
{
public:
	CTvObj();
	virtual ~CTvObj();
public:
	CString m_szname;//��ǩ��
	int		m_ntype; //��������
	int     m_ncomptype;//ѹ������
	double  m_fcompval;//ѹ������

	CVarVal m_valreal;      //��ǰʵʱֵ
	long	m_llastsave;		//�ϴδ洢
public:
	virtual BOOL AddReal(LPVVAL pval,CBaseVal* pbaseval) = 0;//���ʵʱֵ,��ѹ����Ҫѹ��,����TRUE��ʾpbaseval����д������ֵ
	virtual BOOL GetLastHisVal(CBaseVal* pbaseval){return FALSE;};//ȡ������ʷֵ
};

class CTv_digital : public CTvObj //Digital
{
public:
	CTv_digital();
	virtual ~CTv_digital();
	
	BOOL m_bdataok; //������Ч
public:
	virtual BOOL AddReal(LPVVAL pval,CBaseVal* pbaseval);//���ʵʱֵ,��ѹ����Ҫѹ��,����TRUE��ʾpbaseval����д������ֵ
};

class CTv_int32 : public CTvObj //int32
{
public:
	CTv_int32();
	virtual ~CTv_int32();
	BOOL m_bdataok; //������Ч
public:
	virtual BOOL AddReal(LPVVAL pval,CBaseVal* pbaseval);//���ʵʱֵ,��ѹ����Ҫѹ��,����TRUE��ʾpbaseval����д������ֵ
};

class CTv_int64 : public CTvObj //int64
{
public:
	CTv_int64();
	virtual ~CTv_int64();
	BOOL m_bdataok; //������Ч
public:
	virtual BOOL AddReal(LPVVAL pval,CBaseVal* pbaseval);//���ʵʱֵ,��ѹ����Ҫѹ��,����TRUE��ʾpbaseval����д������ֵ
};

class CTv_string : public CTvObj //string
{
public:
	CTv_string();
	virtual ~CTv_string();
public:
	virtual BOOL AddReal(LPVVAL pval,CBaseVal* pbaseval);//���ʵʱֵ,��ѹ����Ҫѹ��,����TRUE��ʾpbaseval����д������ֵ
};

class CTv_blob : public CTvObj //blob
{
public:
	CTv_blob();
	virtual ~CTv_blob();
public:
	virtual BOOL AddReal(LPVVAL pval,CBaseVal* pbaseval);//���ʵʱֵ,��ѹ����Ҫѹ��,����TRUE��ʾpbaseval����д������ֵ
};

class CTv_float32 : public CTvObj //float32
{
public:
	CTv_float32();
	virtual ~CTv_float32();
	int m_nrecs;//��ǰѹ�����������ݸ���
	CVarVal m_valtmp[6]; //6��ѹ��������
public:
	virtual BOOL AddReal(LPVVAL pval,CBaseVal* pbaseval);//���ʵʱֵ,��ѹ����Ҫѹ��,����TRUE��ʾpbaseval����д������ֵ
	virtual BOOL GetLastHisVal(CBaseVal* pval);//ȡ������ʷֵ
};

class CTv_float64 : public CTvObj //float64
{
public:
	CTv_float64();
	virtual ~CTv_float64();
	int m_nrecs;//��ǰѹ�����������ݸ���
	CVarVal m_valtmp[6]; //6��ѹ��������
public:
	virtual BOOL AddReal(LPVVAL pval,CBaseVal* pbaseval);//���ʵʱֵ,��ѹ����Ҫѹ��,����TRUE��ʾpbaseval����д������ֵ
	virtual BOOL GetLastHisVal(CBaseVal* pval);//ȡ������ʷֵ
};
class CMemCashe;
class CCompCashe //����ѹ��,���ڻ����������͵�ʵʱֵѹ��Ϊ��������ֵ
{
public:
	CCompCashe();
	~CCompCashe();
protected:
	//CMap<CString,LPCTSTR,CTvObj*,CTvObj*> m_map;
	CNoCaseMap<CTvObj*,CTvObj*> m_map;
	
public:
	void Clear();//���
	BOOL AddReal(LPCTSTR sname,int ncomptype,double dblcompval,LPVVAL pval,CBaseVal* pbaseval);//���ʵʱֵ
	void SaveLastVal(CMemCashe* pmc);//�洢���һ��ֵ���˳�ʱʹ�ã�
};

/*
	���ػ����������͵Ļ������ݴ��̴洢��ʽ:
	���ļ���ʽ����3����
	1.�ļ�ͷ
	2.name - ID���ձ�
	3.���ݼ�¼
  */
typedef struct t_cashefilehead //�����������ͱ��ػ����ļ�ͷ����ʽ
{
	DWORD dwflag;//��־ 0xDF23AD79
	long  lnameids; //����-ID���ո���
	long  lvals;	//���ݼ�¼����
	long  ldovals;	//�Ѵ����¼����
}TCAFHEAD,*LPCAFHEAD;

#define MEM_CASHE_SIZE	1000000  //100������¼

/*
�����������͵ı��ػ���ѹ���洢��ƣ�
1���ܶ���ʹ�ã���������DDC��DDS������
2��ѹ���������ֵ��100������¼�洢Ϊһ���ļ�������ռ24M��С��ͷ�����ڴ洢ת����,��5���ǩ���㣬ת����Լռ�ռ�4.4M
  */
class CTagVal;
class CMemCashe //�ڴ滺�棬���ڻ����������͵�����ֵ�洢��������д���ļ���
{
protected:
	struct t_caval
	{
		long	ltime;	//ʱ��
		short	snqa;	//����
		WORD	wtype;	//ֵ
		union 
		{
			long	lval;
			float	fval;
			__int64 llval;
			double	dblval;
		};
		DWORD dwid;		//ID
		DWORD dwres;	//����
	};//sizeof() = 24;
	typedef struct t_nameid
	{
		char sname[80];
		DWORD dwid;
		DWORD dwres;
	}TNAMEID,*LPNAMEID;//sizeof() = 88;
protected:
	//{{����ֵ�洢��
	int m_nGetPos;//ȡ���ͻ����λ�ã�
	int				m_nsize;	//��¼����
	struct t_caval*	m_pvals;	//�洢�ռ� 
	//}}

	//{{ ����IDת����
	DWORD m_dwNextID;	//��һ��ID;
	//CMap <CString,LPCTSTR,LPNAMEID,LPNAMEID> m_mapname;	//��ǩ������
	CNoCaseMap <LPNAMEID,LPNAMEID> m_mapname;	//��ǩ������
	CMap <DWORD,DWORD,LPNAMEID,LPNAMEID> m_mapid;		//ID����
	
	DWORD MakeID(LPCTSTR lpszName);
	//}}
	//{{�Ӵ��̶�������

	CString m_szDiskCasheFile;//���̻����ļ�
protected:
	void ClearMap();//���MAP
public:
	CMemCashe();
	~CMemCashe();

	DWORD GetID(LPCTSTR lpszName);
	CString  GetName(DWORD dwid);
	BOOL SaveToDisk(LPCTSTR lpszPath); //���浽����
	void Reset();		//��λ�����³�ʼ��
	BOOL IsFull();
	BOOL IsEmpty(){return (m_nsize == 0);};
	BOOL AddTagVal(CBaseVal* pbaseval);//�������ֵ

	int  GetLastRecs(CTagVal* lprecs,int nsize);//ȡ���µ�cashe��������cashe����,���ظ���
	void GetPosOk()//ȷ��ȥ���ͻ���
	{
		if(m_nGetPos <= m_nsize)
			m_nsize = m_nGetPos;
	};

	BOOL LoadFromDisk(LPCTSTR lpszPath);//�Ӵ��̶�ȡ���µ�
};

/*
	ʼ�մ򿪵�ǰ���µ�ҳ���ļ�����������ɺ�ر�ɾ�������򿪴��µ�ҳ���ļ������û�д����ļ����������ļ���
	����ʱ�䴴���µ�ҳ���ļ���
	ҳ���ļ�����:������ʱ����.pgf

  ҳ���ļ���С���㣬���ÿ����¼��<=1024�ֽڣ���ô����Ŀ¼(65528��)Լ����С�ռ�16M+64M = 80M
  ���ռ�2G����ʱ��ÿ����¼ƽ�����ȿɴﵽ31K��
  */
class CPfCashe //ҳ���ļ�cashe ����string,blob,soe���ͼ�¼
{
public:
	CPfCashe();
	~CPfCashe();
protected:
	CString m_szPath;	//Ŀ¼
	CPageFile m_pf;		//ҳ���ļ�
protected:
	CString GetLastFile(int &n);//��ȡ���µ�ҳ���ļ����������ļ�����
	
public:
	BOOL Init(LPCTSTR lpszPath);//��ʼ�����������ļ���û���򴴽�
	BOOL SaveRec(CPfRecVal* pvar);//д��
	int			GetLastRec(PF_HANDLE &h,CPfRecVal* prec){
		return m_pf.GetLastRec(h,prec);
	}; //h���ڶ����ͳɹ���ɾ����¼��
	int			GetFirstRec(PF_HANDLE &h,CPfRecVal* prec){
		return m_pf.GetFirstRec(h,prec);
	};
	void DeleteRec(PF_HANDLE h){
		m_pf.DeletePf(h);//ɾ����¼
	};
	void OpenNew();
};

class CLocalCashe //���ػ�����װ��
{
public:
	CLocalCashe();
	~CLocalCashe();
protected:
	//�����������ͻ���
	CCompCashe m_compcashe;	//����ѹ��
	CMemCashe  m_memcashe;	//����ֵ����

	//string blob soe����
	CPfCashe   m_pfcashe;	//�ɱ����ͻ���

	//
	CString m_szPath;			//Ŀ¼
	CCriticalSection m_lock;    //ͬ����
public:
	void SetPath(LPCTSTR lpszPath);
	void AddRealVal(LPCTSTR sname,int ncomptype,double dblcompval,LPVVAL pval);//��ӻ�������ʵʱֵ
	void AddRealVar(LPCTSTR sname,CVarVal *pval);//дString,Blob ��cashe
	void AddSoe(CSoe *psoe);//дSOE��cashe
	//ȡ���µ�cashe��������cashe����,���ظ���
	int  GetLastRecs(CTagVal* lprecs,int nsize);
	//ȷ��ȥ���ͻ���
	void GetPosOk();
	//h���ڶ����ͳɹ���ɾ����¼��
	int	GetLastPfRec(PF_HANDLE &h,CPfRecVal* prec);
	int	GetFirstPfRec(PF_HANDLE &h,CPfRecVal* prec);
	void DeletePfRec(PF_HANDLE h);
	void OpenNewPgeFile();
	//�Ӵ��̶�ȡ���µ�
	BOOL LoadFromDisk();
};