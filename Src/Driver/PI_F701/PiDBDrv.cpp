// PiDBDrv.cpp: implementation of the CPiDBDrv class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PI_F701Drv.h"
#include "PiDBDrv.h"
#include "SqlServerConnect.h"

#include "DlgCfg.h"
#include "..\..\Common\DebugUtils.h"
#include "..\..\Common\TimeUtils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef USE_STATIC_SAFELOCK
// ��ʼ����̬�̰߳�ȫ������
TPRI::CCriticalSection CPiDBDrv::m_csLock;
#endif

/////////////////////////////////////////////////////////////////
//ʵ�־�̬�����๤��
/////////////////////////////////////////////////////////////////
CDrvObj* CDrvFactory::NewDrv()
{
	return new CPiDBDrv();
}

DWORD CDrvFactory::GetDriverInfo(LPDRVINFO lpDrvInfo)
{
	memset(lpDrvInfo,0,sizeof(T_DRVINFO));
	strncpy(lpDrvInfo->sname,"PI Driver For DbProxy",31);
	strncpy(lpDrvInfo->sdes,"CopyRight By  ",127);
	strncpy(lpDrvInfo->sver,"2.5.0.0",15);
	lpDrvInfo->dwver = 0x0200;//2.00 �淶�汾
	lpDrvInfo->lthreads = 1; // 0��ʾ���߳����ӣ�>0��ʾ���߳�����

	return DBERR_OK;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPiDBDrv::CPiDBDrv()
:m_pSQLServer(NULL)
,m_bWritePIServer(TRUE)
,m_bWriteSQLServer(FALSE)
{	
	m_arrPoint.SetSize(RTDB_TAGNUM_MAX);
	m_arrDrval.SetSize(RTDB_TAGNUM_MAX);
	m_arrIval.SetSize(RTDB_TAGNUM_MAX);
	m_arrIstat.SetSize(RTDB_TAGNUM_MAX);
	m_arrFlags.SetSize(RTDB_TAGNUM_MAX);
	m_arrTimestamp.SetSize(RTDB_TAGNUM_MAX);
	m_arrErrors.SetSize(RTDB_TAGNUM_MAX);
	m_arrIndex.SetSize(RTDB_TAGNUM_MAX);
	m_arrBval.SetSize(RTDB_TAGNUM_MAX);
	m_arrBsize.SetSize(RTDB_TAGNUM_MAX);

	m_arrHisTimestamp.SetSize(RTDB_TAGNUM_MAX_HIS);
	m_arrHisFRvals.SetSize(RTDB_TAGNUM_MAX_HIS);
	m_arrHisIstat.SetSize(RTDB_TAGNUM_MAX_HIS);
	
	memset(&m_vStrValue, 0, sizeof(TVVAL));
	m_arrStrValue.SetSize(PI_STRING_BUFFER);

	m_bKillThread = FALSE;
	m_pThread = NULL;
	m_nStrRealVals = 0;
	m_nStrHisVals = 0;

	m_summary.SetDrvObj(this);
	m_pSQLServer = new CSqlServerConnect;
	m_pSQLServer->SetOwner(this);

	m_nPIServerIndex = 0;


	m_tServerInfo.szServer = "127.0.0.1";
	m_tServerInfo.uiPort = 1433;
	m_tServerInfo.szDBName = "F701";
	m_tServerInfo.szUser = "sa";
	m_tServerInfo.szPsw = "sa123456";
	m_tServerInfo.szTable = "T_Chen";
	m_mapCount.RemoveAll();

}

CPiDBDrv::~CPiDBDrv()
{
	memset(&m_vStrValue, 0, sizeof(TVVAL));
}

//����ֵ������ת����PI������ת��Ϊ����������
DWORD CPiDBDrv::ErrRetCode(DWORD dwcode)
{
	DWORD dwret = DBERR_OK;

	switch(dwcode)
	{
	case 0: // �ɹ�
		dwret = DBERR_OK;
		break;
	case 1: //ʧ��
		dwret = DBERR_FAIL;	
		break;
	case 2: //û�����ӣ�����ʧ��
		dwret = DBERR_CONNECT;
		break;
	case 3: //�޴˽ӿ�
		dwret = DBERR_NOINTERFACE;
		break;
	case 4: //������������
		dwret = DBERR_HANDLE;
		break;
	case 5: //����ļ�¼��
		dwret = DBERR_RECS;
		break;
	case 6: //�ڴ����
		dwret = DBERR_MEMEORY;
		break;
	case 7: //��֧��
		dwret = DBERR_NOTSUPPORT;
		break;
	case 8: //û�м�¼
		dwret = DBERR_NOMOREREC;
		break;
	case 9: //�޴˱�ǩ
		dwret = DBERR_NOTAG;
		break;
	case 10: //�����޷�ת�������
		dwret = DBERR_TYPE;
		break;
	case 11: //ʱ�����
		dwret = DBERR_TIME;
		break;
	case 12://û������
		dwret = DBERR_NOVAL;
		break;
	}

	return dwret;
}

//���ݴ������װ����PI������ת��Ϊ����������
DWORD CPiDBDrv::ErrDataCode(DWORD dwcode)
{
	return 0;
}

// ʵʱ������ת��Ϊ��������
DWORD CPiDBDrv::DataTypeFrom(PIvaluetype pttype)
{
	DWORD dwtype;

	switch (pttype)
	{
	case PI_Type_int16:
	case PI_Type_int32:
		dwtype = DT_INT32;
		break;
	case PI_Type_int64:
		dwtype = DT_INT64;
		break;
	case PI_Type_float16:
	case PI_Type_float32:
	case PI_Type_PI2:
		dwtype = DT_FLOAT32;
		break;
	case PI_Type_float64:
		dwtype = DT_FLOAT64;
		break;
	case PI_Type_digital:
		dwtype = DT_DIGITAL;
		break;
	case PI_Type_PIstring:
	case PI_Type_PItimestamp:
		dwtype = DT_STRING;
		break;
	case PI_Type_blob:
		dwtype = DT_BLOB;
		break;
	default:
		dwtype = DT_VOID;
		break;
	}

	return dwtype;
}

//��������ת��Ϊʵʱ������
PIvaluetype CPiDBDrv::DataTypeTo(DWORD dwtype)
{
	PIvaluetype pttype;
	
	switch (dwtype)
	{
	case DT_DIGITAL:
		pttype = PI_Type_digital;
		break;
	case DT_INT32:
		pttype = PI_Type_int32;
		break;
	case DT_FLOAT32:
		pttype = PI_Type_float32;
		break;
	case DT_INT64:
		pttype = PI_Type_int64;
		break;
	case DT_FLOAT64:
		pttype = PI_Type_float64;
		break;
	case DT_STRING:
		pttype = PI_Type_PIstring;
		break;
	case DT_BLOB:
		pttype = PI_Type_blob;
		break;
	}
	
	return pttype;
}

//////////////////////////////////////////////////////////////////////////
// ͨ�����ܷ���������ԭ��ʹ��CTime����ʱ��ת������GetHisVal2���������У�
// Time2PiTimestamp��PiTimestamp2TimeռCPUʹ�õ�17%����piar_getarcvaluex
// ռCPUʹ�õ�19%����Time2PiTimestamp��PiTimestamp2Time���������Ż���
// piar_getarcvaluexռCPUʹ��������30%����PIʱ��ת�����API��CPUռ��Ϊ8%��
// CPUռ�÷ֲ���Ϊ����
//////////////////////////////////////////////////////////////////////////

void CPiDBDrv::Time2PiTimestamp(long lTime, PITIMESTAMP *pTimestamp)
{
	//CTime ft(lTime);
	//pitm_settime(pTimestamp, ft.GetYear(), ft.GetMonth(), ft.GetDay(),
	//	ft.GetHour(), ft.GetMinute(), ft.GetSecond());
	
	time_t ttime = lTime; // ����VC2005��time_t��int64���͵�
	struct tm *ptm = localtime(&ttime);
	if (ptm) {
		pitm_settime(pTimestamp, ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
			ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

		/*
		pTimestamp->year = ptm->tm_year + 1900;
		pTimestamp->month = ptm->tm_mon + 1;
		pTimestamp->day = ptm->tm_mday;
		pTimestamp->hour = ptm->tm_hour;
		pTimestamp->minute = ptm->tm_min;
		pTimestamp->second = ptm->tm_sec;
		//pTimestamp->tzinfo = ptm->tm_isdst;
		*/
	}
	else {
		memset(pTimestamp, 0, sizeof(PITIMESTAMP));
	}
}

long CPiDBDrv::PiTimestamp2Time(PITIMESTAMP *pTimestamp)
{
	//long ft = pitm_getpitime(pTimestamp, 0);
	//return ft - _timezone;
	/*
	CTime ft(pTimestamp->year, pTimestamp->month, pTimestamp->day, 
		pTimestamp->hour, pTimestamp->minute, (int)pTimestamp->second);
	return (long)ft.GetTime();
	*/

	struct tm atm;

	atm.tm_sec = (int)pTimestamp->second;
	atm.tm_min = pTimestamp->minute;
	atm.tm_hour = pTimestamp->hour;
	atm.tm_mday = pTimestamp->day;
	atm.tm_mon = pTimestamp->month - 1;        // tm_mon is 0 based
	atm.tm_year = pTimestamp->year - 1900;     // tm_year is 1900 based
	atm.tm_isdst = -1;

	return (long)mktime(&atm);
}

short CPiDBDrv::StateToQuality(int32 nIstat)
{
	short nQuality = (short)nIstat;
	switch(nIstat)
	{
	case 0:
		nQuality = DBP_QUALITY_GOOD;
		break;
	case -194:
		nQuality = DBP_QUALITY_HI_LIMIT;
		break;
	case -195:
		nQuality = DBP_QUALITY_LO_LIMIT;
		break;
	case -199:
	case -224:
	case -290:
	case -299:
	case -307:
		nQuality = DBP_QUALITY_BAD;
		break;
	case  -245:
		nQuality = DBP_QUALITY_CALC_OFF;
		break;
	case -251:
		nQuality = DBP_QUALITY_LOLO_LIMIT;
		break;
	case -252:
		nQuality = DBP_QUALITY_HIHI_LIMIT;
		break;
	case -253:
		nQuality = DBP_QUALITY_NOT_INIT;
		break;
	case -254:
		nQuality = DBP_QUALITY_SHUTDOWN;
		break;
	case -312:
		nQuality = DBP_QUALITY_BAD_OUT_OF_SERVICE;
		break;
	case -313:
		nQuality = DBP_QUALITY_BAD_COMM_FAIL;
		break;
	case -314:
		nQuality = DBP_QUALITY_BAD_NOT_CONNECTED;
		break;
	}

	return nQuality;
}

int32 CPiDBDrv::QualityToState(short nQuality)
{
	int32 nq = nQuality;

	switch(nQuality)
	{
	case DBP_QUALITY_GOOD:
		nq = 0;
		break;
	case DBP_QUALITY_BAD:
		nq = -224;
		break;
	case DBP_QUALITY_HI_LIMIT:
		nq = -194;
		break;
	case DBP_QUALITY_LO_LIMIT:
		nq = -195;
		break;
	case DBP_QUALITY_LOLO_LIMIT:
		nq = -251;
		break;
	case DBP_QUALITY_HIHI_LIMIT:
		nq = -252;
		break;
	case DBP_QUALITY_NOT_INIT:
		nq = -253;
		break;
	case DBP_QUALITY_SHUTDOWN:
		nq = -254;
		break;
	case DBP_QUALITY_CALC_OFF:
		nq = -245;
		break;
	case DBP_QUALITY_BAD_OUT_OF_SERVICE:
		nq = -312;
	case DBP_QUALITY_BAD_COMM_FAIL:
		nq = -313;
		break;
	case DBP_QUALITY_BAD_NOT_CONNECTED:
		nq = -314;
		break;
	}

	if(nq > 0 || nq < -314) 
		nq =  -224;//wzq 20091103

	return nq;
}
//{{wzq ���ô�����
BOOL CPiDBDrv::SetTVVAL(LPVVAL pvval, short snerr)
{
	ASSERT(pvval != NULL);
	pvval->snerr = snerr;
	return TRUE;
}
//}}
BOOL CPiDBDrv::SetTVVAL(LPVVAL pvval, int32 digicode, int32 pttype, float64 drval, int32 ival, 
	void *bval, uint32 bsize, int32 istat, int16 flags, long ltime,short ierror)
{
	ASSERT(pvval != NULL);

	DWORD dwtype = pvval->vt;
	int32 digital = 0;

	switch(pttype) 
	{
	case PI_Type_int16:
	case PI_Type_int32:
	case PI_Type_int64:
		if(dwtype == DT_DIGITAL || dwtype == DT_INT32)
			pvval->lval = ival;
		else if(dwtype == DT_INT64)
			pvval->llval = ival;
		else if(dwtype == DT_FLOAT32)
			pvval->fval = (float)ival;
		else if(dwtype == DT_FLOAT64)
			pvval->dblval = (double)ival;
		else if (dwtype == DT_VOID)
		{
			((CVarVal*)pvval)->ChangeType(DT_INT32);
			pvval->lval = (long)ival;
		}
		
		pvval->snqa = StateToQuality(istat);
		pvval->snerr = ierror ;//0;//wzq �贫���Ӵ�����
		break;	
	case PI_Type_float16:
	case PI_Type_float32:
	case PI_Type_float64:
		if(dwtype == DT_DIGITAL || dwtype == DT_INT32)
			pvval->lval = (long)drval;
		else if(dwtype == DT_INT64)
			pvval->llval = (__int64)drval;
		else if(dwtype == DT_FLOAT32)
			pvval->fval = (float)drval;
		else if(dwtype == DT_FLOAT64)
			pvval->dblval = drval;
		else if (dwtype == DT_VOID)
		{
			if (pttype == PI_Type_float64)
			{
				((CVarVal*)pvval)->ChangeType(DT_FLOAT64);
				pvval->dblval = drval;
			}
			else {
				((CVarVal*)pvval)->ChangeType(DT_FLOAT32);
				pvval->fval = (float)drval;
			}
		}
		
		pvval->snqa = StateToQuality(istat);
		pvval->snerr = ierror ;//0;
		break;
	case PI_Type_digital:
		// ����istat��0 - -314��Χ��ʱ����ʾ��״ֵ̬����PIϵͳ���ɵ�״̬���䶨����SYSTEM״̬����
		if(istat > 0 || istat < -314)//�������� ,���㿪������ʵֵ0,1
		{
			digital = -istat - digicode;
			pvval->snqa = ERRTV_OK;
		}
		else
		{
			digital = istat;
			pvval->snqa = StateToQuality(istat);
		}

		pvval->snerr = ierror ;//0;
		
		if(dwtype == DT_DIGITAL || dwtype == DT_INT32)
			pvval->lval = digital;
		else if(dwtype == DT_INT64)
			pvval->llval = digital;
		else if(dwtype == DT_FLOAT32)
			pvval->fval = (float)digital;
		else if(dwtype == DT_FLOAT64)
			pvval->dblval = digital;
		else if (dwtype == DT_VOID)
		{
			((CVarVal*)pvval)->ChangeType(DT_DIGITAL);
			pvval->lval = (long)digital;
		}
		break;
	case PI_Type_PIstring:		
		pvval->lvlen = bsize;
		pvval->pstr = (char*)bval;
		pvval->pstr[bsize] = '\0';
		pvval->snqa = StateToQuality(istat);
		pvval->snerr = ierror ;//0;
		break;
	case PI_Type_blob:	
		pvval->lvlen = bsize;
		pvval->pstr = (char*)bval;
		pvval->snqa = StateToQuality(istat);
		pvval->snerr = ierror ;//0;
		break;
	}
	pvval->ltime = ltime;

	return TRUE;
}

BOOL CPiDBDrv::SetTVVAL(LPVVAL pvval, int32 digicode, int32 pttype, float rval, int32 istat, long ltime)
{
	ASSERT(pvval != NULL);

	DWORD dwtype = pvval->vt;
	int32 digital = 0;

	switch(pttype) 
	{
	case PI_Type_int16:
	case PI_Type_int32:
	case PI_Type_int64:
		if(dwtype == DT_DIGITAL || dwtype == DT_INT32)
			pvval->lval = (long)rval;
		else if(dwtype == DT_INT64)
			pvval->llval = (__int64)rval;
		else if(dwtype == DT_FLOAT32)
			pvval->fval = rval;
		else if(dwtype == DT_FLOAT64)
			pvval->dblval = (double)rval;
		else if (dwtype == DT_VOID)
		{
			((CVarVal*)pvval)->ChangeType(DT_INT32);
			pvval->lval = (long)rval;
		}

		pvval->snqa = StateToQuality(istat);
		pvval->snerr = ERRTV_OK ;//0;//wzq �贫���Ӵ�����
		break;	
	case PI_Type_float16:
	case PI_Type_float32:
	case PI_Type_float64:
		if(dwtype == DT_DIGITAL || dwtype == DT_INT32)
			pvval->lval = (long)rval;
		else if(dwtype == DT_INT64)
			pvval->llval = (__int64)rval;
		else if(dwtype == DT_FLOAT32)
			pvval->fval = rval;
		else if(dwtype == DT_FLOAT64)
			pvval->dblval = (double)rval;
		else if (dwtype == DT_VOID)
		{
			if (pttype == PI_Type_float64)
			{
				((CVarVal*)pvval)->ChangeType(DT_FLOAT64);
				pvval->dblval = (double)rval;
			}
			else {
				((CVarVal*)pvval)->ChangeType(DT_FLOAT32);
				pvval->fval = (float)rval;
			}
		}

		pvval->snqa = StateToQuality(istat);
		pvval->snerr = ERRTV_OK ;//0;
		break;
	case PI_Type_digital:
		// ����istat��0 - -314��Χ��ʱ����ʾ��״ֵ̬����PIϵͳ���ɵ�״̬���䶨����SYSTEM״̬����
		if(istat > 0 || istat < -314)//�������� ,���㿪������ʵֵ0,1
		{
			digital = -istat - digicode;
			pvval->snqa = ERRTV_OK;
		}
		else
		{
			digital = istat;
			pvval->snqa = StateToQuality(istat);
		}

		pvval->snerr = ERRTV_OK ;//0;

		if(dwtype == DT_DIGITAL || dwtype == DT_INT32)
			pvval->lval = digital;
		else if(dwtype == DT_INT64)
			pvval->llval = digital;
		else if(dwtype == DT_FLOAT32)
			pvval->fval = (float)digital;
		else if(dwtype == DT_FLOAT64)
			pvval->dblval = digital;
		else if (dwtype == DT_VOID)
		{
			((CVarVal*)pvval)->ChangeType(DT_DIGITAL);
			pvval->lval = (long)digital;
		}
		break;
	case PI_Type_PIstring:	
		pvval->snerr =  ERRTV_TYPE;
		break;
	case PI_Type_blob:	
		pvval->snerr =  ERRTV_TYPE;
		break;
	}
	pvval->ltime = ltime;

	return TRUE;
}

BOOL CPiDBDrv::CheckValueType(PIvaluetype pttype, DWORD dwtype)
{	
	switch (pttype) {
	case PI_Type_float16:
	case PI_Type_float32:
	case PI_Type_float64:
	case PI_Type_digital:
	case PI_Type_int16:
	case PI_Type_int32: 
	case PI_Type_int64:		
		switch (dwtype)
		{
		case DT_DIGITAL:
		case DT_INT32:
		case DT_FLOAT32:
		case DT_INT64:
		case DT_FLOAT64:
			return TRUE;
			break;
		case DT_STRING:
		case DT_BLOB:
			return FALSE;
			break;
		}
		break;
	case PI_Type_PIstring:	
		return (dwtype == DT_STRING);
		break;
	case PI_Type_blob:	
		return (dwtype == DT_BLOB);
		break;
	}

	return TRUE;
}

int32 CPiDBDrv::InitWriteTagValues(TTAGVAL vals[], int nsize)
{
	CString szLog;
	int32 nTrueSize = 0;
	if (m_arrPoint.GetSize() < nsize)
		m_arrPoint.SetSize(nsize);
	
	if (m_arrDrval.GetSize() < nsize)
		m_arrDrval.SetSize(nsize);	
	if (m_arrIval.GetSize() < nsize)
		m_arrIval.SetSize(nsize);
	if (m_arrIstat.GetSize() < nsize)
		m_arrIstat.SetSize(nsize);
	if (m_arrFlags.GetSize() < nsize)
		m_arrFlags.SetSize(nsize);	
	if (m_arrTimestamp.GetSize() < nsize)
		m_arrTimestamp.SetSize(nsize);
	if (m_arrErrors.GetSize() < nsize)
		m_arrErrors.SetSize(nsize);
	if (m_arrIndex.GetSize() < nsize)
		m_arrIndex.SetSize(nsize);
	if (m_arrBval.GetSize() < nsize)
		m_arrBval.SetSize(nsize);
	if (m_arrBsize.GetSize() < nsize)
		m_arrBsize.SetSize(nsize);
	
	CTagVal* ptv=NULL;

	//CTimeCount timer;
	//timer.start();
	//if (CheckBatchTagAttr(vals, nsize) != DBERR_OK)
	//	return DBERR_FAIL;
	DWORD dwret = CheckBatchTagAttr(vals, nsize);
	if (dwret == DBERR_CONNECT)
		return -1; // ����-1��ʾδ����
	else if (dwret != DBERR_OK)
		return 0;
	//timer.end();
	//szLog.Format(_T("CPiDBDrv::InitWriteTagValues()��%d����ǩ�ĺ�ʱ:%fms\n"), nsize, timer.time_milli());
	//OutputDebugString(szLog);

	LPDRVTAGATTR ptrs = m_arrTagAttrs;
	int i;
	for(i = 0; i < nsize; i++) 
	{
		if (ptrs->lstatus == DBERR_NOTAG) //ȡ��ǩ����
		{
			OutputDebug(_T("TagName=%s, status=%d"), ptrs->sname, ptrs->lstatus);
			vals[i].m_val.snerr = ERRTV_NOTAG;
			ptrs++;
			continue;
		}
		else if (ptrs->lstatus != DBERR_OK)
		{
			OutputDebug(_T("TagName=%s, status=%d"), ptrs->sname, ptrs->lstatus);
			vals[i].m_val.snerr = ERRTV_ERR;
			ptrs++;
			continue;
		}

		m_arrPoint[nTrueSize] = ptrs->lid;
		pitm_settime(&m_arrTimestamp[nTrueSize], 0, 0, 0, 0, 0, (double)vals[i].m_val.ltime);
		//Time2PiTimestamp(vals[nTrueSize].m_val.ltime, &arrTimestamp[i]);
		
		PIvaluetype pttype = *((PIvaluetype*)ptrs->m_szSite);
		
		/*
		if (!CheckValueType(pttype, vals[i].m_val.vt)) {
			szLog.Format("��ǩ��%s�������Ͳ�ƥ�䣬%d -> %d", vals[i].m_sname, vals[i].m_val.vt, pttype);
			OnLog(0, szLog);
			vals[i].m_val.snerr = ERRTV_TYPE;
			continue;;
		}
		*/

		m_arrBval[nTrueSize] = NULL;
		m_arrBsize[nTrueSize] = 0;

		// ��QualityToState()���������
		//if(vals[i].m_val.snqa > 0 || vals[i].m_val.snqa < -314) 
		//	vals[i].m_val.snqa =  -224;//wzq 20091103

		switch(pttype) 
		{
		case PI_Type_int16:
		case PI_Type_int32:
		case PI_Type_int64:
			if(vals[i].m_val.vt == DT_DIGITAL || vals[i].m_val.vt == DT_INT32)
				m_arrIval[nTrueSize] = (int32)vals[i].m_val.lval;
			else if(vals[i].m_val.vt == DT_INT64)
				m_arrIval[nTrueSize] = (int32)vals[i].m_val.llval;
			else if(vals[i].m_val.vt == DT_FLOAT32)
				m_arrIval[nTrueSize] = (int32)vals[i].m_val.fval;
			else if(vals[i].m_val.vt == DT_FLOAT64)
				m_arrIval[nTrueSize] = (int32)vals[i].m_val.dblval;

			m_arrDrval[nTrueSize] = m_arrIval[nTrueSize]; // ����INT32���ͣ�������Ҫ���ø�ֵ
			m_arrIstat[nTrueSize] =  QualityToState(vals[i].m_val.snqa);
			break;	
		case PI_Type_float16:
		case PI_Type_float32:
		case PI_Type_float64:
			if(vals[i].m_val.vt == DT_DIGITAL || vals[i].m_val.vt == DT_INT32)
				m_arrDrval[nTrueSize] = vals[i].m_val.lval;
			else if(vals[i].m_val.vt == DT_INT64)
				m_arrDrval[nTrueSize] = (double)vals[i].m_val.llval;
			else if(vals[i].m_val.vt == DT_FLOAT32)
				m_arrDrval[nTrueSize] = vals[i].m_val.fval;
			else if(vals[i].m_val.vt == DT_FLOAT64)
				m_arrDrval[nTrueSize] = vals[i].m_val.dblval;
			
			m_arrIval[nTrueSize] = 0;
			m_arrIstat[nTrueSize] = QualityToState(vals[i].m_val.snqa);

			/*
			if ((stricmp(ptrs->sname, "DZ.TT.N4TC_HEATRATE_AVG_DCSZ") == 0) || (stricmp(ptrs->sname, "DZ.TT.N6TC_HEATRATE_AVG_DCSZ") == 0))
			{
				CTime tt(vals[i].m_val.ltime);

				AddInfoFmt(_T("tagname=%s, time=%s, val=%f, snqa=%d"), ptrs->sname, tt.Format(_T("%Y-%m-%d %H:%M:%S")), m_arrDrval[nTrueSize], (int)m_arrIstat[nTrueSize]);
			}*/
			break;
		case PI_Type_digital:
			m_arrDrval[nTrueSize] = 0;
			m_arrIval[nTrueSize] = 0;
			if(DBP_QUALITY_GOOD == vals[i].m_val.snqa) //�������� ,���㿪������ʵֵ0,1
			{
				if(vals[i].m_val.vt == DT_DIGITAL || vals[i].m_val.vt == DT_INT32)
					m_arrIstat[nTrueSize] = -ptrs->lStartCode - (int32)vals[i].m_val.lval;
				else if(vals[i].m_val.vt == DT_INT64)
					m_arrIstat[nTrueSize] = -ptrs->lStartCode - (int32)vals[i].m_val.llval;
				else if(vals[i].m_val.vt == DT_FLOAT32)
					m_arrIstat[nTrueSize] = -ptrs->lStartCode - (int32)vals[i].m_val.fval;
				else if(vals[i].m_val.vt == DT_FLOAT64)
					m_arrIstat[nTrueSize] = -ptrs->lStartCode - (int32)vals[i].m_val.dblval;
			}
			else
			{
				m_arrIstat[nTrueSize] = QualityToState(vals[i].m_val.snqa);
			}
			//OutputDebug(_T("CPiDBDrv::InitWriteTagValues: ip=%s digital=%d"), m_szIP, m_arrIstat[nTrueSize]);
			break;
		case PI_Type_PIstring:
			m_arrDrval[nTrueSize] = 0;
			m_arrIval[nTrueSize] = 0;
			m_arrIstat[nTrueSize] = QualityToState(vals[i].m_val.snqa);

			m_arrBval[nTrueSize] = vals[i].m_val.pstr;
			m_arrBsize[nTrueSize] = vals[i].m_val.lvlen;

			//OutputDebug(_T("CPiDBDrv::InitWriteTagValues: ip=%s str=%s"), m_szIP, vals[i].m_val.pstr);
			break;
		case PI_Type_blob:
			m_arrDrval[nTrueSize] = 0;
			m_arrIval[nTrueSize] = 0;
			m_arrIstat[nTrueSize] = QualityToState(vals[i].m_val.snqa);
			
			m_arrBval[nTrueSize] = vals[i].m_val.pblob;
			m_arrBsize[nTrueSize] = vals[i].m_val.lvlen;
			break;
		}
		
		m_arrIndex[nTrueSize] = i;
		nTrueSize++;

		ptrs++;
	}

	return nTrueSize;
}

int32 CPiDBDrv::InitWriteTagValues(TTAGVAL2 vals[], int nsize)
{
	CString szLog;
	if (m_arrPoint.GetSize() < nsize)
		m_arrPoint.SetSize(nsize);
	
	if (m_arrDrval.GetSize() < nsize)
		m_arrDrval.SetSize(nsize);	
	if (m_arrIval.GetSize() < nsize)
		m_arrIval.SetSize(nsize);
	if (m_arrIstat.GetSize() < nsize)
		m_arrIstat.SetSize(nsize);
	if (m_arrFlags.GetSize() < nsize)
		m_arrFlags.SetSize(nsize);	
	if (m_arrTimestamp.GetSize() < nsize)
		m_arrTimestamp.SetSize(nsize);
	if (m_arrErrors.GetSize() < nsize)
		m_arrErrors.SetSize(nsize);
	if (m_arrIndex.GetSize() < nsize)
		m_arrIndex.SetSize(nsize);
	if (m_arrBval.GetSize() < nsize)
		m_arrBval.SetSize(nsize);
	if (m_arrBsize.GetSize() < nsize)
		m_arrBsize.SetSize(nsize);
	
	for (int i = 0; i < nsize; i++)
	{
		m_arrPoint[i] = vals[i].lid;
		pitm_settime(&m_arrTimestamp[i], 0, 0, 0, 0, 0, (double)vals[i].m_val.ltime);		
		PIvaluetype pttype = vals[i].pitype;
		
		m_arrBval[i] = NULL;
		m_arrBsize[i] = 0;

		//if(vals[i].m_val.snqa > 0 || vals[i].m_val.snqa < -314) vals[i].m_val.snqa =  -224;//wzq 20091103
		switch(pttype) 
		{
		case PI_Type_PIstring:
			m_arrDrval[i] = 0;
			m_arrIval[i] = 0;
			m_arrIstat[i] = QualityToState(vals[i].m_val.snqa);

			m_arrBval[i] = vals[i].m_val.pstr;
			m_arrBsize[i] = vals[i].m_val.lvlen;

			OutputDebug(_T("CPiDBDrv::InitWriteTagValues: ip=%s str=%s"), GetPIServer().m_szIP, vals[i].m_val.pstr);
			break;
		case PI_Type_blob:
			m_arrDrval[i] = 0;
			m_arrIval[i] = 0;
			m_arrIstat[i] = QualityToState(vals[i].m_val.snqa);
			
			m_arrBval[i] = vals[i].m_val.pblob;
			m_arrBsize[i] = vals[i].m_val.lvlen;
			break;
		}
		
		m_arrIndex[i] = i;
	}

	return nsize;
}

//////////////////////////////////////////////////////////////////////
// ����
//////////////////////////////////////////////////////////////////////

DWORD CPiDBDrv::ExecCommand(
		  DWORD dwCommand,		// �������
		  void *pParams,		// �����������ָ��
		  DWORD dwParamSize,	// ���������С
		  void *pResults,		// ����ִ�н������ָ�룬�����������ڴ棬���ʹ���Э�̱�֤��С��������ִ�н��Ҫ��
		  DWORD dwResultSize	// ����ִ�н�����ݴ�С
		  )
{
	return CDrvObj::ExecCommand(dwCommand, pParams, dwParamSize, pResults, dwResultSize);
}

//ɾ��ǰ����
void CPiDBDrv::OnDestroy()	
{
	// DisConnect(); ����Ҫ��������ã�DisConnect()�ɴ���������Ĺر�����ʱ�Զ�����
}

void CPiDBDrv::LoadConfig(CDBDrvConnectString &objConnect, CPIServerList *pPIServerList/* = NULL*/)
{
	if (pPIServerList == NULL)
		pPIServerList = &m_arrPIServerList;

	pPIServerList->RemoveAll();


	m_bWritePIServer = objConnect.GetValueByInt("WRITE_PI");
	m_bWriteSQLServer = objConnect.GetValueByInt("WRITE_SQL");

	CString szVersion = objConnect[DBDCS_DRVVER_NAME];
	if (szVersion == _T("2.0")) // ԭ����PI����
	{
		CPIServerItem item;

		item.m_szIP = objConnect["IP"];
		item.m_szPort = objConnect["PORT"];
		item.m_szUserName = objConnect["USER"];
		item.m_szPassword = objConnect["PWD"];
		item.m_nConnTimeout = objConnect.GetValueByInt("CONNTIMEOUT");
		item.m_nDATimeout = objConnect.GetValueByInt("DATIMEOUT");

		pPIServerList->Add(item);
	}
	else 
	{ // ֧�ּ�Ⱥ��PI����
		CString szKey;
		int nCount = objConnect.GetValueByInt("COUNT");
		for (int i = 0; i < nCount; i++)
		{
			CPIServerItem item;

			szKey.Format(_T("IP%d"), i);
			item.m_szIP = objConnect[szKey];
			szKey.Format(_T("PORT%d"), i);
			item.m_szPort = objConnect[szKey];
			szKey.Format(_T("USER%d"), i);
			item.m_szUserName = objConnect[szKey];
			szKey.Format(_T("PWD%d"), i);
			item.m_szPassword = objConnect[szKey];
			szKey.Format(_T("CONNTIMEOUT%d"), i);
			item.m_nConnTimeout = objConnect.GetValueByInt(szKey);
			szKey.Format(_T("DATIMEOUT%d"), i);
			item.m_nDATimeout = objConnect.GetValueByInt(szKey);

			pPIServerList->Add(item);
		}
	}
}

void CPiDBDrv::SaveConfig(CDBDrvConnectString &objConnect, CPIServerList *pPIServerList/* = NULL*/)
{
	if (pPIServerList == NULL)
		pPIServerList = &m_arrPIServerList;

	objConnect.RemoveAll();

	objConnect[DBDCS_DRVVER_NAME] = _T("2.1");
	int nCount = pPIServerList->GetCount();
	objConnect.SetValue("COUNT", nCount);

	objConnect.SetValue("WRITE_PI", m_bWritePIServer);
	objConnect.SetValue("WRITE_SQL", m_bWriteSQLServer);

	CString szKey;
	for (int i = 0; i < nCount; i++)
	{
		CPIServerItem &item = pPIServerList->GetAt(i);

		szKey.Format(_T("IP%d"), i);
		objConnect.SetValue(szKey, item.m_szIP);
		szKey.Format(_T("PORT%d"), i);
		objConnect.SetValue(szKey, item.m_szPort);
		szKey.Format(_T("USER%d"), i);
		objConnect.SetValue(szKey, item.m_szUserName);
		szKey.Format(_T("PWD%d"), i);
		objConnect.SetValue(szKey, item.m_szPassword);
		szKey.Format(_T("CONNTIMEOUT%d"), i);
		objConnect.SetValue(szKey, item.m_nConnTimeout);
		szKey.Format(_T("DATIMEOUT%d"), i);
		objConnect.SetValue(szKey, item.m_nDATimeout);
	}
}


void CPiDBDrv::LoadSQLConfig(CDBDrvConnectString& objConnect, T_ServerInfo& tServerInfo)
{
	CString szKey;
	szKey.Format(_T("SQL_SERVER"));
	tServerInfo.szServer = objConnect[szKey];
	szKey.Format(_T("SQL_PORT"));
	tServerInfo.uiPort = objConnect.GetValueByInt(szKey);
	szKey.Format(_T("SQL_DBNAME"));
	tServerInfo.szDBName = objConnect[szKey];
	szKey.Format(_T("SQL_USER"));
	tServerInfo.szUser = objConnect[szKey];
	szKey.Format(_T("SQL_PSW"));
	tServerInfo.szPsw = objConnect[szKey];
	szKey.Format(_T("SQL_TABLE"));
	tServerInfo.szTable = objConnect[szKey];
}
void CPiDBDrv::SaveSQLConfig(CDBDrvConnectString &objConnect, const T_ServerInfo& tServerInfo)
{
	CString szKey;
	szKey.Format(_T("SQL_SERVER"));
	objConnect.SetValue(szKey, tServerInfo.szServer);
	szKey.Format(_T("SQL_PORT"));
	objConnect.SetValue(szKey, (int)(tServerInfo.uiPort));
	szKey.Format(_T("SQL_DBNAME"));
	objConnect.SetValue(szKey, tServerInfo.szDBName);
	szKey.Format(_T("SQL_USER"));
	objConnect.SetValue(szKey, tServerInfo.szUser);
	szKey.Format(_T("SQL_PSW"));
	objConnect.SetValue(szKey, tServerInfo.szPsw);
	szKey.Format(_T("SQL_TABLE"));
	objConnect.SetValue(szKey, tServerInfo.szTable);
}

DWORD CPiDBDrv::Connect(const char* sConnectStr)	
{
	SAFELOCK(m_csLock);

	int trys = 0;
	int32 nret = 1;
	
	m_nStrRealVals = 0;
	m_nStrHisVals = 0;

	if (!sConnectStr || ('\0' == sConnectStr[0]))
		return DBERR_FAIL;

	//ParseConStr(sConnectStr, m_szIP, m_szPort, m_szUserName, m_szPassword, m_nConnTimeout, m_nDATimeout);
	CDBDrvConnectString objConnStr(sConnectStr);
	LoadConfig(objConnStr);
	LoadSQLConfig(objConnStr, m_tServerInfo);


	BOOL bRet = ConnectSQL();


	if (m_arrPIServerList.GetCount() == 0)
		return DBERR_FAIL;

	m_nPIServerIndex++; // ʹ����һ��PI����������
	if (m_nPIServerIndex < 0)
		m_nPIServerIndex = 0;
	else if (m_nPIServerIndex > m_arrPIServerList.GetCount() - 1)
		m_nPIServerIndex = m_arrPIServerList.GetCount() - 1;

	CPIServerItem &item = GetPIServer();

	//add by jyf
	if(!m_bWritePIServer)
	{
		AddInfoFmt(_T("no WritePIServer"));
		return DBERR_OK;
	}
	// piut_setservernode����ʱ������PI���������ܻ������ʱ�����
	// ����ʹ�ö�γ������ӣ���������������ʱ���ֵ�����ʧ������	
	while ( nret && trys < PI_CONN_MAXTRY )
	{
		trys++;
		// ���������������̵߳�piut_setservernode���ûᷢ���Ƿ��ڴ���ʴ���

		//piut_setprocname(item.m_szIP);

		nret = piut_setservernode(item.m_szIP);
		if (0 != nret)
		{
			AddDebugFmt(_T("piut_setservernode=%d"), nret);	
		}

		// ����V1.6.2.5�汾��piapi32.dll��piut_setservernode()���Ƿ��سɹ����Ӷ�������PI��¼��
		// ��Bug����ɷ��ʸ߰汾��PI������ʱ��ֻ�ܶ����ݣ�д����ʱ�ᱨ-10401������ʾû��дȨ��

		//if(0 != nret)
// 		{
 			LONG valid;
 			nret = piut_login( item.m_szUserName, item.m_szPassword, &valid );
 			AddDebugFmt(_T("piut_login=%d"), nret);	
 			if(0 != nret)
 				return DBERR_CONNECT;
// 		}
	}
	
	if (0 != nret)
	{
		AddDebugFmt(LOG_OPT_DISPLAY, _T("PI Connectʧ��: ip=%s, port=%s, username=%s"), item.m_szIP, item.m_szPort, item.m_szUserName);
		nret = DBERR_CONNECT;
	}
	else {
		AddInfoFmt(_T("PI Connect�ɹ�: ip=%s, port=%s, username=%s"), item.m_szIP, item.m_szPort, item.m_szUserName);
		nret = DBERR_OK;

		// ������ӳɹ��������ԭ��ǩ�����
		if (m_funCommand)
			m_funCommand(m_handle, DRVCMD_CLEAR_TAG_CACHE_MAP, NULL, 0, NULL, 0);

		StartBatchThread();
	}
	
	return DBERR_OK;
	//return nret;
}

BOOL CPiDBDrv::ConnectSQL()//�������ݿ�
{
	m_pSQLServer->Disconnect();

	AddDebugFmt(_T("IP=%s, Port=%d, database=%s, user=%s,psw=%s"),
		m_tServerInfo.szServer, 
		m_tServerInfo.uiPort, 
		m_tServerInfo.szDBName, 
		m_tServerInfo.szUser, 
		m_tServerInfo.szPsw);

	if(m_tServerInfo.szServer.IsEmpty())
		return FALSE;

	BOOL bConnet = m_pSQLServer->Connect(_T(""), 
		m_tServerInfo.szServer, 
		m_tServerInfo.uiPort, 
		m_tServerInfo.szDBName, 
		m_tServerInfo.szUser, 
		m_tServerInfo.szPsw);

	return IsConnectSQL();
}

BOOL CPiDBDrv::IsConnectSQL()//�������ݿ�
{
	return m_pSQLServer->IsConnected(); 
}

DWORD CPiDBDrv::DisConnect()
{
	StopBatchThread();
	return DBERR_OK;	

	SAFELOCK(m_csLock);
	
	OutputDebug(_T("CPiDBDrv::DisConnect: ip=%s"), GetPIServer().m_szIP);

	SetActiveServer(); // ѡ��ǰ���PI������
	int32 nret = piut_disconnectnode(GetPIServer().m_szIP);
	if(0 != nret)
		return DBERR_FAIL;

	return DBERR_OK;
}

//����
DWORD CPiDBDrv::Config(
		const char* sConnectStr,
		char* sBuf,	
		int  nBufLen
		)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());//��Դ�л�

	CDBDrvConnectString objConnStr(sConnectStr);

	CDlgCfg dlg;
	if (strlen(sConnectStr) > 0)
	{
		LoadConfig(objConnStr, &dlg.m_arrPIServerList);
		LoadSQLConfig(objConnStr, dlg.m_tServerInfo);
		dlg.m_bPIServer = m_bWritePIServer;
		dlg.m_bSqlServer = m_bWriteSQLServer;
	}
	if (dlg.DoModal() != IDOK)
	{
		*sBuf = 0;
		return DBERR_FAIL;
	}
	CString szcon = _T("");
	m_bWriteSQLServer = dlg.m_bSqlServer;
	m_bWritePIServer = dlg.m_bPIServer;
			
	SaveConfig(objConnStr, &dlg.m_arrPIServerList);
	SaveSQLConfig(objConnStr, dlg.m_tServerInfo);
	szcon = objConnStr.GetConnectString();
	if(szcon == sConnectStr) //û�ı�
	{
		*sBuf = 0;
		return DBERR_FAIL;
	}
	if(nBufLen <= szcon.GetLength())
	{
		*sBuf = 0;
		return DBERR_FAIL;
	}
	strcpy(sBuf,szcon); //���ƽ����

	return DBERR_OK;
}

