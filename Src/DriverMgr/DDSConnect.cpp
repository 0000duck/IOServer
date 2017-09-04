#include "StdAfx.h"
#include "DDSConnect.h"

/////////////////////////////////////////////////////////////////////////
CDDSConnect::CDDSConnect()
{
	m_szfile = "";
	m_socket = INVALID_SOCKET;
	m_ndds = 0;
	m_ncurdds = 0;
	m_bConnected = FALSE;

	m_bRun = FALSE; //����
	m_bKill = FALSE;//�����߳�

	m_wPort = 0; //��ǰ���ӵĶ˿�
	m_szSvrIP = "";//��ǰ���ӵ�IP
	m_nNetMode = NET_MODE_TCP;

	m_breal = TRUE; //Ĭ��ʵʱ����
	m_bMsgPeekMode = FALSE;
	m_bBlockingMode = TRUE;
}

CDDSConnect::~CDDSConnect()
{
	Stop();
}

void	CDDSConnect::GetInfo(CString &szinfo)//ȡ������Ϣ
{
	CSafeLock lock(&m_Lock);
	if(!m_bConnected)
		szinfo = "DDSδ����!";
	else
		szinfo.Format("DDS = %s:%d",m_szSvrIP,m_wPort);
}

void CDDSConnect::SetArgs(LPDBPARG pargs,int n)//���ò���
{
	int i;
	m_ndds = 0;
	for(i=0;i<n && i<MAX_DBP_CONARGS;i++)
	{
		memcpy(&m_ddscon[i],&pargs[i],sizeof(TDBPARG));
		m_ndds++;
	}
}

BOOL CDDSConnect::SetBlockingMode(BOOL bBlockingMode)
{
	if (m_socket == INVALID_SOCKET)
		return FALSE;

	if (m_bBlockingMode != bBlockingMode)
	{
		long ul = bBlockingMode ? 0 : 1;

		if (SOCKET_ERROR != ::ioctlsocket(m_socket, FIONBIO, (unsigned long*)&ul))
		{
			m_bBlockingMode = bBlockingMode;
			return TRUE;
		}
		else
			return FALSE;
	}

	return TRUE;
}

BOOL CDDSConnect::Select(BOOL* pbReadReady, BOOL* pbWriteReady, BOOL* pbExceptFlag, DWORD dwTimeout)
{
	FD_SET ReadFds, *ReadFdsptr, WriteFds, *WriteFdsptr, ExceptFds, *ExceptFdsptr;
	timeval tv, *Timeptr;

	BOOL bResult = FALSE;

	if (m_socket == INVALID_SOCKET)
		return bResult;

	if (pbReadReady)
	{
		ReadFdsptr = &ReadFds;
		FD_ZERO(&ReadFds);
		FD_SET(m_socket, &ReadFds);
	}
	else
		ReadFdsptr = NULL;

	if (pbWriteReady)
	{
		WriteFdsptr = &WriteFds;
		FD_ZERO(&WriteFds);
		FD_SET(m_socket, &WriteFds);
	}
	else
		WriteFdsptr = NULL;

	if (pbExceptFlag)
	{
		ExceptFdsptr = &ExceptFds;
		FD_ZERO(&ExceptFds);
		FD_SET(m_socket, &ExceptFds);
	}
	else
		ExceptFdsptr = NULL;

	if (dwTimeout >= 0)
	{
		tv.tv_sec = dwTimeout / 1000;
		tv.tv_usec =  1000 * (dwTimeout % 1000);
		Timeptr = &tv;
	}
	else
		Timeptr = NULL;

	try
	{
		bResult = (SOCKET_ERROR != ::select(m_socket + 1, ReadFdsptr, WriteFdsptr, ExceptFdsptr, Timeptr));
	}
	catch(...)
	{
		bResult = FALSE;
	}

	if (pbReadReady)
		*pbReadReady = FD_ISSET(m_socket, ReadFdsptr);
	if (pbWriteReady)
		*pbWriteReady = FD_ISSET(m_socket, WriteFdsptr);
	if (pbExceptFlag)
		*pbExceptFlag = FD_ISSET(m_socket, ExceptFdsptr);

	return bResult;
}

