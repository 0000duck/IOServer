// DriverMgr.h : DriverMgr DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

#include "DecValMan.h"
#include "DbpConArgs.h"
#include "DDSConnect.h"
#include "WriteDDS.h"
#include "Define.h"
#include "HeartClient.h"
#include "HeartServer.h"
#include "cashe.h "
#include "DdsTagTab.h"
#include "DriverMgr_global.h"

typedef struct _out_file_cfg
{
	CString szPath;
	CString szName;
	long	lInterval;
	BOOL	bReplace;
}T_OutFileCfg, *LPT_OutFileCfg;

#define MAXDDSCONS 10

class _DRIVERMGR_CLASS CDriverMgr
{
private:
	CDriverMgr(void);
	~CDriverMgr(void);
	static CDriverMgr* Instance;
public:
	static CDriverMgr* GetInstance(void);
	//CMap<CString,LPCTSTR,CDrvItem*,CDrvItem*> m_mapDec;//����ʵ���豸��
	CNoCaseMap<CDrvItem*,CDrvItem*> m_mapDec;//����ʵ���豸��
	CDecValMan			m_DecValMan;//�豸����
	CDbpConArgs			m_dbpconargs; //DDS���Ӳ���
	int					m_nWDdsReals;//дʵʱ�߳���
	CDDSConnect*		m_ddsConnects[MAXDDSCONS];	//DDS����
	CWriteDDS*			m_WriteDDSs[MAXDDSCONS];	//дDDS
	CDDSConnect			m_ddsConCache;	//DDS����
	CWriteDDS			m_writeDdsCache;//дDDS ����  
	CLocalCashe			m_cashe;	//���ػ���
	CDdsTagTab			m_ddsTagTab;//���͸�DDS�ı�ǩ��,������
	int					m_nbakddcconnum;	//��DDC��DDS����������
	long				m_lHeartTime;		//�ϴ��յ�������ʱ��
	BYTE				m_ucWorkOrder;		//�������Ĺ�������
	long				m_lfileflag; //�ļ���ʶ 
	BOOL				m_bRefresh;

	//ԭini����
	CString				m_szProjectName;
	static BOOL			m_bAutoRun;		//����ʱ�Զ���������ʵ��
	static BOOL			m_bSaveLog;		//������־
	static BOOL			m_bExitPass;	//�˳�������֤
	static BOOL			m_bCashe;		//�Ƿ�ʹ��cashe
	static BOOL			m_bSkipSameTime;//������ͬʱ����ո���
	static int			m_nTimeZero;	//��ʾʱ��-12,12
	static WORD			m_wHeartPort;	//�����˿�
	static CString		m_szHeartIP;	//����IP,�ձ�ʾ�����Ƿ�����
	static int			m_nLogLevel;	//��־����0-����1-���桢2-��ʾ��3-����
	static CString		m_szTitle;		//DDClient������ʾ���⣬������DDClient�Ĺ���
	static BOOL			m_bWaitTcpAck;	//TCP�Ƿ�����Ӧ����������բ�ǲ���������Ӧ����
	static int			m_nUdpSendDelay;//UDP������ʱ
	static int			m_nUpThreads;	//�ϴ��߳���
	//������
	static long			m_lAdjustSec;	//��ʾ��������,��m_nTimeZero * 3600 + TIME_ZONE_INFORMATION.Bias * 60
	//end
	T_OutFileCfg		m_tOutFileCfg;
	BOOL				m_bUseFileOutput;
	BOOL				m_bOutFileThreadRun;
	long				m_tmLastSave;
protected:
	long				m_lDrvNextID;
	CString				m_szres;  //����
	CString				m_szPrjName;	//������
	CString				m_szRootPath;	//DDC��Ŀ¼,���/
	CHeartServer		m_HeartSrv;//˫ǰ����������
	CHeartClient		m_HeartClient;//�����ͻ���



private:
	void		CreateDrvInst(LPCTSTR lpszdrvpath);//��ʼ������������������ʵ��
	void		Serialize(CArchive& ar);	//����

public:
	void		AddLogContent(DWORD dwLoglevel, LPCTSTR szLog);
	void		SetLogLevel(DWORD dwLevel);
	void		DelOneDec(LPCTSTR lpszDecName);//ɾ���豸,һ��������ɾ��
	CString		GetDDsStatus();	//ȡDDS״̬
	BOOL		IsDecExist(LPCTSTR sdecname);
	BOOL		OpenProject(LPCTSTR szRoot,LPCTSTR szProj); //�򿪹���
	BOOL		SavePrjFile(); //���湤���ļ�
	int			DDSConnectedNum(); //��DDS��дʵʱ���Ӻõ����Ӹ���
	long		GetDrvNextID(); //ȡ��һ��ID��
	BOOL		IsDrvInstRun(LPCTSTR lpszName);//����ʵ���Ƿ�����
	CDrvItem*	GetDrvInst(LPCTSTR lpszName);
	int			GetDecStatus(LPCTSTR lpszDecName);
	void		Start(BOOL bAutoRun);	//����
	void		StartDec(BOOL bAutoRun);//��ʼ������,����������������
	void		Stop();		//ֹͣ
	BOOL		SaveConnectData();
	BOOL		IsCanWork();//�Ƿ�ɹ���
	CString		GetDDCWorkInfo(); //ȡ����״̬
	int			AddDrv(CString szPath, CString szdrvname,CString szDecName);
	int			GetDrvInfo(CString szName, CString szPath, T_DRVINFO1& tDRVINFO);
	int			QueryTag(CString strDecName, CSortObArray* pSortArray,LPCTSTR stagname,LPCTSTR sproxytagname,LPCTSTR sdes,LPCTSTR sdrvtagname,int ntype);
	BOOL		GetDrvTagDef(CString szDrvName, int iIndex, T_DRVTAG& tDRVTAG);
	int			AddTag2Drv(CString szDecName, T_DRVTAG& tDRVTAG, DWORD dwFlag);
	BOOL		AddTag2Drv(CString szDecName, CTagItem& tTagItem, DWORD dwFlag);
	// �����ʽ
	int			CheckExp(const CString& szExp, CString szTagName, CString& szErrMsg, int& nTagCount);
	BOOL		DelDrvTag(CString szDecName, CString szTagName);
	BOOL		UpdateTag(CString szDecName, CTagItem& tagItem);
	BOOL		ImportTags(CString szDecName,CCSVFile& csvFile,int &nUpdate,int &nInsert,int &nAll,DWORD dwflag);

