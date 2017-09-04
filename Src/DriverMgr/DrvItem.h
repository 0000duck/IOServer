#pragma once
#include "drvwap.h"
#include "TagItem.h"

class CDrvItem :
	public CDrvWap
{

public:
	CDrvItem();
	CDrvItem(LPCTSTR lpszPath,LPCTSTR lpszDrvName,LPCTSTR lpszDecName);
	virtual ~CDrvItem();
public:
	void	Serialize(CArchive& ar);	//����
	BOOL	IsSel(int nIndex); //��ǩ�Ƿ�ѡ��
	BOOL	SetSel(int nIndex,BOOL bSel);//���ñ�ǩѡ��
	BOOL	SetSelXor(int nIndex); //�෴��ǩ����
	CString GetText(int nrow,int ncol);//ȡ��ʾ�ַ���
	BOOL	IsRun();
	BOOL	SaveConfig();//��������
	int 	AddDrvTag(LPDRVTAG pdrvtag,DWORD dwflag); //���������ǩ,����0���£�1���
	BOOL	ImportTags(CCSVFile* pf,int &nup,int &nins,int &nall,DWORD dwflag); //�����ǩ
	BOOL	ExportTags(CCSVFile* pf,int *pnExport);//������ǩ
	BOOL	ExportSelTags(CCSVFile* pf,int *pnExport);//����ѡ���ǩ
	BOOL	ExportRdbSelTags(CCSVFile* pf,int *pnExport,int nsel,LPCTSTR sdbname);//����ѡ���ǩΪʵʱ����ñ�ǩ��
	int		GetNextSelTag(int nstart);//��nstart��ѡ��ı�ǩ
	int		DelSelTags();//ɾ��ѡ��ı�ǩ

	int		GetItemErrStatus(int nIndex);
	int		SetItemErrStatus(int nIndex,int nst);

	int		ExportDDSTags(CCSVFile* pf,int *pnExport,int nsel);//����DDS��ǩ,nsel = 0��ʾȫ��

	int		QueryTag(CSortObArray* pa,LPCTSTR stagname,LPCTSTR sproxytagname,LPCTSTR sdes,LPCTSTR sdrvtagname,int ntype);//��ѯ��ǩ

	BOOL	DelOneTag(LPCTSTR lpszName); //ɾ��һ����ǩ
	BOOL	UpdateTag(CTagItem *pi);//���±�ǩ
	BOOL	AddTag(CTagItem *ptag);//��ӱ�ǩ
public: //�ӿ�
	//����Ϊ�ӿڰ�װ
	DRHANDLE Create(	//��������
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
		if(m_handle)
			return m_handle;

		m_handle  = DR_Create(lid,lParam1,lParam2,pconfigdata,ncongfigsize,cbondataok,cbonlogok,cbonsoeok);
		return m_handle;

	};
	DRVRET Destroy()				//��������
	{
		return DR_Destroy(m_handle);
	};
	DRVRET Start()					//����
	{
		return DR_Start(m_handle);
	};
	DRVRET Stop()					//ֹͣ
	{
		return DR_Stop(m_handle);
	};
	DRVRET GetDrvInfo(LPDRVINFO lpDrvInfo)	//��ȡ������Ϣ
	{
		return DR_GetDrvInfo(lpDrvInfo);
	};
	DRVRET GetErrMsg(		//��ȡ�������		
		DRVRET errcode, 			//�������
		char *pcErrBuf,				//������Ϣ������
		int nbufsize,				//������Ϣ����������
		int *pnactsize				//������Ϣʵ�ʳ���
		)
	{
		return DR_GetErrMsg(m_handle,errcode,pcErrBuf,nbufsize,pnactsize);
	}; 
	DRVRET GetTagDef(		//��ȡ��ǩ���壬ÿ��ȡһ��		
		int nindex,					//��ǩ����,0-��ǩ��
		LPDRVTAG  lptag				//��ǩ��Ϣ
		)
	{
		return DR_GetTagDef(m_handle,nindex,lptag);
	};
	DRVRET GetRunStatus(		//��������״̬				
		int *pnStaus				//״̬DRSTATUS_STOP��DRSTATUS_RUN֮һ
		)
	{
		return DR_GetRunStatus(m_handle,pnStaus);
	};
	DRVRET GetConfigSize( //ȡ�����������ݳ���
		int *pnlength				//�����������ݳ���
		)
	{
		return DR_GetConfigSize(m_handle,pnlength);
	};
	DRVRET GetConfigData( //ȡ������������
		char *pbuf,					//�������ݴ洢��
		int nbufsize,				//�������ݴ洢������
		int *pnactconfigsize		//ʵ���������ݳ���
		)
	{	
		return DR_GetConfigData(m_handle,pbuf,nbufsize,pnactconfigsize);
	};
	DRVRET Config(		//��������
		int *pnModify				//�����Ƿ�ı䣬0û�иı䣬��0�ı�
		)
	{
		return DR_Config(m_handle,pnModify);
	};
	DRVRET WriteVar(		//д����
		LPDRREC lprec				//д���ݼ�¼��һ����ǩֵ��¼
		)
	{
		return DR_WriteVar(m_handle,lprec);
	};
	DRVRET SetWriteHistoryCB(DRCB_OnWriteHistoryCB pWriteHistoryCB) {
		return DC_SetWriteHistoryCB(m_handle, pWriteHistoryCB);
	}
	DRVRET SetStreamMessageCB(DRCB_OnStreamMessageCB pStreamMessageCB) {
		return DC_SetStreamMessageCB(m_handle, pStreamMessageCB);
	}

public:
	//�����������
	CString		m_szDecName;	//�豸����
	long		m_lID;			//ID

	char*		m_pconfigdata;
	long		m_lconfiglen;
	//�������
	//CMap <CString,LPCTSTR,CTagItem*,CTagItem*> m_map;	//��ǩ��������MAP
	CNoCaseMap<CTagItem*,CTagItem*> m_map;	//��ǩ��������MAP
	CSortObArray	m_array;	//��ǩ����

protected:
	DRHANDLE m_handle;	//�������
public:
	BOOL IsInstance(void);
};