//////////////////////////////////////////////////////////////////////
// ��ǩ
//////////////////////////////////////////////////////////////////////

//ȡ����
DWORD CPiDBDrv::GetTagAttr(
		LPDRVTAGATTR ptag
		)	
{
	//OutputDebug(_T("CPiDBDrv::GetTagAttr Begin: tag=%s"), ptag->sname);

	long pt;
	char tagname[81];
	PIvaluetype pttype;
	
	SAFELOCK(m_csLock);
	SetActiveServer(); // ѡ��ǰ���PI������

	int32 nret = 0;
	ptag->lstatus = DBERR_NOTAG ;//1;
	ptag->lid = 0;//wzq
	//ptag->dwLastDoTime = (long)::time(NULL); // dwLastDoTime��SDK�и�������

	strncpy(tagname, ptag->sname, sizeof(tagname) - 1);
	nret = pipt_findpoint( tagname, &pt );
	if(-10733 == nret) return DBERR_CONNECT;
	if(-5 == nret) return DBERR_NOTAG;
	if(-2 == nret) return DBERR_NOTAG; // ��ǩ��ȫ�ǿո�
	if(nret > 0 )  return DBERR_CONNECT;

	ptag->lid = pt;
	
	// ���������͵�λ��Ϣ
	ptag->sdes[0] = '\0';
	ptag->sunit[0] = '\0';
	/*
	nret = pipt_descriptor( pt, ptag->sdes, sizeof(ptag->sdes) );
	if(-10733 == nret) return DBERR_CONNECT;
	if(nret > 0 )  return DBERR_CONNECT;
	if(-1 == nret) return DBERR_NOTAG;
	
	nret = pipt_engunitstring( pt, ptag->sunit, sizeof(ptag->sunit));
	if(-10733 == nret) return DBERR_CONNECT;
	if(nret > 0 )  return DBERR_CONNECT;
	if(-1 == nret) return DBERR_NOTAG;
	*/
		
	nret = pipt_pointtypex(pt, &pttype);
	if(-10733 == nret) return DBERR_CONNECT;
	if(nret > 0 )  return DBERR_CONNECT;
	if(-1 == nret) return DBERR_NOTAG;
	if(0 != nret)
		return DBERR_FAIL;
	ptag->nDataType = DataTypeFrom(pttype);
	*((PIvaluetype*)ptag->m_szSite) = pttype;
	if (PI_Type_digital == pttype) {
		int32 digcode = 0, dignumb = 0;
		nret = pipt_digpointers(pt, &digcode, &dignumb);
		if (0 == nret) {
			ptag->lStartCode = digcode;
			ptag->lCodeNum = dignumb;
		}
	}

	// ֻ�д���String��Blob��������²��������߳�
	if ((PI_Type_PIstring == pttype) || (PI_Type_blob == pttype))
	{
		if (NULL == m_pThread)
			StartBatchThread();
	}
/*
	int32 larchiving, lcompressing, filtercode;
	nret = pipt_inprocbits( pt, &larchiving, &lcompressing, &filtercode );
	if(-10733 == nret) return DBERR_CONNECT;
	if(nret > 0 )  return DBERR_CONNECT;
	if(-1 == nret) return DBERR_NOTAG;
	if(0 == nret) {
		ptag->nsave = larchiving;
		ptag->ncomp = lcompressing;
	}	

	float compdev;
	int32 compmin, compmax;
	nret = pipt_compspecseng( pt, &compdev, &compmin, &compmax );
	if(-10733 == nret) return DBERR_CONNECT;
	if(nret > 0 )  return DBERR_CONNECT;
	if(-1 == nret) return DBERR_NOTAG;
	if (0 == nret) {
		float zero, span;
		nret = pipt_scale( pt, &zero, &span );
		if(nret > 0 )  return DBERR_CONNECT;
		if(-1 == nret) return DBERR_NOTAG;
		if ((0 == nret) && (span > 0))
			ptag->dblcompval = compdev / span;

	}
*/	
	ptag->lstatus = DBERR_OK ;//0;//wzq

	//OutputDebug(_T("CPiDBDrv::GetTagAttr End: tag=%s, pid=%d, type=%d, compval=%f, desc=%s"), 
	//	ptag->sname, ptag->lid, ptag->nDataType, ptag->dblcompval, ptag->sdes);

	return DBERR_OK;
} 

