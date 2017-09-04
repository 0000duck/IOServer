//drvsdk.cpp ����������
#include "stdafx.h"
#include "drvsdk.h"

CDrvObj::CDrvObj()
{
	m_handle = NULL;
	m_bRun = FALSE;
	m_bKillThread = FALSE;
	m_cbOnDataOk = NULL;
	m_cbOnLogOk = NULL;

	m_cbWriteHistory = NULL;
	m_cbStreamMessage = NULL;
	m_bBlockingMode = FALSE;
}

CDrvObj::~CDrvObj()
{
	if(m_handle)
		delete m_handle;
}

DRVRET CDrvObj::Create(
		void *pConfigData,
		int nConfigSize,
		DRCB_OnDataOk	cbondataok,	 //���ݻص�����
		DRCB_OnLogOk	cbonlogok,    //��־�ص�����
		DRCB_OnSoeOk	cbOnSoeOk	//�¼��ص�����
		)
{
	//��װ�ص�����
	m_cbOnDataOk = cbondataok;
	m_cbOnLogOk = cbonlogok;
	m_cbOnSoeOk = cbOnSoeOk;
	if((nConfigSize > 0) && (pConfigData != NULL))//װ�������ļ�
	{
		CMemFile mfile;
		mfile.Attach((BYTE*)pConfigData, nConfigSize); // ֱ�ӹҹ��ڴ�飬����Ҫ���·����ڴ�
		//mfile.Write(pConfigData,nConfigSize);
		//mfile.SeekToBegin();
		CArchive ar(&mfile,CArchive::load);
		Serialize(ar);
		ar.Close();
		mfile.Detach();
	}
	return DRERR_OK;
}


int		CDrvObj::GetConfigSize()		//ȡ�������ݳ���
{
	CMemFile mf(4096);
	CArchive ar(&mf,CArchive::store);
	Serialize(ar);
	ar.Close();
	int nl = (int)mf.GetLength();
	mf.Close();
	return nl;
}
DRVRET	CDrvObj::GetConfigData(void *pbuf,int nbufsize,int *pnactsize) //ȡ��������
{
	*pnactsize = 0;

	//����д���ڴ��ļ�
	CMemFile mfile;
	CArchive ar(&mfile,CArchive::store);
	Serialize(ar);
	ar.Close();

	//��������ֽ���
	mfile.SeekToEnd();
	int len=(int)mfile.GetLength();
	if( len > nbufsize)//�ռ䲻��
		return 0;//
		
	//���ڴ��ļ����������ܿ�
	mfile.SeekToBegin();//�ƶ���ͷ��
	len = (int)mfile.Read(pbuf,(DWORD)len);
	mfile.Close();
	*pnactsize = len;
	return 0;
}

DRVRET  CDrvObj::Start()	//����
{
	m_bKillThread = FALSE;
	if(m_bRun)
		return DRERR_OK;//�Ѿ�����
	DRVRET lret = OnStart(); //��ʼ�߳�ǰ����
	if(lret != DRERR_OK)
		return lret;
	::AfxBeginThread(RunThread,this);//���������߳�

	m_bKillThread = FALSE;
	return DRERR_OK;
}

DRVRET  CDrvObj::Stop()		//ֹͣ
{
	m_bKillThread = TRUE;	
	while(m_bRun)
	{
		// ��Ϊ����ģʽʱ��������رյ�ǰ���ӣ������޷������˳������Ĺ����̣߳�
		// һ�ε���OnDisconnect()����Ҳ���ܻᱻ�����߳��Զ�������
		if (m_bBlockingMode)
			OnDisconnect();

		Sleep(100);
	}
	OnStop(); //�߳���ֹ�����
	m_bRun = FALSE;
	m_bKillThread = FALSE;
	return DRERR_OK;
}

DRVRET  CDrvObj::Config(int *pnModify)	//����
{
	if(OnConfig())
		*pnModify = -1;
	else
		*pnModify = 0;
	return DRERR_OK;
}

UINT	CDrvObj::RunThread(void *pParam)// �����߳�
{
	CDrvObj *pdrv=(CDrvObj *)pParam;
	
	pdrv->DoRun();//��������

	TRACE("CDrvObj RunThread Exit Success!\n");
	return DRERR_OK;
}

