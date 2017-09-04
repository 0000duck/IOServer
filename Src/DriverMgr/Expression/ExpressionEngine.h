#pragma once

#include "muParser.h"

class CDecVal;

class CExpressionEngine
{
public:
	CExpressionEngine(void);
	CExpressionEngine(BOOL bCalcMode);
	~CExpressionEngine(void);

	/// ��ռ�����ʽ����
	void Clear() {
		;
	}
	/// ��ʼ��������ʽ���棬bCalcMode=TRUE��ʾ���м���ģʽ�����Լ�����ʵֵ��bCalcMode=FALSE��ʾ���б��ʽ���ģʽ��
	/// ���Լ����ʽ�Ƿ���ȷ�����ӱ��ʽ����ȡ�������ı�ǩ��
	void Init(BOOL bCalcMode = TRUE);
	/// ���ü�����ʽ�ַ���
	void SetExpr(LPCTSTR lpszExpr);
	/// ���õ�ǰ�����ǩ
	void SetThisTag(double dblValue, long lTimeMax, short nQualityMax);
	/// ��ֵ���������ʧ�ܣ��򷵻�ֵΪFALSE�����򷵻�ֵΪTRUE��һ��ļ���ʧ��ԭ���Ǳ�ǩ������
	BOOL Eval(double &dblResult);
	/// �����㹫ʽ�Ƿ���ȷ������ȡ���в������ı�ǩ��lpszTagNameΪ��ǰ��ǩ������
	BOOL Check(LPCTSTR lpszTagName);
public:
	CDecVal *m_pdecval;
	double m_dblTagValue;
	long	m_lTimeMax;
	short	m_nQualityMax;

	/// ��ȡ��ǰ������ʽ��ʹ�õı�ǩ����
	static int GetTagCount() { return m_tags.GetCount(); }
	/// ��ղ��������ʽ����ı�ǩ�б�
	static void RemoveAll() { m_tags.RemoveAll(); }
	/// ��ȡ���������ʽ����ı�ǩ�б����
	static CStringArray &GetTags() { return m_tags; }
	/// ��Ӽ�����ʽ��ʹ�õı�ǩ��
	static BOOL AddTag(LPCTSTR lpszTagName) {
		if ((NULL == lpszTagName) || (*lpszTagName == '\0'))
			return FALSE;

		for (int i = 0; i < m_tags.GetCount(); i++)
		{
			if (m_tags[i].CompareNoCase(lpszTagName) == 0)
				return FALSE;
		}
		m_tags.Add(lpszTagName);
		return TRUE;
	}

protected:
	
private:
	mu::Parser m_parser;		// ������ʽ����
	static CStringArray m_tags;	// ���ڼ���ǩ�Ϸ���ʱ��ȡ��ǰ���ʽ��������Щʹ�õı�ǩ
};
