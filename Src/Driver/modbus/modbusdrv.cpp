//////////////////////////////////////////////////////////////////
// demodrv.cpp
#include "stdafx.h"
#include "modbus.h"
#include "modbusdrv.h"
#include <math.h>
#include "..\sdk\HashTableUtils.h"

/////////////////////////////////////////////////////////////////
//ʵ�־�̬�����๤��
CDrvObj*	CDrvFactory::NewDrv()
{
	return new CModDrv();
}

DRVRET CDrvFactory::GetDriverInfo(LPDRVINFO lpDrvInfo)
{
	memset(lpDrvInfo,0,sizeof(T_DRVINFO));
	strcpy(lpDrvInfo->sname,"Modbus driver for IOServer");
	strcpy(lpDrvInfo->sdes,"CopyRight By ˫���˿Ƽ�");
	strcpy(lpDrvInfo->sver,"2.0.0.1"); //����汾
	lpDrvInfo->dwver = 0x0200;//�淶�汾
	lpDrvInfo->dwres = 0;
	return DRERR_OK;
}

/*
	CRC-16��ļ��㲽��
1��	��16λ�Ĵ���Ϊʮ������FFFF(��ȫΪ1)���ƴ˼Ĵ���ΪCRC�Ĵ�����
2��	��һ��8λ������16λCRC�Ĵ����ĵ�λ����򣬰ѽ������CRC�Ĵ�����
3��	�ѼĴ�������������һλ(����λ)����0����λ��������λ(�Ƴ�λ)��
4��	������λΪ0������3��(�ٴ���λ)��
������λΪ1��CRC�Ĵ��������ʽA001(1010 0000 0000 0001)�������
5��	�ظ�����3��4��ֱ������8�Σ���������8λ����ȫ�������˴���
6��	�ظ�����2������5��������һ��8λ�Ĵ���
7��	���õ���CRC�Ĵ�����ΪCRC�룬���ֽ���ǰ�����ֽ��ں�
  */

//CRC16��modbus RTUר��
unsigned short calccrc(unsigned char crcbuf,unsigned short crc)
{
   int i; 
   crc=crc ^ crcbuf;
   for(i=0;i<8;i++)
   {
       BYTE chk;
       chk=crc&1;
       crc=crc>>1;
       crc=crc&0x7fff;
       if (chk==1)
	       crc=crc^0xa001;
       crc=crc&0xffff;
   }
   return crc; 
}