char *g_DrvErrMsg[]=
{
	"",//OK
	"����ľ��",
	"�������ݴ���",
	"�������ݻ���������",
	"������������ʧ��",
	"�޸����ǩ",
	"�޴˴������",
};
DRVRET  CDrvObj::GetErrMsg(CString& szMsg,DRVRET dwcode)	//��ȡ������Ϣ
{
	szMsg = "";
	if(dwcode > DRERR_NOERRCODE)
		return DRERR_NOERRCODE;
	szMsg = g_DrvErrMsg[dwcode];
	return DRERR_OK;
}

void CDrvObj::AddDebug(LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_DEBUG, LOG_OPT_ALL);
}

void CDrvObj::AddDebugFmt(LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_DEBUG, LOG_OPT_ALL);
}

void CDrvObj::AddDebug(DWORD dwLogOptType, LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_DEBUG, dwLogOptType);
}

void CDrvObj::AddDebugFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_DEBUG, dwLogOptType);
}

void CDrvObj::AddInfo(LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_INFO, LOG_OPT_ALL);
}

void CDrvObj::AddInfoFmt(LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_INFO, LOG_OPT_ALL);
}

void CDrvObj::AddInfo(DWORD dwLogOptType, LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_INFO, dwLogOptType);
}

void CDrvObj::AddInfoFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_INFO, dwLogOptType);
}

void CDrvObj::AddWarning(LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_WARNING, LOG_OPT_ALL);
}

void CDrvObj::AddWarningFmt(LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_INFO, LOG_OPT_ALL);
}

void CDrvObj::AddWarning(DWORD dwLogOptType, LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_WARNING, dwLogOptType);
}

void CDrvObj::AddWarningFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_WARNING, dwLogOptType);
}

void CDrvObj::AddError(LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_ERROR, LOG_OPT_ALL);
}

void CDrvObj::AddErrorFmt(LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_ERROR, LOG_OPT_ALL);
}

void CDrvObj::AddError(DWORD dwLogOptType, LPCTSTR lpszLogMessage)
{
	if(!m_cbOnLogOk)
		return;

	m_cbOnLogOk(m_handle, lpszLogMessage, LOG_LEVEL_ERROR, dwLogOptType);
}

void CDrvObj::AddErrorFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...)
{
	if(!m_cbOnLogOk)
		return;

	TCHAR buf[1024];

	va_list args;
	va_start(args, lpszLogFormat);
	_vstprintf(buf, lpszLogFormat, args);
	va_end(args);

	m_cbOnLogOk(m_handle, buf, LOG_LEVEL_ERROR, dwLogOptType);
}
/////////////////////////////////////////////////////////////////////////////
//�ӿ�

/////////////////////////////////////////////////////////////////////////////
//����ʵ������
extern "C" DRHANDLE __stdcall DC_Create(
			long lid,
			long lParam1,
			long lParam2,
			void *pconfigdata,
			int ncongfigsize,
			DRCB_OnDataOk	cbondataok,	//���ݻص�����
			DRCB_OnLogOk	cbonlogok,  //��־�ص�����
			DRCB_OnSoeOk	cbOnSoeOk	//�¼��ص�����
		)
{
	CDrvObj *pObj = CDrvFactory::NewDrv();
	
	if(pObj == NULL)
		return NULL;
	DRHANDLE h = new T_DRHANDLE;
	h->pObject = pObj;
	
	//��д�����ṩ�Ĳ���
	h->lid = lid;
	h->lParam1 = lParam1;
	h->lParam2 = lParam2;

	pObj->m_handle = h;//���������ص�����ʹ��
	pObj->Create(pconfigdata,ncongfigsize,cbondataok,cbonlogok,cbOnSoeOk);
	return h;
}

