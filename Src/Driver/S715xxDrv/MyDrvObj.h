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
#include <vector>
#include <map>
#include <utility>
#include <afxtempl.h>
#include "include/S712xx.h"
#include "S715Def.h"
#include <algorithm>

class CTimeSpanDif
{
private:
	FILETIME		m_ftStartTime;				// 开始时间
	FILETIME		m_ftEndTime;				// 终止时间
	int                     m_iStatus;                   // 状态
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
	virtual DRVRET	OnStart();					// 准备运行
	virtual	void	OnStop();
protected:
	virtual CDDCDrvProtocol	*CreateProtocol();
	virtual DRVRET GetErrMsg(CString& szMsg,DRVRET dwcode);	
};

#define  USE_PROTOCOL_TCP
#ifdef USE_PROTOCOL_TCP 
	class CMyProtocol : public CTCPClientProtocol
#else  
	class CMyProtocol : public CSerialProtocol 
#endif 
{
public:
	CMyProtocol(std::string DevName, std::string DevIP);
	virtual ~CMyProtocol();

public:

	virtual DRVRET  WriteVar(LPDRREC lpval);
	virtual void	DoRun();
	virtual BOOL Connect();
	virtual BOOL IsConnect();
	virtual void Disconnect();
	BOOL            IsCanDo();

	long			StrTimeToLong(CString strTime);
	CString			LongTimeToStr(long lTime);

	long            m_lStartTime;//开始时间
	long            m_lTime;      // 从站时间


	CMyFrmParse  m_objBuffer;

	CCriticalSection m_Lock;//锁
private:
	CTimeSpanDif m_tmAutoBreak;          // 自动断开周期（用于应对复杂网络环境）
	TS7Client *m_pS7Client;

	CVectorTDataBlock m_vectorDataBlocks;

	std::string strConfigPath;
	std::string strSvrIP;
	std::string strDrvName;//上层实例名称
//	std::string szCpuType;
	int m_nRack;
	int m_nSlot;
	bool m_bEndian;
	int m_nBlockNum;
	int m_nBlockCount;
	BYTE BTVALUE[8];

	bool bInitPeriodTable;

	VarTotalMAP m_VarTotalMap;
private:
	BOOL ParamInit();
	//BOOL IsConnected();
	BOOL ConnectDev();
	int ReConnect();

	BOOL ParsePerTable();
	BOOL GetPointDetail(const PointInfo &getPoint, ParamPointInfo &retPoint);
	int GetPointLenth(const PointInfo &Info, ParamPointInfo &retDate);
	int InsertVarTable(const ParamPointInfo &PTDetail);
	int GetS715DevData(LPTDataBlock TDataBlock);
	void SwapBuffer(void *pdata, int size);
	//BOOL LessStar( const LPTDataBlock &v1, const LPTDataBlock &v2);
};

#endif // !defined(AFX_MYDRVOBJ_H__9D0CA9F6_F8AE_48C3_97F0_7623BFA62115__INCLUDED_)