unsigned short CRC_16(unsigned char *buf,int len)//�ṹΪINTEL˳�򣬵��ֽ���ǰ�����ֽ��ں�
{
	int  i;
	unsigned short crc;
	crc=0xFFFF;
	for (i=0;i<len;i++)
	{	
		crc=calccrc(*buf,crc);
		buf++;
	}
	return crc;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//CModCom485

CModCom485::CModCom485()
{
	m_szPort="\\\\.\\com1";			// �˿�������"\\\\.\\com1","\\\\.\\com2"
	m_dwBaudRate=CBR_9600;			//������,CBR_1200,CBR_2400,CBR_4800,CBR_9600,
									//CBR_14400,CBR_19200,CBR_38400,CBR_57600,CBR_115200,

	m_cByteSize=8;					//����λ number of bits/byte, 4-8
	m_cParity=0;					//0-4��Ӧno,odd,even,mark,space 
	m_cStopBits=0;					// 0,1,2 ��Ӧ 1, 1.5, 2

	m_hComDev=INVALID_HANDLE_VALUE;				//ͨ�Ŷ˿ھ��
	m_nerrnum = 0;

	m_nTimeOut = 200;
}

CModCom485::~CModCom485()
{
	ClosePort();
}

void CModCom485::InitComPort(
		LPCTSTR lpszComName,// �˿�������"\\\\.\\com1","\\\\.\\com2"
		DWORD dwBaudRate,	// ������,
		BYTE cByteSize,		// ����λ number of bits/byte, 4-8
		BYTE cParity,		// 0-4=no,odd,even,mark,space 
		BYTE cStopBits,		// 0,1,2 = 1, 1.5, 2 )
		int	 nTimeOut
	)//��ʼ��ͨ�ſ�
{
	m_szPort=lpszComName;			// �˿�������"\\\\.\\com1","\\\\.\\com2"
	m_dwBaudRate=dwBaudRate;			//������,CBR_1200,CBR_2400,CBR_4800,CBR_9600,
									//CBR_14400,CBR_19200,CBR_38400,CBR_57600,CBR_115200,
	m_cByteSize=cByteSize;			//����λ number of bits/byte, 4-8
	m_cParity=cParity;				//0-4��Ӧno,odd,even,mark,space 
	m_cStopBits=cStopBits;			// 0,1,2 ��Ӧ 1, 1.5, 2
	m_nTimeOut = nTimeOut;
	ClosePort();
}

BOOL CModCom485::SendData(BYTE *pc,int nSize)
{
	if(m_hComDev == INVALID_HANDLE_VALUE)
		return FALSE;
	COMSTAT comstat;
	DWORD dwErrorMask;
	DWORD dwWriteNum=0;
	BOOL bret;
	bret=::WriteFile(
			m_hComDev,      // handle to file
			pc,             // data buffer
			nSize,				// number of bytes to write
			&dwWriteNum,		// number of bytes written
			NULL			// overlapped buffer
		);
	if(!bret)
	{
		ClearCommError(m_hComDev,&dwErrorMask,&comstat);//�������
		PurgeComm(m_hComDev,PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_RXCLEAR);//������������еĻ�����
		
		m_nerrnum++;
		if(m_nerrnum > 10)
		{
			ClosePort();
			Sleep(50);
			OpenPort();
			m_nerrnum = 0;
		}
		return bret;
	}
	if(dwWriteNum == 0)
		return FALSE;
	
	m_nerrnum = 0;
	return TRUE;
}

int  CModCom485::ReadData(BYTE *pc,int nSize)
{
	if(m_hComDev == INVALID_HANDLE_VALUE)
		return 0;
	BOOL bret;
	DWORD dwReadNum=0;
	COMSTAT comstat;
	DWORD dwErrorMask;

	bret = ::ReadFile(m_hComDev,pc,nSize,&dwReadNum,NULL);
	if(!bret)
	{
		ClearCommError(m_hComDev,&dwErrorMask,&comstat);//�������
		PurgeComm(m_hComDev,PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_RXCLEAR);//������������еĻ�����

		m_nerrnum++;
		if(m_nerrnum > 10)
		{
			ClosePort();
			Sleep(50);
			OpenPort();
			m_nerrnum = 0;
		}
		return 0;
	}
	m_nerrnum = 0;
	return (int)dwReadNum;
}

//--------------------��������Ҫ���صĺ���
BOOL CModCom485::OpenPort()
{
	m_hComDev =CreateFile( m_szPort, GENERIC_READ | GENERIC_WRITE,
                  0,                    // exclusive access
                  NULL,                 // no security attrs
                  OPEN_EXISTING,
                  FILE_ATTRIBUTE_NORMAL,
                  NULL ); 
	if(m_hComDev==INVALID_HANDLE_VALUE)
		return FALSE;//CreateFileͨ�ſ�ʧ�ܣ�

	COMMTIMEOUTS  CommTimeOuts ;
	DCB dcb;
	COMMCONFIG cfg;

	cfg.dcb.DCBlength = sizeof( DCB ) ;
	BOOL bRet=FALSE;
	bRet=GetCommState( m_hComDev, &(cfg.dcb) ) ;

	SetupComm( m_hComDev, 4096, 4096 ) ;
    
	PurgeComm( m_hComDev, PURGE_TXABORT | PURGE_RXABORT |
                                      PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
	dcb.DCBlength = sizeof( DCB ) ;

	bRet=GetCommState( m_hComDev, &dcb ) ;
	if(!bRet)
	{
		TRACE("GetCommState error!---------\n");
		return FALSE;
	}
	dcb.BaudRate =m_dwBaudRate;
	dcb.ByteSize =m_cByteSize;
	dcb.Parity =m_cParity;
	dcb.StopBits =m_cStopBits;

   // setup hardware flow controlӲ����������
	dcb.fDtrControl = DTR_CONTROL_ENABLE;//DTR_CONTROL_DISABLE ;// 485ת������DTR_CONTROL_ENABLE
	dcb.fOutxCtsFlow = FALSE ;
    dcb.fRtsControl = RTS_CONTROL_DISABLE ;

   // setup software flow control�����������
	dcb.fInX = dcb.fOutX = FALSE ;

   // other various settings
	dcb.fBinary = TRUE ;
	dcb.fParity = TRUE ;
	
   // set up for overlapped I/O
	int ntm = (2*9600)/dcb.BaudRate;
	if(ntm <= 0)
		ntm = 1;
	int nConstant = m_nTimeOut - 128 * ntm;
	if(nConstant < 0)
		nConstant = 0;
	CommTimeOuts.ReadIntervalTimeout			=	5;//�ַ��䳬ʱ3����,
	CommTimeOuts.ReadTotalTimeoutMultiplier		=	1+ntm;
	CommTimeOuts.ReadTotalTimeoutConstant		=	100;//��̬��ʱ
	CommTimeOuts.WriteTotalTimeoutMultiplier	=   ntm;
	CommTimeOuts.WriteTotalTimeoutConstant		=  200;//��̬100����

	if(!SetCommTimeouts(m_hComDev , &CommTimeOuts ))
	{
		TRACE("SetCommTimeouts error!---------\n");
		return FALSE;
	}
	if (!SetCommState( m_hComDev, &dcb ))
	{
		TRACE("SetCommState error!---------\n");
		return FALSE;//
	}
	m_nerrnum=0;
	return TRUE;
}

void CModCom485::ClosePort()
{
	if(m_hComDev != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComDev);
		m_hComDev = INVALID_HANDLE_VALUE;
	}
}
BOOL CModCom485::IsPortOpen()//�˿��Ƿ��
{
	return (m_hComDev != INVALID_HANDLE_VALUE);
}

int CModCom485::WriteFrm(BYTE *pb,int nsize)////����modbusͨ�Ű�
{
	if(m_hComDev == INVALID_HANDLE_VALUE)
		return FALSE;
	//רΪmodbus 485��˫�����
	PurgeComm(m_hComDev,PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_RXCLEAR);//������������еĻ�����

	COMSTAT comstat;
	DWORD dwErrorMask;
	DWORD dwWriteNum=0;
	BOOL bret;
	bret=::WriteFile(
			m_hComDev,      // handle to file
			pb,             // data buffer
			nsize,				// number of bytes to write
			&dwWriteNum,		// number of bytes written
			NULL			// overlapped buffer
			);
	m_pDrv->WriteDebugLog(FALSE, pb, nsize);
	if(!bret)
	{
		ClearCommError(m_hComDev,&dwErrorMask,&comstat);//�������
		PurgeComm(m_hComDev,PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_RXCLEAR);//������������еĻ�����
		
		m_nerrnum++;
		if(m_nerrnum > 10)
		{
			ClosePort();
			Sleep(50);
			OpenPort();
			m_nerrnum = 0;
		}
		return 0;
	}
	m_nerrnum = 0;
	return (int)dwWriteNum;
}		
int CModCom485::ReadFrm(BYTE *pbuf,int nbufsize,int ncrc16seq)//����modbusͨ�Ű�,�����ֽ���,���������ݰ�����֤
{
	BYTE sread[512];//����
	int  nrsize =0;

	//�ȴ�����
	int i=0;

	int  nr=0;
	BYTE rt[256];
			
	for(i=0;i<5;i++)
	{
		Sleep(20);//��ʱ50����
		nr = ReadData(rt,128);//��128�ֽ�,ÿ������60���Ĵ�����������������ݰ� 3 + 60 * 2  + 2 = 125
		if(nr == 0)
			continue;
		if(nrsize + nr > 512)
			nrsize=0;
		int k;
		for(k=0;k<nr;k++)
		{
			sread[nrsize] = rt[k];
			nrsize++;
		}

		//�������֡
		if(nrsize < 3)
			continue;
					
		WORD wcrc=CRC_16(sread,nrsize-2);//����ǰ

		if(wcrc ==  CModDrv::GetWord(sread + nrsize - 2,ncrc16seq))//�Ƚ�CRC16ֵ
		{
			if(nrsize > nbufsize)
				return 0;
			int j;
			for(j=0;j<nrsize;j++)
				pbuf[j] = sread[j];	//��������֡

			m_pDrv->WriteDebugLog(FALSE, pbuf, nrsize);
			return nrsize;
		}
	}
	return 0;
}	

///////////////////////////////////////////////////////////////////////////////////
// CModComTcp

CModComTcp::CModComTcp()
{
	m_szSvrIp = "172.0.0.1";//������IP
	m_nSvrPort = 502;//����˿�
	m_sClient = INVALID_SOCKET;
	m_nTimeOut = 200;
	m_bTcpCrc16 = FALSE;
}

CModComTcp::~CModComTcp()
{
	ClosePort();
}

BOOL CModComTcp::IsPortOpen()//�˿��Ƿ��
{
	return (m_sClient != INVALID_SOCKET);
}
BOOL CModComTcp::OpenPort()//�򿪶˿�
{
	int nRet;
	if(m_sClient != INVALID_SOCKET)
	{
		::closesocket(m_sClient);
		m_sClient = INVALID_SOCKET;
	}

	unsigned long ul = 1;//��������ʽ
	struct sockaddr_in sServer;

	sServer.sin_family=AF_INET;
	sServer.sin_port=::htons(m_nSvrPort);
	sServer.sin_addr.s_addr=::inet_addr(m_szSvrIp);

	m_sClient=::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(m_sClient==INVALID_SOCKET)
		return 1;
	
	if(SOCKET_ERROR == ioctlsocket(m_sClient, FIONBIO, (unsigned long*)&ul))//���÷�������ʽ����
	{
		::closesocket(m_sClient);
		m_sClient = INVALID_SOCKET;
		return FALSE;
	}

	connect(m_sClient,(const struct sockaddr *)&sServer,sizeof(sServer));
	
	TIMEVAL tv01 = {2,0};//2�����ӳ�
	FD_SET fdw = {1, m_sClient};
	nRet=::select(0, NULL, &fdw, NULL, &tv01);//���Կ�д
	if(nRet != 1)
	{
		::closesocket(m_sClient);
		m_sClient = INVALID_SOCKET;
		return  FALSE;
	}
	return TRUE;
}

void CModComTcp::ClosePort()//�رն˿�
{
	if(m_sClient != INVALID_SOCKET)
	{
		::closesocket(m_sClient);
		m_sClient = INVALID_SOCKET;
	}
}

// ndelaysec��ʱ����
// ���ض����ֽ�����������ͷ,-1�������ش�
int CModComTcp::Read(SOCKET s,BYTE *pbuf,int size,int ndelaymsec)
{
	TIMEVAL tv01 = {ndelaymsec/1000,1000 *(ndelaymsec%1000)};
	FD_SET fdr = {1, s};
	int nRet;
	char buf[512];
	int  nfrmlen=0;

	nRet=::select(0, &fdr, NULL, NULL, &tv01);
	if(SOCKET_ERROR==nRet)//�����������
	{
		int nErrorCode=WSAGetLastError();
		TRACE("�������߳����ö���ʱ����nErrorCode=%d\n",nErrorCode);
		return -1;
	}
	if(nRet==0)//��ʱ�������޿ɶ�����
	{
		TRACE("Read timeout\n");
		return 0;
	}

	nfrmlen=::recv(s, buf, 264 , 0);
	if(SOCKET_ERROR==nfrmlen)//���ܴ���
	{
		TRACE("�������ݴ���\n");
		return -1;
	}
	if(nfrmlen==0)//���ӱ��ر�
		return -1;

	if(nfrmlen > size)
		return 0;
	memcpy(pbuf,buf,nfrmlen);

	return nfrmlen;
}

// ndelaysec��ʱ����
// ���ط����ֽ�������������ͷ,�������ش���ʱ���� -1��
int CModComTcp::Write(SOCKET s,BYTE *pbuf,int size,int ndelaymsec)
{
	TIMEVAL tv01 = {ndelaymsec/1000,1000 * (ndelaymsec%1000)};//1�����ӳ�
	FD_SET fdw = {1, s};

	if(1 != ::select(0, NULL, &fdw, NULL, &tv01))//���Կ�д
		return  0;
	
	int nret = ::send(s, (char*)pbuf,size,0);
	if(SOCKET_ERROR == nret)//���ʹ���
		return -1;
		
	return nret;
}

int CModComTcp::WriteFrm(BYTE *pb,int nsize)
{
	BYTE buf[512];
	if(nsize > 256)
		return 0;

	int nlen = nsize;
	//��д��ͷ
	buf[0] = 0xF5;//����ģ�����ʶ��˫������
	buf[1] = 0x5F;
	buf[2] = 0;//Э��
	buf[3] = 0;
	buf[4] = 0;
	if(m_bTcpCrc16)
	{
		buf[5] = BYTE(nsize);
		nlen = nsize;
	}
	else
	{
		buf[5] = BYTE(nsize - 2);
		nlen = nsize - 2;
	}
	
	memcpy(buf + 6,pb,nlen);
	
	int nret = Write(m_sClient,buf,nlen + 6,400);//400ms��ʱ
	m_pDrv->WriteDebugLog(FALSE, buf, nlen + 6);
	if(nret == -1)
	{
		ClosePort();
		return 0;
	}
	return nret;
}

int CModComTcp::ReadFrm(BYTE *pbuf,int nbufsize,int ncrc16seq)
{
	BYTE buf[512];
	int n = Read(m_sClient,buf,264,m_nTimeOut);
	if(n == -1)
	{
		ClosePort();
		return 0;
	}

	if(n < 6)//
		return 0;

	m_pDrv->WriteDebugLog(TRUE, buf, n);
	
	WORD wlen = CModDrv::GetWord(buf + 4,21);//TCPͷ����д�����ݳ���
	if( n < wlen + 6)//���Ȳ���
	{
		int n2 = Read(m_sClient,buf+n,512 - n,m_nTimeOut);
		if(n2 == -1)
		{
			ClosePort();
			return 0;
		}
		m_pDrv->WriteDebugLog(TRUE, buf + n, n2);
		n+=n2;
	}
	int nf = wlen;
	if( n != wlen + 6)
	{
		m_pDrv->AddDebugFmt(_T("�������ݳ�����Ч�����գ�%d�ֽڣ��ڴ���%d�ֽ�"), n, wlen + 6);
		return 0;
	}

	BYTE *pb = buf + 6;
	if(m_bTcpCrc16)//��CRC16
	{
		WORD wcrc=CRC_16(pb,nf-2);//����ǰ
		if(wcrc ==  CModDrv::GetWord(pb + nf - 2,ncrc16seq))//�Ƚ�CRC16ֵ
		{
			if(nf > nbufsize)
				return 0;
			memcpy(pbuf,pb,nf);
			return nf;
		}
	}
	else //����CRC16,�����,Ϊ�˺���ͳһ����
	{
		if((buf[0] != 0xF5)||(buf[1] != 0x5F))
		{
			m_pDrv->AddDebugFmt(_T("��Ч���ݰ���ʶ�����գ�%02X %02X���ڴ���F5 5F"), (int)buf[0], (int)buf[1]);
			return 0;
		}
		//��CRC16
		WORD wcrc=CRC_16(pb,nf);//����ǰ
		CModDrv::SetWord(&wcrc,pb+nf,ncrc16seq);
		if(nf+2 > nbufsize)
			return 0;
		memcpy(pbuf,pb,nf+2);
		return nf+2;
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////
//
void CModDec::InitSendFrm0102(int norder)//��ʼ�����ͻ�����,ֻ�洢��ʱ��ȡ����01,02
{
	BYTE ucorder;
	WORD wregaddr;

	WORD wt;
	BOOL bs=FALSE;
	CModItem *pi;
	int nbytes=0;
	// �ӻ���ַ1 ������1 ��ʼ��ַ2 ��ȡ����2 CRC2
	BYTE fb[256];
	int  size=0;


	int i;
	for(i=0;i<m_ndatas;i++)
	{
		pi = m_pbuf[i];
		if(pi->m_ucorder != norder)
			continue;
		if(bs == FALSE)//��ʼ
		{
			bs=TRUE;
			ucorder = pi->m_ucorder;
			wregaddr = pi->m_waddr;
			nbytes++;
		}
		else
		{

			if(		((nbytes + wregaddr) == pi->m_waddr) && //��ַ����
					(nbytes < 48)  //48���Ĵ���
				)
				nbytes++;
			else
			{
				//��֡,��������ǰpi
				BYTE *pb = (BYTE *)fb;

				pb[0] = m_ucaddr;//�豸��ַ
				pb[1] = ucorder;//����

				CModDrv::SetWord(&wregaddr,pb+2,21);//��д��ʼ��ַ

				wt = (WORD)(nbytes);//����
				CModDrv::SetWord(&wt,pb+4,21);//��д���ݸ���

				wt = CRC_16( pb, 6 );
				CModDrv::SetWord(&wt,pb+6,m_ncrc16seq);//��дCRC16

				m_sendfrm.AddFrm(fb,8);//���뷢��֡������
				
				//����ǰpi�鵽��һ֡
				ucorder = pi->m_ucorder;
				wregaddr = pi->m_waddr;
				nbytes = 1;
			}
		}
	}
	if(nbytes>0)
	{
		BYTE *pb = (BYTE *)fb;

		pb[0] = m_ucaddr;//�豸��ַ
		pb[1] = ucorder;//����

		CModDrv::SetWord(&wregaddr,pb+2,21);//��д��ʼ��ַ

		wt = (WORD)(nbytes);//����
		CModDrv::SetWord(&wt,pb+4,21);//��д���ݸ���
		
		wt = CRC_16( pb, 6 );
		CModDrv::SetWord(&wt,pb+6,m_ncrc16seq);//��дCRC16
		
		m_sendfrm.AddFrm(fb,8);//���뷢��֡������
	}
}
void CModDec::InitSendFrm0304(int norder)//��ʼ�����ͻ�����,ֻ�洢��ʱ��ȡ����03,04
{
	BYTE ucorder;
	WORD wregaddr;

	WORD wt;
	BOOL bs=FALSE;
	CModItem *pi;
	int nbytes=0;
	// �ӻ���ַ1 ������1 ��ʼ��ַ2 ��ȡ����2 CRC2
	BYTE fb[256];
	int  size=0;

	int i,n;
	for(i=0;i<m_ndatas;i++)
	{
		pi = m_pbuf[i];
		if(pi->m_ucorder != norder)
			continue;
		if(bs == FALSE)//��ʼ
		{
			bs=TRUE;
			ucorder = pi->m_ucorder;
			wregaddr = pi->m_waddr;
			n=pi->GetCVLen();
			nbytes += n;
		}
		else
		{
			n=pi->GetCVLen();//�ֽ���

			if(	((nbytes/2 + wregaddr) == pi->m_waddr) && //��ַ����,���ֱ��ŵļĴ�����ַ
					((nbytes/2) < MODBUS_REGISTER_MAX)  //
				)
				nbytes+=n;
			else
			{
				;//��֡,��������ǰpi
				BYTE *pb = (BYTE *)fb;

				pb[0] = m_ucaddr;//�豸��ַ
				pb[1] = ucorder;//����
				CModDrv::SetWord(&wregaddr,pb+2,21);//������ʼ��ַ
				

				wt = (WORD)(nbytes/2);//���ִ���
				CModDrv::SetWord(&wt,pb+4,21);//�������ݸ���
				
				wt = CRC_16( pb, 6 );
				CModDrv::SetWord(&wt,pb+6,m_ncrc16seq);//��дCRC16
				
				m_sendfrm.AddFrm(fb,8);//���뷢��֡������
				
				//����ǰpi�鵽��һ֡
				ucorder = pi->m_ucorder;
				wregaddr = pi->m_waddr;
				n=pi->GetCVLen();
				nbytes = n;
			}
		}
	}

	if(nbytes>0)
	{
		BYTE *pb = (BYTE *)fb;

		pb[0] = m_ucaddr;//�豸��ַ
		pb[1] = ucorder;//����
		CModDrv::SetWord(&wregaddr,pb+2,21);//������ʼ��ַ

		wt = (WORD)(nbytes/2);//���ִ���
		CModDrv::SetWord(&wt,pb+4,21);//�������ݸ���

		wt = CRC_16( pb, 6 );
		CModDrv::SetWord(&wt,pb+6,m_ncrc16seq);//��дCRC16

		m_sendfrm.AddFrm(fb,8);//���뷢��֡������
	}
}

void CModDec::InitSendFrm()//��ʼ����ʱ����
{
	sort();//����
	m_sendfrm.Clear();//�����
	InitSendFrm0102(1);
	InitSendFrm0102(2);
	InitSendFrm0304(3);
	InitSendFrm0304(4);
}

////////////////////////////////////////////////////////////////////////////
//CModDrv 2.0
short CModDrv::GetShort(BYTE *pb,int nseq)//���֣�nseq = 0���ֽ���ǰ,1:���ֽ���ǰ
{
	BYTE ca[4];
	if(nseq != 12)//���ֽ���ǰ
	{
		ca[0] = pb[1];		ca[1] = pb[0];
	}
	else
	{
		ca[0] = pb[0];		ca[1] = pb[1];
	}
	return *((short*)ca);
}

WORD CModDrv::GetWord(BYTE *pb,int nseq)//���֣�nseq = 0���ֽ���ǰ,1:���ֽ���ǰ
{
	BYTE ca[4];
	if(nseq != 12)//���ֽ���ǰ
	{
		ca[0] = pb[1];		ca[1] = pb[0];
	}
	else
	{
		ca[0] = pb[0];		ca[1] = pb[1];
	}
	return *((WORD*)ca);
}

//nseq = �ֽ�˳�� 0:1234; 1:2143; 2:4321; 3:3412;1Ϊ����ֽڣ�4Ϊ����ֽ�,��4321������˳��
long CModDrv::GetLong(BYTE *pb,int nseq)//
{
	BYTE ca[4];
	if(nseq == 1234)//
	{
		ca[0] = pb[0];		ca[1] = pb[1];		ca[2] = pb[2];		ca[3] = pb[3];
	}
	else if(nseq == 2143)//
	{
		ca[0] = pb[1];		ca[1] = pb[0];		ca[2] = pb[3];		ca[3] = pb[2];
	}

	else if(nseq == 4321)//
	{
		ca[0] = pb[3];		ca[1] = pb[2];		ca[2] = pb[1];		ca[3] = pb[0];
	}
	else
	{
		ca[0] = pb[2];		ca[1] = pb[3];		ca[2] = pb[0];		ca[3] = pb[1];
	}
	return *((int*)ca);
}

DWORD CModDrv::GetDWord(BYTE *pb,int nseq)//
{
	BYTE ca[4];
	if(nseq == 1234)//
	{
		ca[0] = pb[0];		ca[1] = pb[1];		ca[2] = pb[2];		ca[3] = pb[3];
	}
	else if(nseq == 2143)//
	{
		ca[0] = pb[1];		ca[1] = pb[0];		ca[2] = pb[3];		ca[3] = pb[2];
	}

	else if(nseq == 4321)//
	{
		ca[0] = pb[3];		ca[1] = pb[2];		ca[2] = pb[1];		ca[3] = pb[0];
	}
	else
	{
		ca[0] = pb[2];		ca[1] = pb[3];		ca[2] = pb[0];		ca[3] = pb[1];
	}
	return *((DWORD*)ca);
}

//nseq = �ֽ�˳�� 0:1234; 1:2143; 2:4321; 3:3412;1Ϊ����ֽڣ�4Ϊ����ֽ�,��4321������˳��
float CModDrv::GetFloat(BYTE *pb,int nseq)//
{
	BYTE ca[4];
	if(nseq == 1234)//
	{
		ca[0] = pb[0];		ca[1] = pb[1];		ca[2] = pb[2];		ca[3] = pb[3];
	}
	else if(nseq == 2143)//
	{
		ca[0] = pb[1];		ca[1] = pb[0];		ca[2] = pb[3];		ca[3] = pb[2];
	}

	else if(nseq == 4321)//
	{
		ca[0] = pb[3];		ca[1] = pb[2];		ca[2] = pb[1];		ca[3] = pb[0];
	}
	else
	{
		ca[0] = pb[2];		ca[1] = pb[3];		ca[2] = pb[0];		ca[3] = pb[1];
	}
	return *((float*)ca);
}

void CModDrv::SetWord(void* pv,BYTE *pb,int nseq)
{
	BYTE *p = (BYTE*)pv;
	if(nseq != 12)//���ֽ���ǰ
	{
		pb[0] = p[1];		pb[1] = p[0];
	}
	else
	{
		pb[0] = p[0];		pb[1] = p[1];
	}
}
void CModDrv::SetDWord(void* pv,BYTE *pb,int nseq)
{
	BYTE *p = (BYTE*)pv;
	if(nseq == 1234)//
	{
		pb[0] = p[0];		pb[1] = p[1];		pb[2] = p[2];		pb[3] = p[3];
	}
	else if(nseq == 2143)//
	{
		pb[0] = p[1];		pb[1] = p[0];		pb[2] = p[3];		pb[3] = p[2];
	}

	else if(nseq == 4321)//
	{
		pb[0] = p[3];		pb[1] = p[2];		pb[2] = p[1];		pb[3] = p[0];
	}
	else
	{
		pb[0] = p[2];		pb[1] = p[3];		pb[2] = p[0];		pb[3] = p[1];
	}
}
void CModDrv::SetFloat(void* pv,BYTE *pb,int nseq)
{
	BYTE *p = (BYTE*)pv;
	if(nseq == 1234)//
	{
		pb[0] = p[0];		pb[1] = p[1];		pb[2] = p[2];		pb[3] = p[3];
	}
	else if(nseq == 2143)//
	{
		pb[0] = p[1];		pb[1] = p[0];		pb[2] = p[3];		pb[3] = p[2];
	}

	else if(nseq == 4321)//
	{
		pb[0] = p[3];		pb[1] = p[2];		pb[2] = p[1];		pb[3] = p[0];
	}
	else
	{
		pb[0] = p[2];		pb[1] = p[3];		pb[2] = p[0];		pb[3] = p[1];
	}
}

CModDrv::CModDrv()
{
	m_ndecsize = 0;//�豸����
	int i;
	for(i=0;i<MAX_MODBUSDEC;i++)
		m_pdec[i] = 0;

	m_pmodcom = NULL;
	m_wVersion = 0;
}

CModDrv::~CModDrv()
{
	CString szid;
	CModItemIndex *pmi;
	POSITION pos = m_mapitem.GetStartPosition();
	while(pos!=NULL)
	{
		m_mapitem.GetNextAssoc( pos, szid, pmi );
		delete pmi;
	}
	m_mapitem.RemoveAll();//���MAP

	int i;
	for(i=0;i<m_ndecsize;i++)
		delete m_pdec[i];//ɾ���豸��

}

void	CModDrv::Serialize(CArchive& ar)//���л����棬��ȡ����
{
	if(ar.IsLoading()) //����Ƕ���
	{
		DWORD dwFlag = 0;
		ar >> dwFlag;
		if(dwFlag != DRVCFG_FLAG_MODBUS)
			return;
		
		// ���Ӱ汾����Ϣ
		ar >> m_wVersion;
		if (m_wVersion > DRVCFG_VERSION)
			return;		

		m_cfg.Serialize(ar, m_wVersion);
		TransCfg();//�������ļ�ת��Ϊ���ж���
	}
	else {
		ar << (DWORD)DRVCFG_FLAG_MODBUS;		
		// ���Ӱ汾����Ϣ
		m_wVersion = DRVCFG_VERSION;
		ar << m_wVersion;

		m_cfg.Serialize(ar, m_wVersion);
	}
}

DRVRET  CModDrv::GetErrMsg(CString& szMsg,DRVRET dwcode)			//��ȡ������Ϣ
{
	if(dwcode < 100)
		return CDrvObj::GetErrMsg(szMsg,dwcode);

	//>100�������Զ���ģ�����û���Զ��������Ϣ��ֱ�ӷ����޴˴������
	DRVRET lret = DRERR_OK;
	if(dwcode == RESULT_OPENCOMPORTERR)
		szMsg = "��COMͨ�ſڴ�!";
	else if(dwcode == RESULT_NOCOMTYPEERR)
		szMsg = "��֧�ֵ�ͨ�ŷ�ʽ!";
	else if(dwcode == RESULT_TCPCONNECTSERVERERR)
		szMsg = "����TCP����������!";
	else 
		lret = DRERR_NOERRCODE;
	return lret;
}

#include "DlgConfig.h"
BOOL	CModDrv::OnConfig()	//���ú����ý���,����TRUE��ʾ�����޸���
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDlgConfig dlg;
	dlg.m_pcfg = &(m_cfg);
	dlg.DoModal(); 
	if(m_cfg.IsModify())//�����޸�
	{
		TransCfg();//���½������ļ�ת��Ϊ���ж���
		m_cfg.SetModify(FALSE);
		return TRUE;
	}
	return FALSE;
}

DRVRET	CModDrv::OnStart()			//׼������,����
{
	//��ʼ���̶����Ͱ�
	int i;
	for(i=0;i<m_ndecsize;i++)
		m_pdec[i]->InitSendFrm();

	//����ͨ�Ŷ���
	if(m_pmodcom != NULL)
	{
		delete m_pmodcom;
		m_pmodcom = NULL;
		return RESULT_OPENCOMPORTERR;
	}
	if(m_cfg.m_nComType == COM_TYPE_485)
	{
		CModCom485 *pcom = new CModCom485;
		//��ʼ������
		pcom->InitComPort(m_cfg.m_szPort,
			m_cfg.m_dwBaudRate,
			m_cfg.m_cByteSize,
			m_cfg.m_cParity,
			m_cfg.m_cStopBits,
			m_cfg.m_nTimeOut);
		if(!pcom->OpenPort())
		{
		//	delete pcom;
		//	return RESULT_OPENCOMPORTERR;
			if(m_cbOnLogOk)
			{
				AddErrorFmt("Modbus 485���ܴ�<%s>!",m_cfg.m_szPort);
			}
		}
		m_pmodcom = pcom;
	}
	else
	{
		CModComTcp *pcom  = new CModComTcp;
		pcom->InitArgs(m_cfg.m_szIP,m_cfg.m_wtcpport,m_cfg.m_bTcpCrc16,m_cfg.m_nTimeOut);
		if(!pcom->OpenPort())
		{
		//	delete pcom;
		//	return RESULT_TCPCONNECTSERVERERR;
			if(m_cbOnLogOk)
			{
				AddErrorFmt("Modbus TCP�������ӵ�������<%s;%d>!",m_cfg.m_szIP,m_cfg.m_wtcpport);
			}
		}
		m_pmodcom = pcom;
	}
	m_pmodcom->m_pDrv = this;
#ifdef _ENABLED_RECONNECT_TIMEOUT
	m_tmLastReconnectTime = ::time(NULL);
#endif

	return DRERR_OK;
}

void CModDrv::WriteDebugLog(BOOL nReadMode, BYTE *pb, int nsize)
{
	if (!m_cfg.m_bSaveLog)
		return;
	
	CString szLog = (nReadMode) ? _T("recv: ") : _T("send: ");
	CString szt;
	for(int i = 0 ; i < nsize ; i++)
	{
		szt.Format("%02X ",pb[i]);
		szLog += szt;
	}
	AddDebug(szLog);
}

UINT	CModDrv::DoRun()//ͨ�Ŵ���
{
	m_bRun = TRUE; //��������һ��

	int		nsendsize;	//�����ֽ���
	BYTE	ssend[512];	//���ͻ���
	BYTE	sread[512];	//���ܻ���
	int		nrsize;		//�����ֽ���
	WORD	wAddr;		//�Ĵ�����ַ
	int i;
	CModDec	*pdec = NULL;	
	
	::CoFileTimeNow(&m_ftStartTime);
	while(!m_bKillThread)//�ȷ�������Ҫ�������͵İ�
	{
		if(m_pmodcom == NULL)
		{
			Sleep(50);
			continue;
		}

		// �����Ƿ�ﵽ��������
		::CoFileTimeNow(&m_ftEndTime);
		if (GetElapsedTime() < (long)m_cfg.m_nUpdateRate)
		{
			Sleep(50);
			continue;
		}
		memcpy(&m_ftStartTime, &m_ftEndTime, sizeof(FILETIME));

#ifdef _ENABLED_RECONNECT_TIMEOUT
		// ÿ2��Сʱ�Ͽ�Socket���ӣ�����������
		time_t tmNow = ::time(NULL);
		if ((tmNow - m_tmLastReconnectTime) > _RECONNECT_TIMEOUT)
		{
			m_pmodcom->ClosePort();
			m_tmLastReconnectTime = tmNow;
			AddDebug(_T("Modbus��ʱ����"));
		}
#endif

		if(!m_pmodcom->IsPortOpen())
		{
			m_pmodcom->OpenPort();
			continue;
		}

		//{{
		/*
		pdec = m_pdec[0];
		nrsize = m_pmodcom->ReadFrm(sread,512,pdec->m_ncrc16seq);//����
		if(nrsize > 0)//�����������
		{
			//WriteDebugLog(TRUE, sread, nrsize);
			wAddr = 0x150;
			DoReadFrm(3,wAddr,pdec,sread,nrsize);//������յ������ݰ������ݰ�����֤
		}
		Sleep(1);
		continue;
		*/
		//}}

		//����Ƿ�����������֡
		nsendsize = m_randfrm.GetFrm(ssend,512);//ȡ�غ�ɾ����
		if(nsendsize > 0)
		{
			//����豸ָ��
			pdec = GetDecByAddr(ssend[0]);
			if(pdec == NULL)
				continue;
			wAddr = GetRegAddr(ssend,nsendsize);//ȡ�Ĵ�����ַ;
			m_pmodcom->WriteFrm(ssend,nsendsize);//����
			//WriteDebugLog(FALSE, ssend, nsendsize);

			nrsize = m_pmodcom->ReadFrm(sread,512,pdec->m_ncrc16seq);//����
			if(nrsize > 0)//�����������
			{
				//WriteDebugLog(TRUE, sread, nrsize);
				DoReadFrm(ssend[1],wAddr,pdec,sread,nrsize);//������յ������ݰ������ݰ�����֤
			}
			Sleep(1);
			continue;//����������Ҫ�������͵İ�
		}
		Sleep(1);
		
		//�̶��ķ��Ͱ�
		for(i = 0; i< m_ndecsize; i++)
		{
			if(m_bKillThread)
				break;
			pdec = m_pdec[i];
			nsendsize = pdec->m_sendfrm.GetNextFrm(ssend,512);//ȡ�غ�ɾ����
			if(nsendsize > 0)
			{
				wAddr = GetRegAddr(ssend,nsendsize);//ȡ�Ĵ�����ַ;
				m_pmodcom->WriteFrm(ssend,nsendsize);//����
				//WriteDebugLog(FALSE, ssend, nsendsize);

				nrsize = m_pmodcom->ReadFrm(sread,512,pdec->m_ncrc16seq);//����
				if(nrsize > 0)//�����������
				{
					AddDebugFmt(_T("�������ݰ���ucod=%d, wAddr=%d, decidx=%d, size=%d"), (int)ssend[1], (int)wAddr, i, nrsize);

					//WriteDebugLog(TRUE, sread, nrsize);
					DoReadFrm(ssend[1],wAddr,pdec,sread,nrsize);//������յ������ݰ������ݰ�����֤
				}
			}
			Sleep(1);
			continue;//��������̶����Ͱ�
		}
		Sleep(1);
	}
	m_bRun = FALSE; //��������һ��
	return DRERR_OK;
}

void	CModDrv::OnStop()				//ֹͣ���к��������
{
	if(m_pmodcom != NULL)
	{
		delete m_pmodcom;
		m_pmodcom = NULL;
	}
}

void	CModDrv::Send05(CModDec *pdec,WORD regaddr,int nVal)//����05����
{
	WORD wVal;
	if(nVal == 0)
		wVal = 0x0000;
	else
		wVal = 0xFF00;
	BYTE pbuf[256];
	
	pbuf[0] = pdec->m_ucaddr;//�豸��ַ
	pbuf[1] = 5;//����
	
	WORD wt = regaddr;
	SetWord(&wt,pbuf+2,21);//��д�Ĵ�����ַ

	SetWord(&wVal,pbuf+4,pdec->m_nwordseq);//��д����

	WORD crc = CRC_16(pbuf,6);
	SetWord(&crc,pbuf+6,pdec->m_ncrc16seq);//��дCRC16
	
	m_randfrm.AddFrm(pbuf,8);
}

void	CModDrv::Send06(CModDec *pdec,WORD regaddr,WORD wVal)//����06����
{
	BYTE pbuf[256];
	
	pbuf[0] = pdec->m_ucaddr;//�豸��ַ
	pbuf[1] = 6;//����
	
	WORD wt = regaddr;
	SetWord(&wt,pbuf+2,21);//��д�Ĵ�����ַ

	wt = wVal;
	SetWord(&wt,pbuf+4,pdec->m_nwordseq);//��д����

	WORD crc = CRC_16(pbuf,6);
	SetWord(&crc,pbuf+6,pdec->m_ncrc16seq);//��дCRC16

	m_randfrm.AddFrm(pbuf,8);
}

void	CModDrv::Send16(CModDec *pdec,WORD regaddr,DWORD dwVal)//����16����
{
	BYTE pbuf[256];
	
	pbuf[0] = pdec->m_ucaddr;//�豸��ַ
	pbuf[1] = 16;//����
	
	WORD wt = regaddr;
	SetWord(&wt,pbuf+2,21);//��д�Ĵ�����ʼ��ַ ,2,3

	wt = 2;
	SetWord(&wt,pbuf+4,21);//��д�Ĵ�������,4,5

	pbuf[6] = 4;//��д�ֽ���,6

	DWORD dwt = dwVal;

	SetDWord(&dwt,pbuf+7,pdec->m_ndwordseq);//��д���� ,7,8,9,10,

	WORD crc = CRC_16(pbuf,11);
	SetWord(&crc,pbuf+ 11,pdec->m_ncrc16seq);//��дCRC16,11,12

	m_randfrm.AddFrm(pbuf,13);
}
void	CModDrv::Send16(CModDec *pdec,WORD regaddr,float fVal)//����16����
{
	BYTE pbuf[256];
	
	pbuf[0] = pdec->m_ucaddr;//�豸��ַ
	pbuf[1] = 16;//����
	
	WORD wt = regaddr;
	SetWord(&wt,pbuf+2,21);//��д�Ĵ�����ʼ��ַ ,2,3

	wt = 2;
	SetWord(&wt,pbuf+4,21);//��д�Ĵ�������,4,5

	pbuf[6] = 4;//��д�ֽ���,6

	float ft = fVal;

	SetFloat(&ft,pbuf+7,pdec->m_nfloatseq);//��д����,7,8,9,10

	WORD crc = CRC_16(pbuf,11);
	SetWord(&crc,pbuf+ 11,pdec->m_ncrc16seq);//��дCRC16 ,11,12

	m_randfrm.AddFrm(pbuf,13);
}

void	CModDrv::Send16(CModDec *pdec,WORD regaddr,BYTE *pb,int nsize)//����16����
{
	BYTE pbuf[256];
	
	pbuf[0] = pdec->m_ucaddr;//�豸��ַ
	pbuf[1] = 16;//����
	
	WORD wt = regaddr;
	SetWord(&wt,pbuf+2,21);//��д�Ĵ�����ʼ��ַ ,2,3

	wt = nsize/2;
	SetWord(&wt,pbuf+4,21);//��д�Ĵ�������,4,5

	pbuf[6] = (BYTE)nsize;//��д�ֽ���,6

	for(int i=0;i<nsize;i++)
		pbuf[7 + i] = pb[i];//��д����
	
	WORD crc = CRC_16(pbuf,7 + nsize);
	SetWord(&crc,pbuf+ 7 + nsize,pdec->m_ncrc16seq);//��дCRC16

	m_randfrm.AddFrm(pbuf,9 + nsize);
}

void CModDrv::TransCfg()//�������ļ�ת��Ϊ���ж���
{
	//ɾ��map�����
	CString szid;
	CModItemIndex *pmi;
	POSITION pos = m_mapitem.GetStartPosition();
	while(pos!=NULL)
	{
		m_mapitem.GetNextAssoc( pos, szid, pmi );
		delete pmi;
	}
	m_mapitem.RemoveAll();//���MAP
	DeleteAllDec();//ɾ������MODDEC����
	int i;

	//���ؽ�
	int nitems = 0;
	CModDec *pdec;
	CModDec *pcfgdec;
	int ndecs = m_cfg.GetSize();
	for(i=0;i<ndecs;i++)
	{
		pcfgdec = m_cfg.GetAt(i);
		if(pcfgdec == NULL)
			continue;
		pdec = new CModDec;
		if(pdec == NULL)
			continue;
		//�豸����
		pdec->m_szname = pcfgdec->m_szname;
		pdec->m_ucaddr = pcfgdec->m_ucaddr;
		pdec->m_ncrc16seq = pcfgdec->m_ncrc16seq;
		pdec->m_ndwordseq = pcfgdec->m_ndwordseq;
		pdec->m_nfloatseq = pcfgdec->m_nfloatseq;
		
		//������
		int j,n;
		CModItem *pi;
		CModItem	mi;
		n = pcfgdec->GetItems();
		for(j=0;j<n;j++)
		{
			pi = pcfgdec->GetAt(j);
			if(pi == NULL)
				continue;
			mi.m_sname = pi->m_sname;
			mi.m_ucorder = pi->m_ucorder;
			mi.m_uctype = pi->m_uctype;
			mi.m_waddr = pi->m_waddr;
			pdec->Add(&mi);
			nitems++;
		}
		Adddec(pdec);
	}

	if(nitems >0)//�ؽ�MAP
	{
		m_mapitem.InitHashTable( ::CalcHashTableSize(nitems) );
	}
	
	int j,n; 
	CModItem	*pi;
	for(i=0;i<m_ndecsize;i++)
	{
		if(m_pdec[i] == NULL)
			continue;
		pdec = m_pdec[i];
		n = pdec->GetItems();
		for(j=0;j<n;j++)
		{
			pi = pdec->GetAt(j);
			if(pi == NULL)
				continue;
			szid = pdec->m_szname + "." + pi->m_sname;
			pmi = new CModItemIndex(pdec,pi);
			if(pmi != NULL)
				m_mapitem.SetAt(szid,pmi);
		}
	}
}

DRVRET  CModDrv::GetTagDef(int nindex,LPDRVTAG  lptag)
{
	CString szID;
	CModDec	*pd;
	CModItem *pi;
	int ni=0,n,i,j,ndec = m_cfg.GetSize();
	for(i=0;i<ndec;i++)
	{
		pd  = m_cfg.GetAt(i);
		if(pd == NULL)
			continue;
		n = pd->GetItems();
		for(j=0;j<n;j++)
		{
			pi = pd->GetAt(j);
			if(pi == NULL)
				continue;
			if(ni < nindex)
			{
				ni++;
				continue;
			}
			else if(ni > nindex)
				return DRERR_NOMORETAG;
			
			

			if(pi->m_ucorder <5)
				lptag->waccess = 0;//����ֻ��
			else //5, 6,16
				lptag->waccess = 2;//����ֻд

			if(pi->m_uctype != CModItem::FLOATV) 
			{
				if((pi->m_ucorder < 3)||(pi->m_ucorder == 5))
					lptag->wtype = DRT_DIGITAL;
				else
					lptag->wtype = DRT_INT32;
			}
			else
				lptag->wtype = DRT_FLOAT32;
			strncpy(lptag->sname,pd->m_szname + "." +pi->m_sname,63);
			strncpy(lptag->sdes,pi->m_szdes,79);
			strncpy(lptag->sunit,pi->m_szunit,15);

			return DRERR_OK;
		}
	}
	return DRERR_NOMORETAG;
}

DRVRET  CModDrv::WriteVar(LPDRREC lpval)
{
	return DRERR_OK;
	/* �ݲ�֧����д
	//���ҵ���������
	CString szid = lpval->sname;
	CModItemIndex *pi;
	
	if(!m_mapitem.Lookup(szid,pi))
	{
		TRACE("Err: No WriteVar VAR! \n");
		return FALSE;
	}
	
	if((pi->m_pitem == NULL)||(pi->m_pdec == NULL))
		return FALSE;
	if(pi->m_pitem->m_ucorder == 5)
	{
		CString szVal = EcdApi::EcdItemVal2StrDec(lpVal);  // ������ת��Ϊ�ַ���10����
		Send05(pi->m_pdec,pi->m_pitem->m_waddr,atoi(szVal));//����
	}
	else if(pi->m_pitem->m_ucorder == 6)
	{
		if(pi->m_pitem->GetCVLen() != 2)
			return FALSE;//���ǵ��Ĵ���
		CString szVal = EcdApi::EcdItemVal2StrDec(lpVal);  // ������ת��Ϊ�ַ���10����
		Send06(pi->m_pdec,pi->m_pitem->m_waddr,WORD(atoi(szVal)));//����
	}
	else if(pi->m_pitem->m_ucorder == 16) //д��Ĵ���
	{
		if(pi->m_pitem->GetCVLen() != 4)
			return FALSE;//����˫�Ĵ���
	
		DWORD dwt;
		float ft;
		CString szVal = EcdApi::EcdItemVal2StrDec(lpVal);  // ������ת��Ϊ�ַ���10����

		if(pi->m_pitem->m_uctype != CModItem::FLOATV) //4�ֽ�����
		{
			dwt = (DWORD)atol(szVal);
			Send16(pi->m_pdec,pi->m_pitem->m_waddr,dwt);//����
		}
		else		//������
		{
			ft = (float)atof(szVal);
			Send16(pi->m_pdec,pi->m_pitem->m_waddr,ft);//����
		}
	}
	return TRUE;
	*/
}
/*
BOOL CModDrv::WriteVar(LPCTSTR lpszid,LPECDITEMVAL	lpVal)//д����,��MAP���ң���Ч��
{
	//���ҵ���������
	CString szid = lpszid;
	CModItemIndex *pi;
	
	if(!m_mapitem.Lookup(szid,pi))
	{
		TRACE("Err: No WriteVar VAR! \n");
		return FALSE;
	}
	
	if((pi->m_pitem == NULL)||(pi->m_pdec == NULL))
		return FALSE;
	if(pi->m_pitem->m_ucorder == 5)
	{
		CString szVal = EcdApi::EcdItemVal2StrDec(lpVal);  // ������ת��Ϊ�ַ���10����
		Send05(pi->m_pdec,pi->m_pitem->m_waddr,atoi(szVal));//����
	}
	else if(pi->m_pitem->m_ucorder == 6)
	{
		if(pi->m_pitem->GetCVLen() != 2)
			return FALSE;//���ǵ��Ĵ���
		CString szVal = EcdApi::EcdItemVal2StrDec(lpVal);  // ������ת��Ϊ�ַ���10����
		Send06(pi->m_pdec,pi->m_pitem->m_waddr,WORD(atoi(szVal)));//����
	}
	else if(pi->m_pitem->m_ucorder == 16) //д��Ĵ���
	{
		if(pi->m_pitem->GetCVLen() != 4)
			return FALSE;//����˫�Ĵ���
	
		DWORD dwt;
		float ft;
		CString szVal = EcdApi::EcdItemVal2StrDec(lpVal);  // ������ת��Ϊ�ַ���10����

		if(pi->m_pitem->m_uctype != CModItem::FLOATV) //4�ֽ�����
		{
			dwt = (DWORD)atol(szVal);
			Send16(pi->m_pdec,pi->m_pitem->m_waddr,dwt);//����
		}
		else		//������
		{
			ft = (float)atof(szVal);
			Send16(pi->m_pdec,pi->m_pitem->m_waddr,ft);//����
		}
	}
	return TRUE;
}
*/

void	CModDrv::DoReadFrm(BYTE ucod,WORD wregaddr,CModDec *pdec,BYTE *pb,int nsize)//������յ������ݰ������ݰ�����֤
{
	WORD wAddr = wregaddr;

	BYTE ucorder = pb[1];//����

	if((pdec->m_ucaddr != pb[0])||(pb[1] != ucod)) // ��Ҫ����������,mydebug 20090713
		return;//���Ǳ��豸��ͨ�Ű�

	//BYTE val[8];
	CModItem *pi;
	int i,nIndex = -1,ndatas = pdec->GetItems();
	for(i=0;i<ndatas;i++)
	{
		pi = pdec->GetAt(i);
		if(	(pi->m_ucorder == ucorder)&&
			(pi->m_waddr == wAddr))
		{
			nIndex = i;
			break;
		}
	}
	if(nIndex == -1)
		return ;//û�ҵ���ʼ��

	BYTE* pdata = pb+3;		//���ݿ�ʼλ��
	int ndatabytes = pb[2];	//�����ֽ���

	long ltime = (long)::time(NULL);
	CRdRec rv;
	CString szvarid;
	if((ucorder == 1)||(ucorder == 2))//��������
	{
		WORD wa = wAddr;
		int j,k;
		i = nIndex;
		for(j=0;j<ndatabytes;j++)//�ֽ�ѭ��
		{
			for(k=0;k<8;k++)//λѭ��
			{
				if(i >= ndatas)
					break;
				pi =  pdec->GetAt(i);
			
				if( pi->m_ucorder != ucorder)
					break;//���ͬ
				if(pi->m_waddr != wAddr + j * 8 + k)
					break;//��ַ����
				
				szvarid = pdec->m_szname + "." + pi->m_sname;
				rv.SetName(szvarid);
				rv.ReSetType(DRT_DIGITAL);

				rv.val.snqa = 0;
				rv.val.ltime = ltime;
				if((pdata[j] & (0x01 << k)) != 0)
					rv.val.lval = 1;
				else
					rv.val.lval = 0;
				if(m_cbOnDataOk)
					m_cbOnDataOk(m_handle,&rv,1);
				i++;//��һ������
			}
		}
	}
	else if((ucorder == 3)||(ucorder == 4))//��ģ����
	{
		int n=0;
		int ndx=0;				//��ַ����
		i = nIndex;
		while(( i < ndatas)&&(n<nsize)&& (n<ndatabytes))
		{
			pi =  pdec->GetAt(i);
			
			if(pi->m_ucorder != ucorder)
				break;

			ndx = pi->GetCVLen();

			if(n+ndx > nsize-3)
				break;
			
			if(ndx == 2)//˫�ֽ�
			{
				szvarid = pdec->m_szname + "." + pi->m_sname;
				rv.SetName(szvarid);
				rv.ReSetType(DRT_INT32);

				rv.val.snqa = 0;
				rv.val.ltime = ltime;	

				//��д����
				if (pi->m_uctype == CModItem::SHORTV)
				{
					rv.val.lval = (long)GetShort(pdata + n,pdec->m_nwordseq);
				}
				else {
					rv.val.lval = (long)GetWord(pdata + n,pdec->m_nwordseq);
				}

				if(m_cbOnDataOk)
					m_cbOnDataOk(m_handle,&rv,1);
			}
			else 
			{
				if(pi->m_uctype == CModItem::LONGV)//����
				{
					//��д����
					szvarid = pdec->m_szname + "." + pi->m_sname;
					rv.SetName(szvarid);
					rv.ReSetType(DRT_INT32);
					rv.val.snqa = 0;
					rv.val.ltime = ltime;	
					rv.val.lval = GetLong(pdata + n,pdec->m_ndwordseq);
					if(m_cbOnDataOk)
						m_cbOnDataOk(m_handle,&rv,1);
				}
				else if(pi->m_uctype != CModItem::FLOATV)//����
				{
					//��д����
					szvarid = pdec->m_szname + "." + pi->m_sname;
					rv.SetName(szvarid);
					rv.ReSetType(DRT_INT32);
					rv.val.snqa = 0;
					rv.val.ltime = ltime;	
					rv.val.lval = (long)GetDWord(pdata + n,pdec->m_ndwordseq);
					if(m_cbOnDataOk)
						m_cbOnDataOk(m_handle,&rv,1);
				}
				else	// float
				{
					//��д����
					szvarid = pdec->m_szname + "." + pi->m_sname;
					rv.SetName(szvarid);
					rv.ReSetType(DRT_FLOAT32);
					rv.val.snqa = 0;
					rv.val.ltime = ltime;	
					rv.val.fval = GetFloat(pdata + n,pdec->m_nfloatseq);
					if(m_cbOnDataOk)
						m_cbOnDataOk(m_handle,&rv,1);
				}
			}
			i++;
			n += ndx;
		}
	}
	
	//��������Ҫ����
}


