#include "StdAfx.h"
#include "HeartClient.h"
#include "define.h"

CHeartClient::CHeartClient()
{
	m_socket = INVALID_SOCKET;

	m_bSrvExist = TRUE;		//�������Ƿ����

	m_bRun = FALSE; //����
	m_bKill = FALSE;//�����߳�

	m_wport = 0; //���ӵĶ˿�
	m_szip = "";//���ӵ�IP
}
CHeartClient::~CHeartClient()
{
	Stop();
}
void CHeartClient::SetArgs(LPCTSTR lpszip,WORD wport)//���ò���
{
	m_wport = wport; //���ӵĶ˿�
	m_szip = lpszip;//���ӵ�IP
}

BOOL	CHeartClient::IsSrvExist() //�����������Ƿ����
{
	return m_bSrvExist;
}

void CHeartClient::Start()
{
	if(m_bRun)
		return;
	AfxBeginThread(ThreadRun,this,THREAD_PRIORITY_NORMAL);//����DDS�����߳�
}

void CHeartClient::Stop()
{
	m_bKill = TRUE;
	while(m_bRun)
		Sleep(100);

	if(m_socket != INVALID_SOCKET) //�ȶϿ�
	{
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
	m_bKill = FALSE;
}

BOOL	CHeartClient::ConnectSvr()//����
{
	if(m_szip.IsEmpty())
		return FALSE;
	if(m_szip == "0.0.0.0")
		return FALSE; //��ЧIP

	if(m_socket != INVALID_SOCKET) //�ȶϿ�
	{
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	struct sockaddr_in ServerHostAddr;//������վ���ն�������ַ
	ServerHostAddr.sin_family=AF_INET;
	ServerHostAddr.sin_port=::htons(m_wport);

	ServerHostAddr.sin_addr.s_addr=::inet_addr(m_szip);

	SOCKET s_socket = INVALID_SOCKET;
	s_socket=::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	if(s_socket==INVALID_SOCKET)
		return FALSE;

	long ul=1;
	if(SOCKET_ERROR == ioctlsocket(s_socket, FIONBIO, (unsigned long*)&ul))//���÷�������ʽ����
	{
		::closesocket(s_socket);
		s_socket = INVALID_SOCKET;
		return FALSE;
	}

	connect(s_socket,(sockaddr *)(&ServerHostAddr),sizeof(SOCKADDR));

	int i,nret=0;
	for(i=0;i<21;i++) //40 * 100 = 4�� 
	{
		if(m_bKill) //Ҫ����ֹ�߳�
		{
			::closesocket(s_socket);
			return FALSE;
		}
		TIMEVAL tv01 = {0,200 * 1000};//100�������ӳ�
		FD_SET fdw = {1, s_socket};

		nret = ::select(0, NULL, &fdw, NULL, &tv01);
		if(nret == 0)
			continue; //��ʱ
		break;
	}
	if(nret <= 0)
	{
		::closesocket(s_socket);
		s_socket = INVALID_SOCKET;
		return FALSE;
	}
	ul=0;
	if(SOCKET_ERROR == ioctlsocket(s_socket, FIONBIO, (unsigned long*)&ul))//���û�������ʽ����
	{
		::closesocket(s_socket);
		s_socket = INVALID_SOCKET;
		return FALSE;
	}
	m_socket = s_socket;
	SetTcpNoDelay();//��ֹNagle�Ż�

	//���͵�¼����

	CMakeFrm frmsend;
	SYSTEMTIME syst;
	::GetLocalTime(&syst);
	BYTE sendbuf[32];
	memcpy(sendbuf,&syst,sizeof(SYSTEMTIME));
	DWORD *pdw = (DWORD*)(sendbuf + 16);
	*pdw  = Utility::GetCRC32(sendbuf,16);

	frmsend.MakeFrm(OD_DDC_LOGIN,0,0,0x00,(BYTE*)sendbuf,20);//���¼��

	CMyByteArray baread;
	BYTE *pb = NULL;
	int nsize;

	pb = frmsend.GetFrm(&nsize); //ȡ���ͱ���

	nret =  WriteSrv(pb,nsize,2000,&baread); //д��DDS,����0��ʾ��ȷ,<0���󣬴���0���յķ����ֽ���
	if(nret == SOCKET_ERROR) //����
	{
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		return FALSE;
	}
	pb = baread.GetBuf(nsize);
	if((pb == NULL)||(nsize != 16))
	{
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		return FALSE;
	}

	if((pb[8] != OD_DDC_HEART) && (pb[11] != 0))//��¼ʧ��
	{
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		return FALSE;
	}
	return TRUE;
}

int		CHeartClient::ReadFrm(CMyByteArray *puca,int ntimeout)//��һ��ָ������������,����-1,0,>0�ֽ���
{
	puca->ClearData();
	//int nt = (ntimeoutsec * 1000 )/200 + 1; //ת�ɺ���
	int nt = ntimeout / 200 + 1; //ת�ɺ���
	int nret = 0;
	for(int i=0;i<nt;i++)
	{
		nret = Read(m_ucTmp,8192,200);//ÿ�ζ�8K,û200���볬ʱ��һ��
		if(nret >0 )
			break;
		if(nret == SOCKET_ERROR)
			return SOCKET_ERROR;
	}
	if(nret <= 0) //�����ʱ���Ͽ�����������
		return SOCKET_ERROR;


	int nst;
	BYTE *pb;
	BOOL bbreak = FALSE;
	while(nret > 0) //������
	{
		nst = m_FrmParse.PutData(m_ucTmp,nret);
		pb = m_FrmParse.m_Frm.GetBuf();
		while(nst == FRMP_FRMOK)
		{
			puca->PutData(pb,m_FrmParse.m_Frm.GetDataSize());
			m_FrmParse.m_Frm.ClearData();//���������³�ʼ��
			m_FrmParse.SetStatus(FRMP_NOHEAD);

			return puca->GetDataSize(); //���ض����������ֽ���
		}
		nret = Read(m_ucTmp,8192,1000);//ÿ�ζ�8K
	}
	m_FrmParse.m_Frm.ClearData();//���������³�ʼ��
	m_FrmParse.SetStatus(FRMP_NOHEAD);

	return SOCKET_ERROR;
}

int CHeartClient::WriteSrv(void* pdata,int nsize,int ntimeoutm,CMyByteArray *puread) //д��DDS,����0��ʾ��ȷ,<0���󣬴���0���յķ����ֽ���
{
	int nret = Send((BYTE*)pdata,nsize,1000);
	if(nret <= 0)
		return -1;
	return ReadFrm(puread,ntimeoutm);
}

UINT	CHeartClient::DoRun()//�߳�����
{
	m_bKill = FALSE;
	m_bRun = TRUE;

	SYSTEMTIME syst;
	BYTE sendbuf[32];
	DWORD *pdw = (DWORD*)(sendbuf + 16);
	CMyByteArray baread;
	BYTE *pb = NULL;
	int nsize;
	CMakeFrm sendfrm;

	int i,nret;
	while(!m_bKill)
	{

		if(m_socket == INVALID_SOCKET)
		{
			if(!ConnectSvr())
			{
				m_bSrvExist = FALSE;
				Sleep(100);
				continue;
			}
			m_bSrvExist = TRUE;
		}
		for(i=0;i<10;i++) //1���ӷ���һ��
		{
			if(m_bKill)
				break;
		}

		::GetLocalTime(&syst);

		memcpy(sendbuf,&syst,sizeof(SYSTEMTIME));
		*((long *)sendbuf) = gGetDriveMgr()->DDSConnectedNum(); //��д��DDS�������Ӹ���

		*pdw  = Utility::GetCRC32(sendbuf,16);
		sendfrm.MakeFrm(OD_DDC_HEART,0,0,0x00,(BYTE*)sendbuf,20);//��֯������

		pb = sendfrm.GetFrm(&nsize); //ȡ���ͱ���
		nret =  WriteSrv(pb,nsize,2000,&baread); //д��DDS,����0��ʾ��ȷ,<0���󣬴���0���յķ����ֽ���
		if(nret == SOCKET_ERROR)
		{
			::closesocket(m_socket);
			m_socket = INVALID_SOCKET;
			m_bSrvExist = FALSE;
		}
		else
		{
			pb = baread.GetBuf(nsize);
			if((pb == NULL)||(nsize != 16))
			{
				m_bSrvExist = FALSE;
				gGetDriveMgr()->m_ucWorkOrder = 0;
			}
			else
			{
				if(pb[8] != OD_DDC_HEART)
					m_bSrvExist = FALSE;
				else
				{
					m_bSrvExist = TRUE; //������
					if(pb[11] == 0xFE) //����Ҫ����
						gGetDriveMgr()->m_ucWorkOrder = 0xFE;
					else
						gGetDriveMgr()->m_ucWorkOrder = 0x00;

				}
			}
		}
		Sleep(1);
	}

	m_bKill = FALSE;
	m_bRun = FALSE;	
	TRACE("CWriteDDS Thread Exit\n");
	return 0;
}

UINT CHeartClient::ThreadRun(LPVOID lparam) //�߳�
{
	CHeartClient* pwd = (CHeartClient*)lparam;
	return pwd->DoRun();
}
void	CHeartClient::SetTcpNoDelay()//��ֹNagle�Ż�
{
	int bNodelay = 1; 
	if(m_socket==INVALID_SOCKET)
		return;
	setsockopt( 
		m_socket,
		IPPROTO_TCP,
		TCP_NODELAY,
		(char *)&bNodelay,
		sizeof(bNodelay));//��������ʱ�㷨 
}

int	CHeartClient::Read(BYTE *pbuf,int nsize,int ntimeout)
{
	if(m_socket == INVALID_SOCKET)
		return SOCKET_ERROR;
	TIMEVAL tv01 = {ntimeout/1000,1000 *(ntimeout%1000)};
	FD_SET fdr;
	fdr.fd_count = 1;
	fdr.fd_array[0] = m_socket;

	int nRet=::select(0, &fdr, NULL, NULL, &tv01);
	if(SOCKET_ERROR == nRet)//�����������
		return SOCKET_ERROR;

	if(nRet==0)//��ʱ�������޿ɶ�����
		return 0;

	nRet=::recv(m_socket, (char*)pbuf, nsize , 0);
	if(nRet == SOCKET_ERROR)
		return SOCKET_ERROR;
	return nRet;
} //��

int		CHeartClient::Send(BYTE *pbuf,int nsize,int ntimeout)
{
	if(m_socket == INVALID_SOCKET)
		return SOCKET_ERROR;

	TIMEVAL tv01 = {ntimeout/1000,1000 * (ntimeout%1000)};//1�����ӳ�
	FD_SET fdw;
	fdw.fd_count = 1;
	fdw.fd_array[0] = m_socket;

	int nRet = ::select(0, NULL, &fdw, NULL, &tv01);//���Կ�д
	if(SOCKET_ERROR == nRet)//�����������
		return SOCKET_ERROR;

	if(nRet==0)//��ʱ���������ܷ�������
		return 0;
	nRet = ::send(m_socket, (char*)pbuf,nsize,0);
	if(SOCKET_ERROR == nRet)//�����������
		return SOCKET_ERROR;
	return nRet;
} //����

