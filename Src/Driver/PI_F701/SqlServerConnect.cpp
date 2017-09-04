#include "StdAfx.h"
#include "SqlServerConnect.h"
#include "pidbdrv.h"
#include "TagValue.h"
#include "..\..\Common\HashTableUtils.h"
#include "..\..\Common\TimeUtils.h"
#include "..\..\Common\DebugUtils.h"
#include "..\..\Common\DBDrvConnectString.h"

/*
������Ϣ���£���Ϊ���У���һ��ΪODBC API������Ϣ���ڶ���ΪODBC����������Ϣ
�ж϶Ͽ�����ֻ���жϵ�һ��StateֵΪ
08001	�޷����ӵ�����Դ,һ����Open DBʱ�Ĵ�����Ϣ
08003	����δ��
08007	��ִ������Ĺ���������ʧ��
08S01	ͨѶ����ʧ�ܣ�һ���ڴ򿪼�¼�������ֱ��ִ��SQL���ʱ����

State:08S01,Native:11,Origin:[Microsoft][ODBC SQL Server Driver][TCP/IP Sockets]
State:01000,Native:10054,Origin:[Microsoft][ODBC SQL Server Driver][TCP/IP Sockets]

SQLSTATE	ODBC API���������������������
00000	�ɹ�
01000	���澯��
01002	�Ͽ����Ӵ���
01004	���ݱ��ض�
01006	δ�ϳ���Ȩ
01S00	��Ч�����ַ�������
01S01	���еĴ���
01S02	ѡ��ֵ�Ѹ���
01S03	δ���»�ɾ���κ���
01S04	���»�ɾ���˶���
01S05	ȡ������������ FreeStmt/Close
01S06	�跨�ڽ�����ص�һ���м�֮ǰ������ȡ
07001	������������
07006	Υ�������Ƶ�������������
07S01	Ĭ�ϲ���ʹ����Ч
08001	�޷����ӵ�����Դ
08002	��������ʹ��
08003	����δ��
08004	����Դ�ܾ���������
08007	��ִ������Ĺ���������ʧ��
08S01	ͨѶ����ʧ��
21S01	����ֵ�б������б�ƥ��
21S02	������ĳ̶������б�ƥ��
22001	�ַ��������ҽض�
22002	��Ҫָʾ����������δ�ṩ
22003	����ֵ������Χ
22005	����ʱ����
22008	Datetime �ֶ����
22012	���������
22026	�ַ������ݣ����Ȳ�ƥ��
23000	Υ��������Լ��
24000*	��Ч���α�״̬
25000	��Ч������״̬
28000	��Ч����Ȩ�淶
34000	��Ч���α�����
37000	�﷨�����Υ�����
3C000	�ظ����α�����
40001	���д���
42000	�﷨�����Υ�����
70100	������ֹ
IM001	��������֧�ֱ�����
IM002	δ�ҵ�Դ���Ʋ���δָ��Ĭ����������
IM003	�޷�װ��ָ������������
IM004	��������� SQLAllocEnv ʧ��
IM005	��������� SQLAllocConnect ʧ��
IM006	��������� SQLSetConnectOption ʧ��
IM007	δָ������Դ���������򣻽�ֹ�Ի�
IM008	�Ի�ʧ��
IM009	�޷�װ��ת�� DLL
IM010	����Դ���ƹ���
IM011	�����������ƹ���
IM012	DRIVER �ؼ����﷨����
IM013	�����ļ�����
S0001	�������ͼ�Ѵ���
S0002	δ�ҵ�����
S0011	�����Ѿ�����
S0012	δ�ҵ�����
S0021	���Ѵ���
S0022	δ�ҵ���
S0023	����Ĭ��ֵ
S1000	�������
S1001	�ڴ����ʧ��
S1002	�б����Ч
S1003	�������ͳ�����Χ
S1004	SQL �������ͳ�����Χ
S1008	��ȡ������
S1009	����ֵ��Ч
S1010	�������д���
S1011	�����ڴ�ʱ��Ч
S1012	��ָ�����������������Ч
S1015	�޿��õ��α�����
S1090	�ַ����򻺳���������Ч
S1091	���������ͳ�����Χ
S1092	ѡ�����ͳ�����Χ
S1093	���������Ч
S1094	С��λ��ֵ��Ч
S1095	�������ͳ�����Χ
S1096	��Ϣ���ͳ�����Χ
S1097	�����ͳ�����Χ
S1098	���������ͳ�����Χ
S1099	��Ϊ�����ͳ�����Χ
S1100	Ψһ��ѡ�����ͳ�����Χ
S1101	׼ȷ��ѡ�����ͳ�����Χ
S1103	����ѡ�����Χ
S1104	����ֵ��Ч
S1105	����������Ч
S1106	��ȡ���ͳ�����Χ
S1107	��ֵ������Χ
S1108	����ѡ�����Χ
S1109	�α�λ����Ч
S1110	�������������Ч
S1111	��ǩֵ��Ч
S1C00	���������޷�ִ��
S1DE0	ִ��ֵ����ʱ������
S1T00	��ʱʧЧ
*/

