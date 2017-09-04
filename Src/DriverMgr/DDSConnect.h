#pragma once
#include "Define.h"


class CDDSConnect //DDS������������,�ڲ�һ�������̣߳��ⲿ����д���߳�
{
public:
	CDDSConnect();
	~CDDSConnect();
	CString m_szfile;
	BOOL m_breal; //ʵʱ����
public:
	void 	SetConnect(BOOL bconnect);//����
	void	SetArgs(LPDBPARG,int n);//���ò���
	void	ClearConArg();//������Ӳ���
	void	DoConnect();//���ӹ���

	void	Start();
	void	Stop();

	BOOL	SetBlockingMode(BOOL bBlockingMode);
	BOOL	IsConnect(); //�Ƿ�����
	BOOL	DoWrite(void* pdata,int nsize,int ntimeoutmsec,BYTE &ucret,BYTE &ddsorder);	

	void	GetInfo(CString &szinfo);//ȡ������Ϣ
	int		GetDDSNum(){return m_ndds;} //ȡDDS����������
	int		GetNetMode() {	return m_nNetMode;}
protected:
	CCriticalSection m_Lock;

	//���Ӳ���
	int		m_ndds;	//DDS���ø���
	int		m_ncurdds;//��ǰ����λ��
	TDBPARG m_ddscon[MAX_DBP_CONARGS];

	//���ӹ���
	WORD	m_wPort; //��ǰ���ӵĶ˿�
	CString m_szSvrIP;//��ǰ���ӵ�IP
	int		m_nNetMode;

	BOOL	m_bConnected;//�Ƿ�������
	SOCKET	m_socket;	//�׽���

	BOOL	m_bBlockingMode;
	// ĳ������£����ܻ����select()��ѯʱ�޿ɶ����ݣ���ʵ���������Ѿ�����Ӷ���ǧ������ϵĿͻ�������
	// �᲻�ϵĶϿ��������������ͨ����״ֵ̬���Զ��л����ݿɶ�״̬�Ĳ�ѯ��ʽ���ɽ��������
	BOOL	m_bMsgPeekMode;
	//�̹߳���
	BOOL	m_bRun; //����
	BOOL	m_bKill;//�����߳�

	BOOL	Select(BOOL* pbReadReady, BOOL* pbWriteReady, BOOL* pbExceptFlag, DWORD dwTimeout);
	int		PeekBuffer(BYTE* pBuffer, int nBufferSize, BOOL bBlockingMode = FALSE);
	BOOL	WaitForData(DWORD dwTimeout);
	BOOL	GetNextConDef(LPDBPARG pcon); //��ȡ��һ�����Ӷ���	
	BOOL	ConnectSvr();//����
	void	SetTcpNoDelay();//��ֹNagle�Ż�
	static UINT	ThreadConnectDDS(LPVOID lparam);//����DDS���߳�
	FORCEINLINE void ThreadSleep(DWORD dwTimeout) {
		Sleep(dwTimeout);

		/*
		HANDLE hHandle = ::GetCurrentProcess();
		while(1)
		{
			MSG __msg;

			DWORD dwResult = ::MsgWaitForMultipleObjects(1, &hHandle, FALSE, dwTimeout, QS_ALLINPUT);

			switch(dwResult)
			{
			case WAIT_OBJECT_0:
				break;
			case WAIT_TIMEOUT:
				break;
			default:
				while(::PeekMessage(&__msg, NULL, 0, 0, PM_REMOVE))
				{
					::TranslateMessage(&__msg);
					::DispatchMessage(&__msg);
				}
			}
		}
		*/
	}

	int		Read(BYTE *pbuf,int nsize,int ntimeout);
	int		Send(BYTE *pbuf,int nsize,int ntimeout);
	int		WriteDDS(void* pdata,int nsize,int ntimeoutmsec,BYTE *pread); //д��DDS,����0��ʾ��ȷ,<0���󣬴���0���յķ����ֽ���
	BOOL	WriteDDS_NoAck(void* pdata,int nsize);

};