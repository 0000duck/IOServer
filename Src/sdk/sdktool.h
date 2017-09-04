//sdktool.h
#pragma once
#include "drvdef.h"

#ifndef DT_VOID
#define DT_VOID		DRT_VOID	//������
#define DT_DIGITAL	DRT_DIGITAL	//������	,Ҳ��4�ֽ�����		
#define DT_INT32	DRT_INT32	//4�ֽ�����
#define DT_FLOAT32	DRT_FLOAT32	//4�ֽڸ�����
#define DT_INT64	DRT_INT64	//8�ֽ�����
#define DT_FLOAT64	DRT_FLOAT64	//8�ֽڸ�����
#define DT_STRING	DRT_STRING	//�ַ���
#define DT_BLOB		DRT_BLOB	//���������ݿ�
#endif

//������
class CVarVal : public TVVAL  //�������
{
public:
	CVarVal();
	CVarVal(DWORD dwtype);
	~CVarVal();
public:
	void ResetType(WORD wtype);
	void Init(DWORD dwtype);
	void ClearData(); //����ɱ䳤�������ڴ�pstr,pblob,����ǰ��֤�Ǿ�����ʼ���ġ�
	void Clear(); //����ɱ䳤�������ڴ�pstr,pblob,����ǰ��֤�Ǿ�����ʼ���ġ�
	void SetVoid();
	CVarVal& operator=(const CVarVal& vv); //����=����
	CVarVal& operator=(const long lv); //����=����
	CVarVal& operator=(const __int64 llv); //����=����
	CVarVal& operator=(const float fv); //����=����
	CVarVal& operator=(const double dblv); //����=����
	CVarVal& operator=(const char* ps); //����=����

	BOOL	 operator==(const CVarVal& vv); //����==����,�Ƚ����


	BOOL	SetBlob(void *pdata,int nlen); //����BLOB����

	int		GetInt();
	__int64 GetInt64();
	long	GetLong(){return GetInt();};
	float   GetFloat();
	double  GetDouble();

	BOOL ChangeType(DWORD dwtype); //�ı�����

	CString GetValStr(); //ȡֵ��ת��Ϊ�ַ�����ʽ
	CString GetTimeStr(long ltimeadjust = 0);
	CString GetQaStr();

protected:
	
	void ToVoid();
	void ToDigital();
	void ToInt32();
	void ToInt64();
	void ToFloat32();
	void ToFloat64();
	void ToString();
	void ToBlob();
};

class CRdRec : public T_DRREC
{
public:
	CRdRec();
	CRdRec(int ntype);
	~CRdRec();
public:
	void SetName(const char* ps);
	void SetVal(TVVAL* pval);
	void SetVoid();
	void ClearData();
	void ReSetType(int ntype){GetVarVal()->ResetType(ntype);};
	CVarVal* GetVarVal(){return (CVarVal*)&val;};

};

class CSoe : public TSOE //�¼�����
{
public:
	CSoe();
	~CSoe();
public:
	void	Init();
	void	Clear(); //�������Ŀɱ䳤������m_pbinargs,����ǰһ��Ҫ��֤�Ǿ�����ʼ����.
	CSoe& operator=(CSoe& soe); //����=����
	void	SetBinArgs(void* pargs,int nsize); //���ö����Ʋ���
};