CString StrToQuoteQuery(LPCTSTR lpszSql, TCHAR chQuote/* = _T('\'')*/)
{
	CString szt;
	szt.AppendChar(chQuote);

	TCHAR ch = *lpszSql;
	while(ch != '\0')
	{
		szt.AppendChar(ch);

		if (ch == chQuote)
			szt.AppendChar(chQuote);

		lpszSql++;
		ch = *lpszSql;
	}
	szt.AppendChar(chQuote);

	return szt;
}

CSqlServerConnect::CSqlServerConnect()
:	m_bAutoCommit(TRUE)
{	
	m_db.otl_initialize(TRUE); // ���̰߳�ȫģʽ��ʼ��OCI����

#ifdef OTL_STREAM_POOLING_ON
	//m_db.set_stream_pool_size(2);
	// set the maximum stream pool size and actually initializes
	// the stream pool.
	// if this function is not called, the stream pool
	// gets initialized anyway, with the default size of 32 entries.
#endif
}

CSqlServerConnect::~CSqlServerConnect()
{
	Disconnect();
}

DWORD CSqlServerConnect::Error2DBPError(otl_exception& p)
{
	CString szSQLState(p.sqlstate);

	/*
	08001	�޷����ӵ�����Դ,һ����Open DBʱ�Ĵ�����Ϣ
	08003	����δ��
	08007	��ִ������Ĺ���������ʧ��
	08S01	ͨѶ����ʧ�ܣ�һ���ڴ򿪼�¼�������ֱ��ִ��SQL���ʱ����
	*/

	if ((szSQLState == "08S01") ||
		(szSQLState == "08001") ||
		(szSQLState == "08003") ||
		(szSQLState == "08007")
		)
		return DBERR_CONNECT;
	else
		return DBERR_FAIL;
}

