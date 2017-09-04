#pragma once
#include "DriverMgr_global.h"


class _DRIVERMGR_CLASS CTagItem :
	public CSortItem
{
public:
	CTagItem();
	virtual ~CTagItem();
	virtual		int Compare(CSortItem *pOb);	//����Ƚ�
	void			Serialize(CArchive& ar);	//����
	void			CopyTo(CTagItem *pt);
	static  CString GetTypeStr(int ntype);
	static int		GetTypeVal(LPCTSTR lpszType);
	static  CString GetCompTypeStr(int ntype);
	static int		GetCompTypeVal(LPCTSTR lpszType);
protected:
	void			parsecomp(LPCTSTR lpsztxt);//�ֽ�ѹ������
public:
	int				m_ntype;	//��������
	CString			m_szName;	//��ǩ��
	CString 		m_szdes;	//����
	CString 		m_szunit;	//��λ
	CString 		m_sznamedb;	//��Ŀ��������ж�Ӧ�ı�ǩ��
	CString 		m_sznamedrv;//��Ӧ�����б�ǩ
	CString 		m_szexpcp;	//�ɼ�ֵת��������ֵת����ʽ
	CString 		m_szexppc;	//����ֵת��Ϊ�ɼ�ֵ��ʽ,�ݲ���

	//	CString m_szres;	//���������汣����ѹ�����壬��\n�ֿ�����һ��ѹ����ʽ���ڶ���ѹ���� needcode

	//{{��������
	int				m_nCompType;//ѹ����ʽ
	CString 		m_szCompVal;//ѹ����
	BOOL			m_bSel;		//ѡ��
	int				m_nErrStatus;
	//}}

	long	m_ldecid;//�豸ID modify811
};