int CDDSConnect::PeekBuffer(BYTE* pBuffer, int nBufferSize, BOOL bBlockingMode/* = FALSE*/)
{
	if (bBlockingMode)
		return ::recv(m_socket, (char*)pBuffer, nBufferSize, MSG_PEEK);
	else {
		SetBlockingMode(FALSE);
		int n = ::recv(m_socket, (char*)pBuffer, nBufferSize, MSG_PEEK);
		SetBlockingMode(TRUE);

		return n;
	}
}

BOOL CDDSConnect::WaitForData(DWORD dwTimeout)
{
	BOOL bReadReady, bExceptFlag;
	BYTE btDataByte;

	if (m_bMsgPeekMode)
	{
		ThreadSleep(1);
#define WAIT_FOR_TIMEOUT 5

		int n = dwTimeout / WAIT_FOR_TIMEOUT + 1;
		for (int i = 0; i < n; i++)
		{
			if (PeekBuffer(&btDataByte, sizeof(BYTE)) == 1)
				return TRUE;

			if (Select(&bReadReady, NULL, &bExceptFlag, WAIT_FOR_TIMEOUT))
			{
				if (bReadReady && (!bExceptFlag) && (PeekBuffer(&btDataByte, sizeof(BYTE)) == 1))
					return TRUE;
			}

			//ThreadSleep(10);
		}

		return FALSE;
	}
	else { // ���������ʹ�ø÷�ʽ��ѯ�����Ƿ�ɶ�		
		// Select also returns True when connection is broken.
		if (Select(&bReadReady, NULL, &bExceptFlag, dwTimeout))
		{
			if (bReadReady && (!bExceptFlag) && (PeekBuffer(&btDataByte, sizeof(BYTE)) == 1))
			{
				return TRUE;
			}
		}

		//__DebugOutput(_T("[DBPAPI] [Socket:%d] CConnect::WaitForData()����bReadReady=%d, bExceptFlag=%d"), (int)m_socket, bReadReady, bExceptFlag);

		if (PeekBuffer(&btDataByte, sizeof(BYTE)) == 1)
		{
			// ������е��ˣ���˵������select()��ѯʱ�޿ɶ����ݣ���ʵ���������Ѿ�������쳣��������л����ݲ�÷�ʽ
			m_bMsgPeekMode = TRUE;
			//__DebugOutput(_T("[DBPAPI] [Socket:%d] CConnect::WaitForData()�쳣��PeekBuffer()��������"), (int)m_socket);
			return TRUE;
		}

		return FALSE;
	}
}

int	CDDSConnect::Read(BYTE *pbuf,int nsize,int ntimeout)
{
	if (m_socket == INVALID_SOCKET)
		return SOCKET_ERROR;

	if (!WaitForData(ntimeout))
		return SOCKET_ERROR;

	int nResult = ::recv(m_socket, (char*)pbuf, nsize, 0);
	if (nResult == 0) //���ӹر�
		return SOCKET_ERROR;

	return nResult;

	/*
	if(m_socket == INVALID_SOCKET)
		return SOCKET_ERROR;
	TIMEVAL tv01 = {ntimeout/1000,1000 *(ntimeout%1000)};
	FD_SET fdr;
	fdr.fd_count = 1;
	fdr.fd_array[0] = m_socket;

	int nRet=::select(0, &fdr, NULL, NULL, &tv01);
	if(nRet <= 0)//��ʱ�������
		return nRet;
	nRet=::recv(m_socket, (char*)pbuf, nsize , 0);
	return nRet;
	*/
} //��

int		CDDSConnect::Send(BYTE *pbuf,int nsize,int ntimeout)
{
	BOOL bWriteReady, bExceptFlag;

	if (m_socket == INVALID_SOCKET)
		return SOCKET_ERROR;

	if (!Select(NULL, &bWriteReady, &bExceptFlag, ntimeout))
		return SOCKET_ERROR;

	if ((!bWriteReady) || bExceptFlag)
		return SOCKET_ERROR;

	return ::send(m_socket, (const char*)pbuf, nsize, 0);
	/*
	if(m_socket == INVALID_SOCKET)
		return SOCKET_ERROR;

	int nRet = 0;

	TIMEVAL tv01 = {ntimeout/1000,1000 * (ntimeout%1000)};//1�����ӳ�
	FD_SET fdw;
	fdw.fd_count = 1;
	fdw.fd_array[0] = m_socket;

	nRet = ::select(0, NULL, &fdw, NULL, &tv01);//���Կ�д
	if (nRet <= 0)//��ʱ���������ܷ�������
		return nRet;

	nRet = ::send(m_socket, (char*)pbuf,nsize,0);

	return nRet;
	*/
	
} //����


