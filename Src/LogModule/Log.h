#pragma once
#include "stdafx.h"
#include "LogItem.h"
#include <afxmt.h>
#include "log_global.h"
#include "WriteLog.h"

namespace KIMI
{
	class _LOG_CLASS CLog
	{
	public:
		static CLog* GetInstance();
		static CLog *instance;
	private:
		CLog();
		virtual ~CLog();

	public:
		DWORD GetAllowLogLevel() {
			return m_dwAllowLogLevel;
		}
		void SetAllowLogLevel(DWORD dwAllowLogLevel) {
			m_dwAllowLogLevel = dwAllowLogLevel;
		}
		int GetMaxLogCount() {
			return m_nMaxLogCount;
		}
		void SetMaxLogCount(int nCount) {
			m_nMaxLogCount = nCount;
		}
		int GetLogCount() {
			return m_nLogCount;
		}
		int GetMaxLogFreeCount() {
			return m_nMaxLogFreeCount;
		}
		void SetMaxLogFreeCount(int nCount) {
			m_nMaxLogFreeCount = nCount;
		}
		int GetLogFreeCount() {
			return m_nLogFreeCount;
		}
		CLogItem *GetLogItem();
		void FreeLogItem(CLogItem *pItem);

	public:
		void AddSeparator(LPCTSTR lpszText = NULL);
		void AddSeparator(DWORD dwLogOptType, LPCTSTR lpszText = NULL);

		void AddDebug(LPCTSTR lpszLogMessage);
		void AddDebug(DWORD dwLogOptType, LPCTSTR lpszLogMessage);
		void AddDebugFmt(LPCTSTR lpszLogFormat, ...);
		void AddDebugFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...);

		void AddInfo(LPCTSTR lpszLogMessage);
		void AddInfo(DWORD dwLogOptType, LPCTSTR lpszLogMessage);
		void AddInfoFmt(LPCTSTR lpszLogFormat, ...);
		void AddInfoFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...);

		void AddWarning(LPCTSTR lpszLogMessage);
		void AddWarning(DWORD dwLogOptType, LPCTSTR lpszLogMessage);
		void AddWarningFmt(LPCTSTR lpszLogFormat, ...);
		void AddWarningFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...);

		void AddError(LPCTSTR lpszLogMessage);
		void AddError(DWORD dwLogOptType, LPCTSTR lpszLogMessage);
		void AddErrorFmt(LPCTSTR lpszLogFormat, ...);
		void AddErrorFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...);

		void AddLogMessage(LPCTSTR lpszLogMsg, DWORD dwLogLevel, DWORD dwLogOptType, BOOL bCheckLogLevel = TRUE);

		BOOL Write2Disk(CString &szContent);
		CString InitLogPath(LPCTSTR lpszPath, LPCTSTR lpszModule);
	protected:
		CLogItem *GetFreeLogItem();

	private:
		/// �����̰߳�ȫ���ٽ�������
		CCriticalSection m_csLock;
		/// ������־����
		DWORD			m_dwAllowLogLevel;
		/// ���������־��
		int				m_nMaxLogCount;
		/// ��ǰ��־����
		int				m_nLogCount;
		/// ��־�������ͷ
		CLogItem		*m_pHead;
		/// ��־�������β
		CLogItem		*m_pTail;

		/// ������������־��
		int				m_nMaxLogFreeCount;
		/// δ��ʹ�õ���־������
		int				m_nLogFreeCount;
		/// δ��ʹ�õ���־������
		CLogItem		*m_pFreeHead;
		CString			m_szLogPath;
		CWriteLog       m_writelog;
	};
};
