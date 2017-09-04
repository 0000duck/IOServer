#pragma once
#include "tool/SpinLock.h"

class CExpressionEngine;
class CTagVal
{
public:
	CTagVal();
	CTagVal(LPCTSTR lpszName,LPCTSTR lpszNamedb,LPCTSTR lpszExpcp,int ntype,int ncomptype,LPCTSTR scompval);
	CTagVal(int ntype);
	virtual ~CTagVal();

	CTagVal& operator=(const CTagVal& tv); //����=����
public:
	CString m_szName;		//������ǩ��
	CString m_szExpcp;		//ת����ʽ
	CString m_szNamedb;		//Ŀ���������ǩ��
	int		m_nType;		//��������
	int		m_nCompType;	//ѹ����ʽ
	CString m_szCompVal;	//ѹ����
	int		m_nUpdate;		//����
	BOOL	m_bresend;		//5�����ط�
	long	m_lsendtime;	//����ʱ��
	double m_dblcompval;

	CSpinLock m_lock;		//���ڶ�дֵ�̰߳�ȫ��������
	CVarVal m_val;			//ֵ

	CExpressionEngine *m_pEngine;
};