//1�ֽڰ�
BOOL	CDDSConnect::DoWrite(void* pdata,int nsize,int ntimeoutmsec,BYTE &ucret,BYTE &ddsorder)
{
	if(!IsConnect())
		return FALSE;

	if (CDriverMgr::m_bWaitTcpAck && (m_nNetMode == NET_MODE_TCP)) // ����Ӧ���汾
	{
		BYTE read[32];
		int nret = WriteDDS(pdata,nsize,ntimeoutmsec,read);
		if(nret > 0)
		{
			ucret  = read[0];
			ddsorder = read[0];
			return TRUE;
		}
		CString szerr = "����";
		if(nret == 0)
			szerr = "��ʱ";

		if(m_breal)
			GetLogger()->AddErrorFmt("ʵʱ����(socket = %d)%s����!",m_socket,szerr);
		else
			GetLogger()->AddErrorFmt("��������(socket = %d)%s����!",m_socket,szerr);

		SetConnect(FALSE); 
		return FALSE;
	}
	else {	// ����Ӧ���汾
		if (WriteDDS_NoAck(pdata, nsize))
		{
			ucret = 0;
			ddsorder = 0;
			return TRUE;
		}
		else 
		{
			if (m_nNetMode == NET_MODE_UDP)
			{
				if(m_breal)
					GetLogger()->AddErrorFmt("UDPʵʱ����(socket = %d)�������!", m_socket);
				else
					GetLogger()->AddErrorFmt("UDP��������(socket = %d)�������!", m_socket);
			}
			else 
			{
				if(m_breal)
					GetLogger()->AddErrorFmt("ʵʱ����(socket = %d)�������!", m_socket);
				else
					GetLogger()->AddErrorFmt("��������(socket = %d)�������!", m_socket);
			}
			SetConnect(FALSE); 

			return FALSE;
		}
	}
}

//1�ֽڰ�
int CDDSConnect::WriteDDS(void* pdata,int nsize,int ntimeoutmsec,BYTE *pret) //д��DDS,����0��ʾ��ȷ,<0���󣬴���0���յķ����ֽ���
{
	int nret = Send((BYTE*)pdata,nsize,2000);
	if(nret <= 0)
	{
		GetLogger()->AddDebugFmt("socket<%d>��������ʧ�ܣ�0x%02X, ���ݰ���С=%d, nret=%d", m_socket, WSAGetLastError(), nsize, nret);
		return nret;
	}

	BYTE buf[32];
	nret = Read(buf,16,ntimeoutmsec);//��

	if(nret > 0)
	{
		*pret = buf[nret-1]; //ȡ���һ���ֽ�
		return 1;
	}
	else 
		*pret = 0;
	return nret;
}

// ����Ӧ����
BOOL CDDSConnect::WriteDDS_NoAck(void* pdata,int nsize)
{
	int nret = Send((BYTE*)pdata,nsize,2000);

	if (nret < 0)
	{
		GetLogger()->AddDebugFmt("socket<%d>��������ʧ�ܣ�0x%02X, ���ݰ���С=%d", m_socket, WSAGetLastError(), nsize);
	}

	return (nret > 0);
}

BOOL	CDDSConnect::IsConnect()
{
	CSafeLock lock(&m_Lock);
	return m_bConnected;
}
void	CDDSConnect::SetConnect(BOOL bconnect)//����
{
	CSafeLock lock(&m_Lock);
	m_bConnected = bconnect;
}

void CDDSConnect::ClearConArg()//������Ӳ���
{
	CSafeLock lock(&m_Lock);
	m_ndds = 0;
	m_ncurdds = 0;
}

BOOL	CDDSConnect::GetNextConDef(LPDBPARG pcon)//��ȡ��һ�����Ӷ���
{
	CSafeLock lock(&m_Lock);

	if(m_ndds == 0)
		return FALSE;
	if((m_ncurdds< 0)||(m_ncurdds >= m_ndds))
		m_ncurdds = 0;

	memcpy(pcon,&m_ddscon[m_ncurdds],sizeof(TDBPARG));
	m_ncurdds = (m_ncurdds + 1)%m_ndds;
	return TRUE;
}

