//�첽TCP����
#pragma once
#include <afxsock.h>
#include <afxtempl.h> //ģ��
#include "../tool/frmtool.h"


//////////////////////////////////////////////////////////////////////////
// CAsynConnect �첽����
class CAsynConnect 
{
public:
	CAsynConnect();
	virtual ~CAsynConnect();

public:
	DWORD		m_dwID;				//����ID,Ψһ��ʾһ���ͻ�����,����m_hSocket��ID��ԭ���ǿ��ܴ��·���
	SOCKET		m_hSocket;			//=INVALID_SOCKET;
	WORD		m_wPort;			//�ͻ��˿�
	SYSTEMTIME	m_DateTime;			//���ӿ�ʼʱ��
	LONGLONG 	m_l64Read;			//�����ֽ�
	LONGLONG    m_l64Write;			//�����ֽ�
	char		m_sIp[16];			//�ͻ�ip
	CString		m_szUser;			//�û�
	CString		m_szActor;			//��ɫ
protected:
	CFrmParse	m_frmparse;			//��������
public:
	int	writesocket(BYTE *pbuf,int nsize,int ntimeout);//д
	BOOL OnData(char *pdata,int nsize,CMyByteArray &ba);//���ݵ���
};

class CAsynConInfo : public CSortItem//�첽������Ϣ
{
public:
	CAsynConInfo();
	virtual ~CAsynConInfo();
public:
	virtual		int Compare(CSortItem *pOb);//����Ƚ�
	CAsynConInfo& operator=(const CAsynConnect* pcon); //����=����
public:
	DWORD		m_dwID;				//�˿�
	char		m_sIp[16];			//IP
	char		m_sTime[32];		//����ʱ��
	char		m_sUser[16];		//�û�
	char		m_sActor[16];		//��ɫ
	LONGLONG 	m_l64Read;			//�����ֽ�
	LONGLONG    m_l64Write;			//�����ֽ�

	BOOL		m_bSel;					//ѡ��
};

//////////////////////////////////////////////////////////////////////////
//CAsynConPool �첽���ӳ�

#define MAX_CONNECT_NUM   FD_SETSIZE //���ͬʱ������

class CAsynConPool
{
public:
	CAsynConPool();
	~CAsynConPool();

protected:
	CMap <DWORD,DWORD, CAsynConnect*, CAsynConnect*> m_mapcon;	//�첽���ӱ�
	DWORD m_dwNextID; //��һ��ID
	CCriticalSection m_Lock;//��
public:
	BOOL IsFull();//���ӳ��Ƿ���
	BOOL AddConnect(SOCKET s,WORD wport,const char* sip,LPCTSTR sUser,LPCTSTR sActor);
	void AddReadByteNum(DWORD dwid,long lbytes);//�ӽ����ֽ���
	BOOL WriteSocket(DWORD dwid,BYTE *pbuf,int nsize,int ntimeout); //��������

	int  Makefdsets(fd_set *fd); //��дfd_set
	DWORD DelCon(SOCKET s);//ɾ������

	BOOL OnReadData(SOCKET s,char *pdata,int nsize,CMyByteArray &frm,DWORD& dwid);//���ݵ����������ID,���Ĵ����frm��

	void OnStop();//ֹͣ������

	void GetConInfo(CSortObArray* pa);//ȡ������Ϣ

	BOOL GetActor(DWORD dwid,CString &sActor);//ȡĳ�����ӵĽ�ɫ
	int	 GetConnectNum();//��ȡ������

protected:
	void Clear(); //���map��
	void DelOneCon(DWORD dwID);//ɾ��һ������
	CAsynConnect* GetConnect(DWORD dwID);
	CAsynConnect* GetConBySocket(SOCKET s);

};

//////////////////////////////////////////////////////////////////////////
//CJobList ��ҵ����
class CJobItem //������ҵ
{
public:
	CJobItem(){
		m_dwid = 0;
		m_bin.SetGrown(1024 * 32);//32K
	};
	CJobItem(DWORD dwid){
		m_dwid = dwid;
		m_bin.SetGrown(1024 * 32);//32K
	};
	~CJobItem(){};
public:
	DWORD			m_dwid;
	CMyByteArray	m_bin;
};

