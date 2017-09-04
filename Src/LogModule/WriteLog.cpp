// WriteLog.cpp
#include "stdafx.h"
#include "WriteLog.h"
#include "General\Utility.h"

using namespace KIMI;

CWriteLog::CWriteLog()
{
	m_szPath = "";
	m_szCurFile = "";
	m_nFileNo = 1;
	m_dwSizeMax = DEFAULT_LOGFILE_SIZE_MAX;
	m_lLastdiskerrtime = 0;
}

CWriteLog::~CWriteLog()
{
	
}

BOOL CWriteLog::Init(LPCTSTR lpszPath, CString szModule, DWORD dwLogFileSizeMax/* = MAX_LOGFILE_SIZE*/)
{
	m_dwSizeMax = dwLogFileSizeMax;
	m_szPath = lpszPath;
	Utility::MakePath(m_szPath); //��ʽ��Ŀ¼

	if(!Utility::CreateDir(m_szPath))
		return FALSE;

	//ɨ�������ļ����ļ�����ʽΪ������-%03d
	CString szFilter = m_szPath;
	szFilter += "*.log";
	CFileFind ff;
	CTime time = 0,timenew = 0;
	CString szFile="";
	BOOL bFirst=TRUE;
	BOOL bFind = ff.FindFile(szFilter);
    while ( bFind )
    {
        bFind = ff.FindNextFile();
        if ( ff.IsDots() )
            continue;
		if ( ff.IsDirectory() )
			continue;

		if(ff.GetCreationTime( time )) //ȡ����ʱ��
		{
			if(bFirst)
			{
				timenew = time;
				bFirst = FALSE;
				szFile = ff.GetFileName();
			}
			else
			{
				if(time > timenew)
				{
					timenew = time;
					szFile = ff.GetFileName();
				}
			}
		//	TRACE("%s = %04d-%02d-%02d %02d:%02d:%02d\n",ff.GetFileName(),
		//		time.GetYear(),time.GetMonth(),time.GetDay(),
		//		time.GetHour(),time.GetMinute(),time.GetSecond());
		}
    }
    ff.Close();
	CTime today = CTime::GetCurrentTime();
	if(szFile.IsEmpty()) //û���ļ�
	{
		m_szCurFile.Format("%s%04d%02d%02d-%04d.log",szModule, today.GetYear(),today.GetMonth(),today.GetDay(),m_nFileNo);
		return TRUE;
	}
	
	CString szday;
	szday.Format("%s%04d%02d%02d",szModule, today.GetYear(),today.GetMonth(),today.GetDay());
	if(szFile.Left(8) != szday) //���ǵ����ļ�
	{
		m_szCurFile.Format("%s%04d%02d%02d-%04d.log",szModule, today.GetYear(),today.GetMonth(),today.GetDay(),m_nFileNo);
		return TRUE;
	}
	int npos = szFile.Find('.');
	if(npos > 0)
	{
		szFile = szFile.Left(npos);
		npos = szFile.Find('-');
		if(npos > 0)
		{
			szFile = szFile.Right(szFile.GetLength() - npos-1);
			m_nFileNo = atoi(szFile);
			if(m_nFileNo <= 0)
				m_nFileNo = 1;
		}
	}
	m_szCurFile.Format("%s%04d%02d%02d-%04d.log", szModule, today.GetYear(),today.GetMonth(),today.GetDay(),m_nFileNo);
	return TRUE;
}

BOOL CWriteLog::WriteStr(CString &szTxt) //дlog
{
	CString szdisk="";
	szdisk = m_szPath.Left(3);

	if(Utility::GetDiskSpace(szdisk) < 50)  //���̿ռ䲻��
	{
		long ltime = (long)::time(NULL);
		if((ltime - m_lLastdiskerrtime) > 300) //5���ӱ���һ��
		{
			OnReport(_T("���̿ռ�С��50M����ֹ��־д��!"), LOG_LEVEL_ERROR, LOG_OPT_ALL);
			m_lLastdiskerrtime = ltime;
		}
		return FALSE;
	}

	CString szfile = m_szPath + m_szCurFile;
	CFile fl;
	BOOL bopen = FALSE;
	try
	{
		if(fl.Open(szfile,CFile::modeReadWrite))
		{
			fl.SeekToEnd();
			fl.Write(szTxt,szTxt.GetLength());
			bopen = TRUE;
		}
		else if(fl.Open(szfile,CFile::modeCreate|CFile::modeWrite))
		{
			fl.Write(szTxt,szTxt.GetLength());
			bopen = TRUE;
		}
	
		if(bopen)
		{
			if(fl.GetLength() >= m_dwSizeMax) //����
			{
				SYSTEMTIME syst;
				::GetLocalTime(&syst);
				m_nFileNo++;
				m_szCurFile.Format("%04d%02d%02d-%04d.txt",syst.wYear,syst.wMonth,syst.wDay,m_nFileNo);
			}
			fl.Close();
			return TRUE;
		}
	}
	catch ( CFileException *ex ) 
	{
	   ex->Delete();
	   return FALSE;
	}
	return FALSE;
}

BOOL CWriteLog::OnReport(LPCTSTR lpszMsg, DWORD nLogLevel, DWORD nLogOptType)
{
	return TRUE;
}
