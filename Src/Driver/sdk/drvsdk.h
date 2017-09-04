//drsdk.h ������������
#pragma once
#include "drvdef.h"

// ��־��Ϣ����
#define LOG_LEVEL_ERROR		0x00	// ���󼶱�
#define LOG_LEVEL_WARNING	0x01	// ���漶��
#define LOG_LEVEL_INFO		0x02	// ��ʾ����
#define LOG_LEVEL_DEBUG		0x03	// Debug����

// ��־��������
#define LOG_OPT_DISPLAY		0x01	// �ڴ�������ʾ��־
#define LOG_OPT_STORAGE		0x02	// д��־����־�ļ�
#define LOG_OPT_ALL			(LOG_OPT_DISPLAY | LOG_OPT_STORAGE)

//�����������,�����麯�����̳еľ�����������Ӧ����
class CDrvObj
{
public:
	CDrvObj();
	virtual ~CDrvObj();

	DRHANDLE m_handle; //���
	void SetBlockingMode(BOOL bMode) {
		m_bBlockingMode = bMode;
	}
protected:
	DRCB_OnDataOk	m_cbOnDataOk;	//���ݻص�����
	DRCB_OnLogOk	m_cbOnLogOk;	//��־�ص�����
	DRCB_OnSoeOk	m_cbOnSoeOk;	//�¼��ص�����
	BOOL	m_bRun;					//����״̬
	BOOL	m_bKillThread;			//��ֹ�̱߳�־

	DRCB_OnWriteHistoryCB m_cbWriteHistory;
	DRCB_OnStreamMessageCB m_cbStreamMessage;

	BOOL	m_bBlockingMode;

public:
	DRVRET	Create(
		void *pConfigData,
		int nConfigSize,
		DRCB_OnDataOk	cbondataok,	//���ݻص�����
		DRCB_OnLogOk	cbonlogok,  //��־�ص�����
		DRCB_OnSoeOk	cbOnSoeOk	//�¼��ص�����
		);
	int		GetConfigSize();		//ȡ�������ݳ���
	DRVRET	GetConfigData(void *pbuf,int nbufsize,int *pnactsize); //ȡ��������
	DRVRET  Start();	//����
	DRVRET  Stop();		//ֹͣ

	DRVRET  Config(int *pnModify);	//����
	BOOL	IsRun(){return m_bRun;};
	BOOL	IsKillThread(){return m_bKillThread;};

	void SetWriteHistoryCB(DRCB_OnWriteHistoryCB pWriteHistoryCB) {
		m_cbWriteHistory = pWriteHistoryCB;
	}
	void SetStreamMessageCB(DRCB_OnStreamMessageCB pStreamMessageCB) {
		m_cbStreamMessage = pStreamMessageCB;
	}

public:
	void AddDebug(LPCTSTR lpszLogMessage);
	void AddDebug(DWORD dwLogOptType, LPCTSTR lpszLogMessage);
	void AddDebugFmt(LPCTSTR lpszLogFormat, ...);
	void AddDebugFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...);

	void AddInfo(LPCTSTR lpszLogMessage);
	void AddInfo(DWORD dwLogOptType, LPCTSTR lpszLogMessage);
	void AddInfoFmt(LPCTSTR lpszLogFormat, ...);
	void AddInfoFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...);

	void AddWarning(LPCTSTR lpszLogMessage);
	void AddWarning(DWORD dwLogOptType, LPCTSTR lpszLogMessage);
	void AddWarningFmt(LPCTSTR lpszLogFormat, ...);
	void AddWarningFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...);

	void AddError(LPCTSTR lpszLogMessage);
	void AddError(DWORD dwLogOptType, LPCTSTR lpszLogMessage);
	void AddErrorFmt(LPCTSTR lpszLogFormat, ...);
	void AddErrorFmt(DWORD dwLogOptType, LPCTSTR lpszLogFormat, ...);

protected:
	static	UINT	RunThread(void *pParam);// �����߳�

//������Ҫ����
public: //�麯��
	virtual DRVRET  GetErrMsg(CString& szMsg,DRVRET dwcode);			//��ȡ������Ϣ
	virtual DRVRET  WriteVar(LPDRREC lpval){return DRERR_OK;};			//д����
	virtual DRVRET  GetTagDef(int nindex,LPDRVTAG  lptag){return DRERR_NOMORETAG;};
	virtual short	ToDBPQuality(short nQuality) { return nQuality; } // ���������ɼ�������ת��ΪĿ���������׼����
					
protected: //���麯��
	virtual BOOL	OnConfig() = 0;					//���ú����ý���,����TRUE��ʾ�޸�������
	virtual void	Serialize(CArchive& ar) = 0;	//���л����棬��ȡ����
	virtual DRVRET	OnStart() = 0;					//׼������
	virtual void	OnStop() = 0;					//ֹͣ���к��������
	virtual void	OnDisconnect() {};				//�Ͽ����ӣ������������ģʽ������Բ�ʵ�ָú���
	virtual UINT	DoRun() = 0;					//��������й��̣���RunThread�е���
};

class CDrvFactory //��������,����һ����̬��,ÿ��������Ҫ�޸�,���������ʵ�ֺ���
{
public:
	static CDrvObj*	NewDrv(); //��������
	static DRVRET GetDriverInfo(LPDRVINFO lpDrvInfo);	//��ȡ������Ϣ��ָ�����������Ϣ���Ǿ���ʵ������Ϣ
};