#define MAX_JOBS    1024
typedef CJobItem*  PJOB;
class CJobList  //��ҵ����
{
public:
	CJobList();
	~CJobList();
protected:
	PJOB  m_jobs[MAX_JOBS];
	int	  m_nHead,m_nTail;

	CCriticalSection m_Lock;//��
public:
	PJOB Get(); //ȡһ��PJOB,���� NULL ��ʾ��
	BOOL Put(PJOB pj); //����һ��JOB������TRUE��ʾ�ɹ���FALSEʧ��
	void Del(DWORD dwjobid); //ɾ��
	
protected:
	PJOB GetIn();
	void Clear();	//
	BOOL DelOne(DWORD dwjobid);
};

//////////////////////////////////////////////////////////////////////////
//CAsynThread �߳���
class CAsynThread //�̳߳����߳���,�Ӹ���̳�һ���༴��
{
public:
	CAsynThread();
	virtual ~CAsynThread();
protected:
	BOOL*	m_pbKill;		//��ֹ�߳�
	BOOL*	m_pbRun;		//״̬

	CJobList* m_pjoblist;	//��ҵ����
	CAsynConPool* m_pconpool;//���ӳ�
	DWORD  m_dwThreadID;
public:
	BOOL Start(BOOL* pbKill,BOOL* pbRun,CJobList* pjoblist,CAsynConPool* pconpool);//��������

protected:
	static  UINT ThreadProcess(LPVOID pParam);
	UINT	ProcessRun();

	//��Ҫ���ص�
protected:
	virtual int DoOneJob(CJobItem* pjob); //����һ����ҵ
};


//////////////////////////////////////////////////////////////////////////
//CAsynTcpServer TCP�첽������

#define	TCP_READBUFSIZE		16384   //16K��С�Ļ���
#define MAX_SERVER_THREADS	10		//�������߳���
class CAsynTcpServer  //����TCP����
{
public:
	CAsynTcpServer();
	virtual ~CAsynTcpServer();
protected:
	CAsynConPool	m_conpool;	//���ӳ�
	CJobList		m_joblist;	//��ҵ��

	WORD				m_wPort;				//�˿ںţ�Ĭ��ֵ12084
	SOCKET				m_hSocket;				//=INVALID_SOCKET;
	struct	sockaddr_in m_HostAddr;				//������������ַ

	char	m_readbuf[TCP_READBUFSIZE];			//���ջ���
	
	BOOL			m_bKillThread; //��ֹ�߳�

	//�̶��߳�
	BOOL	m_bTcpAcceptRun;	//�����߳����б�־
	BOOL	m_bTcpEventRun;		//TCP�¼��̱߳�־

	//JOB�̳߳�
	int				m_nThreadNum; //�߳���
	CAsynThread*	m_threadpool[MAX_SERVER_THREADS];		//�̳߳�
	BOOL			m_bPoolThreadRun[MAX_SERVER_THREADS];	//�߳�״̬
	
public:
	BOOL			Start(int nthreadnum,WORD wPort,CString &szErr);//����
	void			Stop();
	void			GetConInfo(CSortObArray* pa)//ȡ������Ϣ
	{
		m_conpool.GetConInfo(pa);
	};
	BOOL GetActor(DWORD dwid,CString &sActor)//ȡĳ�����ӵĽ�ɫ
	{
		return m_conpool.GetActor(dwid,sActor);
	};
protected:
	UINT			ProcessTcpAccept();
	UINT			ProcessTcpEvent();

	//�̺߳���
	static UINT		ThreadTcpAccept(LPVOID pParam);	//�����߳�
	static UINT		ThreadTcpEvent(LPVOID pParam);	//TCP�¼��߳�

	//��Ҫ���ص�
protected:
	virtual CAsynThread* CreateThread(){return NULL;};
	virtual BOOL	OnLogin(SOCKET s,LPCTSTR szip,CString &szUser,CString &szActor){return TRUE;}; //Accept��ĵ�¼����
};
