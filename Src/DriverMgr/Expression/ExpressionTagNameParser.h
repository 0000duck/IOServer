// TagItemObject.h: interface for the CTagItemObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPRESSIONTAGNAMEPARSER_H__5168BBA8_0765_43F2_A2D1_624FD9B6DF64__INCLUDED_)
#define AFX_EXPRESSIONTAGNAMEPARSER_H__5168BBA8_0765_43F2_A2D1_624FD9B6DF64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
 * �ӽű���ʹ��������ʽ�����������а����ı�ǩ���б������¼��������ȷ��
 * ��ǩ�ַ�����

 *  TV("Tag Name")
 */

/*
 * ���ڴ����7�͵�8���������ǩ��ƥ�����ڵ�������
 * GRETA::Regex :  [^a-zA-Z0-9_]*TV[' ']*\([' ']*"(("\\".|[^\\])*?)"
 * CATLRegExp   :  [^a-zA-Z0-9_]*TV\b*\(\b*{(\"[^\"]*\")|(\'[^\']*\')}
 */
#define REGEX_EXPR _T("[^a-zA-Z0-9_]*TV\b*\\(\\b*{(\\\"[^\\\"]*\\\")|(\\'[^\\']*\\')}")

/*
 * ����this����
 * [^a-zA-Z0-9_]{this}[^a-zA-Z0-9_]
 */
#define REGEX_THIS_EXPR _T("[^a-zA-Z0-9_]{this}[^a-zA-Z0-9_]")

// �÷�����δ����ע�Ϳ�����⣬���Խ���ķ�ʽʱ�Ƚ��ַ����е�ע�ʹ���ȥ�����ٽ���������ʽ����
class CExpressionTagNameParser
{
public:
	CExpressionTagNameParser(LPCTSTR lpszScript = NULL);
	~CExpressionTagNameParser();

	int GetCount() { return m_arrTagNames.GetSize(); }
	LPCTSTR GetAt(int nIndex) const;
	void Parse(LPCTSTR lpszScript);
	BOOL IsExists(LPCTSTR lpszTagName);

protected:
	void _AdjustTagName(CString &szTagName);
	BOOL _AddTagName(LPCTSTR lpszTagName);

	BOOL _Match_X(LPCTSTR lpszRegexExpr, LPCTSTR lpszStr, UINT nResultIndex, BOOL bCaseSensitive = FALSE);

private:
	CStringArray m_arrTagNames;
};

#endif // !defined(AFX_EXPRESSIONTAGNAMEPARSER_H__5168BBA8_0765_43F2_A2D1_624FD9B6DF64__INCLUDED_)
