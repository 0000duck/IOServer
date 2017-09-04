// MyDrvObj.h: interface for the CMyDrvObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYDRVOBJ_H__9D0CA9F6_F8AE_48C3_97F0_7623BFA62115__INCLUDED_)
#define       AFX_MYDRVOBJ_H__9D0CA9F6_F8AE_48C3_97F0_7623BFA62115__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\..\SDK\ddclib.h"
#include "MyFrmParse.h"
#include "xml/tinyxml.h"
#include <vector>
#include <map>
#include <utility>
#include <afxtempl.h>

class CTimeSpanDif
{
private:
	FILETIME		m_ftStartTime;				// ��ʼʱ��
	FILETIME		m_ftEndTime;				// ��ֹʱ��
	int                     m_iStatus;                   // ״̬
public:

	CTimeSpanDif()
	{
		::CoFileTimeNow(&m_ftStartTime);;
	}

	long  GetElapsedTime()
	{
		ULONGLONG qst = *((ULONGLONG*)(&m_ftStartTime));
		ULONGLONG qet = *((ULONGLONG*)(&m_ftEndTime));
		return (long)((qet - qst) / 10000);
	}

	BOOL CheckUpdatePeriod(UINT lTime)
	{	
		::CoFileTimeNow(&m_ftEndTime);
		int iTime = GetElapsedTime();
		if (GetElapsedTime() < lTime)
		{
			Sleep(100);
			return FALSE;
		}
		memcpy(&m_ftStartTime, &m_ftEndTime, sizeof(FILETIME));
		return TRUE;
	}

	BOOL CheckUpdatePeriod(UINT lTime, int iStatus)
	{	
		::CoFileTimeNow(&m_ftEndTime);
		if (GetElapsedTime() < lTime|| m_iStatus != iStatus)
		{
			Sleep(100);
			return FALSE;
		}
		memcpy(&m_ftStartTime, &m_ftEndTime, sizeof(FILETIME));
		return TRUE;
	}
};

class CMyDrvObj : public CDDCDrvObj  
{
public:
	CMyDrvObj();
	virtual ~CMyDrvObj();

public:
	virtual BOOL TestConnect(CConfigInfo &config);
	virtual DRVRET	OnStart();					// ׼������
	virtual	void	OnStop();
protected:
	virtual CDDCDrvProtocol	*CreateProtocol();
	virtual DRVRET GetErrMsg(CString& szMsg,DRVRET dwcode);

	BOOL			m_bListenThreadRun;
	BOOL	        StartThread();
	static UINT		OnListenThread(LPVOID pParam);

};

typedef std::pair<long, long>		    pairAddr;
typedef std::map<pairAddr, SOCKET>		mapSocket;


#define  USE_PROTOCOL_TCP
#ifdef USE_PROTOCOL_TCP 
	class CMyProtocol : public CTCPServerProtocol
#else  
	class CMyProtocol : public CSerialProtocol 
#endif 
{
public:
	CMyProtocol();
	virtual ~CMyProtocol();

public:

	virtual DRVRET  WriteVar(LPDRREC lpval);
	virtual void	DoRun();
	BOOL            IsCanDo();

	long			StrTimeToLong(CString strTime);
	CString			LongTimeToStr(long lTime);
	void			AddTypeXML(TiXmlElement* eType,CString str);
	void			ListenClientEx();
	BOOL            Release();
	mapSocket		m_mapConnect;

	void			OutMsg(BYTE *pData, int iSize,CString strID,float fValue); // ���Ա��� ���ֳ���ȡ

	long            m_lStartTime;//��ʼʱ��
	long            m_lTime;      // ��վʱ��

	CList<SOCKET,SOCKET>              sockList;

	CMyFrmParse  m_objBuffer;

	CCriticalSection m_Lock;//��
private:
	CTimeSpanDif m_tmAutoBreak;          // �Զ��Ͽ����ڣ�����Ӧ�Ը������绷����
};

#endif // !defined(AFX_MYDRVOBJ_H__9D0CA9F6_F8AE_48C3_97F0_7623BFA62115__INCLUDED_)
