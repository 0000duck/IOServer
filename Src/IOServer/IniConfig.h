#pragma once

class CIniConfig : public CReadIni
{
public:
	CIniConfig(void);
	~CIniConfig(void);
public:
	CString m_szProjectName;
	BOOL	m_bAutoRun;		//����ʱ�Զ���������ʵ��
	BOOL	m_bSaveLog;		//������־
	BOOL	m_bExitPass;	//�˳�������֤
	BOOL	m_bCashe;		//�Ƿ�ʹ��cashe
	BOOL	m_bSkipSameTime;//������ͬʱ����ո���
	int		m_nTimeZero;	//��ʾʱ��-12,12
	WORD	m_wheartport;	//�����˿�
	CString m_szHeartIP;	//����IP,�ձ�ʾ�����Ƿ�����
	int		m_nLogLevel;	//��־����0-����1-���桢2-��ʾ��3-����
	CString m_szTitle;		//DDClient������ʾ���⣬������DDClient�Ĺ���
	BOOL	m_bWaitTcpAck;	//TCP�Ƿ�����Ӧ����������բ�ǲ���������Ӧ����
	int		m_nUdpSendDelay;//UDP������ʱ
	int		m_nupthreads;	//�ϴ��߳���
	//������
	long	m_lAdjustSec;	//��ʾ��������,��m_nTimeZero * 3600 + TIME_ZONE_INFORMATION.Bias * 60
protected:
	virtual void OnDoKeyVal(LPCTSTR lpszBlkName,LPCTSTR lpszKeyName,LPCTSTR lpszKeyVal);
};