	BOOL		LoadPrjFile(LPCTSTR lpszFile);		//���빤���ļ�	
	int			GetDrvInstances(vecDrivers&  vecDatas);
	BOOL		Start(CString szModule, CString szDecName);
	BOOL		Stop(CString szDecName);

	BOOL		ConfigDrv(CString szDrvName);
	void		CreateDrvInst(LPCTSTR lpszPath, LPCTSTR szDecName);
	int			SetDBPConnectArgs(const CDBPArgs& dbpArgs);
	int			GetDBPConnectArgs(CDBPArgs& dbpArgs);
	int			GetDrvTags(DWORD dwFlag, CString szDrvName, CDrvTags& DrvTags, int &nNew, int& nUpdate);
	int			QueryTag(LPCTSTR szDecName, 
		CSortObArray& pSortArray,
		LPCTSTR szTagName,
		LPCTSTR szProxyTagName,
		LPCTSTR szDes,
		LPCTSTR szDrvTagName,
		int nType);
	BOOL		DelTag(CString szDecName, CString szTagName);
	void		ExitServer(void);
	BOOL		GetTagValue(LPCTSTR lpszDecName, LPCTSTR lpszTagName, CVarVal& varValue);
	void		GetOutFileCfig(BOOL& bUseFileOutput, T_OutFileCfg& tOutFileCfg);
	void		SetOutFileCfig(BOOL bUseFileOutput, const T_OutFileCfg& tOutFileCfg);

	void		ProcessOutFile(void);
	static  UINT threadOutFile(LPVOID pParam);
public://static
	static  DWORD WINAPI Fun_OnDataOk(	//���ݻص�����
		DRHANDLE handle,	//���
		T_DRREC recs[],		//��¼��
		int nrecs			//��¼����
		);
	static	DWORD WINAPI Fun_OnLogOk( //��־�ص�����
		DRHANDLE handle,	//���
		const char *plog,			//��־�ַ�����0�������ַ���
		DWORD dwLogLevel,	//��־����
		DWORD dwLogOptType	//��־��������
		);
	static	DWORD WINAPI Fun_OnSoeOk( //SOE�¼��ص�����,ÿ�λص�һ���¼�
		DRHANDLE handle,	//���
		LPSOE psoe			//�¼�
		);

	// ��ʷ���ݻص�
	static DWORD WINAPI Fun_OnWriteHistoryCB(
		DRHANDLE handle,		// ���
		T_DRREC  recs[],		// ��¼��
		int nrecs				// ��¼����
		);

	// ͨ�����ݱ��Ļص�
	static DWORD WINAPI Fun_OnStreamMessageCB(
		DRHANDLE handle,		// ���
		const char* lpszTagName,// ��ǩ����
		DWORD	dwCommandID,	// ����ָ��ID
		BYTE*	pBuffer,		// �������ݻ���
		int		nSize			// ���ĳ���
		);
};



// CDriverMgrApp
// �йش���ʵ�ֵ���Ϣ������� DriverMgr.cpp
//
#include "Log.h"
#include "../AuthorityMgr/AuthorityMgr.h"

using namespace KIMI;
class CDriverMgrApp : public CWinApp
{
public:
	CDriverMgrApp();
	CLog*	m_logger;
	CAuthorityMgr* m_pAuthorityMgr;
	CString GetOperator();
	//CIniConfig	m_Config;		//����
	// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};