BOOL CSqlServerConnect::Connect(LPCTSTR lpszInstance, LPCTSTR lpszServer, UINT nPort, LPCTSTR lpszDatabase, LPCTSTR lpszUser, LPCTSTR lpszPassword, BOOL bAutoCommit/* = TRUE*/)
{
	m_bAutoCommit = bAutoCommit;

	CString szConnectString;

	if (!lpszInstance || !*lpszInstance)
		szConnectString.Format("DRIVER={SQL Server};DSN='';Server=%s;Database=%s;Network=DBMSSOCN;Address=%s,%d;UID=%s;PWD=%s",
			lpszServer,
			lpszDatabase,
			lpszServer,
			nPort,
			lpszUser,
			lpszPassword
			);
	else
		szConnectString.Format("DRIVER={SQL Server};DSN='';Server=%s\\%s;Database=%s;Network=DBMSSOCN;Address=%s,%d;UID=%s;PWD=%s",
			lpszServer, 
			lpszInstance, 
			lpszDatabase,
			lpszServer,
			nPort,
			lpszUser,
			lpszPassword
			);

	try
	{
		//m_db.rlogon(szConnectString, (int)bAutoCommit, NULL, NULL, true);
		OutputDebug(_T("CSqlServerConnect: %s)"), szConnectString);

		m_db.rlogon(szConnectString, (int)bAutoCommit);

		return IsConnected();
	}
	catch (otl_exception& p)
	{
		OutputDebug(_T("CSqlServerConnect::Connect error(msg:%s, stm_text: %s, sqlstate: %s, var_info: %s)"), p.msg, p.stm_text, p.sqlstate, p.var_info);
		return FALSE;
	}
	catch(...)
	{
		// ����δ֪ԭ�򣬻ᷢ�������쳣��
		// OraOciMTDrv!otl_conn::server_attach+0xc1 [d:\workspace\sisdbdrv\trunk\thirdparty\otl\include\otlv4.h @ 22878]

		//m_pdrv->AddError(LOG_OPT_DISPLAY, _T("�޷�����Oracle������Oracle������δ�������������ô���"));
		OutputDebug(_T("CSqlServerConnect::Connect error: Unknown error"));
		return FALSE;
	}
}

void CSqlServerConnect::Disconnect()
{
	try
	{
		m_db.logoff();
	}
	catch (otl_exception& p)
	{
		OutputDebug(_T("CSqlServerConnect::Disconnect error(msg:%s, stm_text: %s, sqlstate: %s, var_info: %s)"), p.msg, p.stm_text, p.sqlstate, p.var_info);
	}
	catch(...)
	{
		// ����δ֪ԭ�򣬻ᷢ�������쳣��
		// OraOciMTDrv!otl_conn::server_attach+0xc1 [d:\workspace\sisdbdrv\trunk\thirdparty\otl\include\otlv4.h @ 22878]

		OutputDebug(_T("CSqlServerConnect::Connect error: Unknown error"));
	}
}

BOOL CSqlServerConnect::ExecSQL(LPCTSTR lpszSQL, int *pRowsAffected/* = NULL*/)
{
	try
		{
		long lret = otl_cursor::direct_exec(
			m_db,							// OCI������
			lpszSQL,						// SQL���
			otl_exception::enabled			// ��ֹOTL�쳣
			);

		if (pRowsAffected)
			*pRowsAffected = lret;

		return TRUE;
	}
	catch (otl_exception& p)
	{
		OutputDebug(_T("CSqlServerConnect::ExecSQL error(msg:%s, stm_text: %s, sqlstate: %s, var_info: %s)"), p.msg, p.stm_text, p.sqlstate, p.var_info);
		return FALSE;
	}
	catch(...)
	{
		OutputDebug(_T("CSqlServerConnect::ExecSQL error: Unknown error"));
		return FALSE;
	}
}

