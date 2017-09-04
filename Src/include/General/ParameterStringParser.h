///////////////////////////////////////////////////////////////////
//   �����ַ���������
//
//   �õ�Ԫ��װ�˲������ַ����Ľ����빹�칦�ܡ�
//   ���ǶԲ������ַ�����ʽ�������£�
//
//       Param1=Value1;Param2=Value2;....
//
//       �����ַ����Ĳ�����һϵ�е�Param��Value��ɣ�Param��Value
//   ֮���õȺŷָ����������֮ǰ�÷ֺŷָ���������Paramֻ����ʹ�ú�
//   �֡����ֻ�Ӣ����ĸ������ֵValue���ر�Լ����
//       Param��ValueԼ�����ô���255���ַ�
//
//
//   ���ߣ�����
//   tech
// 
///////////////////////////////////////////////////////////////////
//   ������ʷ:
// 
//   @2010-11-28
//      �����ĵ�
///////////////////////////////////////////////////////////////////
#ifndef __PARAMETER_STRING_PARSER_H__
#define __PARAMETER_STRING_PARSER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


TRUNK_NAMESPACE_BEGIN

class _GENERAL_CLASS CParameterStringParser
{
public:
	CParameterStringParser(LPCTSTR lpszParameterString = NULL);
	virtual ~CParameterStringParser();

public:
	BOOL SetParameterString(LPCTSTR lpszParameterString);
	CString GetParameterString();
	
	int GetCount() const {
		return (int)m_mapValues.GetCount();
	}
	BOOL IsEmpty() const {
		return (GetCount() == 0);
	}
	
	BOOL Lookup(LPCTSTR lpszParam, CString& szValue) const;
	void RemoveAll();
	void Remove(LPCTSTR lpszParam);

	CString& operator []( LPCTSTR lpszParam );
	int GetValueByInt(LPCTSTR lpszParam, int nDefault = 0);
	long GetValueByLong(LPCTSTR lpszParam, long nDefault = 0);
	float GetValueByFloat(LPCTSTR lpszParam, float fDefault = 0.0);
	double GetValueByDouble(LPCTSTR lpszParam, double dbDefault = 0.0);
	CString GetValueByString(LPCTSTR lpszParam, LPCTSTR lpszDefault = _T(""));
	void SetValue(LPCTSTR lpszParam, int nValue);
	void SetValue(LPCTSTR lpszParam, long nValue);
	void SetValue(LPCTSTR lpszParam, float nValue);
	void SetValue(LPCTSTR lpszParam, double nValue);
	void SetValue(LPCTSTR lpszParam, LPCTSTR lpszValue);

protected:
	BOOL DoParse();
	BOOL DoParseLine(LPCTSTR lpszLine, int nSeparator);
	void EncodeString(LPCTSTR lpszStr, CString &szEncodeStr); // �����ַ���

private:
	CString				m_szParameterString;
	CMapStringToString	m_mapValues;
};


TRUNK_NAMESPACE_END

#endif // end of __PARAMETER_STRING_PARSER_H__