//��ӱ�ǩ
DWORD CPiDBDrv::AddTag(
		TDRVTAGATTR* ptag
		)	
{
	OutputDebug(_T("CPiDBDrv::AddTag: tag=%s"), ptag->sname);

	return DBERR_NOTSUPPORT;
} 

//ɾ����ǩ
DWORD CPiDBDrv::DelTag(
		const char* stagname
		)	
{
	OutputDebug(_T("CPiDBDrv::DelTag: tag=%s"), stagname);

	return DBERR_NOTSUPPORT;
} 

//�޸ı�ǩ,�ѱ�ǩ��Ϊ����
DWORD CPiDBDrv::EditTag(
		TDRVTAGATTR* ptag
		)	
{
	OutputDebug(_T("CPiDBDrv::EditTag: tag=%s"), ptag->sname);

	return DBERR_NOTSUPPORT;
}

//////////////////////////////////////////////////////////////////////
// дʵʱ
//////////////////////////////////////////////////////////////////////

//#define  REAL_TO_HIS
//дʵʱ
DWORD CPiDBDrv::WriteReal(
	TTAGVAL vals[],
	int nsize
	)
{
	//OutputDebug(_T("CPiDBDrv::WriteReal begin: ip=%s size=%d"), GetPIServer().m_szIP, nsize);
	AddInfoFmt(_T("no pi server return ")); 
	return DBERR_OK;

#ifdef REAL_TO_HIS
	AddDebugFmt("turn real into his for daqi");
	return WriteHis(vals,nsize);
#endif

	if (nsize <= 0)
		return DBERR_FAIL;	

	if ((nsize == 1) && (vals[0].m_val.vt == DT_STRING || vals[0].m_val.vt == DT_BLOB))
	{
		return AddBatchTag(vals, TRUE);
	}

	//AddDebugFmt("After AddBatchTag");

	SAFELOCK(m_csLock);

	int nTrueSize = InitWriteTagValues(vals, nsize);
	if (nTrueSize == -1)
		return DBERR_CONNECT;
	else if (nTrueSize == 0)
		return DBERR_NOTAG;

	//AddDebugFmt("-------3--------vals[0] = %s",vals[0].m_sname);
	SetActiveServer(); // ѡ��ǰ���PI������
	int32 nret = pisn_putsnapshotsx(nTrueSize, m_arrPoint.GetData(), m_arrDrval.GetData(),
		m_arrIval.GetData(), m_arrBval.GetData(), m_arrBsize.GetData(), m_arrIstat.GetData(), 
			m_arrFlags.GetData(), m_arrTimestamp.GetData(), m_arrErrors.GetData());

	if (nret != 0)
	{
		AddDebugFmt("pisn_putsnapshotsx���ô��󣬴�����Ϊ%d", nret);
	}

	if(-10733 == nret) return DBERR_CONNECT;
	if (nret > 0) return DBERR_CONNECT;
	if (nret == -1) return DBERR_NOTAG;
	if (nret == -998) return DBERR_MEMEORY;

	//AddDebugFmt("------4--------nret = %d",nret);
	// nret == -30000��ʾ���ֳɹ����������ڸü�ʱ���Ѿ����ڶ�����
	// ����б�ǩֵ�ܴ󣬻ᱨ-15010���󣬿ɺ��Ըô���
	// -109��ʾ��ֵ�Ѿ����ڣ�����Ϊд�ɹ�
	if ((nret < 0) && (nret != -30000) && (nret != -15010) && (nret != -109))
	{
		return DBERR_FAIL;
	}

	DWORD dwCount = 0;
	for(int i = 0; i < nTrueSize; i++)
	{
		int nIndex = m_arrIndex[i];
		int32 nError = m_arrErrors[i];
		if ((0 == nError) || (nError == -109)) // -109 ��ʾ��ʱ����Ѵ��ڣ�����Ϊ��������
		{
			dwCount++;
			vals[nIndex].m_val.snerr = ERRTV_OK;

			/*

			if (i == 0)
			{
			AddDebugFmt("------5--------nError = %d  vals[nIndex].m_val.snerr = %d",nError,vals[nIndex].m_val.snerr);
			}

			if ((vals[nIndex].m_val.vt == DT_INT32) && (m_arrIval[i] != 0) && (m_arrIval[i] != 1))
			{
				CVarVal *pVal = (CVarVal*)&vals[nIndex].m_val;

				AddInfoFmt("дINT32���ͱ�ǩ<%s>�Ŀ��գ�TagID=%d,Time=%s,Value=%s,Quality=%s,Drval=%f,Ival=%d,Istat=%d", 
					vals[nIndex].m_sname, m_arrPoint[i], 
					pVal->GetTimeStr(), pVal->GetValStr(), pVal->GetQaStr(),
					m_arrDrval[i], m_arrIval[i], m_arrIstat[i]);
			}
			if (strstr(vals[nIndex].m_sname, "_S_Cir"))
			{
				CVarVal *pVal = (CVarVal*)&vals[nIndex].m_val;

				AddInfoFmt("дINT32���ͱ�ǩ<%s>�Ŀ��գ�TagID=%d,Time=%s,Value=%s,Quality=%s,Drval=%f,Ival=%d,Istat=%d", 
					vals[nIndex].m_sname, m_arrPoint[i], 
					pVal->GetTimeStr(), pVal->GetValStr(), pVal->GetQaStr(),
					m_arrDrval[i], m_arrIval[i], m_arrIstat[i]);
			}
			*/
		}
		else
		{
			switch(nError)
			{
			case -1:
				vals[nIndex].m_val.snerr = ERRTV_NOTAG;
				break;
			case -11046:
			case -11049:
				vals[nIndex].m_val.snerr = ERRTV_TIME;
				break;
			case -15014:
			case -15017:
				vals[nIndex].m_val.snerr = ERRTV_TYPE;
				break;
			default:
				vals[nIndex].m_val.snerr = ERRTV_ERR;
				break;
			}

			// ��ʱ��ֵ�Ѿ�����
// 			CVarVal *pVal = (CVarVal*)&vals[nIndex].m_val;
// 			AddDebugFmt("д��ǩ��%s���Ŀ��ճ���������Ϊ��%d��Time=%s��Value=%s��Quality=%s", vals[nIndex].m_sname, nError,
// 				pVal->GetTimeStr(), pVal->GetValStr(), pVal->GetQaStr());

// 			if (i == 0)
// 			{
// 				AddDebugFmt("------6--------nError = %d  vals[nIndex].m_val.snerr = %d",nError,vals[nIndex].m_val.snerr);
// 			}
		}
	}

	OutputDebug(_T("CPiDBDrv::WriteReal end: ip=%s nret=%d dwCount=%d"), GetPIServer().m_szIP, nret, dwCount);

	return DBERR_OK;
}

