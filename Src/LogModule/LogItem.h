#pragma once

#include "stdafx.h"

namespace KIMI
{
	class CLog;
	class CLogItem
	{
	public:
		friend CLog;

		DWORD GetLogLevel() {
			return m_dwLogLevel;
		}
		DWORD GetLogOptType() {
			return m_dwLogOptType;
		}
		LPCTSTR GetLogMessage() {
			return m_szLogMessage;
		}
		BOOL IsDisplay() {
			return ((m_dwLogOptType & LOG_OPT_DISPLAY) == LOG_OPT_DISPLAY);
		}
		BOOL IsStorage() {
			return ((m_dwLogOptType & LOG_OPT_STORAGE) == LOG_OPT_STORAGE);
		}

	public:
		void SetLogMessage(LPCTSTR lpszLogMsg, DWORD dwLogLevel = LOG_LEVEL_INFO, DWORD dwLogOptType = LOG_OPT_ALL);

	protected:
		BOOL AllocBuffer(size_t nSize);

	private:
		CLogItem();
		~CLogItem();

	private:
		/// ��־��Ϣ����
		DWORD	m_dwLogLevel;
		/// ��־��������
		DWORD	m_dwLogOptType;
		/// ��־����ʱ��
		//CString m_szLogMessage;
		size_t	m_nLogSize;
		TCHAR	*m_szLogMessage;
		/// ��һ����־
		CLogItem *m_pNext;
	};
};
