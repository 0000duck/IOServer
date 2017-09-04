//drvwap.h
#pragma once
#include "../sdk/drvdef.h"
#include "../sdk/sdktool.h"

#define  DRERR_NODRV  (-1)
#define  DRERR_NOINTREFACE (-2)
//�ӿں���
typedef DRHANDLE (WINAPI *iDR_Create)(	//��������
			long lid,					//id		
			long lParam1,				//����1	
			long lParam2,				//����2	
			void *pconfigdata,			//��������	
			int ncongfigsize,			//�������ݳ���
			DRCB_OnDataOk  cbondataok,	//���ݻص�
			DRCB_OnLogOk  cbonlogok,	//��־�ص�
			DRCB_OnSoeOk	cbonsoeok	//SOE�¼��ص�
		);

typedef DRVRET (WINAPI *iDR_Destroy)(DRHANDLE h);				//��������
typedef DRVRET (WINAPI *iDR_Start)(DRHANDLE h);					//����
typedef DRVRET (WINAPI *iDR_Stop)(DRHANDLE h);					//ֹͣ
typedef DRVRET (WINAPI *iDR_GetDrvInfo)(LPDRVINFO lpDrvInfo);	//��ȡ������Ϣ
typedef DRVRET (WINAPI *iDR_GetErrMsg)(		//��ȡ�������
				DRHANDLE h,				
				DRVRET errcode, 			//�������
				char *pcErrBuf,				//������Ϣ������
				int nbufsize,				//������Ϣ����������
				int *pnactsize				//������Ϣʵ�ʳ���
				); 
typedef DRVRET (WINAPI *iDR_GetTagDef)(		//��ȡ��ǩ���壬ÿ��ȡһ��
				DRHANDLE h,			
				int nindex,					//��ǩ����,0-��ǩ��
				LPDRVTAG  lptag				//��ǩ��Ϣ
				);
typedef DRVRET (WINAPI *iDR_GetRunStatus)(  //��������״̬
				DRHANDLE h,					
				int *pnStaus				//״̬DRSTATUS_STOP��DRSTATUS_RUN֮һ
				);
typedef DRVRET (WINAPI *iDR_GetConfigSize)( //ȡ�����������ݳ���
				DRHANDLE h,
				int *pnlength				//�����������ݳ���
				);
typedef DRVRET (WINAPI *iDR_GetConfigData)( //ȡ������������
				DRHANDLE h,
				char *pbuf,					//�������ݴ洢��
				int nbufsize,				//�������ݴ洢������
				int *pnactconfigsize		//ʵ���������ݳ���
				);
typedef DRVRET (WINAPI *iDR_Config)(		//��������
				DRHANDLE h,
				int *pnModify				//�����Ƿ�ı䣬0û�иı䣬��0�ı�
				); 
typedef DRVRET (WINAPI *iDR_WriteVar)(		//д����
				DRHANDLE h,
				LPDRREC lprec				//д���ݼ�¼��һ����ǩֵ��¼
				);


//////////////////////////////////////////////////////////////////////////
// 2015-09-22�������ݻص��ӿ�
//////////////////////////////////////////////////////////////////////////

// ��ʷ���ݻص�
typedef DWORD (WINAPI * DRCB_OnWriteHistoryCB)(
	DRHANDLE handle,		// ���
	T_DRREC  recs[],		// ��¼��
	int nrecs				// ��¼����
	);

// ͨ�����ݱ��Ļص�
typedef DWORD (WINAPI * DRCB_OnStreamMessageCB)(
	DRHANDLE handle,		// ���
	const char* lpszTagName,// ��ǩ����
	DWORD	dwCommandID,	// ����ָ��ID
	BYTE*	pBuffer,		// �������ݻ���
	int		nSize			// ���ĳ���
	);

typedef DRVRET (WINAPI *iDC_SetWriteHistoryCB)(DRHANDLE h, DRCB_OnWriteHistoryCB pWriteHistoryCB);
typedef DRVRET (WINAPI *iDC_SetStreamMessageCB)(DRHANDLE h, DRCB_OnStreamMessageCB pStreamMessageCB);

//�����ӿڰ�װ��
class CDrvWap
{
public:
	CDrvWap();
	virtual ~CDrvWap();
public:
	CString		m_szDrvFile;//���������ļ���,��dll�ļ���,��Ϊ������
	CString		m_szDrvPath;//��������·��,ĩ�ַ�Ϊ"/";
	T_DRVINFO   m_drvinfo;
public:
	BOOL		LoadDrv(CString &szErr);//��������
	void		FreeDrv();