//д��ʷ
DWORD CPiDBDrv::WriteHis(
	TTAGVAL vals[],
	int nsize
	)	
{
	//OutputDebug(_T("CPiDBDrv::WriteHis: ip=%s, size=%d"), GetPIServer().m_szIP, nsize);
	
	return DBERR_OK;

	if (nsize <= 0)
		return DBERR_FAIL;
		
	if ((nsize == 1) && (vals[0].m_val.vt == DT_STRING || vals[0].m_val.vt == DT_BLOB))
	{
		return AddBatchTag(vals, FALSE);
	}

	SAFELOCK(m_csLock);
	
	int nTrueSize = InitWriteTagValues(vals, nsize);
	if (nTrueSize == -1)
		return DBERR_CONNECT;
	else if (nTrueSize == 0)
		return DBERR_NOTAG;

	SetActiveServer(); // ѡ��ǰ���PI������
	int nret = piar_putarcvaluesx(nTrueSize, ARCREPLACE, m_arrPoint.GetData(), m_arrDrval.GetData(),
		m_arrIval.GetData(), m_arrBval.GetData(), m_arrBsize.GetData(), m_arrIstat.GetData(), 
		m_arrFlags.GetData(), m_arrTimestamp.GetData(), m_arrErrors.GetData());

	if (nret != 0)
	{
		AddDebugFmt("piar_putarcvaluesx���ô��󣬴�����Ϊ%d", nret);
	}

	if(-10733 == nret) return DBERR_CONNECT;
	if (nret > 0) return DBERR_CONNECT;
	if (nret == -1) return DBERR_NOTAG;
	if (nret == -998) return DBERR_MEMEORY;
	// nret == -30000��ʾ���ֳɹ����������ڸü�ʱ���Ѿ����ڶ�����
	// ����б�ǩֵ�ܴ󣬻ᱨ-15010���󣬿ɺ��Ըô���
	if ((nret < 0) && (nret != -30000) && (nret != -15010)) return DBERR_FAIL;

	DWORD dwCount = 0;
	for(int i = 0; i < nTrueSize; i++)
	{
		int nIndex = m_arrIndex[i];
		int nError = m_arrErrors[i];
		if ((0 == nError) || (nError == -109)) // -109 == Value at This Time Already Exists
		{
			dwCount++;
			vals[nIndex].m_val.snerr = ERRTV_OK;
		}
		else
		{
			switch(nError)
			{
			case -1:
				vals[nIndex].m_val.snerr = ERRTV_NOTAG;
				break;
			case -11046:
			case -11049:
				vals[nIndex].m_val.snerr = ERRTV_TIME;
				break;
			case -15014:
			case -15017:
				vals[nIndex].m_val.snerr = ERRTV_TYPE;
				break;
			default:
				vals[nIndex].m_val.snerr = ERRTV_ERR;
				break;
			}

			AddDebugFmt("д��ǩ��%s������ʷ��¼����������Ϊ��%d", vals[nIndex].m_sname, nError);
		}
	}

	OutputDebug(_T("CPiDBDrv::WriteHis end: ip=%s nret=%d dwCount=%d"), GetPIServer().m_szIP, nret, dwCount);

	return DBERR_OK;
}
	
DWORD CPiDBDrv::WriteSoe(
	TSOE    	soes[], //�¼��ṹ����
	int			 nsize  //�¼�����
	)	
{
	OutputDebug(_T("CPiDBDrv::WriteSoe: size=%s"), nsize);

	return DBERR_NOTSUPPORT;
}

//////////////////////////////////////////////////////////////////////
//��ʵʱ
//////////////////////////////////////////////////////////////////////

//��ʵʱ,ֻ�ʺϻ�����������
DWORD CPiDBDrv::GetRealVal(
	TTAGVAL vals[],
	int		nsize
	)	
{
	OutputDebug(_T("CPiDBDrv::GetRealVal: ip=%s, size=%d"), GetPIServer().m_szIP, nsize);
	
	if (nsize <= 0)
		return DBERR_FAIL;
	
	SAFELOCK(m_csLock);
	
	if (m_arrPoint.GetSize() < nsize)
		m_arrPoint.SetSize(nsize);
	if (m_arrIndex.GetSize() < nsize)
		m_arrIndex.SetSize(nsize);
	if (m_arrIval.GetSize() < nsize)
		m_arrIval.SetSize(nsize);
	if (m_arrIstat.GetSize() < nsize)
		m_arrIstat.SetSize(nsize);

	CString szLog;
	int32 nTrueSize = 0;

	DWORD dwret = CheckBatchTagAttr(vals, nsize);
	if (dwret != DBERR_OK)
		return dwret;
	
	int i;
	for(i = 0; i < nsize; i++) 
	{
		if (m_arrTagAttrs[i].lstatus == DBERR_OK) { //ȡ��ǩ����
			if (vals[i].m_val.vt == DT_VOID)
				vals[i].m_val.vt = m_arrTagAttrs[i].nDataType;

			m_arrPoint[nTrueSize] = m_arrTagAttrs[i].lid;
			m_arrIndex[nTrueSize] = i;
			m_arrIval[nTrueSize] = m_arrTagAttrs[i].lStartCode;
			m_arrIstat[nTrueSize] = *((PIvaluetype*)m_arrTagAttrs[i].m_szSite);

			nTrueSize++;
		}
		else 
		{
			//{{wzq
			//SetTVVAL(&vals[i].m_val, (short)m_arrTagAttrs[i].lstatus);//��ǩ�����ڻ��������ʹ��󣬴˴����ظ��Դ�����
			//}}
			if (m_arrTagAttrs[i].lstatus == DBERR_NOTAG)
				vals[i].m_val.snerr = ERRTV_NOTAG;
			else
				vals[i].m_val.snerr = ERRTV_ERR;
		}
	}
	if (nTrueSize == 0)
		return DBERR_NOTAG;
	
	int32 pt_count = nTrueSize;
	float64 dbDval;
	int32 nIval, nIstat, nError;
	int16 nFlags;
	PITIMESTAMP tsTimes;
	DWORD dwCount = 0;
	int nIndex = 0;
	
	i = 0;
	SetActiveServer(); // ѡ��ǰ���PI������
	dbDval = 0.0;
	nIval = 0;
	nIstat = 0;
	nFlags = 0;
	int32 nret = pisn_getsnapshotsx( m_arrPoint.GetData(), &pt_count, &dbDval,
				&nIval, NULL, NULL, &nIstat, &nFlags, &tsTimes, &nError, GETFIRST);

	if (nret != 0)
	{
		AddDebugFmt("pisn_getsnapshotsx���ô��󣬴�����Ϊ%d", nret);
	}

	if(-10733 == nret) return DBERR_CONNECT;
	if (nret > 0) return DBERR_CONNECT;
	if (nret == -1) return DBERR_NOTAG;
	if (nret == -998) return DBERR_MEMEORY;
	// nret == -30000��ʾ���ֳɹ����������ڸü�ʱ���Ѿ����ڶ�����
	// ����б�ǩֵ�ܴ󣬻ᱨ-15010���󣬿ɺ��Ըô���
	if ((nret < 0) && (nret != -30000) && (nret != -15010)) return DBERR_FAIL;

	while(nret != PI_NOMOREVALUES)
	{
		int nIndex = m_arrIndex[i];
		// ���ؿ�����Ϣ	

		PIvaluetype pttype = (PIvaluetype)m_arrIstat[i];
		int32 digicode = m_arrIval[i];
		long lt = PiTimestamp2Time(&tsTimes);

		SetTVVAL(&vals[nIndex].m_val, digicode, pttype, dbDval, nIval, NULL, 0, nIstat, nFlags, lt, (short)nError);//wzq �����Ӹ�����ǩ�Ӵ����뷵��
		dwCount++;
		
		dbDval = 0.0;
		nIval = 0;
		nIstat = 0;
		nFlags = 0;
		nret = pisn_getsnapshotsx( m_arrPoint.GetData(), &pt_count, &dbDval,
			&nIval, NULL, NULL, &nIstat, &nFlags, &tsTimes, &nError, GETNEXT);

		i++;
	}
	
	OutputDebug(_T("CPiDBDrv::GetRealVal end: ip=%s, dwCount=%d"), GetPIServer().m_szIP, dwCount);

	return (dwCount > 0) ? DBERR_OK : DBERR_FAIL;
}

//ȡʵʱ�ַ�������,��ǩ����ptv->m_sname��
DWORD CPiDBDrv::GetRealStr(
	LPCTSTR sTagName,
	TVVAL** ppv
	)	
{
	OutputDebug(_T("CPiDBDrv::GetRealStr: tag=%s"), sTagName);
	
	SAFELOCK(m_csLock);
	
	*ppv = NULL;
	memset(&m_vStrValue, 0, sizeof(TVVAL));
	m_vStrValue.vt = DT_STRING;

	CString szLog;

	DWORD dwret = CheckBatchTagAttr(sTagName, DT_STRING);
	if (dwret != DBERR_OK)
		return dwret;

	if (m_arrTagAttrs[0].lstatus) //ȡ��ǩ����		
		return DBERR_NOTAG;

	PIvaluetype pttype = *((PIvaluetype*)m_arrTagAttrs[0].m_szSite);
	/*
	if (pttype != PI_Type_PIstring)
	{
		szLog.Format("��ǩ %s ���ʹ���", sTagName);
		OnLog(0,szLog);
		return DBERR_TYPE;
	}
	*/
	int32 istat;
	int16 flags;
	uint32 bsize = m_arrStrValue.GetSize();
	PITIMESTAMP timestamp;
	
	SetActiveServer(); // ѡ��ǰ���PI������
	int32 nret = pisn_getsnapshotx(m_arrTagAttrs[0].lid, NULL, NULL, m_arrStrValue.GetData(), &bsize, 
		&istat, &flags, &timestamp);

	if (nret != 0)
	{
		AddDebugFmt("pisn_getsnapshotx���ô��󣬴�����Ϊ%d", nret);
	}

	if (nret == -15010) // PI_OVERFLOW
	{
		bsize++; // ����NULL������
		if (bsize > PI_STRING_BUFFER_MAX)
		{
			return DBERR_MEMEORY;
		}
		m_arrStrValue.SetSize(bsize);
		
		nret = pisn_getsnapshotx(m_arrTagAttrs[0].lid, NULL, NULL, m_arrStrValue.GetData(), &bsize, 
			&istat, &flags, &timestamp);
	}
	
	if (nret == -15010) return DBERR_MEMEORY;
	if(-10733 == nret) return DBERR_CONNECT;
	if (nret > 0) return DBERR_CONNECT;
	if (nret == -1) return DBERR_NOTAG;
	if (nret == -998) return DBERR_MEMEORY;
	// nret == -30000��ʾ���ֳɹ����������ڸü�ʱ���Ѿ����ڶ�����
	// ����б�ǩֵ�ܴ󣬻ᱨ-15010���󣬿ɺ��Ըô���
	if ((nret < 0) && (nret != -30000) && (nret != -15010)) return DBERR_FAIL;

	long lt = PiTimestamp2Time(&timestamp);

	SetTVVAL(&m_vStrValue, 0, pttype, 0, 0, m_arrStrValue.GetData(), bsize, istat, flags, lt, 0);//wzq
	*ppv = &m_vStrValue;

	return DBERR_OK;
}

//ȡʵʱblob����,��ǩ����ptv->m_sname��
DWORD CPiDBDrv::GetRealBlob(
	LPCTSTR sTagName,
	TVVAL** ppv
	)	
{
	OutputDebug(_T("CPiDBDrv::GetRealBlob: tag=%s"), sTagName);
	
	SAFELOCK(m_csLock);
	
	*ppv = NULL;
	memset(&m_vStrValue, 0, sizeof(TVVAL));
	m_vStrValue.vt = DT_BLOB;
	
	CString szLog;
	DWORD dwret = CheckBatchTagAttr(sTagName, DT_BLOB);
	if (dwret != DBERR_OK)
		return dwret;

	if (m_arrTagAttrs[0].lstatus) //ȡ��ǩ����		
		return DBERR_NOTAG;

	PIvaluetype pttype = *((PIvaluetype*)m_arrTagAttrs[0].m_szSite);
	if (pttype != PI_Type_blob)
	{
		AddErrorFmt("��ǩ<%s>���ʹ���", sTagName);
		return DBERR_TYPE;
	}
	int32 istat;
	int16 flags;
	uint32 bsize = m_arrStrValue.GetSize();
	PITIMESTAMP timestamp;
	
	SetActiveServer(); // ѡ��ǰ���PI������
	int32 nret = pisn_getsnapshotx(m_arrTagAttrs[0].lid, NULL, NULL, m_arrStrValue.GetData(), &bsize, 
		&istat, &flags, &timestamp);

	if (nret != 0)
	{
		AddDebugFmt("pisn_getsnapshotx���ô��󣬴�����Ϊ%d", nret);
	}

	if (nret == -15010) // PI_OVERFLOW
	{
		if (bsize > PI_STRING_BUFFER_MAX)
		{
			return DBERR_MEMEORY;
		}
		m_arrStrValue.SetSize(bsize);
		
		nret = pisn_getsnapshotx(m_arrTagAttrs[0].lid, NULL, NULL, m_arrStrValue.GetData(), &bsize, 
			&istat, &flags, &timestamp);
	}
	
	if (nret == -15010) return DBERR_MEMEORY;
	if(-10733 == nret) return DBERR_CONNECT;
	if (nret > 0) return DBERR_CONNECT;
	if (nret == -1) return DBERR_NOTAG;
	if (nret == -998) return DBERR_MEMEORY;
	// nret == -30000��ʾ���ֳɹ����������ڸü�ʱ���Ѿ����ڶ�����
	// ����б�ǩֵ�ܴ󣬻ᱨ-15010���󣬿ɺ��Ըô���
	if ((nret < 0) && (nret != -30000) && (nret != -15010)) return DBERR_FAIL;
	
	long lt = PiTimestamp2Time(&timestamp);
	SetTVVAL(&m_vStrValue, 0, pttype, 0, 0, m_arrStrValue.GetData(), bsize, istat, flags, lt,0);
	*ppv = &m_vStrValue;

	return DBERR_OK;
}

//////////////////////////////////////////////////////////////////////
//����ʷ
//////////////////////////////////////////////////////////////////////

