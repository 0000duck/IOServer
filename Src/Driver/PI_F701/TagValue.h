// TagValue.h: interface for the CTagValue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TAGVALUE_H__317DE317_86D4_4F17_BD6D_D3B00B7A993F__INCLUDED_)
#define AFX_TAGVALUE_H__317DE317_86D4_4F17_BD6D_D3B00B7A993F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include <afxmt.h>
#include "../sdk/dbsdk.h"

typedef struct t_odbctagval	//��ǩֵ
{
	TVVAL	m_val;		//ֵ
	long    m_lid;		//��ǩID
	long	m_lres;		//����
}TODBCTAGVAL,*LPODBCTAGVAL; //sizeof() = 32

class CSqlServerDrv;

class COdbcTagVal : public TODBCTAGVAL
{
public:
	COdbcTagVal()
	{
		Init(DT_VOID);
	};
	COdbcTagVal(int ntype)
	{
		Init(ntype);
	};
	~COdbcTagVal()
	{
		Clear();
	};
public:
	void Init(int ntype)
	{
		GetVval().Init(ntype);
		m_lid = 0;
		m_lres = 0;
	};
	void Clear()
	{
		GetVval().Clear();
		m_lres = 0;
	};
	COdbcTagVal& operator=( COdbcTagVal& ov)
	{
		GetVval() = ov.GetVval();	//ֵ
		m_lid = ov.m_lid;
		m_lres = ov.m_lres;
		return *this;
	}; //����=����
	COdbcTagVal& operator=( CTagVal& tv)
	{
		GetVval() = tv.GetVval();//ֵ
		m_lres = 0;
		return *this;
	}; //����=����
	COdbcTagVal& operator=( CVarVal& vv)
	{
		GetVval() = vv;//ֵ
		m_lres = 0;
		return *this;
	}; //����=����
	void SetToTagVal(CTagVal *ptv)
	{
		ptv->GetVval() = GetVval();
	};
	
	CVarVal& GetVval()
	{
		return *((CVarVal*)&m_val);
	};
};

//typedef CArray<COdbcTagVal*,COdbcTagVal*> aodbcval;
//��ǩֵʹ�õĻ���
class CTvObj
{
public:
	CTvObj();
	virtual ~CTvObj();
public:
	CString m_szname;//��ǩ��
	long	m_lid;	//��ǩID
	BOOL	m_bsave; //�Ƿ����ʷ
	int		m_ntype; //��������
	int     m_ncomptype;//ѹ������
	double  m_fcompval;//ѹ������

	CVarVal m_valreal;      //��ǰʵʱֵ
	CVarVal m_valhis;       //��������ʷֵ
	BOOL    m_bRealUpdate;  //ʵʱ���±�־
	BOOL	m_bHisUpdate;	//��ʷ���±�־

	long	m_lLastDoTime; // �����ʱ��
public:
	BOOL	GetRealVal(COdbcTagVal* pval);//ȡ����ʵʱֵ
	BOOL	GetHisVal(COdbcTagVal* pval);//ȡ������ʷֵ
	virtual void AddReal(LPVVAL pval) = 0;//���ʵʱֵ,��ѹ����Ҫѹ��

	virtual BOOL GetLastHisVal(COdbcTagVal* pval){return FALSE;};//ȡ������ʷֵ
};

class CTv_digital : public CTvObj //Digital
{
public:
	CTv_digital();
	virtual ~CTv_digital();
public:
	virtual void AddReal(LPVVAL pval);//���ʵʱֵ,��ѹ����Ҫѹ��
};

class CTv_int32 : public CTvObj //int32
{
public:
	CTv_int32();
	virtual ~CTv_int32();
public:
	virtual void AddReal(LPVVAL pval);//���ʵʱֵ,��ѹ����Ҫѹ��
};

class CTv_int64 : public CTvObj //int64
{
public:
	CTv_int64();
	virtual ~CTv_int64();
public:
	virtual void AddReal(LPVVAL pval);//���ʵʱֵ,��ѹ����Ҫѹ��
};

class CTv_string : public CTvObj //string
{
public:
	CTv_string();
	virtual ~CTv_string();
public:
	virtual void AddReal(LPVVAL pval);//���ʵʱֵ,��ѹ����Ҫѹ��
};

class CTv_blob : public CTvObj //blob
{
public:
	CTv_blob();
	virtual ~CTv_blob();
public:
	virtual void AddReal(LPVVAL pval);//���ʵʱֵ,��ѹ����Ҫѹ��
};

class CTv_float32 : public CTvObj //float32
{
public:
	CTv_float32();
	virtual ~CTv_float32();
	int m_nrecs;//��ǰѹ�����������ݸ���
	CVarVal m_valtmp[6]; //6��ѹ��������
public:
	virtual void AddReal(LPVVAL pval);//���ʵʱֵ,��ѹ����Ҫѹ��
	virtual BOOL GetLastHisVal(COdbcTagVal* pval);//ȡ������ʷֵ
};

class CTv_float64 : public CTvObj //float64
{
public:
	CTv_float64();
	virtual ~CTv_float64();
	int m_nrecs;//��ǰѹ�����������ݸ���
	CVarVal m_valtmp[6]; //6��ѹ��������
public:
	virtual void AddReal(LPVVAL pval);//���ʵʱֵ,��ѹ����Ҫѹ��
	virtual BOOL GetLastHisVal(COdbcTagVal* pval);//ȡ������ʷֵ
};
typedef CArray<COdbcTagVal*,COdbcTagVal*> CArrayOdbcVal;

class CMemRdb //�ڴ�ʵʱ�⣬������ʷ
{
public:
	CMemRdb();
	~CMemRdb();
	
	CMap<CString,LPCTSTR,CTvObj*,CTvObj*> m_map;
protected:
	CCriticalSection m_Lock; //����״̬m_bConnected����
public:
	int GetCount() { return m_map.GetCount(); }
	void InitHashSize(int nHashSize) { m_map.InitHashTable(nHashSize); }
	CTvObj *GetTag(LPCTSTR lpszTagName) {
		CTvObj *pt;
		if (m_map.Lookup(lpszTagName, pt))
			return pt;
		else
			return NULL;
	}
	void AddTag(LPDRVTAGATTR ptd); //��ӱ�ǩ
	void AddRealVal(LPCTSTR sname,LPVVAL pval); //��ӱ�ǩֵ

	void GetSaveVals(CArrayOdbcVal* par,CArrayOdbcVal* pah); //ȡ��Ҫ���̵�ʵʱ����ʷ,����֮ǰpar��pahҪ��������SetSize(0,1024);

	void RemoveTag(LPCTSTR lpszTagName);
	void Clear();//���

};

#endif // !defined(AFX_TAGVALUE_H__317DE317_86D4_4F17_BD6D_D3B00B7A993F__INCLUDED_)