extern "C" DRVRET __stdcall DC_Destroy(DRHANDLE h)
{
	if(h == NULL)
		return DRERR_HANDLE;
	CDrvObj *pObj = (CDrvObj *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	pObj->Stop();//ֹͣ����
	delete pObj; //ɾ������
	return DRERR_OK;

}

extern "C" DRVRET __stdcall DC_SetWriteHistoryCB(DRHANDLE h, DRCB_OnWriteHistoryCB pWriteHistoryCB)
{
	if(h == NULL)
		return DRERR_HANDLE;

	CDrvObj *pObj = (CDrvObj *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;

	pObj->SetWriteHistoryCB(pWriteHistoryCB);

	return DRERR_OK;
}

extern "C" DRVRET __stdcall DC_SetStreamMessageCB(DRHANDLE h, DRCB_OnStreamMessageCB pStreamMessageCB)
{
	if(h == NULL)
		return DRERR_HANDLE;

	CDrvObj *pObj = (CDrvObj *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;

	pObj->SetStreamMessageCB(pStreamMessageCB);

	return DRERR_OK;
}

extern "C" DRVRET __stdcall DC_Start(DRHANDLE h)
{
	if(h == NULL)
		return DRERR_HANDLE;
	CDrvObj *pObj = (CDrvObj *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	return pObj->Start();
}


extern "C" DRVRET __stdcall DC_Stop(DRHANDLE h)
{
	if(h == NULL)
		return DRERR_HANDLE;
	CDrvObj *pObj = (CDrvObj *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	return pObj->Stop();
}



//////////////////////////////////////////////////////////////
//��Ϣ��ȡ
extern "C" DRVRET __stdcall DC_GetDrvInfo(LPDRVINFO lpDrvInfo)
{
	return CDrvFactory::GetDriverInfo(lpDrvInfo);
}


extern "C" DRVRET __stdcall DC_GetErrMsg(
			 DRHANDLE h, 
			 DRVRET errcode,
			 char *pcErrBuf,
			 int nbufsize,
			 int *pnactsize //���ش����ַ�������
			 ) 
{
	*pnactsize = 0;
	if(h == NULL)
		return DRERR_HANDLE;
	CDrvObj *pObj = (CDrvObj *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	CString szMsg;

	DRVRET ret = pObj->GetErrMsg(szMsg,errcode);
	if(ret == DRERR_OK)
	{
		*pnactsize = (szMsg.GetLength() > (nbufsize - 1)) ? (nbufsize-1) : szMsg.GetLength();
		strncpy(pcErrBuf,szMsg,*pnactsize);
	}
	return ret;
}

extern "C" DRVRET __stdcall   DC_GetTagDef(DRHANDLE h,int nindex,LPDRVTAG  lptag)
{
	if(h == NULL)
		return DRERR_HANDLE;
	CDrvObj *pObj = (CDrvObj *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	return pObj->GetTagDef(nindex,lptag);
}

extern "C" DRVRET __stdcall   DC_GetRunStatus(DRHANDLE h,int *pnStaus)
{
	if(h == NULL)
		return DRERR_HANDLE;
	CDrvObj *pObj = (CDrvObj *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	if(pObj->IsRun())
		*pnStaus = DRSTATUS_RUN;
	else
		*pnStaus = DRSTATUS_STOP;
	return DRERR_OK;
}
//////////////////////////////////////////////////////////////
//����
extern "C" DRVRET __stdcall  DC_GetConfigSize(DRHANDLE h,int *pnlength)
{
	if(h == NULL)
		return DRERR_HANDLE;
	CDrvObj *pObj = (CDrvObj *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	*pnlength = pObj->GetConfigSize();
	return DRERR_OK;
}


extern "C" DRVRET __stdcall  DC_GetConfigData(DRHANDLE h,char *pbuf,int nbufsize,int *pnactconfigsize)
{
	if(h == NULL)
		return DRERR_HANDLE;
	CDrvObj *pObj = (CDrvObj *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	return pObj->GetConfigData(pbuf,nbufsize,pnactconfigsize);
}

extern "C" DRVRET __stdcall  DC_Config(DRHANDLE h,int *pnModify) //��������
{
	if(h == NULL)
		return DRERR_HANDLE;
	CDrvObj *pObj = (CDrvObj *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	return pObj->Config(pnModify);
}

//////////////////////////////////////////////////////////////
//���ݷ���
extern "C" DRVRET __stdcall   DC_WriteVar(DRHANDLE h,LPDRREC lprec)//д����
{
	if(h == NULL)
		return DRERR_HANDLE;
	CDrvObj *pObj = (CDrvObj *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	return pObj->WriteVar(lprec);
}