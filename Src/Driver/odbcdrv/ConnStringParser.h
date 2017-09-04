// ConnStringParser.h: interface for the CConnStringParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNSTRINGPARSER_H__9C588319_455A_41F5_B144_780BB80F13AD__INCLUDED_)
#define AFX_CONNSTRINGPARSER_H__9C588319_455A_41F5_B144_780BB80F13AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
 * ���ݿ����ͣ�ʹ�ô���ֵ���壬�Է����Ժ����֧�ָ�������ݿ����ͣ�
 * ���罫�����ܻ������Ҫ����MSSQLϸ��Ϊ DBT_MSSQL2k=1��DBT_MSSQL2005=2��DBT_MSSQL2008=3...
*/
#define DBT_UNKNOWN		0	// δ֪���ݿ�����
#define DBT_MSSQL		1	// Microsoft SQL Server 2000/2005/2008
#define DBT_MYSQL		100	// MySQL
#define DBT_ORACLE		200	// Oracle
#define DBT_SYBASE11	300	// Sybase 11
#define DBT_SYBASE12	301	// Sybase 12
#define DBT_DB2			400	// DB2

class CConnStringParser;

typedef void (*InitParamProc)(CConnStringParser *parser);
typedef CString (*MakeConnStringProc)(CConnStringParser *parser);
typedef BOOL (*ParserValueProc)(CConnStringParser *parser, LPCTSTR lpszKey, LPCTSTR lpszValue);

typedef struct tagDBTYPEDATA {
	int		nTypeID;			// ���ݿ�����ID��DBT_XXXX
	TCHAR	szDriver[64];		// �����������ַ�����DRIVER������
	TCHAR	szName[64];			// ���ݿ����ʾ��
	InitParamProc		procInitParam;		// ��ʼ�������ַ��������ĺ���ָ��
	MakeConnStringProc	procMakeConnStr;	// �Ӳ������������ַ����ĺ���ָ��
	ParserValueProc		procParserValue;
}DBTYPEDATA, *LPDBTYPEDATA;

class CConnStringParser  
{
public:
	CConnStringParser(LPCTSTR lpszConnStr = NULL);
	CConnStringParser(const CString &szConnStr);
	virtual ~CConnStringParser();

public:
	BOOL IsSuccess() { return m_bSuccess; }
	int GetDBType() { return m_nDbType; }
	void SetDBType(int type);
	LPCTSTR GetDBTypeStr();
	LPCTSTR GetConnectString();
	BOOL SetConnectString(LPCTSTR lpszConnStr);
	int GetCount() { return m_arrKeys.GetCount(); }
	LPCTSTR GetKey(int nIdex);
	LPCTSTR GetValue(int nIdex);
	LPCTSTR GetValue(LPCTSTR lpszKey);
	int SetValue(LPCTSTR lpszKey, LPCTSTR lpszValue);
	int SetValue(LPCTSTR lpszKey, int nValue);
	int SetValue(LPCTSTR lpszKey, UINT nValue);
	int SetValue(LPCTSTR lpszKey, long lValue);
	int SetValue(LPCTSTR lpszKey, float fValue);
	int SetValue(LPCTSTR lpszKey, double dbValue);
	int IndexOf(LPCTSTR lpszKey);
	void RemoveAll();

	CConnStringParser& operator=(CConnStringParser &ConnStr);
	LPCTSTR operator[](LPCTSTR lpszKey);

	static LPDBTYPEDATA GetDBTypeData() {
		return m_arrDbTypeData;
	}
	static int GetDBTypeCount() {
		return m_nDbTypeCount;
	}
	
protected:
	void Init();
	void DoParser();
	LPDBTYPEDATA FindDBTypeData(int ntype);
	LPDBTYPEDATA FindDBTypeData(LPCTSTR szDriver);

private:
	BOOL	m_bSuccess;
	int		m_nDbType;
	CString m_szConnStr;

	CStringList	m_arrKeys;
	CStringList	m_arrValues;
	static int m_nDbTypeCount;
	static DBTYPEDATA	m_arrDbTypeData[];
};

// ��ȡ���ص�Oracle���ñ�����ȡ���ݿ������б�
int GetOracleServers(CStringArray &arrServers);

#endif // !defined(AFX_CONNSTRINGPARSER_H__9C588319_455A_41F5_B144_780BB80F13AD__INCLUDED_)