//��һ����ǩ����ʷ��ֻ��Ի�����������
/*
DWORD CPiDBDrv::GetHisVal(
	const char* stagname,	//��ǩ��
	long ltimes,			//��ʼʱ��
	long ltimee,			//����ʱ��
	long ldsec,				//���ʱ����
	long lflag,				//ʵʱ���־��
	TVVAL vals[],			//���ݻش�ռ�
	int	 nsize,				//�ռ��С
	int* pnvals				//ʵ�����ݸ���
	)	
{
	OutputDebug(_T("CPiDBDrv::GetHisVal begin: ip=%s, tag=%s, ltimes=%ld, ltimee=%ld, ldsec=%d, nsize=%ld"), 
		m_szIP, stagname, ltimes, ltimee, ldsec, nsize);
	
	if (NULL == stagname || nsize <= 0)
		return DBERR_FAIL;

	if (ltimes > ltimee)
		return DBERR_TIME;
	
	SAFELOCK(m_csLock);
		
	*pnvals = 0;
	CString szLog;
	
	DWORD dwret = CheckBatchTagAttr(stagname, vals[0].vt);
	if (dwret != DBERR_OK)
		return dwret;

	if (m_arrTagAttrs[0].lstatus) //ȡ��ǩ����		
		return DBERR_NOTAG;

	if (m_arrTagAttrs[0].lid <= 0)
		return DBERR_NOTAG;

	int32 pt = m_arrTagAttrs[0].lid;
	int32 nCount = nsize, nIndex = 0;
	PIvaluetype pttype = *((PIvaluetype*)m_arrTagAttrs[0].m_szSite);
	DWORD dwtype = DataTypeFrom(pttype);
	
	if (m_arrHisTimestamp.GetSize() < nsize)
		m_arrHisTimestamp.SetSize(nsize);
	if (m_arrHisFRvals.GetSize() < nsize)
		m_arrHisFRvals.SetSize(nsize);
	if (m_arrHisIstat.GetSize() < nsize)
		m_arrHisIstat.SetSize(nsize);
	
	PITIMESTAMP timestamp;
	Time2PiTimestamp(ltimes, &timestamp);
	ltimes = pitm_getpitime(&timestamp, 0);
	
	Time2PiTimestamp(ltimee, &timestamp);
	ltimee = pitm_getpitime(&timestamp, 0);

	int nret = 0, rev = FALSE;
	m_arrHisTimestamp[0] = ltimes;

	long lcount;//add 20090427
	long lmEndTime ; //add 20090427
	if (RTDB_HISTORY_RAW == lflag)	// ����ֵ
	{
		m_arrHisTimestamp[nCount-1] = ltimee;
		
		SetActiveServer(); // ѡ��ǰ���PI������
		nret = piar_compvalues(pt, &nCount, m_arrHisTimestamp.GetData(), m_arrHisFRvals.GetData(), m_arrHisIstat.GetData(), rev );
	}
	//else if (RTDB_HISTORY_INTERP == lflag) // ��ֵ
	//{
	//	arcmode = ARCflag_even;
	//	Time2PiTimestamp(ltimes, &m_arrHisTimestamp[0]);
	//}
	else // RTDB_HISTORY_PLOT���������ݣ�����Ѳ�ֵ����������ȡ��ʷ��ʽͬ������	
	{		
		if(ldsec <= 0) return DBERR_TIME;
		lcount = (ltimee - ltimes) / ldsec + 1;
		nCount = (lcount <= nsize) ? lcount : nsize;
		lmEndTime = ltimes + ldsec * (nCount - 1);
		m_arrHisTimestamp[nCount - 1] = lmEndTime;

		SetActiveServer(); // ѡ��ǰ���PI������
		nret = piar_interpvalues(pt, &nCount, m_arrHisTimestamp.GetData(), m_arrHisFRvals.GetData(), m_arrHisIstat.GetData());//ȡ��ֵ �����
	}

	if (nret != 0)
	{
		AddDebugFmt("piar_interpvalues���ô��󣬴�����Ϊ%d", nret);
	}

	if(-10733 == nret) return DBERR_CONNECT;
	if (nret == PI_NOMOREVALUES) return DBERR_NOMOREREC;
	if (nret > 0) return DBERR_CONNECT;
	if (nret == -1) return DBERR_NOTAG;
	if (nret == -998) return DBERR_MEMEORY;
	if (nret == -101) return DBERR_NOMOREREC;
	if (nret == -103) return DBERR_NOMOREREC;
	if (nret == -105) return DBERR_TIME;
	// nret == -30000��ʾ���ֳɹ����������ڸü�ʱ���Ѿ����ڶ�����
	// ����б�ǩֵ�ܴ󣬻ᱨ-15010���󣬿ɺ��Ըô���
	if ((nret < 0) && (nret != -30000) && (nret != -15010)) return DBERR_FAIL;

	//��������
	int32 digital = 0;
	nCount = (nCount < nsize) ? nCount : nsize;
	for(int i = 0; i < nCount; i++)
	{
		vals[i].vt = dwtype;
		switch(pttype) 
		{
		case PI_Type_int16:
		case PI_Type_int32:
		case PI_Type_int64:
			if(dwtype == DT_DIGITAL || dwtype == DT_INT32)
				vals[i].lval = (long)m_arrHisFRvals[i];
			else if(dwtype == DT_INT64)
				vals[i].llval = (__int64)m_arrHisFRvals[i];
			else if(dwtype == DT_FLOAT32)
				vals[i].fval = (float)m_arrHisFRvals[i];
			else if(dwtype == DT_FLOAT64)
				vals[i].dblval = (double)m_arrHisFRvals[i];
			
			vals[i].snqa = StateToQuality(m_arrHisIstat[i]);
			vals[i].snerr = 0;
			break;	
		case PI_Type_float16:
		case PI_Type_float32:
		case PI_Type_float64:
			if(dwtype == DT_DIGITAL || dwtype == DT_INT32)
				vals[i].lval = (long)m_arrHisFRvals[i];
			else if(dwtype == DT_INT64)
				vals[i].llval = (__int64)m_arrHisFRvals[i];
			else if(dwtype == DT_FLOAT32)
				vals[i].fval = (float)m_arrHisFRvals[i];
			else if(dwtype == DT_FLOAT64)
				vals[i].dblval = m_arrHisFRvals[i];
			
			vals[i].snqa = StateToQuality(m_arrHisIstat[i]);
			vals[i].snerr = 0;
			break;
		case PI_Type_digital:
			if(m_arrHisIstat[i] > 0 || m_arrHisIstat[i] < -314)//�������� ,���㿪������ʵֵ0,1
			{
				digital = -m_arrHisIstat[i] - m_arrTagAttrs[0].lStartCode;
				vals[i].snqa = ERRTV_OK;
			}
			else
			{
				digital = m_arrHisIstat[i];
				vals[i].snqa = StateToQuality(m_arrHisIstat[i]);
			}
			vals[i].snerr = 0;
			
			if(dwtype == DT_DIGITAL || dwtype == DT_INT32)
				vals[i].lval = digital;
			else if(dwtype == DT_INT64)
				vals[i].llval = digital;
			else if(dwtype == DT_FLOAT32)
				vals[i].fval = (float)digital;
			else if(dwtype == DT_FLOAT64)
				vals[i].dblval = digital;
			break;
		}
		pitm_setpitime(&timestamp, m_arrHisTimestamp[i], 0);
		vals[i].ltime = PiTimestamp2Time(&timestamp);
	}
	*pnvals = nCount;
	
	OutputDebug(_T("CPiDBDrv::GetHisVal end: ip=%s, tag=%s, count=%d"), m_szIP, stagname, nCount);

	return DBERR_OK;
}
*/
DWORD CPiDBDrv::GetHisVal(
	const char* stagname,	//��ǩ��
	long ltimes,			//��ʼʱ��
	long ltimee,			//����ʱ��
	long ldsec,				//���ʱ����
	long lflag,				//ʵʱ���־��
	TVVAL vals[],			//���ݻش�ռ�
	int	 nsize,				//�ռ��С
	int* pnvals				//ʵ�����ݸ���
	)	
{
	OutputDebug(_T("CPiDBDrv::GetHisVal begin: ip=%s, tag=%s, ltimes=%ld, ltimee=%ld, ldsec=%d, nsize=%ld"), 
		GetPIServer().m_szIP, stagname, ltimes, ltimee, ldsec, nsize);
	
	if (NULL == stagname || nsize <= 0)
		return DBERR_FAIL;

	if (ltimes > ltimee)
		return DBERR_TIME;
	
	SAFELOCK(m_csLock);

	if (m_arrTimestamp.GetSize() < nsize)
		m_arrTimestamp.SetSize(nsize);
		
	*pnvals = 0;
	CString szLog;
	
	DWORD dwret = CheckBatchTagAttr(stagname, vals[0].vt);
	if (dwret != DBERR_OK)
		return dwret;

	if (m_arrTagAttrs[0].lstatus) //ȡ��ǩ����		
		return DBERR_NOTAG;

	if (m_arrTagAttrs[0].lid <= 0)
		return DBERR_NOTAG;

	int32 pt = m_arrTagAttrs[0].lid;
	int32 nCount = nsize, nIndex = 0;
	float64 drval = 0.0;
	int32 ival = 0, istat = 0;
	int16 flags = 0;
	PITIMESTAMP time1;
	PIvaluetype pttype = *((PIvaluetype*)m_arrTagAttrs[0].m_szSite);
	DWORD dwtype = DataTypeFrom(pttype);

	int32 arcmode;
	if (RTDB_HISTORY_RAW == lflag)	// ����ֵ
	{
		arcmode = ARCflag_comp;
		Time2PiTimestamp(ltimes, &m_arrTimestamp[0]);
		Time2PiTimestamp(ltimee, &time1);
	}
	else if (RTDB_HISTORY_INTERP == lflag) // ��ֵ
	{
		if (ldsec <= 0) return DBERR_TIME;

		arcmode = ARCflag_time;

		nCount = min((ltimee - ltimes) / ldsec + 1, nsize);
		int i;
		for(i = 0; i < nCount; i++)
		{
			Time2PiTimestamp(ltimes + ldsec * i, &m_arrTimestamp[i]);
		}
		//Time2PiTimestamp(ltimee, &m_arrTimestamp[i]); // �����һ��86400����¼�������������ڴ����
		Time2PiTimestamp(ltimee, &time1);
	}
	else // RTDB_HISTORY_PLOT���������ݣ�����Ѳ�ֵ����������ȡ��ʷ��ʽͬ������	
	{
		// ÿ��ʱ��������󷵻�5��ֵ����ʼ����ֹ�������С���쳣
		//nCount = 5 * 1800;//min(((ltimee - ltimes) / ldsec + 1) * 5, nsize);

		if (ldsec <= 0)
			ldsec = 1800;
		nCount = min(ldsec * 5, nsize);

		CArray<int32, int32> times;
		CArray<float, float> rvals;
		CArray<int32, int32> istats;

		times.SetSize(nCount);
		rvals.SetSize(nCount);
		istats.SetSize(nCount);

		times[0] = ltimes - timezone;
		times[nCount - 1] = ltimee - timezone;

		SetActiveServer(); // ѡ��ǰ���PI������
		int32 nret = piar_plotvalues(pt, ldsec, &nCount, times.GetData(), rvals.GetData(), istats.GetData());

		if (nret != 0)
		{
			AddDebugFmt("piar_plotvalues���ô��󣬴�����Ϊ%d", nret);
		}

		if(-10733 == nret) return DBERR_CONNECT;
		if (nret == PI_NOMOREVALUES) return DBERR_NOMOREREC;
		if (nret > 0) return DBERR_CONNECT;
		if (nret == -1) return DBERR_NOTAG;
		if (nret == -998) return DBERR_MEMEORY;
		// nret == -30000��ʾ���ֳɹ����������ڸü�ʱ���Ѿ����ڶ�����
		// ����б�ǩֵ�ܴ󣬻ᱨ-15010���󣬿ɺ��Ըô���
		if ((nret < 0) && (nret != -30000) && (nret != -15010)) return DBERR_FAIL;

		if (nCount == 0)
		{
			return DBERR_NOVAL;
		}

		int32 digicode = m_arrTagAttrs[0].lStartCode;
		for (int i = 0; i < nCount; i++)
		{
			vals[i].vt = dwtype;
			SetTVVAL(&vals[i], digicode, pttype, rvals[i], istats[i], times[i] + timezone);
			if (i > nCount - 4)
			{
				AddDebugFmt(_T("PLOT DATA: time=%d, val=%f, snqa=%d"), vals[i].ltime, vals[i].fval, vals[i].snqa);
			}
		}

		// ȥ�����һ����ͬʱ����ֵ
		if (nCount > 1)
		{
			if (vals[nCount - 1].ltime == vals[nCount - 2].ltime)
				nCount--;
		}

		// ����PLOT��ʷ�����һ�����ܳ���������Ϊ-248��������ü�¼Ӧ�ù��ǵ�
		if (nCount > 0)
		{
			if (vals[nCount - 1].snqa == -248)
				nCount--;
		}

		*pnvals = nCount;

		return DBERR_OK;
		
		/*
		if (ldsec <= 0) return DBERR_TIME;

		arcmode = ARCflag_time;
		
		nCount = min((ltimee - ltimes) / ldsec + 1, nsize);
		int i;
		for(i = 0; i < nCount; i++)
		{
			Time2PiTimestamp(ltimes + ldsec * i, &m_arrTimestamp[i]);
		}
		//Time2PiTimestamp(ltimee, &m_arrTimestamp[i]); // �����һ��86400����¼�������������ڴ����
		Time2PiTimestamp(ltimee, &time1);
		*/
	}
	
	SetActiveServer(); // ѡ��ǰ���PI������
	int32 nret = piar_getarcvaluesx(pt, arcmode, &nCount, &drval, &ival, NULL, NULL, &istat,
		&flags, m_arrTimestamp.GetData(), &time1, GETFIRST);

	if (nret != 0)
	{
		AddDebugFmt("piar_getarcvaluesx���ô��󣬴�����Ϊ%d", nret);
	}

	if(-10733 == nret) return DBERR_CONNECT;
	if (nret == PI_NOMOREVALUES) return DBERR_NOMOREREC;
	if (nret > 0) return DBERR_CONNECT;
	if (nret == -1) return DBERR_NOTAG;
	if (nret == -998) return DBERR_MEMEORY;
	// nret == -30000��ʾ���ֳɹ����������ڸü�ʱ���Ѿ����ڶ�����
	// ����б�ǩֵ�ܴ󣬻ᱨ-15010���󣬿ɺ��Ըô���
	if ((nret < 0) && (nret != -30000) && (nret != -15010)) return DBERR_FAIL;

	int32 digicode = m_arrTagAttrs[0].lStartCode;

	long lPrevTime = 0;

	while(nret != PI_NOMOREVALUES)
	{
		long lt = PiTimestamp2Time(&time1);
		if (lPrevTime != lt) // ���ͬһʱ���ж��ֵ����ȡ�׸�
		{
			// ȡ����ʷ����
			vals[nIndex].vt = dwtype;
			SetTVVAL(&vals[nIndex], digicode, pttype, drval, ival, NULL, 0, istat, flags, lt, 0);
			nIndex++;

			lPrevTime = lt;
		}

		// �����������ռ�
		if (nsize == nIndex)
			break;
		
		drval = 0.0;
		ival = 0;
		istat = 0;
		flags = 0;
		nret = piar_getarcvaluesx(pt, arcmode, &nCount, &drval, &ival, NULL, NULL, &istat,
			&flags, m_arrTimestamp.GetData(), &time1, GETNEXT);
	}

	// ȥ�����һ����ͬʱ����ֵ
	if (nIndex > 1)
	{
		if (vals[nIndex - 1].ltime == vals[nIndex - 2].ltime)
			nIndex--;
	}
	
	*pnvals = nIndex;
	
	OutputDebug(_T("CPiDBDrv::GetHisVal end: ip=%s, tag=%s, count=%d"), GetPIServer().m_szIP, stagname, nIndex);

	return DBERR_OK;
}

