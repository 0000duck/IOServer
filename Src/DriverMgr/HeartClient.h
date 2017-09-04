#pragma once

////////////////////////////////////////////////////////////////////////////////
//����Ϊ˫ǰ��TCP��������
#define OD_DDC_ERROR 0x07 //��������
#define OD_DDC_LOGIN 0x08 //��¼����
#define OD_DDC_HEART 0x09 //��������
class CHeartClient
{
public:
	CHeartClient(void);
	~CHeartClient(void);
	void SetArgs(LPCTSTR lpszip,WORD wport);//���ò���
	void Start();
	void Stop();

	BOOL	IsSrvExist(); //�����������Ƿ����
	UINT	DoRun();//�߳�����		
protected:
	int		Read(BYTE *pbuf,int nsize,int ntimeout);
	int		Send(BYTE *pbuf,int nsize,int ntimeout);
	int		ReadFrm(CMyByteArray *puca,int ntimeout);
	int		WriteSrv(void* pdata,int nsize,int ntimeoutm,CMyByteArray *puca); //д��DDS,����0��ʾ��ȷ,<0���󣬴���0���յķ����ֽ���
	BOOL	ConnectSvr();//����
	void	SetTcpNoDelay();//��ֹNagle�Ż�
	BOOL	DoWrite(void* pdata,int nsize,int ntimeoutmsec,BYTE &ucret,BYTE &ddsorder);	
	static UINT	ThreadRun(LPVOID lparam);//�����߳�
	//�̹߳���
	BOOL	m_bRun; //����
	BOOL	m_bKill;//�����߳�

	CFrmParse		m_FrmParse;		//���ձ��ķ���
	//���Ӳ���
	CString m_szip;
	WORD	m_wport;

	BOOL	m_bSrvExist;//�������Ƿ����
	BYTE	m_ucTmp[8192];
	SOCKET	m_socket;	//�׽���
};