BOOL CSqlServerConnect::SaveBlob(LPCTSTR lpszSQL, void *pBlob, int nLen)
{
	try
	{
		otl_stream o(1, // buffer size has to be set to 1 for operations with LOBs
			lpszSQL,	// SQL statement
			m_db // connect object
			);

		otl_lob_stream blob;

		o.set_commit(0); // ��ֹ�Զ��ύ

		o << blob; // Ϊд������ʼ��otl_lob_stream

		blob.set_len(nLen);
		if (nLen > 0)
		{
			otl_long_string buf(pBlob, nLen);
			blob << buf;
		}

		blob.close();
		m_db.commit();

		if (m_bAutoCommit)
			m_db.auto_commit_on();
		else
			m_db.auto_commit_off();

		return TRUE;
	}
	catch (otl_exception& p)
	{
		// �ڴ���ع�ʱ���ܻᷢ���쳣������ʹ��Ƕ��try-catch������
		try
		{
			m_db.rollback();

			if (m_bAutoCommit)
				m_db.auto_commit_on();
			else
				m_db.auto_commit_off();

			return DBERR_FAIL;
		}
		catch(...)
		{
			//OutputDebug(_T("COdbcDrv::WriteHis error: Unknown error"));
			return FALSE;
		}

		OutputDebug(_T("CSqlServerConnect::SaveBlob error(msg:%s, stm_text: %s, sqlstate: %s, var_info: %s)"), p.msg, p.stm_text, p.sqlstate, p.var_info);
		return FALSE;
	}
	catch(...)
	{
		if (m_bAutoCommit)
			m_db.auto_commit_on();
		else
			m_db.auto_commit_off();

		// ����δ֪ԭ�򣬻ᷢ�������쳣��
		// OraOciMTDrv!otl_conn::server_attach+0xc1 [d:\workspace\sisdbdrv\trunk\thirdparty\otl\include\otlv4.h @ 22878]

		OutputDebug(_T("CSqlServerConnect::SaveBlob error: Unknown error"));
		return FALSE;
	}
}

long CSqlServerConnect::GetTimeZone()
{
	long lTimeZone = 28800; // Ĭ��+8ʱ��

	try
	{
		otl_stream o(1, "SELECT DATEDIFF(second,GetUTCDate(),GetDate()) AS TimeZone", m_db);

		o.set_commit(0);

		if (!o.eof())
		{
			o >> lTimeZone;
		}

		return lTimeZone;
	}
	catch (otl_exception& p)
	{
		OutputDebug(_T("CSqlServerConnect::GetTimeZone error(msg:%s, stm_text: %s, sqlstate: %s, var_info: %s)"), p.msg, p.stm_text, p.sqlstate, p.var_info);
		return lTimeZone;
	}
	catch(...)
	{
		OutputDebug(_T("CSqlServerConnect::GetTimeZone error: Unknown error"));
		return lTimeZone;
	}
}

BOOL CSqlServerConnect::GetLastCount(int nID, int& iCount)
{
	CString strSQL;
	strSQL.Format("select top 1 ZDS from T_Chen where id=\'%04d\' order by ltime desc", nID);

	try
	{
		otl_stream o(1, // buffer size has to be set to 1 for operations with LOBs
			strSQL,	// SQL statement
			m_db // connect object
			);

		if (!o.eof())
		{
			o >> iCount;
		}
		return TRUE;
	}
	catch (otl_exception& p)
	{
		OutputDebug(_T("CSqlServerConnect::GetTimeZone error(msg:%s, stm_text: %s, sqlstate: %s, var_info: %s)"), p.msg, p.stm_text, p.sqlstate, p.var_info);
		return FALSE;
	}
	catch(...)
	{
		OutputDebug(_T("CSqlServerConnect::GetTimeZone error: Unknown error"));
		return FALSE;
	}
}


BOOL CSqlServerConnect::GetLastCountTime(int nID, int& iCount, long& lTime)
{
	CString strSQL;
	strSQL.Format("select top 1 ZDS, ltime from T_Chen where id=\'%04d\' order by ltime desc", nID);

	try
	{
		otl_stream o(1, // buffer size has to be set to 1 for operations with LOBs
			strSQL,	// SQL statement
			m_db // connect object
			);

		if (!o.eof())
		{
			o >> iCount;
			o >> lTime;
		}
		return TRUE;
	}
	catch (otl_exception& p)
	{
		OutputDebug(_T("CSqlServerConnect::GetTimeZone error(msg:%s, stm_text: %s, sqlstate: %s, var_info: %s)"), p.msg, p.stm_text, p.sqlstate, p.var_info);
		return FALSE;
	}
	catch(...)
	{
		OutputDebug(_T("CSqlServerConnect::GetTimeZone error: Unknown error"));
		return FALSE;
	}
}