//�����ǩͬһʱ����ʷ��ֻ��Ի�����������
/*
DWORD CPiDBDrv::GetHisVal2(
	TTAGVAL vals[],		//��ǩ���ͻ���ֵ�ռ�
	int		nsize,		//��ǩ����
	long	ltime		//ʱ��
	)	
{
	OutputDebug(_T("CPiDBDrv::GetHisVal2: ip=%s, size=%d"), m_szIP, nsize);

	if (nsize <= 0)
		return DBERR_FAIL;
	
	SAFELOCK(m_csLock);
	SetActiveServer(); // ѡ��ǰ���PI������
	
	CString szLog;
	
	int32 nret;	
	float64 drval = 0.0;
	int32 ival = 0, istat = 0;
	int16 flags = 0;
	PITIMESTAMP ftime;

	DWORD dwret = CheckBatchTagAttr(vals, nsize);
	if (dwret != DBERR_OK)
		return dwret;

	register int i;
	for (i = 0; i < nsize; i++)
	{
		if (m_arrTagAttrs[i].lstatus) { //ȡ��ǩ����
			vals[i].m_val.snqa = QA_ERR;
			vals[i].m_val.snerr = ERRTV_NOTAG;
			continue;
		}
		PIvaluetype pttype = *((PIvaluetype*)m_arrTagAttrs[i].m_szSite);
		DWORD dwtype = DataTypeFrom(pttype);
		Time2PiTimestamp(ltime, &ftime);
		
		nret = piar_getarcvaluex(m_arrTagAttrs[i].lid, ARCVALUEINTERP, &drval, &ival, NULL, NULL, &istat,
			&flags, &ftime);

		if (nret != 0)
		{
			AddDebugFmt("piar_getarcvaluex���ô��󣬴�����Ϊ%d", nret);
		}

		if(-10733 == nret) return DBERR_CONNECT;
		if (nret > 0) return DBERR_CONNECT;

		if (nret)
		{
			switch(nret)
			{
			case -1:
				vals[i].m_val.snqa = QA_ERR;
				vals[i].m_val.snerr = ERRTV_NOTAG;
				break;
			case -983: // Invalid mode
			case -991: // Not implemented
			case -998: // Memory allocation error
			case -11032: // No events after passed eventid
			case -11033: // No events before passed eventid
			case -15010: // Value truncated
				vals[i].m_val.snqa = QA_ERR;
				vals[i].m_val.snerr = ERRTV_ERR;
				break;;
			case -11049: // Invalid timestamp
				vals[i].m_val.snqa = QA_ERR;
				vals[i].m_val.snerr = ERRTV_TIME;
				break;
			default: // > 0 System error
				vals[i].m_val.snqa = QA_ERR;
				vals[i].m_val.snerr = ERRTV_ERR;
				break;
			}		
		}
		else {			
			vals[i].m_val.vt = dwtype;
			SetTVVAL(&(vals[i].m_val), m_arrTagAttrs[i].lStartCode, pttype, drval, ival, NULL, 0, istat, flags, &ftime,0);
		}
		
	}

	return DBERR_OK;
}
*/
//ȡһ���ַ������ͱ�ǩ��ʷֵ
DWORD CPiDBDrv::GetHisStr(
	DWORD dwrst,
	LPCTSTR sTagName,
	long ltimes,
	TVVAL** ppv
	)	
{
	OutputDebug(_T("CPiDBDrv::GetHisStr: tag=%s"), sTagName);
	
	SAFELOCK(m_csLock);
	CString szLog;
	
	int32 nret;	
	int32 istat = 0;
	int16 flags = 0;
	uint32 bsize = m_arrStrValue.GetSize();
	PITIMESTAMP ftime;
	
	*ppv = NULL;
	memset(&m_vStrValue, 0, sizeof(TVVAL));
	m_vStrValue.vt = DT_STRING;

	DWORD dwret = CheckBatchTagAttr(sTagName, DT_STRING);
	if (dwret != DBERR_OK)
		return dwret;

	if (m_arrTagAttrs[0].lstatus) //ȡ��ǩ����		
		return DBERR_NOTAG;

	PIvaluetype pttype = *((PIvaluetype*)m_arrTagAttrs[0].m_szSite);
	if (pttype != PI_Type_PIstring)
	{
		AddErrorFmt("��ǩ<%s>���ʹ���", sTagName);
		return DBERR_TYPE;
	}
	DWORD dwtype = DataTypeFrom(pttype);
	Time2PiTimestamp(ltimes, &ftime);
	
	SetActiveServer(); // ѡ��ǰ���PI������
	nret = piar_getarcvaluex(m_arrTagAttrs[0].lid, ARCVALUEINTERP, NULL, NULL, m_arrStrValue.GetData(), &bsize, &istat,
		&flags, &ftime);

	if (nret != 0)
	{
		AddDebugFmt("piar_getarcvaluex���ô��󣬴�����Ϊ%d", nret);
	}

	if (nret == -15010) // PI_OVERFLOW
	{
		bsize++; // ����NULL������
		if (bsize > PI_STRING_BUFFER_MAX)
		{
			return DBERR_MEMEORY;
		}
		m_arrStrValue.SetSize(bsize);
		
		nret = piar_getarcvaluex(m_arrTagAttrs[0].lid, ARCVALUEINTERP, NULL, NULL, m_arrStrValue.GetData(), &bsize, &istat,
			&flags, &ftime);
	}

	if(-10733 == nret) return DBERR_CONNECT;
	if (nret > 0) return DBERR_CONNECT;
	
	if (nret)
	{
		switch(nret)
		{
		case -1:
			m_vStrValue.snqa = DBP_QUALITY_BAD;
			m_vStrValue.snerr = ERRTV_NOTAG;
			break;
		case -983: // Invalid mode
		case -991: // Not implemented
		case -998: // Memory allocation error
		case -11032: // No events after passed eventid
		case -11033: // No events before passed eventid
		case -15010: // Value truncated
			m_vStrValue.snqa = DBP_QUALITY_BAD;
			m_vStrValue.snerr = ERRTV_ERR;
			break;;
		case -11049: // Invalid timestamp
			m_vStrValue.snqa = DBP_QUALITY_BAD;
			m_vStrValue.snerr = ERRTV_TIME;
			break;
		default: // > 0 System error
			m_vStrValue.snqa = DBP_QUALITY_BAD;
			m_vStrValue.snerr = ERRTV_ERR;
			break;
		}		
	}
	else {
		long lt = PiTimestamp2Time(&ftime);
		SetTVVAL(&m_vStrValue, m_arrTagAttrs[0].lStartCode, pttype, 0, 0, m_arrStrValue.GetData(), bsize, istat, flags, lt,0);//wzq
	}
	*ppv = &m_vStrValue;

	return DBERR_OK;
}

//ȡһ��blob���ͱ�ǩ��ʷֵ
DWORD CPiDBDrv::GetHisBlob(
	DWORD dwrst,
	LPCTSTR sTagName,
	long ltimes,
	TVVAL** ppv
	)	
{
	OutputDebug(_T("CPiDBDrv::GetHisBlob: tag=%s"), sTagName);
	
	SAFELOCK(m_csLock);

	CString szLog;
	
	int32 nret;	
	int32 istat;
	int16 flags;
	uint32 bsize = m_arrStrValue.GetSize();
	PITIMESTAMP ftime;
	
	*ppv = NULL;
	memset(&m_vStrValue, 0, sizeof(TVVAL));
	m_vStrValue.vt = DT_BLOB;
	
	DWORD dwret = CheckBatchTagAttr(sTagName, DT_BLOB);
	if (dwret != DBERR_OK)
		return dwret;

	if (m_arrTagAttrs[0].lstatus) //ȡ��ǩ����		
		return DBERR_NOTAG;

	PIvaluetype pttype = *((PIvaluetype*)m_arrTagAttrs[0].m_szSite);
	if (pttype != PI_Type_blob)
	{
		AddErrorFmt("��ǩ<%s>���ʹ���", sTagName);
		return DBERR_TYPE;
	}
	DWORD dwtype = DataTypeFrom(pttype);
	Time2PiTimestamp(ltimes, &ftime);
	
	SetActiveServer(); // ѡ��ǰ���PI������
	nret = piar_getarcvaluex(m_arrTagAttrs[0].lid, ARCVALUEINTERP, NULL, NULL, m_arrStrValue.GetData(), &bsize, &istat,
		&flags, &ftime);

	if (nret != 0)
	{
		AddDebugFmt("piar_getarcvaluex���ô��󣬴�����Ϊ%d", nret);
	}

	if (nret == -15010) // PI_OVERFLOW
	{
		if (bsize > PI_STRING_BUFFER_MAX)
		{
			return DBERR_MEMEORY;
		}
		m_arrStrValue.SetSize(bsize);
		
		nret = piar_getarcvaluex(m_arrTagAttrs[0].lid, ARCVALUEINTERP, NULL, NULL, m_arrStrValue.GetData(), &bsize, &istat,
			&flags, &ftime);
	}

	if(-10733 == nret) return DBERR_CONNECT;
	if (nret > 0) return DBERR_CONNECT;
	
	if (nret)
	{
		switch(nret)
		{
		case -1:
			m_vStrValue.snqa = DBP_QUALITY_BAD;
			m_vStrValue.snerr = ERRTV_NOTAG;
			break;
		case -983: // Invalid mode
		case -991: // Not implemented
		case -998: // Memory allocation error
		case -11032: // No events after passed eventid
		case -11033: // No events before passed eventid
		case -15010: // Value truncated
			m_vStrValue.snqa = DBP_QUALITY_BAD;
			m_vStrValue.snerr = ERRTV_ERR;
			break;;
		case -11049: // Invalid timestamp
			m_vStrValue.snqa = DBP_QUALITY_BAD;
			m_vStrValue.snerr = ERRTV_TIME;
			break;
		default: // > 0 System error
			m_vStrValue.snqa = DBP_QUALITY_BAD;
			m_vStrValue.snerr = ERRTV_ERR;
			break;
		}		
	}
	else {
		long lt = PiTimestamp2Time(&ftime);
		SetTVVAL(&m_vStrValue, m_arrTagAttrs[0].lStartCode, pttype, 0, 0, m_arrStrValue.GetData(), bsize, istat, flags, lt,0);
	}
	*ppv = &m_vStrValue;
	
	return DBERR_OK;
}

//////////////////////////////////////////////////////////////////////
//������
//////////////////////////////////////////////////////////////////////

//����һ��SOE
DWORD CPiDBDrv::GetSoe(
	DWORD dwrst,
	long ltimes, //��ʼʱ��
	long lmsecs,	//��ʼ����
	const char* ssrc, //��Դ,NULL��ʾȫ��
	const char* sdec, //DDClient�豸,NULL��ʾȫ��
	long lclsid,       //�¼�����,0��ʾȫ��
	TSOE**  lppSoe
	)	
{
	OutputDebug(_T("CPiDBDrv::GetSoe"));

	return DBERR_NOTSUPPORT;
}

//��ͳ��ֵ��ֻ��Ի�����������
DWORD CPiDBDrv::GetSumVal(
	const char* stagname,	//��ǩ��
	long ltimes,			//��ʼʱ��
	long ltimee,			//����ʱ��
	int  nSumType,			//ͳ�Ʒ�ʽ
	long ldsec,				//�ֶ�ʱ�䣬��0��ʾȫ����
	TVVAL vals[],			//���ݻش�ռ�
	int	 nsize,				//�ռ��С
	int* pnvals				//ʵ�ʸ���
	)	
{
	OutputDebug(_T("CPiDBDrv::GetSumVal: tag=%s, SumType=%d"), stagname, nSumType);
	
	*pnvals = 0;

	if (NULL == stagname || NULL == *stagname) return DBERR_FAIL;

	if (nsize <= 0) return DBERR_FAIL;

	if (ltimes > ltimee || ldsec < 0) return DBERR_TIME;
	
	if(nSumType < RTDB_SUMMARY_FIRST || nSumType > RTDB_SUMMARY_LAST) return DBERR_FAIL;
	
	int nCount = 0;
	float rval;
	float pctgood;
	int32 nret;
	float64 frac;
	int32 lStartTime, lEndTime;
	long endtime = (ldsec == 0) ? ltimee : (ltimes + ldsec);
	if (endtime > ltimee) endtime = ltimee;


	SAFELOCK(m_csLock);
	
	DWORD dwret = CheckBatchTagAttr(stagname, vals[0].vt);
	if (dwret != DBERR_OK)
		return dwret;

	if (m_arrTagAttrs[0].lstatus) //ȡ��ǩ����		
		return DBERR_NOTAG;

	switch(nSumType)
	{
	case  RTDB_SUMMARY_INTEGRAL_TOTAL:
		return m_summary.GetIntegralTotalValue(&m_arrTagAttrs[0], ltimes, ltimee, ldsec, vals, nsize, pnvals);
		break;
	case RTDB_SUMMARY_CIRC_TOTAL:
		return m_summary.GetCircTotalValue(&m_arrTagAttrs[0], ltimes, ltimee, ldsec, vals, nsize, pnvals);
		break;
	case RTDB_SUMMARY_DEVIATION:
		return m_summary.GetDeviationValue(&m_arrTagAttrs[0], ltimes, ltimee, ldsec, vals, nsize, pnvals);
		break;
	case  RTDB_SUMMARY_INTEGRAL_AVG:
		return m_summary.GetIntegralAvgValue(&m_arrTagAttrs[0], ltimes, ltimee, ldsec, vals, nsize, pnvals);
		break;	
	}

	PITIMESTAMP timestamp;
	PIvaluetype pttype = *((PIvaluetype*)m_arrTagAttrs[0].m_szSite);
	
	SetActiveServer(); // ѡ��ǰ���PI������
	while(ltimes < ltimee)
	{
		Time2PiTimestamp(ltimes, &timestamp);
		lStartTime = pitm_getpitime(&timestamp, &frac);

		Time2PiTimestamp(endtime, &timestamp);
		lEndTime = pitm_getpitime(&timestamp, &frac);

		switch( nSumType) 
		{
		case RTDB_SUMMARY_AVERAGE: // ƽ��ֵ
			nret = piar_summary(m_arrTagAttrs[0].lid, &lStartTime, &lEndTime, &rval, &pctgood, ARCAVERAGE);
			break;
		case RTDB_SUMMARY_MINIMUM: // ��Сֵ
			nret = piar_summary(m_arrTagAttrs[0].lid, &lStartTime,&lEndTime,	&rval, &pctgood, ARCMINIMUM);
			break;
		case RTDB_SUMMARY_MAXIMUM: // ���ֵ
			nret = piar_summary(m_arrTagAttrs[0].lid, &lStartTime,&lEndTime,	&rval, &pctgood, ARCMAXIMUM);
			break;
		}

		if (nret != 0)
		{
			AddDebugFmt("piar_summary���ô��󣬴�����Ϊ%d", nret);
		}

		if(-10733 == nret) return DBERR_CONNECT;
		if (nret > 0) return DBERR_CONNECT;

		switch(nret)
		{
		case 0: // Success
		case -15010: // ֵ̫�󣬱��ض�
			vals[nCount].vt = DT_FLOAT32;
			vals[nCount].fval = rval;
			vals[nCount].snqa = DBP_QUALITY_GOOD;
			vals[nCount].snerr = ERRTV_OK;
			OutputDebug(_T("CPiDBDrv::GetSumVal: tag=%s, SumType=%d, rval=%f, pctgood=%f"), stagname, nSumType, rval, pctgood);
			break;
		case -1: // Bad point number
			OutputDebug(_T("CPiDBDrv::GetSumVal: tag=%s, SumType=%d, rval=%f, pctgood=%f, Error=%d"), stagname, nSumType, rval, pctgood, nret);
			return DBERR_NOTAG;
		case -105: // Last time not after first time or time < 0
			OutputDebug(_T("CPiDBDrv::GetSumVal: tag=%s, SumType=%d, rval=%f, pctgood=%f, Error=%d"), stagname, nSumType, rval, pctgood, nret);
			return DBERR_TIME;
		case -106: // No good data for this point for this time
		case -11059:
			vals[nCount].vt = DT_FLOAT32;
			vals[nCount].fval = rval;
			vals[nCount].snqa = DBP_QUALITY_BAD;
			vals[nCount].snerr = ERRTV_ERR;
			OutputDebug(_T("CPiDBDrv::GetSumVal: tag=%s, SumType=%d, rval=%f, pctgood=%f, Error=%d"), stagname, nSumType, rval, pctgood, nret);
			break;
		case -104: // Invalid code
		case -120: // Invalid code for the currently connected server.
		default: // >0 System error
			OutputDebug(_T("CPiDBDrv::GetSumVal: tag=%s, SumType=%d, rval=%f, pctgood=%f, Error=%d"), stagname, nSumType, rval, pctgood, nret);
			return DBERR_FAIL;
		}

		if (0 == pitm_setpitime(&timestamp, lEndTime, 0))
			vals[nCount].ltime = PiTimestamp2Time(&timestamp);
		else
			vals[nCount].ltime = lEndTime;

		nCount++;
		ltimes = endtime/* + 1*/;
		endtime = ltimes + ldsec;
		if (endtime > ltimee) endtime = ltimee;

		if (nCount == nsize)
			break;
	}

	*pnvals = nCount;

	return (nCount > 0) ? DBERR_OK : DBERR_FAIL;
}  

//////////////////////////////////////////////////////////////////////
//ɾ��
//////////////////////////////////////////////////////////////////////

//ɾ��SOE�¼�
DWORD CPiDBDrv::DelSoe(
	long ltimes, //��ʼʱ��
	long lmsecs, //��ʼ����
	long timee,  //����ʱ��
	long lmsece, // ��������
	const char* ssrc, //��Դ,NULL��ʾȫ��
	const char* sdec, //DDClient�豸,NULL��ʾȫ��
	long lclsid       //�¼�����,0��ʾȫ��
	)	
{
	OutputDebug(_T("CPiDBDrv::DelSoe"));

	return DBERR_NOTSUPPORT;
}

/*
 * ����������ش��󣬺�������DBERR_CONNECT��DBERR_FAIL��
 * ���δ�ҵ��κο�ɾ����¼����������-1
 * �м�¼��ɾ������������0x0FFFFF
 */