//1�ֽڰ�
BOOL	CDDSConnect::ConnectSvr()//����
{
	TDBPARG args;
	if(!GetNextConDef(&args))
		return FALSE;

	m_szSvrIP = args.sip;
	m_wPort = args.wport;
	m_nNetMode = (int)args.shNetMode;

	if(m_szSvrIP.IsEmpty())
		return FALSE;
	if(m_szSvrIP == "0.0.0.0")
		return FALSE; //��ЧIP

	if(m_socket != INVALID_SOCKET) //�ȶϿ�
	{
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	if (m_nNetMode == NET_MODE_UDP)
	{
		m_socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

		struct sockaddr_in ServerHostAddr;//������վ���ն�������ַ
		ServerHostAddr.sin_family = AF_INET;
		ServerHostAddr.sin_port = ::htons(m_wPort);
		ServerHostAddr.sin_addr.s_addr=::inet_addr(m_szSvrIP);

		if (::connect(m_socket, (sockaddr *)(&ServerHostAddr),sizeof(SOCKADDR)) == SOCKET_ERROR)
		{
			::closesocket(m_socket);
			m_socket = INVALID_SOCKET;
			return FALSE;
		}

		m_bConnected = TRUE;

		if(m_breal)
			GetLogger()->AddInfoFmt("UDPʵʱ���ӳɹ�(socket = %d)!",m_socket);
		else
			GetLogger()->AddInfoFmt("UDP�������ӳɹ�(socket = %d)!",m_socket);
	}
	else {
		struct sockaddr_in ServerHostAddr;//������վ���ն�������ַ
		ServerHostAddr.sin_family=AF_INET;
		ServerHostAddr.sin_port=::htons(m_wPort);

		ServerHostAddr.sin_addr.s_addr=::inet_addr(m_szSvrIP);

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
		CMyByteArray ba;
		ba.PutData(args.suser,16);
		ba.PutData(args.spass,16);
		BYTE *pb = NULL;
		int nsize;
		pb = ba.GetBuf(nsize);
		
		frmsend.MakeFrm(WDDS_OD_LOGIN,0,0,0,pb,nsize); //���¼��
		pb = frmsend.GetFrm(&nsize); //ȡ���ͱ���

		if (CDriverMgr::m_bWaitTcpAck)
		{
			BYTE readbuf[32];
			nret =  WriteDDS(pb,nsize,1000,readbuf); //д��DDS,����0��ʾ��ȷ,<0���󣬴���0���յķ����ֽ���
			if(nret != 1) //����
			{
				::closesocket(m_socket);
				m_socket = INVALID_SOCKET;
				return FALSE;
			}
			if(readbuf[0] != 0x00) //��¼ʧ��
			{
				::closesocket(m_socket);
				m_socket = INVALID_SOCKET;
				return FALSE;
			}
		}
		else 
		{
			if (!WriteDDS_NoAck(pb, nsize))
			{
				::closesocket(m_socket);
				m_socket = INVALID_SOCKET;
				return FALSE;
			}
		}

		if(m_breal)
			GetLogger()->AddInfoFmt("ʵʱ���ӳɹ�(socket = %d)!",m_socket);
		else
			GetLogger()->AddInfoFmt("�������ӳɹ�(socket = %d)!",m_socket);
	}

	return TRUE;
}

void	CDDSConnect::SetTcpNoDelay()//��ֹNagle�Ż�
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

void CDDSConnect::DoConnect()//���ӹ���
{
	m_bRun = TRUE;
	m_bKill = FALSE;
	while(!m_bKill)
	{
		if(IsConnect())
		{
			Sleep(100);
			continue;
		}
		if(ConnectSvr())
			SetConnect(TRUE);	//����������ʹ��
		else
			Sleep(100);
	}
	m_bKill = FALSE;
	m_bRun = FALSE; 

	TRACE("CDDSConnect Thread Exit\n");
}

UINT	CDDSConnect::ThreadConnectDDS(LPVOID lparam)//����DDS���߳�
{
	CDDSConnect* pcon = (CDDSConnect*)lparam;
	pcon->DoConnect();
	return 0;
}

void CDDSConnect::Start()
{
	if(m_bRun)
		return;
	AfxBeginThread(ThreadConnectDDS,this,THREAD_PRIORITY_NORMAL);//����DDS�����߳�
}

void CDDSConnect::Stop()
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
