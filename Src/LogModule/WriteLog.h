// WriteLog.h
#ifndef __WRITELOG_H__
#define __WRITELOG_H__

#include "log_global.h"

namespace KIMI
{
#define DEFAULT_LOGFILE_SIZE_MAX	2097152 //���LOG�ļ���С2M 
	class _LOG_CLASS CWriteLog
	{
	public:
		CWriteLog();
		virtual ~CWriteLog();

	public:
		BOOL Init(LPCTSTR lpszPath, CString szModule, DWORD dwLogFileSizeMax = DEFAULT_LOGFILE_SIZE_MAX); //��ʼ��
		BOOL WriteStr(CString &szTxt); //дlog

	protected:
		virtual BOOL OnReport(LPCTSTR lpszMsg, DWORD nLogLevel, DWORD nLogOptType);

	protected:
		CString m_szPath;
		CString m_szCurFile;//��ǰ�ļ�,����·��
		DWORD	m_dwSizeMax;
		int		m_nFileNo;
		long	m_lLastdiskerrtime;
	};
};


#endif // end of __WRITELOG_H__