int CPiDBDrv::DoDeleteHisVal(int32 nPointID, long ltimes, long ltimee)
{
	/*
	int32 count = RTDB_TAGNUM_MAX;
	int nDelCount = 0;
	m_arrIval[0] = ltimes;
	m_arrIval[count - 1] = ltimee;
	
	int32 nret = piar_compvalues(nPointID, &count, m_arrIval.GetData(), 
		(float*)m_arrDrval.GetData(), m_arrIstat.GetData(), 0);
		
	if ((nret == -101) || (nret == -103) || (nret == -105))
		return -1; // ��ʱ������޿�ɾ����¼

	if(-10733 == nret) return DBERR_CONNECT;
	if (nret > 0) return DBERR_CONNECT;

	if (count == 0) // û����ʷ������
		return 0;

	if (nret != 0)
		return DBERR_FAIL;

	for(int i = 0; i < count; i++)
	{
		nret = piar_deletevalue(nPointID, m_arrIval[i]);
		
		if(-10733 == nret) return DBERR_CONNECT;
		if (nret > 0) return DBERR_CONNECT;

		if ((nret == -101) || (nret == -103) || (nret == -108))
			continue;

		if (nret != 0)
			return DBERR_FAIL;

		nDelCount++;
	}
	*/	
	int32 count = RTDB_TAGNUM_MAX;
	float64 drval;
	int32 ival, istat;
	uint32 bsize = m_arrStrValue.GetSize();
	int16 flags;
	PITIMESTAMP time0, time1;
	int nCount = 0, nDelCount = 0;

	Time2PiTimestamp(ltimes, &time0);
	Time2PiTimestamp(ltimee, &time1);
	
	int32 nret = piar_getarcvaluesx(nPointID, ARCflag_comp, &count, &drval, &ival, m_arrStrValue.GetData(), &bsize,
		&istat, &flags, &time0, &time1, GETFIRST);

	if (nret != 0)
	{
		AddDebugFmt("piar_getarcvaluesx���ô��󣬴�����Ϊ%d", nret);
	}

	if (-10733 == nret) return DBERR_CONNECT;
	if (nret == PI_NOMOREVALUES) return -1;
	if (nret > 0) return DBERR_CONNECT;
	if (nret != 0) return DBERR_FAIL;
	
	while((nret != PI_NOMOREVALUES) && (nCount < RTDB_TAGNUM_MAX))
	{
		m_arrTimestamp[nCount] = time1;

		nret = piar_getarcvaluesx(nPointID, ARCflag_comp, &count, &drval, &ival, m_arrStrValue.GetData(), &bsize,
			&istat, &flags, &time0, &time1, GETNEXT);
		nCount++;
	}
	for(int i = 0; i < nCount; i++)
	{
		int32 ftime = pitm_getpitime(&m_arrTimestamp[i], &drval);
		
		nret = piar_deletevalue(nPointID, ftime);
		
		if(-10733 == nret) return DBERR_CONNECT;
		if (nret > 0) return DBERR_CONNECT;
		
		if ((nret == -101) || (nret == -103) || (nret == -108))
			continue;
		
		if (nret != 0)
			return DBERR_FAIL;
		
		nDelCount++;
	}
	return (nDelCount > 0) ? 0x0FFFFF : -1;
}

DWORD CPiDBDrv::DelHisVal(
	const char* stagname, //��ǩ��
	long	 ltimes,    //��ʼʱ��
	long	 ltimee		//����ʱ��
	)	
{
	OutputDebug(_T("CPiDBDrv::DelHisVal: tag=%s"), stagname);
	
	SAFELOCK(m_csLock);

	DWORD dwret = CheckBatchTagAttr(stagname, DT_VOID);
	if (dwret != DBERR_OK)
		return dwret;

	if (m_arrTagAttrs[0].lstatus) //ȡ��ǩ����		
		return DBERR_NOTAG;

	/*
	Time2PiTimestamp(ltimes, &timestamp);
	ltimes = pitm_getpitime(&timestamp, &frac);
	
	Time2PiTimestamp(ltimee, &timestamp);
	ltimee = pitm_getpitime(&timestamp, &frac);
	*/
	SetActiveServer(); // ѡ��ǰ���PI������
	int nret = 0x0FFFFF;
	while(nret == 0x0FFFFF)
	{
		nret = DoDeleteHisVal(m_arrTagAttrs[0].lid, ltimes, ltimee);
		if (nret == DBERR_CONNECT || nret == DBERR_FAIL)
			return nret;
	}

	return DBERR_OK;
}

DWORD CPiDBDrv::DelHisVal2(
	const char stagnames[][80], //��ǩ������
	long	ltimes[],			//��Ӧÿ����ǩ��ʱ��
	int		nerrcodes[],			//�����룬����
	int		nsize				//��¼����
	)
{
	OutputDebug(_T("CPiDBDrv::DelHisVal2: size = %d"), nsize);
	
	SAFELOCK(m_csLock);

	SetActiveServer(); // ѡ��ǰ���PI������

	int32 nret, ftime;
	PITIMESTAMP timestamp;
	float64 frac;
	
	DWORD dwret = CheckBatchTagAttr(stagnames, nsize);
	if (dwret != DBERR_OK)
		return dwret;

	int nCount = 0;
	register int i;
	for (i = 0; i < nsize; i++)
	{	
		if (m_arrTagAttrs[i].lstatus) 
		{
			nerrcodes[i] = ERRTV_NOTAG;
			continue;
		}
		
		Time2PiTimestamp(ltimes[i], &timestamp);
		ftime = pitm_getpitime(&timestamp, &frac);

		nret = piar_deletevalue( m_arrTagAttrs[i].lid, ftime );		

		if (nret != 0)
		{
			AddDebugFmt("piar_deletevalue���ô��󣬴�����Ϊ%d", nret);
		}

		if(-10733 == nret) return DBERR_CONNECT;
		if (nret > 0) return DBERR_CONNECT;
		
		switch(nret)
		{
		case 0: // Success
			nerrcodes[i] = ERRTV_OK;
			nCount++;
			break;
		case -1: // Bad point number
			nerrcodes[i] = ERRTV_NOTAG;
			break;
		case -101: // Date not on-line
		case -103: // No data for this point during the specified time range
		case -108: // Time is before the editing time limit
			nerrcodes[i] = ERRTV_TIME;
			break;
		default: // < 0
			nerrcodes[i] = ERRTV_ERR;
			break;
		}
	}

	return (nCount > 0) ? DBERR_OK : DBERR_FAIL;
}

//////////////////////////////////////////////////////////////////////////
//
// ����дSTRING��BLOB����ʵʱ����ʷ
//
//////////////////////////////////////////////////////////////////////////

void CPiDBDrv::InitBatchParams()
{
	m_nStrRealVals = 0;
	m_nStrHisVals = 0;
	memset(m_arrStrRealVals, 0, sizeof(TTAGVAL) * CACHE_ITEM_MAX);
	memset(m_arrStrHisVals, 0, sizeof(TTAGVAL) * CACHE_ITEM_MAX);
}

void CPiDBDrv::ClearTagVals(LPTAGVAL2 pvals, int nsize)
{
	for(int i = 0; i < nsize; i++)
	{
		((CVarVal*)(&pvals[i].m_val))->Clear();
	}
}

DWORD CPiDBDrv::AddBatchTag(LPTAGVAL pval, BOOL bReal/* = TRUE*/)
{	
	OutputDebug(_T("CPiDBDrv::AddBatchTag m_csBatchLock.Enter(0)"));
	TPRI::CCriticalSection::Owner lock(m_csBatchLock);
	OutputDebug(_T("CPiDBDrv::AddBatchTag m_csBatchLock.Enter(1)"));

	DWORD dwret = CheckBatchTagAttr(pval, 1);

	AddDebugFmt("AddBatchTag-----%s-------", pval[0].m_sname);

	if (dwret != DBERR_OK)
	{
		pval->m_val.snerr = ERRTV_NOTAG;
		return dwret;
	}
	
	if(m_arrTagAttrs[0].lstatus) //ȡ��ǩ����
	{
		pval->m_val.snerr = ERRTV_NOTAG;
		return DBERR_NOTAG;
	}

	if (m_arrTagAttrs[0].lid <= 0)
	{
		pval->m_val.snerr = ERRTV_NOTAG;
		return DBERR_NOTAG;
	}

	if (bReal)
	{
		m_arrStrRealVals[m_nStrRealVals].lid = m_arrTagAttrs[0].lid;
		m_arrStrRealVals[m_nStrRealVals].pitype = *((PIvaluetype*)m_arrTagAttrs[0].m_szSite);
		*((CVarVal*)&m_arrStrRealVals[m_nStrRealVals].m_val) = pval->m_val;
		m_nStrRealVals++;

		if (m_nStrRealVals == CACHE_ITEM_MAX)
			return FlushBatchTags(TRUE);
	}
	else {
		m_arrStrHisVals[m_nStrHisVals].lid = m_arrTagAttrs[0].lid;
		m_arrStrHisVals[m_nStrHisVals].pitype = *((PIvaluetype*)m_arrTagAttrs[0].m_szSite);
		*((CVarVal*)&m_arrStrHisVals[m_nStrHisVals].m_val) = pval->m_val;
		m_nStrHisVals++;

		if (m_nStrHisVals == CACHE_ITEM_MAX)
			return FlushBatchTags(FALSE);
	}
	
	OutputDebug(_T("CPiDBDrv::AddBatchTag m_csBatchLock.Leave()"));
	return DBERR_OK;
}

DWORD CPiDBDrv::FlushBatchTags(BOOL bReal/* = TRUE*/)
{
	SAFELOCK(m_csLock);
	
	CString szLog;
	if (bReal)
	{
		int nTrueSize = InitWriteTagValues(m_arrStrRealVals, m_nStrRealVals);
		if (nTrueSize == -1)
			return DBERR_CONNECT;
		else if (nTrueSize == 0)
			return DBERR_NOTAG;
		
		SetActiveServer(); // ѡ��ǰ���PI������
		int32 nret = pisn_putsnapshotsx(nTrueSize, m_arrPoint.GetData(), m_arrDrval.GetData(),
			m_arrIval.GetData(), m_arrBval.GetData(), m_arrBsize.GetData(), m_arrIstat.GetData(), 
			m_arrFlags.GetData(), m_arrTimestamp.GetData(), m_arrErrors.GetData());

		if (nret != 0)
		{
			AddDebugFmt("pisn_putsnapshotsx���ô��󣬴�����Ϊ%d", nret);
		}
		
		if(-10733 == nret) return DBERR_CONNECT;
		if (nret > 0) return DBERR_CONNECT;
		if (nret == -1) return DBERR_NOTAG;
		if (nret == -998) return DBERR_MEMEORY;
		// nret == -30000��ʾ���ֳɹ����������ڸü�ʱ���Ѿ����ڶ�����
		// ����б�ǩֵ�ܴ󣬻ᱨ-15010���󣬿ɺ��Ըô���
		if ((nret < 0) && (nret != -30000) && (nret != -15010)) return DBERR_FAIL;
		
		DWORD dwCount = 0;
		for(int i = 0; i < nTrueSize; i++)
		{
			int nIndex = m_arrIndex[i];
			DWORD dwError = m_arrErrors[i];
			if (0 == dwError)
			{
				dwCount++;
			}
			else
			{
				AddErrorFmt("д��ǩID��%d���Ŀ��ճ���������Ϊ��%d", m_arrStrRealVals[nIndex].lid, dwError);
			}
		}
		
		OutputDebug(_T("CPiDBDrv::WriteBatchReal end: ip=%s nret=%d dwCount=%d"), GetPIServer().m_szIP, nret, dwCount);
		
		ClearTagVals(m_arrStrRealVals, m_nStrRealVals);
		m_nStrRealVals = 0;
	}
	else {
		int nTrueSize = InitWriteTagValues(m_arrStrHisVals, m_nStrHisVals);
		if (nTrueSize == -1)
			return DBERR_CONNECT;
		else if (nTrueSize == 0)
			return DBERR_NOTAG;
		
		SetActiveServer(); // ѡ��ǰ���PI������
		int nret = piar_putarcvaluesx(nTrueSize, ARCREPLACE, m_arrPoint.GetData(), m_arrDrval.GetData(),
			m_arrIval.GetData(), m_arrBval.GetData(), m_arrBsize.GetData(), m_arrIstat.GetData(), 
			m_arrFlags.GetData(), m_arrTimestamp.GetData(), m_arrErrors.GetData());

		if (nret != 0)
		{
			AddDebugFmt("piar_putarcvaluesx���ô��󣬴�����Ϊ%d", nret);
		}

		if(-10733 == nret) return DBERR_CONNECT;
		if (nret > 0) return DBERR_CONNECT;
		if (nret == -1) return DBERR_NOTAG;
		if (nret == -998) return DBERR_MEMEORY;
		// nret == -30000��ʾ���ֳɹ����������ڸü�ʱ���Ѿ����ڶ�����
		// ����б�ǩֵ�ܴ󣬻ᱨ-15010���󣬿ɺ��Ըô���
		if ((nret < 0) && (nret != -30000) && (nret != -15010)) return DBERR_FAIL;
		
		DWORD dwCount = 0;
		for(int i = 0; i < nTrueSize; i++)
		{
			int nIndex = m_arrIndex[i];
			DWORD dwError = m_arrErrors[i];
			if (0 == dwError)
			{
				dwCount++;
			}
			else
			{				
				AddErrorFmt("д��ǩID��%d������ʷ��¼����������Ϊ��%d", m_arrStrHisVals[nIndex].lid, dwError);
			}
		}
		
		OutputDebug(_T("CPiDBDrv::WriteBatchHis end: ip=%s nret=%d dwCount=%d"), GetPIServer().m_szIP, nret, dwCount);
		

		ClearTagVals(m_arrStrHisVals, m_nStrHisVals);
		m_nStrHisVals = 0;
	}

	return DBERR_OK;
}

void CPiDBDrv::StartBatchThread()
{
	TPRI::CCriticalSection::Owner lock(m_csBatchLock);

	m_bKillThread = FALSE;
	if (NULL == m_pThread)
	{		
		m_pThread = ::AfxBeginThread(RunBatchThread, this);
		m_pThread->m_bAutoDelete = FALSE;
	}
	OutputDebug(_T("CPiDBDrv::StartBatchThread m_csBatchLock.Leave()"));
}

void CPiDBDrv::StopBatchThread()
{
	if (NULL == m_pThread)
		return;
	
	m_bKillThread = TRUE;
	while(TRUE) {
		MSG __msg;
		if (::MsgWaitForMultipleObjects(1, &m_pThread->m_hThread, FALSE, INFINITE, QS_ALLINPUT) == WAIT_OBJECT_0)
		{
			break;
		}
		else
		{
			while (PeekMessage(&__msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&__msg);
				DispatchMessage(&__msg);
			}
		}
	}
	delete m_pThread;
	m_pThread = NULL;
	m_bKillThread = FALSE;
}

UINT CPiDBDrv::RunBatchThread(void *pParam)// �����߳�
{
	CPiDBDrv *pdrv=(CPiDBDrv *)pParam;
	
	LARGE_INTEGER litmp; 
	LONGLONG QPart1, QPart2;
	double dfMinus, dfFreq, dfTim;

	// ��ü�������ʱ��Ƶ��
	::QueryPerformanceFrequency(&litmp);
	dfFreq = (double)litmp.QuadPart;

	DWORD dwTime = 0;

	while(!pdrv->m_bKillThread)
	{
		::QueryPerformanceCounter(&litmp);
		QPart1 = litmp.QuadPart;// ��ó�ʼֵ
		do
		{
			if (pdrv->m_bKillThread)
			{
				TRACE("CPiDBDrv RunBatchThread Exit Success!\n");
				break;
			}

			Sleep(10);
			/*
			MSG message;
			while (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&message);
				::DispatchMessage(&message);
			}
			*/

			::QueryPerformanceCounter(&litmp);
			QPart2 = litmp.QuadPart;//�����ֵֹ

			dfMinus = (double)(QPart2-QPart1);
			dfTim = dfMinus / dfFreq;// ��ö�Ӧ��ʱ��ֵ����λΪ��
		}while(dfTim < FLUSH_DELAY_TIME);

		//OutputDebug(_T("CPiDBDrv::RunBatchThread Real m_csBatchLock.Enter(0)"));
		pdrv->m_csBatchLock.Enter();
		//OutputDebug(_T("CPiDBDrv::RunBatchThread Real m_csBatchLock.Enter(1)"));
		if (pdrv->m_nStrRealVals > 0)
		{
			pdrv->FlushBatchTags(TRUE);
		}

		if (pdrv->m_nStrHisVals > 0)
		{
			pdrv->FlushBatchTags(FALSE);
		}

		// ����ʱ�������Ӳ���
		dwTime++;
		if (dwTime * FLUSH_DELAY_TIME > 30) // 30�봥��һ��
		{
			pdrv->CheckConnect();
			dwTime = 0;
		}

		//OutputDebug(_T("CPiDBDrv::RunBatchThread His m_csBatchLock.Leave()"));
		pdrv->m_csBatchLock.Leave();
	}
	
	// ���������ܻ��������
	pdrv->m_csBatchLock.Enter();
	if (pdrv->m_nStrRealVals > 0)
	{
		pdrv->FlushBatchTags(TRUE);
	}

	if (pdrv->m_nStrHisVals > 0)
	{
		pdrv->FlushBatchTags(FALSE);
	}
	pdrv->m_csBatchLock.Leave();
	
	//TRACE("CPiDBDrv RunBatchThread Exit Success!\n");
	return 0;
}