	//����Ϊ�ӿڰ�װ
	DRHANDLE DR_Create(	//��������
			long lid,					//id		
			long lParam1,				//����1	
			long lParam2,				//����2	
			void *pconfigdata,			//��������	
			int ncongfigsize,			//�������ݳ���
			DRCB_OnDataOk  cbondataok,	//���ݻص�
			DRCB_OnLogOk  cbonlogok,	//��־�ص�
			DRCB_OnSoeOk	cbonsoeok	//SOE�¼��ص�
		)
	{
		if(!m_hinstLib)
			return NULL;
		if(!iiDR_Create)
			return NULL;
		return iiDR_Create(lid,lParam1,lParam2,pconfigdata,ncongfigsize,cbondataok,cbonlogok,cbonsoeok);
	};
	DRVRET DR_Destroy(DRHANDLE h)				//��������
	{
		if(!m_hinstLib)
			return DRERR_NODRV;
		if(!iiDR_Destroy)
			return DRERR_NOINTREFACE;
		return iiDR_Destroy(h);
	};
	DRVRET DR_Start(DRHANDLE h)					//����
	{
		if(!m_hinstLib)
			return DRERR_NODRV;
		if(!iiDR_Start)
			return DRERR_NOINTREFACE;
		return iiDR_Start(h);
	};
	DRVRET DR_Stop(DRHANDLE h)					//ֹͣ
	{
		if(!m_hinstLib)
			return DRERR_NODRV;
		if(!iiDR_Stop)
			return DRERR_NOINTREFACE;
		return iiDR_Stop(h);
	};
	DRVRET DR_GetDrvInfo(LPDRVINFO lpDrvInfo)	//��ȡ������Ϣ
	{
		if(!m_hinstLib)
			return DRERR_NODRV;
		if(!iiDR_GetDrvInfo)
			return DRERR_NOINTREFACE;
		return iiDR_GetDrvInfo(lpDrvInfo);
	};
	DRVRET DR_GetErrMsg(		//��ȡ�������
					DRHANDLE h,				
					DRVRET errcode, 			//�������
					char *pcErrBuf,				//������Ϣ������
					int nbufsize,				//������Ϣ����������
					int *pnactsize				//������Ϣʵ�ʳ���
					)
	{
		if(!m_hinstLib)
			return DRERR_NODRV;
		if(!iiDR_GetErrMsg)
			return DRERR_NOINTREFACE;
		return iiDR_GetErrMsg(h,errcode,pcErrBuf,nbufsize,pnactsize);
	}; 
	DRVRET DR_GetTagDef(		//��ȡ��ǩ���壬ÿ��ȡһ��
					DRHANDLE h,			
					int nindex,					//��ǩ����,0-��ǩ��
					LPDRVTAG  lptag				//��ǩ��Ϣ
					)
	{
		if(!m_hinstLib)
			return DRERR_NODRV;
		if(!iiDR_GetTagDef)
			return DRERR_NOINTREFACE;
		return iiDR_GetTagDef(h,nindex,lptag);
	};
	DRVRET DR_GetRunStatus(  //��������״̬
					DRHANDLE h,					
					int *pnStaus				//״̬DRSTATUS_STOP��DRSTATUS_RUN֮һ
					)
	{
		if(!m_hinstLib)
			return DRERR_NODRV;
		if(!iiDR_GetRunStatus)
			return DRERR_NOINTREFACE;
		return iiDR_GetRunStatus(h,pnStaus);
	};
	DRVRET DR_GetConfigSize( //ȡ�����������ݳ���
					DRHANDLE h,
					int *pnlength				//�����������ݳ���
					)
	{
		if(!m_hinstLib)
			return DRERR_NODRV;
		if(!iiDR_GetConfigSize)
			return DRERR_NOINTREFACE;
		return iiDR_GetConfigSize(h,pnlength);
	};
	DRVRET DR_GetConfigData( //ȡ������������
					DRHANDLE h,
					char *pbuf,					//�������ݴ洢��
					int nbufsize,				//�������ݴ洢������
					int *pnactconfigsize		//ʵ���������ݳ���
					)
	{	
		if(!m_hinstLib)
			return DRERR_NODRV;
		if(!iiDR_GetConfigData)
			return DRERR_NOINTREFACE;
		return iiDR_GetConfigData(h,pbuf,nbufsize,pnactconfigsize);
	};
	DRVRET DR_Config(		//��������
					DRHANDLE h,
					int *pnModify				//�����Ƿ�ı䣬0û�иı䣬��0�ı�
					)
	{
		if(!m_hinstLib)
			return DRERR_NODRV;
		if(!iiDR_Config)
			return DRERR_NOINTREFACE;
		return iiDR_Config(h,pnModify);
	};
	DRVRET DR_WriteVar(		//д����
					DRHANDLE h,
					LPDRREC lprec				//д���ݼ�¼��һ����ǩֵ��¼
					)
	{
		if(!m_hinstLib)
			return DRERR_NODRV;
		if(!iiDR_WriteVar)
			return DRERR_NOINTREFACE;
		return iiDR_WriteVar(h,lprec);
	};

	DRVRET DC_SetWriteHistoryCB(DRHANDLE h, DRCB_OnWriteHistoryCB pWriteHistoryCB)
	{
		if(!m_hinstLib)
			return DRERR_NODRV;

		if(!iiDR_SetWriteHistoryCB)
			return DRERR_NOINTREFACE;

		return iiDR_SetWriteHistoryCB(h, pWriteHistoryCB);
	}
	DRVRET DC_SetStreamMessageCB(DRHANDLE h, DRCB_OnStreamMessageCB pStreamMessageCB)
	{
		if(!m_hinstLib)
			return DRERR_NODRV;

		if(!iiDR_SetStreamMessageCB)
			return DRERR_NOINTREFACE;

		return iiDR_SetStreamMessageCB(h, pStreamMessageCB);
	}
protected:
	HINSTANCE	m_hinstLib;	//������
	
	//�ӿ�
	iDR_Create			iiDR_Create;
	iDR_Destroy			iiDR_Destroy;
	iDR_Start			iiDR_Start;
	iDR_Stop			iiDR_Stop;
	iDR_GetDrvInfo		iiDR_GetDrvInfo;
	iDR_GetErrMsg		iiDR_GetErrMsg;
	iDR_GetTagDef		iiDR_GetTagDef;
	iDR_GetRunStatus	iiDR_GetRunStatus;
	iDR_GetConfigSize	iiDR_GetConfigSize;
	iDR_GetConfigData	iiDR_GetConfigData;
	iDR_Config			iiDR_Config;
	iDR_WriteVar		iiDR_WriteVar;

	iDC_SetWriteHistoryCB iiDR_SetWriteHistoryCB;
	iDC_SetStreamMessageCB iiDR_SetStreamMessageCB;

protected:
	
};