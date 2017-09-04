#pragma once
#include "TagVal.h"
#include "DrvItem.h"
#include "Dec2TagVal.h"

class CDecVal
{
public:
	CDecVal();

	CDecVal(long lid,int ntags);
	~CDecVal();

public:
	BOOL	m_bvardataok; //�ɱ��������͸���
	CTagVal* GetTagVal(LPCTSTR lpszName);//ȡ��ǩֵָ��	
	int		GetSaveRecs(CTagVal* lprecs,int nsize);//ȡ��������(��������)
	BOOL	GetSaveVarVal(CTagVal* lprecs);//ȡ���̵Ŀɱ����ݣ�string blob
	CString GetDecName(){return m_szDecName;};
protected:
	void	Init(CDrvItem* pdec);//��ʼ��
	void	OnDecDataOK(T_DRREC recs[],int nsize); //����豸ֵ�������ڻص�������ʹ�ã�����
	DWORD	OnWriteHistoryCB(T_DRREC recs[],int nsize);

protected:
	CString m_szDecName;
	long	m_id;
	int		m_nposval;//ȡ�������ݵ�λ��
	int		m_nposvar;//�ɱ�����λ��
	//CMap<CString,LPCTSTR,CTagVal*,CTagVal*> m_map;	//CTagVal* map,�Ա�ǩ��Ϊ����
	CNoCaseMap<CTagVal*,CTagVal*> m_map;	//CTagVal* map,�Ա�ǩ��Ϊ����
	CArray<CTagVal*,CTagVal*> m_array;				//CTagVal*���飬����ѭ��ȡ���浽ʵʱ����
	CMap<CString,LPCTSTR,CDec2TagVal*,CDec2TagVal*> m_mapd2t;//ӳ���,��������ǩ��Ϊ������
protected:
	friend class CDecValMan;
};