// ������ʱ�õĲ�ֵ
DWORD  CPiDBDrv::GetMultiPointHisVal(
						  char sTagNames[][80], 
						  long lTimes[], 
						  long lMode,
						  TVVAL tvs[], 
						  int nsize
						  )
{
	OutputDebug(_T("CPiDBDrv::GetMultiPointHisVal: ip=%s, size=%d"), GetPIServer().m_szIP, nsize);

	if (nsize <= 0)
		return DBERR_FAIL;

	SAFELOCK(m_csLock);
	SetActiveServer(); // ѡ��ǰ���PI������

	CString szLog;

	int32 nret;	
	float64 drval = 0.0;
	int32 ival = 0, istat = 0;
	int16 flags = 0;
	PITIMESTAMP ftime;

	DWORD dwret = CheckBatchTagAttr(sTagNames, tvs, nsize);
	if (dwret != DBERR_OK)
		return dwret;

	int32 nmode;
	switch(lMode)
	{
	case RHV_BEFORE:
		nmode = ARCVALUEBEFORE;
		break;
	case RHV_AFTER:
		nmode = ARCVALUEAFTER;
		break;
	case  RHV_INTERP:
	default:
		nmode = ARCVALUEINTERP;
		break;
	}

	int i;
	for (i = 0; i < nsize; i++)
	{
		if (m_arrTagAttrs[i].lstatus) { //ȡ��ǩ����
			tvs[i].snqa = DBP_QUALITY_BAD;
			tvs[i].snerr = ERRTV_NOTAG;
			continue;
		}
		PIvaluetype pttype = *((PIvaluetype*)m_arrTagAttrs[i].m_szSite);
		DWORD dwtype = DataTypeFrom(pttype);
		if ((dwtype == DT_STRING) || (dwtype == DT_BLOB))
		{
			tvs[i].snqa = DBP_QUALITY_BAD;
			tvs[i].snerr = ERRTV_NOTAG;
			continue;
		}
		Time2PiTimestamp(lTimes[i], &ftime);

		nret = piar_getarcvaluex(m_arrTagAttrs[i].lid, nmode, &drval, &ival, NULL, NULL, &istat,
			&flags, &ftime);

		if (nret != 0)
		{
			AddDebugFmt("piar_getarcvaluex���ô��󣬴�����Ϊ%d", nret);
		}

		if(-10733 == nret) return DBERR_CONNECT;
		if (nret > 0) return DBERR_CONNECT;

		if (nret && (nret != -15010))
		{
			switch(nret)
			{
			case -1:
				tvs[i].snqa = DBP_QUALITY_BAD;
				tvs[i].snerr = ERRTV_NOTAG;
				break;
			case -983: // Invalid mode
			case -991: // Not implemented
			case -998: // Memory allocation error
			case -11032: // No events after passed eventid
			case -11033: // No events before passed eventid
			//case -15010: // Value truncated
				tvs[i].snqa = DBP_QUALITY_BAD;
				tvs[i].snerr = ERRTV_ERR;
				break;;
			case -11049: // Invalid timestamp
				tvs[i].snqa = DBP_QUALITY_BAD;
				tvs[i].snerr = ERRTV_TIME;
				break;
			default: // > 0 System error
				tvs[i].snqa = DBP_QUALITY_BAD;
				tvs[i].snerr = ERRTV_ERR;
				break;
			}		
		}
		else {			
			tvs[i].vt = dwtype;
			long lt = PiTimestamp2Time(&ftime);
			SetTVVAL(&(tvs[i]), m_arrTagAttrs[i].lStartCode, pttype, drval, ival, NULL, 0, istat, flags, lt,0);
		}

	}

	return DBERR_OK;
}

DWORD  CPiDBDrv::GetDiSumVal(
						   LPCTSTR lpszTagName, 
						   long lts, 
						   long lte, 
						   long lds, 
						   long lval,
						   long lpSumCount[], 
						   long lpSumTimes[], 
						   int nsize, 
						   int* pnrecsize
						   )
{
	OutputDebug(_T("CPiDBDrv::GetDiSumVal: tag=%s"), lpszTagName);

	SAFELOCK(m_csLock);

	DWORD dwret = CheckBatchTagAttr(lpszTagName, DT_DIGITAL);
	if (dwret != DBERR_OK)
		return dwret;

	if (m_arrTagAttrs[0].lstatus) //ȡ��ǩ����		
		return DBERR_NOTAG;

	PIvaluetype pttype = *((PIvaluetype*)m_arrTagAttrs[0].m_szSite);
	if (pttype != PI_Type_digital)
	{
		AddErrorFmt("��ǩ<%s>���ʹ���", lpszTagName);
		return DBERR_TYPE;
	}
	//DWORD dwtype = DataTypeFrom(pttype);

	return m_summary.GetDiSumVal(lpszTagName, lts, lte, lds, lval, lpSumCount, lpSumTimes, nsize, pnrecsize);
}

/*
PI��Ч���������ʽ��

1. ��ѧ�������+���ӣ���-��������*���ˣ���/��������^��ָ�����㣩
2. ��ѧ������abs��atn��con��exp��int��log��sgn��sin��sqr��tan
3. ��ϵ�������<��=��>��<=��<>��>=
4. �߼��������and��or

���ӣ�

1. (('si:nusoid')^3 + 'cd:f144' / 10)
2. (log('cd:ep158') - cos('cd:ep158'))
3. (abs('21:01t3.pv') >= 101.4)
4. ('si:nusoid' < 45 and sqr('vd:f1002') > 2)
*/
DWORD CPiDBDrv::GetHisValFilter(
								  const char* stagname,	//��ǩ��
								  long ltimes,			//��ʼʱ��
								  long ltimee,			//����ʱ��
								  long ldsec,				//���ʱ����
								  long lflag,				//ʵʱ���־��
								  const char* expression,	//���������ַ���
								  TVVAL vals[],			//���ݻش�ռ�
								  int	 nsize,				//�ռ��С
								  int* pnvals				//ʵ�����ݸ���
								  )//���������˵Ķ���ʷ
{
	OutputDebug(_T("CPiDBDrv::GetHisValFilter begin: ip=%s, tag=%s, ltimes=%ld, ltimee=%ld, ldsec=%d, nsize=%ld, expression=%s"), 
		GetPIServer().m_szIP, stagname, ltimes, ltimee, ldsec, nsize, expression ? expression : _T(""));

	// ������˱��ʽΪ�գ���ֱ�ӵ���GetHisVal()
	if (!expression || !*expression)
		return GetHisVal(stagname, ltimes, ltimee, ldsec, lflag, vals, nsize, pnvals);

	// ��Rython�Ĺ��˱��ʽת��ΪPI���
	CString szExpression = expression;
	szExpression.Replace(_T("tagval"), _T("")); // PI���ʽ��û��tagval()������ֱ��ʹ��('tagname')�Ϳ���
	szExpression.Replace(_T("!="), _T("<>"));	// PI���ʽ��֧��!=��ϵ���������Ч��<>

	if (NULL == stagname || nsize <= 0)
		return DBERR_FAIL;

	if (ltimes > ltimee)
		return DBERR_TIME;

	SAFELOCK(m_csLock);

	if (m_arrTimestamp.GetSize() < nsize)
		m_arrTimestamp.SetSize(nsize);

	*pnvals = 0;
	CString szLog;

	DWORD dwret = CheckBatchTagAttr(stagname, vals[0].vt);
	if (dwret != DBERR_OK)
		return dwret;

	if (m_arrTagAttrs[0].lstatus) //ȡ��ǩ����		
		return DBERR_NOTAG;

	if (m_arrTagAttrs[0].lid <= 0)
		return DBERR_NOTAG;

	int32 pt = m_arrTagAttrs[0].lid;
	int32 nCount = nsize, nIndex = 0;
	float64 drval = 0.0;
	int32 ival = 0, istat = 0;
	int16 flags = 0;
	PITIMESTAMP time1;
	PIvaluetype pttype = *((PIvaluetype*)m_arrTagAttrs[0].m_szSite);
	DWORD dwtype = DataTypeFrom(pttype);

	int32 arcmode;
	if (RTDB_HISTORY_RAW == lflag)	// ����ֵ
	{
		arcmode = ARCflag_comp;// | ARCflag_filter;
		Time2PiTimestamp(ltimes, &m_arrTimestamp[0]);
		Time2PiTimestamp(ltimee, &time1);
	}
	//else if (RTDB_HISTORY_INTERP == lflag) // ��ֵ
	//{
	//	arcmode = ARCflag_even;
	//	Time2PiTimestamp(ltimes, &m_arrTimestamp[0]);
	//}
	else // RTDB_HISTORY_PLOT���������ݣ�����Ѳ�ֵ����������ȡ��ʷ��ʽͬ������	
	{
		if (ldsec <= 0) return DBERR_TIME;

		arcmode = ARCflag_time;// | ARCflag_filter;

		nCount = min((ltimee - ltimes) / ldsec + 1, nsize);
		int i;
		for(i = 0; i < nCount; i++)
		{
			Time2PiTimestamp(ltimes + ldsec * i, &m_arrTimestamp[i]);
		}
		//Time2PiTimestamp(ltimee, &m_arrTimestamp[i]); // �����һ��86400����¼�������������ڴ����
		Time2PiTimestamp(ltimee, &time1);
	}

	SetActiveServer(); // ѡ��ǰ���PI������
	int32 nret = piar_getarcvaluesfilterx(pt, arcmode, &nCount, &drval, &ival, NULL, NULL, &istat,
		&flags, m_arrTimestamp.GetData(), &time1, (char*)(LPCTSTR)szExpression, GETFIRST);

	if(-10733 == nret) return DBERR_CONNECT;
	if (nret == PI_NOMOREVALUES) return DBERR_NOMOREREC;
	if (nret > 0) return DBERR_CONNECT;
	if (nret == -1) return DBERR_NOTAG;
	if (nret == -998) return DBERR_MEMEORY;
	// nret == -30000��ʾ���ֳɹ����������ڸü�ʱ���Ѿ����ڶ�����
	// ����б�ǩֵ�ܴ󣬻ᱨ-15010���󣬿ɺ��Ըô���
	if ((nret < 0) && (nret != -30000) && (nret != -15010)) return DBERR_FAIL;

	int32 digicode = m_arrTagAttrs[0].lStartCode;

	long lPrevTime = 0;
	while(nret != PI_NOMOREVALUES)
	{
		long lt = PiTimestamp2Time(&time1);

		if (lPrevTime != lt)
		{
			// ȡ����ʷ����
			vals[nIndex].vt = dwtype;
			SetTVVAL(&vals[nIndex], digicode, pttype, drval, ival, NULL, 0, istat, flags, lt, 0);
			nIndex++;

			lPrevTime = lt;
		}

		// �����������ռ�
		if (nsize == nIndex)
			break;

		drval = 0.0;
		ival = 0;
		istat = 0;
		flags = 0;
		nret = piar_getarcvaluesfilterx(pt, arcmode, &nCount, &drval, &ival, NULL, NULL, &istat,
			&flags, m_arrTimestamp.GetData(), &time1, (char*)expression, GETNEXT);
	}

	*pnvals = nIndex;

	OutputDebug(_T("CPiDBDrv::GetHisValFilter end: ip=%s, tag=%s, count=%d"), GetPIServer().m_szIP, stagname, nIndex);

	return DBERR_OK;
}

void CPiDBDrv::CheckConnect()
{
	SAFELOCK(m_csLock);
	SetActiveServer(); // ѡ��ǰ���PI������

	// ͨ����ʱ��PI�ķ�����ʱ��������������·����
	int32 nServerTime = 0;
	pitm_servertime(&nServerTime);

	TRACE(_T("PI������ʱ�䣺%d"), nServerTime);
}

DWORD CPiDBDrv::TagSearch(
						  const char* szTagNameMask,		// ��ǩ�����룬*��ʾȫ����?��ʾ����һ���ַ�
						  const char* szTagDescrMask,		// ��ǩ�������룬*��ʾȫ����?��ʾ����һ���ַ�
						  int nDataTypeMask,				// �����������룬-1��ʾȫ��
						  LPTAGSEARCHRESULT pSearchResults,	// ���ر�ǩ��������ı�ǩ��������
						  DWORD* pdwSearchResultCount,		// ����pSearchResults�����С�������ر�ǩ����������������������������������Ҫ���������������ʾ������ɣ�
															// ���*pdwSearchResultCount==0�������������Ƿ�֧�ֱ�ǩ������DBERR_OK��ʾ֧�֣�DBERR_NOTSUPPORT��ʾ��֧��
						  DWORD_PTR* pdwSearchSession,		// ���δ���0�����ر�ǩ����Session
						  float* pfltPerProgress			// ����������ɰٷ��ʣ���Чֵ0.0-100.0���������ֵС��0����ʾ��֧����������
	)
{
	int nTagCount = (pdwSearchResultCount) ? (int)*pdwSearchResultCount : 0;

	// ѯ���Ƿ�֧�ֱ�ǩͬ��
	if (nTagCount <= 0)
	{
		return DBERR_NOTSUPPORT;
	}

	return DBERR_NOTSUPPORT;
}

DWORD CPiDBDrv::WriteStreamMessage(
								 const char* lpszDeviceName,			// DDClient�豸ʵ����
								 const char* lpszTagName,			// ���ݿ��ǩ��
								 DWORD dwCommandID,					// ��Ϣָ��ID
								 BYTE* pBuffer,						// ����Ϣ��
								 int nSize							// ����Ϣ���С
								 )
{
	AddInfoFmt(_T("Call WriteStreamMessage: DeviceName=%s, TagName=%s, CID=0x%02X"), 
		lpszDeviceName, lpszTagName, dwCommandID);

	if(dwCommandID != 0x101010)
		return DBERR_NOTSUPPORT;

	if(!m_bWriteSQLServer)
		return DBERR_OK;

	AddInfo(_T("Call WriteStreamMessage: IsConnectSQL"));
	if(!IsConnectSQL())
		ConnectSQL();

	AddInfo(_T("Call WriteStreamMessage: IsConnectSQL 2"));
	if(!IsConnectSQL())
		return DBERR_OK;

	
	LPT_WeightData pWeight = (LPT_WeightData)pBuffer;
	int iCount = 0;
	long lTime = 0;
	
	if(pWeight->iTotalNum == 0 && pWeight->btFlag == 0)
	{
		m_pSQLServer->GetLastCountTime(pWeight->iID, iCount, lTime);
		if(iCount != 0)
		{
			UpdateFlag(pWeight->iID, lTime);
		}
	}

	if(pWeight->btFlag == 0)
	{
		if(m_mapCount.Lookup(pWeight->iID, iCount))
		{
			if(iCount == pWeight->iTotalNum)
				return DBERR_OK;
		}
		m_pSQLServer->GetLastCount(pWeight->iID, iCount);
		if(iCount == pWeight->iTotalNum && iCount !=0)
			return DBERR_OK;

		m_mapCount.SetAt(pWeight->iID, iCount);
	}
	else
		AddInfo(_T("Call WriteStreamMessage: WriteSQL flag = 1"));

	if(WriteSQL(pWeight))
		WriteSQL(pWeight);
	return DBERR_OK;
}

int CPiDBDrv::WriteSQL(LPT_WeightData pWeightData)
{
	CString strSQL;
	//if(m_tServerInfo.szTable.IsEmpty())
	//	return FALSE;

	//strSQL.Format("insert into %s(ID, Ltime, Snqa, SDVal, SJVal,ZDS, BZJM, BZC) values (%d, %d, %d, %f, %f, %d,\'%s\',\'%s\')", 
	//	m_tServerInfo.szTable,
	//	pWeightData->iID,
	//	pWeightData->ltime,
	//	pWeightData->iQuality,
	//	pWeightData->fDefineValue,
	//	pWeightData->fRealValue,
	//	pWeightData->iTotalNum,
	//	pWeightData->chMechineName,
	//	pWeightData->chChenName);

	strSQL.Format("insert into [F701].[dbo].[T_Chen](ID, Ltime, Snqa, SDVal, SJVal,ZDS, SJZL, Y1,UPPER) values (\'%04d\', %d, %d, %.2f, %.2f, %d, %.2f, \'%d\', %d)", 
		//m_tServerInfo.szTable,
		pWeightData->iID,
		pWeightData->ltime,
		pWeightData->iQuality,
		pWeightData->fDefineValue,
		pWeightData->fRealValue,
		pWeightData->iTotalNum,
		pWeightData->fTotalValue,
		pWeightData->btFlag,
		pWeightData->iUpper
		);


	AddDebug(strSQL);

	if (!m_pSQLServer->ExecSQL(strSQL))
		m_pSQLServer->ExecSQL(strSQL);
	
	return DBERR_OK;
}


int CPiDBDrv::UpdateFlag(int nID, long ltime)
{
	CString strSql;
	strSql.Format("update T_Chen set Y1=\'1\' where ID=\'%04d\' and Ltime=%d", nID, ltime);

	AddDebug(strSql);

	if (!m_pSQLServer->ExecSQL(strSql))
		m_pSQLServer->ExecSQL(strSql);

	return 0;
}