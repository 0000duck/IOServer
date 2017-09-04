/////////////////////////////////////////////////////////////////////////////////
//  �ӿں���    interface.cpp
//

#include "stdafx.h"
#include "opcdrv.h"
//#include "../ecdrv/ecdriver.h"
#include "myopc.h"
//---------------------------------------------------------------
//1������������
/*

DWORD __stdcall ECD_GetDriverInfo(		//��ȡ������Ϣ��ָ�����������Ϣ���Ǿ���ʵ������Ϣ
	LPECDINFO lpecdinfo
	)
{
	lpecdinfo->dwEcdVer = 0x102;               //�����淶�汾1.02  add20090601
	sprintf(lpecdinfo->szVersion,"V1.02");
	sprintf(lpecdinfo->szCategory,"OPC");
	sprintf(lpecdinfo->szProducer,"Jiang Yong");
	sprintf(lpecdinfo->szDriverName,"OPC");
	sprintf(lpecdinfo->szCommDesc,"��");
	sprintf(lpecdinfo->szDriverFileName,"OPCDRV");//һ��������������ļ���(��Ҫ.dll��׺)
	sprintf(lpecdinfo->szNote,"OPC DA 2.0 Driver For EcNet");
	return RESULT_OK;
}

ECDHANDLE __stdcall ECD_CreateDriver(	//	��������ʵ��

	const char *szDeviceName,			//	[in] �豸���ƣ�
	void *pParam,						//  [in] ��д��handle��pParam�У�
	const void *pConfigData,			//	[in] �������� NULL��ʾʵʱ�����û�ṩ������ʹ��Ĭ��ֵ
										//  ����ʵ������ʹ��
	DWORD dwSize,						//	[in] �������ݴ�С
	ECDCB_VarOk		funOnVarOk,			// ʵʱ���ݱ仯����ָ��
	ECDCB_BlkVarOk  funOnBlkVarOk,			// �����׼���ú���ָ��
	ECDCB_EventOk   funOnEventOk			// �¼�׼���ú���ָ��
	)
{

	COpcDrv * pDrv=new COpcDrv();
	if( pDrv == NULL)
		return NULL;

	ECDHANDLE  handle=new TAG_ECDHANDEL;		// �������

	memset(handle, 0, sizeof(TAG_ECDHANDEL));	// �������
	handle->pParam = pParam;					// ���������Ĳ���

	handle->dwDriverType = ECD_DRIVERTYPE_CALLBACK;
	strcpy(handle->szDriverName, szDeviceName);

	handle->pDriverObj = (void *)pDrv;			// ����а�װ��������

	pDrv->OnVarOk	= funOnVarOk;	// ��װ�ص�����ָ��
	pDrv->OnBlkVarOk = funOnBlkVarOk;
	pDrv->OnEventOk = funOnEventOk;

	pDrv->m_handle = handle;
	// ������������
	if((pConfigData!=NULL)&&(dwSize!=0))
	{
		pDrv->LoadConfig(pConfigData,dwSize);
	}

	return handle;

}

DWORD __stdcall ECD_KillDriver(			// ɾ������,�����Լ����ڴ��ͷŹ���,����������У���Ҫ�˳����������߳�

	ECDHANDLE handle					// [in] ����ʵ�����							 
	)
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);
	pDrv->StopRun();
	delete pDrv;		// ɾ������ʵ������
	delete handle;		// ɾ�������
	return RESULT_OK;
}


DWORD __stdcall ECD_Start(				// ��������ʵ������������

	ECDHANDLE		handle				// [in] ����ʵ�����
	)
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);

	pDrv->StartRun();
	return RESULT_OK;
}

DWORD __stdcall ECD_Stop(				// ֹͣ����ʵ����ֹͣ����

	ECDHANDLE		handle				// [in] ����ʵ�����
	)
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);

	pDrv->StopRun();

	return RESULT_OK;
}


DWORD __stdcall ECD_GetStatus(			// ��ȡ����ʵ��״̬��ֹͣ����

	ECDHANDLE		handle,				// [in] ����ʵ�����
	DWORD			*pdwStatus			// [out] ״̬ECD_STATUS_XXX
	)
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);
	
	if(pDrv->m_bRun)
		*pdwStatus=	ECD_STATUS_RUN;
	else
		*pdwStatus=	ECD_STATUS_STOP;

	return RESULT_OK;
}


DWORD __stdcall ECD_GetErrInfo(			// ��ȡ�����Զ��������Ϣ

	ECDHANDLE		handle,				// [in] ����ʵ�����
	int				nErrCode,			// [in] ���ϴ���
	char            *szInfo,			// [out] ����ʵ����д������Ϣ
	int				nInfoBufSize,		// [in] ��Ϣ����������
	int				*pnActSize			// [out] ��Ϣʵ�ʳ���
	)
{
	//������û���Զ��������Ϣ
	szInfo[0]=0;
	pnActSize=0;
	return RESULT_FAILURE;
}


//---------------------------------------------------------------
// 2������������

DWORD __stdcall ECD_Config(				// ��������

	ECDHANDLE	handle,					// [in] ����ʵ�����	
	int			*pnModify				// [out] ������Ϣ�Ƿ�ı䣬0��δ�ı䣬��0 �ı䣬����ı���
										// ʵʱ����Ҫ����ECD_GetConfigBlk()����ȡ��������
	)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); //��Դ�л��������

	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);
	
	if(pDrv->Config(handle->szDriverName))
		*pnModify=1;
	else
		*pnModify=0;

	return RESULT_OK;
}


DWORD __stdcall ECD_GetConfigBlkSize(	//	��ȡ�������ã�����ʵ��������������ʵʱ�Ᵽ��

	ECDHANDLE handle,				//	[in] ����ʵ�����
	int		*pnBlkSize				//	[out] ���ݿ��С���ֽ���								 
	)
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);

	int n=pDrv->GetItemCount();
	//����д���ڴ��ļ���ȡ�䳤��
	CMemFile mfile;
	CArchive ar(&mfile,CArchive::store);
	pDrv->SaveConfig(ar);
	ar.Close();
	mfile.SeekToEnd();
	int  len=mfile.GetLength();
	if((len % 8) !=0)
		len=((len/8)+1) * 8;
	*pnBlkSize=len;
	mfile.Close();
	return RESULT_OK;
}


DWORD __stdcall ECD_GetConfigBlk(	//	��ȡ�������ã�����ʵ��������������ʵʱ�Ᵽ��

	ECDHANDLE handle,				//	[in] ����ʵ�����
	void	 *pConfigBlk,			//	[out] ���ڽ������ݿ�ָ���ָ�룬ʵʱ�⿽��ʹ��
	int		nSize,					//  [in]  �ֽ���
	int		*pnBlkSize				//	[out] ʵ�����ݿ��С���ֽ���								 
	)
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);

	//����д���ڴ��ļ�
	CMemFile mfile;
	CArchive ar(&mfile,CArchive::store);
	pDrv->SaveConfig(ar);
	ar.Close();
	mfile.SeekToEnd();
	int len=(int)mfile.GetLength();
	if( len > nSize)
		return RESULT_CONFIGBLKSML;
	*pnBlkSize = len;
	
	//���ڴ��ļ����������ܿ�
	mfile.SeekToBegin();//�ƶ���ͷ��
	*pnBlkSize=mfile.Read(pConfigBlk,len);
	mfile.Close();
	return RESULT_OK;
}


//---------------------------------------------------------------
// 3��������itemsö����

DWORD __stdcall ECD_EnumVarDef( //ö�������ڱ���
	ECDHANDLE		handle,				// [in] ����ʵ�����
	ECDCB_EnumVarDef funEnum,		// [in] ö�ٺ���
	void *lpParam
	)
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);
	pDrv->EnumVarDef(funEnum,lpParam);
	
	return RESULT_OK;
}

DWORD __stdcall ECD_EnumBlkVarDef( //ö�������ڿ����
	ECDHANDLE		handle,				// [in] ����ʵ�����
	ECDCB_EnumBlkVarDef funEnum,		// [in] ö�ٺ���
	void *lpParam
	)
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);
	pDrv->EnumBlkVarDef(funEnum,lpParam);
	
	return RESULT_OK;
}

DWORD __stdcall ECD_GetVarDef(	// ȡ��������
	ECDHANDLE		handle,		// [in] ����ʵ�����
	const char* lpszID,			// [in] ö�ٺ���
	LPECDITEMVAL lpVal			// [out] ��������
	)
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);
	
	CString szErr="";
	EcdApi::SetEcdDataQuality(&(lpVal->vt),ECD_DQ_NOITEM);

	pDrv->GetItem(lpszID,lpVal,szErr);

	return RESULT_OK;
}

//---------------------------------------------------------------
// 4����/д����
// 
DWORD __stdcall ECD_WriteVar(			// ������ʵ��д���������ݣ�ÿ��ֻ����дһ��

	ECDHANDLE		handle,				// [in] ����ʵ�����
	const char*		lpszID,				// [in] ����ID
	LPECDITEMVAL	lpVal				// [in] ֵ��
	)
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);
	return pDrv->WriteVar(lpszID,lpVal);

}

DWORD __stdcall ECD_WriteBlkVar( //д�����
		ECDHANDLE		handle,	// [in] ����ʵ�����
		const char* lpszID,		// [in] ����ID
		const void* lpData,		// [in] ����
		int   nDataSize			// [in] ���ݳ���
		)
{
	return RESULT_OK;
}

DWORD __stdcall ECD_AsynReadBlkVar(ECDHANDLE handle, const char* lpszID)//�첽�������
{
	return 0;
}

//{{ Ver1.01
DWORD __stdcall ECD_SetCallBack_T(ECDHANDLE handle,
								  ECDCB_VarOk_T     fun_VarOK_T,
								  ECDCB_BlkVarOk_T  fun_BlkVarOk_T
								  )
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);
	pDrv->OnVarOk_T = fun_VarOK_T;
	pDrv->OnBlkVarOk_T = fun_BlkVarOk_T;
	return RESULT_OK;
}

//{{ Ver1.02
DWORD __stdcall ECD_SetMultiTagsCallBack_T(ECDHANDLE handle,
								  ECDCB_MultiVarOk_T     fun_MultiVarOk_T
								  )
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);
	pDrv->OnMultiVarOk_T = fun_MultiVarOk_T;
	return RESULT_OK;
}

DWORD __stdcall ECD_SetLogCallBack(ECDHANDLE handle,
										ECDCB_Log     fun_Log
								  )
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);
	pDrv->OnLog = fun_Log;
	return RESULT_OK;
}

//}}
*/

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
			DRCB_OnDataOk  cbondataok,	 //���ݻص�����
			DRCB_OnLogOk  cbonlogok,    //��־�ص�����
			DRCB_OnSoeOk	cbOnSoeOk	//�¼��ص�����
		)
{
	COpcDrv * pObj=new COpcDrv();
	if( pObj == NULL)
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
	COpcDrv *pObj = (COpcDrv *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	pObj->StopRun();//ֹͣ����
	delete pObj; //ɾ������
	return DRERR_OK;

}

extern "C" DRVRET __stdcall DC_Start(DRHANDLE h)
{
	if(h == NULL)
		return DRERR_HANDLE;
	COpcDrv *pObj = (COpcDrv *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	pObj->StartRun();
	return  DRERR_OK;
}


extern "C" DRVRET __stdcall DC_Stop(DRHANDLE h)
{
	if(h == NULL)
		return DRERR_HANDLE;
	COpcDrv *pObj = (COpcDrv *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	pObj->StopRun();
	return DRERR_OK;
}



//////////////////////////////////////////////////////////////
//��Ϣ��ȡ
extern "C" DRVRET __stdcall DC_GetDrvInfo(LPDRVINFO lpDrvInfo)
{
	memset(lpDrvInfo,0,sizeof(T_DRVINFO));
	strcpy(lpDrvInfo->sname,"Opc2.0 DA Driver for IOServer");
	strcpy(lpDrvInfo->sdes,"CopyRight By ����Ƽ�");
	strcpy(lpDrvInfo->sver,"0.1.0.0"); //����汾
	lpDrvInfo->dwver = 0x0200;//�淶�汾
	lpDrvInfo->dwres = 0;
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

DRVRET  GetErrMsg(CString& szMsg,DRVRET dwcode)
{
	szMsg = "";
	if(dwcode > DRERR_NOERRCODE)
		return DRERR_NOERRCODE;
	szMsg = g_DrvErrMsg[dwcode];
	return DRERR_OK;
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
	COpcDrv *pObj = (COpcDrv *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	CString szMsg;
	
	DRVRET ret = GetErrMsg(szMsg,errcode);
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
	COpcDrv *pObj = (COpcDrv *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	return pObj->GetTagDef(nindex,lptag);
}

extern "C" DRVRET __stdcall   DC_GetRunStatus(DRHANDLE h,int *pnStaus)
{
	if(h == NULL)
		return DRERR_HANDLE;
	COpcDrv *pObj = (COpcDrv *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	if(pObj->m_bRun)
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
	COpcDrv *pObj = (COpcDrv *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	*pnlength = pObj->GetConfigSize();
	return DRERR_OK;
}


extern "C" DRVRET __stdcall  DC_GetConfigData(DRHANDLE h,char *pbuf,int nbufsize,int *pnactconfigsize)
{
	if(h == NULL)
		return DRERR_HANDLE;
	COpcDrv *pObj = (COpcDrv *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	return pObj->GetConfigData(pbuf,nbufsize,pnactconfigsize);
}

extern "C" DRVRET __stdcall  DC_Config(DRHANDLE h,int *pnModify) //��������
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(h == NULL)
		return DRERR_HANDLE;
	COpcDrv *pObj = (COpcDrv *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	if(pObj->Config())
		*pnModify=1;
	else
		*pnModify=0;
	return DRERR_OK;
}

//////////////////////////////////////////////////////////////
//���ݷ���
extern "C" DRVRET __stdcall   DC_WriteVar(DRHANDLE h,LPDRREC lprec)//д����
{
	if(h == NULL)
		return DRERR_HANDLE;
	COpcDrv *pObj = (COpcDrv *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	return pObj->WriteVar(lprec);
}

