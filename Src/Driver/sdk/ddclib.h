#ifndef __TPRI_DDCLIB_H__
#define __TPRI_DDCLIB_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxsock.h>		// MFC socket extensions
#include <afxmt.h>
#include <afxtempl.h> //ģ��֧��

#include <drvdef.h>
#include <drvsdk.h>
#include <sdktool.h>
#include <SortObArray.h>
#include <WinList.h>
#include <TagItem.h>

#ifndef _DDC_LIB
#ifdef _DEBUG
#pragma comment( lib, "ddclib_debug.lib")
#else
#pragma comment( lib, "ddclib.lib")
#endif

//#pragma comment( lib, "ddclib.res")
#endif
//#include <afxdao.h>

// ͨѶЭ�鶨��
#define COM_TYPE_COM		0x01	// ���п�
#define COM_TYPE_UDP		0x02	// UDP ClientЭ��
#define COM_TYPE_UDPSERV	0x04	// UDP ServerЭ��
#define COM_TYPE_TCP		0x08	// TCP ClientЭ��
#define COM_TYPE_TCPSERV	0x10	// TCP ServerЭ��
#define COM_TYPE_DB		    0x20	// ODBC DB
#define COM_TYPE_FILE		0x40	// �ļ�����
///////////////////////////////////////////////////////////////////

#define BUF_LENGTH_SERIAL	4096
#define BUF_LENGTH_TCP		8192
#define BUF_LENGTH_UDP		65536

#define UPDATEVALTAGS		2000	// ÿ�������±�ǩ��

// ����ϵͳ�����룬��ø�ʽ���Ĵ�����Ϣ��dwErrorCodeһ����GetLastError()�������
CString GetSystemErrorMessage(DWORD dwErrorCode);

class CDDCDrvObj;

class CConfigInfo
{
public:
	friend CDDCDrvObj;
public:
	CConfigInfo();
	virtual ~CConfigInfo();
	void Serialize(CArchive& ar, DWORD dwVersion);
	
public:	
	BOOL IsModified() { return m_bModified; }

	CConfigInfo& operator=(const CConfigInfo& config);
	BOOL operator==( const CConfigInfo& config);
	BOOL operator!=( const CConfigInfo& config);
	CString GetConnectString();

	void CalcTimeOffset() {
		m_nTimeOffset = m_nTimeBias * 3600;
	}
public:
	static CString m_szHelpFile;	// �������ð����ļ����ƣ�����·���������ļ���������̬�����һ�����Ϊ res://1234 ��ʽ����ʾ����Դ�м�����ԴIDΪ1234��TEXT��Դ��ʾ���û�
	static CString m_szParamDisplayName; // ���Ӳ�����ʾ��
	static CString m_szParamHintText;	// ���Ӳ�������ʾ�ı�

	static BOOL	m_bSelectFileName;		// �����ļ�·�������ļ�������
	static CString m_szSelectFileFilter;		// ���ļ�������
	static CString m_szSelectFileHint;	// ��Ŀ¼��ʾ�ı�

	BOOL		m_bModified;
	
	WORD		m_wComType;			// ͨѶЭ������

	// TCP/UDPͨѶ����
	CString		m_szIP;				// TCP/UDP������IP��ַ�������COM_TYPE_TCPSERV��ʽ��������
	WORD	    m_wPort;			// TCP/UDP����˿�
	CString		m_szTcpUdpParam;	// TCP/UDP���Ӳ��������������ͨѶЭ�飬һ�㲻��
	
	// ���п�ͨѶ����
	CString		m_szComPort;		// �˿�������"\\\\.\\com1","\\\\.\\com2"
	DWORD		m_dwBaudRate;		// ������,CBR_1200,CBR_2400,CBR_4800,CBR_9600,
									// CBR_14400,CBR_19200,CBR_38400,CBR_57600,CBR_115200,
									// CBR_14400,CBR_19200,CBR_38400,CBR_57600,CBR_115200,
	BYTE		m_cByteSize;		// ����λ number of bits/byte, 4-8
	BYTE		m_cParity;			// 0-4��Ӧno,odd,even,mark,space 
	BYTE		m_cStopBits;		// 0,1,2 ��Ӧ 1, 1.5, 2
	CString		m_szComParam;		// ���п����Ӳ��������������ͨѶЭ�飬һ�㲻��
   
	//odbc����
	CString		m_szID;
	CString		m_szPsw;
	CString		m_szDataSource;

	// File��ʽ����
	CString		m_szFileName;		// �ļ���
	BOOL		m_bAutoDeleteFile;	// �Ƿ��ڶ�ȡ���ݺ��Զ�ɾ���ļ�����
	CString		m_szFileParam;		// ����չ�ļ�����

	// �������ò���
	int			m_nReadTimeout;		//����ʱ
	int			m_nWriteTimeout;	//����ʱ
	int			m_nUpdateSec;       //�ɼ����ڣ�����
	int			m_nTimeBias;		//ʱ��ƫ����
	BOOL		m_bDebug;           //������Ϣ��¼
	BOOL		m_bLocalTime;		//�Ƿ�ʹ�ñ���ʱ�꣬�������Э���в�����ʱ�꣬����Ը�����

protected:	
	long		m_nTimeOffset;
	CDDCDrvObj *m_pdrv;
};


/*
CRC32���㺯��
szData:����
dwSize:�����ֽ���
*/
//DWORD	TL_GetCRC32(unsigned char *szData, unsigned long dwSize);	// ����CRC32

class CCheckAlgorithm
{
public:
	CCheckAlgorithm(){};
	~CCheckAlgorithm(){};	
public:
	/*
	CRC32���㺯��
	szData:����
	dwSize:�����ֽ���
	*/
	static DWORD TL_GetCRC32(unsigned char *szData, unsigned long dwSize);	// ����CRC32

	// �����������
	static void  Init_CRC32_Table();

	static DWORD Reflect(unsigned long ref, char ch);
//CRC16
private:
	static unsigned long crc32_table[256]; 
	static BOOL		b_crc32IninTab;
};
/*------------------------------------------------------------------------------------
																	
�ֽڶ�̬����CMyByteArray�����ڱ������ݱ��Ļ����������
																	  
*/
#define MAX_BYTEARRAYSIZE	(1024 * 2048)	// ����ֽڶ�̬�����С2M

class CMyByteArray //�ֽڶ�̬���飬���ڱ������ݱ��Ļ����������
{
public:
	CMyByteArray();
	~CMyByteArray();
public:
	BOOL PutData(BYTE *pBuf,int nSize);
	
	BOOL PutStr(LPCTSTR lpszData,int nSize);
	
	BYTE *GetBuf();
	int  GetDataSize();
	void ClearData();
	void SetGrown(int ngrown);
	void LeftMove(int n);
	void CopyTo(CMyByteArray *pba); //���Ƶ���һ���ֽ�����β��.
protected:
	BYTE *m_pData;		//���ݶ�
	int  m_nSize;		//���ݴ�С
	int  m_nBufSize;	//�ռ��С
	int	 m_nGrown;		//��������
};

/*------------------------------------------------------------------------------------

  ���Ľ��� CFrmParse,ÿ���ͻ����Ӻ�ʹ���һ���ö���
  
*/

// ����ְ�����״̬
#define FRAME_STATUS_NO_HEAD			0x00	// û�ҵ���ͷ��־
#define FRAME_STATUS_HEAD_NO_ENOUGH		0x01	// �ҵ���ͷ��־,���Ȳ����޷���֤��ͷ
#define FRAME_STATUS_DATA_NO_ENOUGH		0x02	// ��ͷУ�����,���ݳ��Ȳ���
#define FRAME_STATUS_OK					0x03	// �������

class CFrmParse //���Ľ���,ֻ�Ƿְ�
{
public:
	CFrmParse();
	virtual ~CFrmParse();

public:
	CMyByteArray	m_Frm;

	int		GetFrmLen() { return m_nFrmLen; }
	void*	GetBuffer() { return m_buf.GetBuf(); }

	void	Clear() { m_buf.ClearData(); }

	int		PutData(BYTE *buf,int nsize);//����δ��������,nsize < 256K
	int		GetStatus(){return m_nStatus;};
	void	SetStatus(int nst){m_nStatus = nst;};
	BOOL	CheckCrc32(BYTE *pd);//����16�ֽڱ�ͷУ��
	BYTE*	FindFlag(BYTE *pDataBuffer, int nDataSize, BYTE *pFlag, int nFlagSize);

public:
	virtual int	Parse();//�ְ�,����m_nStatus

protected:
	int				m_nFrmLen;		//���ĳ���
	int				m_nStatus;		//0:�ޣ�1����ͷ��ʼ��־�ҵ���2����ͷ��ɣ�3���������
	CMyByteArray	m_buf;			//�������ֽ�	
};

/*
��ȡ��������ʱ��
*/
class CTimeCount   
{   
public:
	CTimeCount(){};
	~CTimeCount(){};
	
	//��ʼ��ʱ
	void start() 
	{
		QueryPerformanceFrequency(&litmp);
		dfFreq = (double)litmp.QuadPart;//   ��ü�������ʱ��Ƶ��
		QueryPerformanceCounter(&litmp);
		QPart1 = litmp.QuadPart;//   ��ó�ʼֵ
	};

	//ֹͣ��ʱ 
	void end()   
	{
		QueryPerformanceCounter(&litmp);
		QPart2	= litmp.QuadPart;//�����ֵֹ
		dfMinus	= (double)(QPart2-QPart1);
		dfTim	= dfMinus / dfFreq;//   ��ö�Ӧ��ʱ��ֵ����λΪ�� 
	}; 
	
	//�Ժ��뾫���������ʱ�� 
	double time_milli() 
	{
		return dfTim * 1000.0;
	};
	   
private:   
	LARGE_INTEGER   litmp;     
    LONGLONG   QPart1,QPart2;   
    double   dfMinus,   dfFreq,   dfTim;     
}; 
  
//����
class CSafeLock  
{
public:
	CSafeLock(CCriticalSection *pLock){
		m_pLock = pLock;
		m_pLock->Lock();
	};
	virtual ~CSafeLock() {
		m_pLock->Unlock();
	};
private:
	CCriticalSection *m_pLock;
};

class CDDCDrvObj;

class CDDCDrvProtocol
{
public:
	friend CDDCDrvObj;
	CDDCDrvProtocol();
	virtual ~CDDCDrvProtocol();

	CDDCDrvObj *GetDrvObj() {
		return m_pDrvObj;
	}
	BOOL GetBlockingMode() {
		return m_bBlockingMode;
	}
public:
	void SetConfig(CConfigInfo &config) { m_config = config; }
	CConfigInfo &GetConfig() { return m_config; }
	
	void SetAutoUpdateSec(BOOL bUpdateSec = TRUE) { m_bAutoUpdateSec = bUpdateSec; }
	BOOL GetAutoUpdateSec() { return m_bAutoUpdateSec; }
	void WriteDebugLog(BOOL nReadMode, BYTE *pb, int nsize);

public: //���麯��
	//���涨��Ĵ��麯�����������غ������
	virtual BOOL	IsConnect()						= 0;//�˿��Ƿ��
	virtual BOOL	Connect()						= 0;
	virtual void	Disconnect()					= 0;
	virtual int		SendData(BYTE *pb, int nsize)	= 0;//����ͨ�Ű�,���ط��͵��ֽ���	
	virtual int		ReadData(BYTE *pb, int nsize)   = 0;//����ͨ�Ű�,���ؽ��յ��ֽ���

	virtual DRVRET  WriteVar(LPDRREC lpval)			= 0;
	virtual void	DoRun()							= 0;

protected:
	BOOL				m_bAutoUpdateSec;
	CConfigInfo			m_config;
	CDDCDrvObj			*m_pDrvObj;
	BOOL				m_bBlockingMode;
};

//���п�ͨ�ŷ�ʽ
class CSerialProtocol : public CDDCDrvProtocol
{
public:
	CSerialProtocol();
	virtual ~CSerialProtocol();

protected:
	HANDLE  m_hComDev;				//ͨ�Ŷ˿ھ��
	int		m_nerrnum;				//�������,���ڼ�⴮�ڴ����Ը�λ��
	
public:
	virtual BOOL	IsConnect();
	virtual BOOL	Connect();
	virtual void	Disconnect();
	virtual int		SendData(BYTE *pb, int nsize);
	virtual int		ReadData(BYTE *pb, int nsize);
	virtual DRVRET  WriteVar(LPDRREC lpval);
};

//UDPͨ�ŷ�ʽ����
class CUDPProtocolBase : public CDDCDrvProtocol 
{
public:
	CUDPProtocolBase();
	virtual ~CUDPProtocolBase();
	
protected:
	SOCKET	m_sClient;	
	struct sockaddr m_sockaddr;
	
	int	Read(BYTE *pbuf,int nsize,int ndelaymsec);
	int	Write(BYTE *pbuf,int nsize,int ndelaymsec);
	BOOL SetRcvBuf(int nSize);
	BOOL GetRcvBuf(int &nSize);
public:
	virtual BOOL	IsConnect();
	virtual void	Disconnect();
	virtual int		SendData(BYTE *pb, int nsize);
	virtual int		ReadData(BYTE *pb, int nsize);
	virtual DRVRET  WriteVar(LPDRREC lpval);
};

//UDP Clientͨ�ŷ�ʽ
class CUDPClientProtocol : public CUDPProtocolBase 
{
public:
	virtual BOOL	Connect();
};

//UDP Serverͨ�ŷ�ʽ
class CUDPServerProtocol : public CUDPProtocolBase 
{
public:
	CUDPServerProtocol();
	virtual BOOL	Connect();
};

/*
	CUDPProtocolBase����select()����ʵ��recvfrom()���������ĳ�ʱ���ܣ���select()����
	��ĳЩ����»������ѯ�������ȷ�����⣬��дUDPͨ�ŷ�ʽ����CUDPProtocolBaseEx��ֱ
	��ʹ������recvfrom()����ʹ��select()������ʱ
 */
class CUDPProtocolBaseEx : public CDDCDrvProtocol 
{
public:
	CUDPProtocolBaseEx();
	virtual ~CUDPProtocolBaseEx();

protected:
	SOCKET	m_sClient;	
	struct sockaddr m_sockaddr;

	int	Read(BYTE *pbuf,int nsize,int ndelaymsec);
	int	Write(BYTE *pbuf,int nsize,int ndelaymsec);
	BOOL SetRcvBuf(int nSize);
	BOOL GetRcvBuf(int &nSize);
public:
	virtual BOOL	IsConnect();
	virtual void	Disconnect();
	virtual int		SendData(BYTE *pb, int nsize);
	virtual int		ReadData(BYTE *pb, int nsize);
	virtual DRVRET  WriteVar(LPDRREC lpval);
};

//UDP Clientͨ�ŷ�ʽ
class CUDPClientProtocolEx : public CUDPProtocolBaseEx 
{
public:
	virtual BOOL	Connect();
};

//UDP Serverͨ�ŷ�ʽ
class CUDPServerProtocolEx : public CUDPProtocolBaseEx 
{
public:
	CUDPServerProtocolEx();
	virtual BOOL	Connect();
};

//TCP Clientͨ�ŷ�ʽ

class CTCPClientProtocol : public CDDCDrvProtocol 
{
public:
	CTCPClientProtocol();
	virtual ~CTCPClientProtocol();

protected:
	SOCKET	m_sClient;
		
	int		Read(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec);
	int		Write(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec);

public:
	//{{�������صĺ���
	virtual BOOL	IsConnect();
	virtual BOOL	Connect();
	virtual void	Disconnect();
	virtual int		SendData(BYTE *pb, int nsize);
	virtual int		ReadData(BYTE *pb, int nsize);
	virtual DRVRET  WriteVar(LPDRREC lpval);
	//}}
};

// ������ʽ��TCP Clientͨ�ŷ�ʽ
class CTCPClientProtocolEx : public CDDCDrvProtocol 
{
public:
	CTCPClientProtocolEx();
	virtual ~CTCPClientProtocolEx();

protected:
	SOCKET	m_sClient;

	int		Read(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec);
	int		Write(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec);

public:
	//{{�������صĺ���
	virtual BOOL	IsConnect();
	virtual BOOL	Connect();
	virtual void	Disconnect();
	virtual int		SendData(BYTE *pb, int nsize);
	virtual int		ReadData(BYTE *pb, int nsize);
	virtual DRVRET  WriteVar(LPDRREC lpval);
	//}}
};

//TCP SERVERͨ�ŷ�ʽ
class CTCPServerProtocol : public CDDCDrvProtocol 
{
public:
	CTCPServerProtocol();
	virtual ~CTCPServerProtocol();

public:
	//{{�������صĺ���
	virtual BOOL	IsConnect();
	virtual BOOL	Connect();
	virtual void	Disconnect();
	virtual int		SendData(BYTE *pb, int nsize);
	virtual int		ReadData(BYTE *pb, int nsize);	
	virtual DRVRET  WriteVar(LPDRREC lpval);
	//}}

protected:
	BOOL	IsAccept();
	BOOL	ListenClient();
	void	DisconnectClient();
	int		Read(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec);
	int		Write(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec);
	
protected:
	SOCKET				m_hSocket;				//=INVALID_SOCKET;
	struct	sockaddr_in m_HostAddr;				//������������ַ
	
	BOOL				m_bRun;					//���б�־
	
	CCriticalSection	m_Lock;					//��
	SOCKET				m_hConnect;             //���ӵ�SOCKET
	struct sockaddr_in	m_addrClient;           //�ͻ���ַ
	int					m_nConn;				//��ǰ�����Ӹ���	
};

// ������ʽ��TCP SERVERͨ�ŷ�ʽ
class CTCPServerProtocolEx : public CDDCDrvProtocol 
{
public:
	CTCPServerProtocolEx();
	virtual ~CTCPServerProtocolEx();

public:
	//{{�������صĺ���
	virtual BOOL	IsConnect();
	virtual BOOL	Connect();
	virtual void	Disconnect();
	virtual int		SendData(BYTE *pb, int nsize);
	virtual int		ReadData(BYTE *pb, int nsize);	
	virtual DRVRET  WriteVar(LPDRREC lpval);
	//}}

protected:
	BOOL	IsAccept();
	BOOL	ListenClient();
	void	DisconnectClient();
	int		Read(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec);
	int		Write(SOCKET s, BYTE *pbuf, int nsize, int ndelaymsec);

protected:
	SOCKET				m_hSocket;				//=INVALID_SOCKET;
	struct	sockaddr_in m_HostAddr;				//������������ַ

	BOOL				m_bRun;					//���б�־

	CCriticalSection	m_Lock;					//��
	SOCKET				m_hConnect;             //���ӵ�SOCKET
	struct sockaddr_in	m_addrClient;           //�ͻ���ַ
	int					m_nConn;				//��ǰ�����Ӹ���	
};

class CDataBaseProtocol: public CDDCDrvProtocol 
{
public:
	CDataBaseProtocol(){};
	virtual ~CDataBaseProtocol(){};
public:
	virtual BOOL IsConnect() {return FALSE;};
	virtual BOOL IsNeedReConnect(const CString& szStatus)
	{
		//ȡindexΪ6-10
		CString szval = szStatus.Left(11);
		szval = szval.Right(5);
		
		// 	08001	�޷����ӵ�����Դ,һ����Open DBʱ�Ĵ�����Ϣ
		// 	08003	����δ��
		// 	08007	��ִ������Ĺ���������ʧ��
		// 	08S01	ͨѶ����ʧ�ܣ�һ���ڴ򿪼�¼�������ֱ��ִ��SQL���ʱ����
		
		if((szval == "08S01")||
			(szval == "08001")||
			(szval == "08003")||
			(szval == "08007")
			)
			return TRUE;
		return FALSE;
	};
	virtual BOOL DBConnect() {return FALSE;};
	virtual BOOL Connect() {return TRUE;};
	virtual void Disconnect() {};
	virtual int	SendData(BYTE *pb, int nsize) {return 0;};
	virtual int	ReadData(BYTE *pb, int nsize) {return 0;};
	virtual DRVRET  WriteVar(LPDRREC lpval) {
		return DRERR_NOTSUPPORT;
	}
};

//UDPͨ�ŷ�ʽ����
class CFileProtocol : public CDDCDrvProtocol 
{
public:
	CFileProtocol() {};
	virtual ~CFileProtocol() {};

public:
	virtual BOOL	IsConnect() {
		return TRUE;
	};
	virtual BOOL	Connect() {
		return TRUE;
	};
	virtual void	Disconnect() {
	};
	virtual int		SendData(BYTE *pb, int nsize) {
		return 0;
	};
	virtual int		ReadData(BYTE *pb, int nsize) {
		return 0;
	};
	virtual DRVRET  WriteVar(LPDRREC lpval) {
		return DRERR_NOTSUPPORT;
	};
};

class CDDCDrvObj : public CDrvObj  
{
public:
	CDDCDrvObj();
	virtual ~CDDCDrvObj();
		
public:
	CTagDefTab			m_tagtab;	//��ǩ��
	CConfigInfo			m_config;
	CTagDefTab::CTagItemMap	m_mapTagIDs;

	void EnabledTestConnect(BOOL bEnabled = TRUE) {	m_bCanTestConnect = bEnabled; }
	BOOL CanTestConnect() { return m_bCanTestConnect; }
	DWORD GetSerializeID() { return m_dwSerializeID; }
	WORD GetVersion() { return m_wVersion; }
	void SetSerializeID(DWORD dwSerializeID) { m_dwSerializeID = dwSerializeID; }
	WORD GetComTypes() { return m_wComTypes; }
	void SetComTypes(WORD wComTypes) { 
		if ((wComTypes & COM_TYPE_COM) == COM_TYPE_COM)
			m_config.m_wComType = COM_TYPE_COM;
		else if ((wComTypes & COM_TYPE_UDP) == COM_TYPE_UDP)
			m_config.m_wComType = COM_TYPE_UDP;
		else if ((wComTypes & COM_TYPE_UDPSERV) == COM_TYPE_UDPSERV)
			m_config.m_wComType = COM_TYPE_UDPSERV;
		else if ((wComTypes & COM_TYPE_TCP) == COM_TYPE_TCP)
			m_config.m_wComType = COM_TYPE_TCP;
		else if ((wComTypes & COM_TYPE_TCPSERV) == COM_TYPE_TCPSERV)
			m_config.m_wComType = COM_TYPE_TCPSERV;
		else if ((wComTypes & COM_TYPE_DB) == COM_TYPE_DB)
			m_config.m_wComType = COM_TYPE_DB;
		else if ((wComTypes & COM_TYPE_FILE) == COM_TYPE_FILE)
			m_config.m_wComType = COM_TYPE_FILE;
		m_wComTypes = wComTypes; 
	}
	BOOL CanSerialType() { return ((m_wComTypes & COM_TYPE_COM) == COM_TYPE_COM); }
	BOOL CanUdpType() { return ((m_wComTypes & COM_TYPE_UDP) == COM_TYPE_UDP); }
	BOOL CanUdpServerType() { return ((m_wComTypes & COM_TYPE_UDPSERV) == COM_TYPE_UDPSERV); }
	BOOL CanTcpType() { return ((m_wComTypes & COM_TYPE_TCP) == COM_TYPE_TCP); }
	BOOL CanTcpServerType() { return ((m_wComTypes & COM_TYPE_TCPSERV) == COM_TYPE_TCPSERV); }
	BOOL CanDBType() { return ((m_wComTypes & COM_TYPE_DB) == COM_TYPE_DB); }
	BOOL CanFileType() { return ((m_wComTypes & COM_TYPE_FILE) == COM_TYPE_FILE); }
	// ������Ϣ����
	BOOL IsDebug() { return m_config.m_bDebug; }
	BOOL IsLocalTime() { return m_config.m_bLocalTime; }
	int GetReadTimeout() { return m_config.m_nReadTimeout; }
	int GetWriteTimeout() { return m_config.m_nWriteTimeout; }
	int GetUpdateSec() { return m_config.m_nUpdateSec; }
	BOOL EnabledConnectionFailedStartup() { return m_bConnectionFailedStartup; }
	void SetConnectionFailedStartup(BOOL bConnectionFailedStartup) { m_bConnectionFailedStartup = bConnectionFailedStartup; }
	
	DWORD OnDataOk(	//���ݻص�����
		T_DRREC  recs[],		//��¼��
		int nrecs			//��¼����
		);
	/*
	DWORD OnLogOk( //��־�ص�����
		long ltime,			//ʱ��
		const char *plog			//��־�ַ�����0�������ַ���
		);*/
	
	DWORD OnSoeOk( //SOE�¼��ص�����,ÿ�λص�һ���¼�
		LPSOE psoe			//�¼�
		);

	void InitTagIdMap(BOOL bIgnoreCase = FALSE);	// ��ʼ����ǩID����MAP�����Э�鲻��ҪID��TagName��ӳ���ϵ������Բ��õ��øú���
	CTagItem *FindByID(LPCTSTR lpszID); // ���ݱ�ǩID���ƴ�Map���п��ٲ��ұ�ǩ
	CTagItem *FindByName(LPCTSTR lpszTagName) { return m_tagtab.GetTag(lpszTagName); } // ���ݱ�ǩ���ƴ�Map���п��ٲ��ұ�ǩ

	// ���ݱ�ǩ��ID�������һ���ص�ֵ
	BOOL AddValueByID(LPCTSTR lpszID, long ltime, short snqa, long val);
	BOOL AddValueByID(LPCTSTR lpszID, long ltime, short snqa, __int64 val);
	BOOL AddValueByID(LPCTSTR lpszID, long ltime, short snqa, float val);
	BOOL AddValueByID(LPCTSTR lpszID, long ltime, short snqa, double val);
	BOOL AddValueByID(LPCTSTR lpszID, long ltime, short snqa, LPCTSTR val);
	BOOL AddValueByID(LPCTSTR lpszID, long ltime, short snqa, void *val, int nsize);

	// ���ݱ�ǩ���������һ���ص�ֵ
	BOOL AddValueByName(LPCTSTR lpszTagName, long ltime, short snqa, long val);
	BOOL AddValueByName(LPCTSTR lpszTagName, long ltime, short snqa, __int64 val);
	BOOL AddValueByName(LPCTSTR lpszTagName, long ltime, short snqa, float val);
	BOOL AddValueByName(LPCTSTR lpszTagName, long ltime, short snqa, double val);
	BOOL AddValueByName(LPCTSTR lpszTagName, long ltime, short snqa, LPCTSTR val);
	BOOL AddValueByName(LPCTSTR lpszID, long ltime, short snqa, void *val, int nsize);

	BOOL FlushValue();	// ��ʣ���ǩֵ�ش���DDC

	DWORD WriteStreamMessageCB( const char* lpszTagName, DWORD dwCommandID, BYTE* pBuffer, int nSize );
	DWORD WriteHistoryCB( T_DRREC recs[], int nrecs );

	virtual BOOL TestConnect(CConfigInfo &config);
	
protected:
	BOOL				m_bCanTestConnect;
	DWORD				m_dwSerializeID;		// �������ñ�ʶ��
	WORD				m_wVersion;				// �����汾��
	WORD				m_wComTypes;			// ����Э������
	CDDCDrvProtocol		*m_pProtocol;			// ����Э�鴦����ʵ��ָ��
	CRdRec				m_recs[UPDATEVALTAGS];	// �ɼ�ֵ�ص�����
	int					m_nRecCount;			// �ɼ�ֵ����������ﵽUPDATEVALTAGS���Զ��ύ����������Ϊ0

	BOOL				m_bIgnoreCase;			// TagID�Ƿ���Դ�Сд��Ĭ��ΪFALSE
	BOOL				m_bConnectionFailedStartup;	// ����ʧ�ܺ��Ƿ�������������
	
	//�������ػ���ĺ���
public: //�麯��
	virtual DRVRET  GetErrMsg(CString& szMsg,DRVRET dwcode);			//��ȡ������Ϣ
	virtual DRVRET  WriteVar(LPDRREC lpval);							//д����
	virtual DRVRET  GetTagDef(int nindex,LPDRVTAG  lptag);
	
protected:
	virtual CDDCDrvProtocol	*CreateProtocol() = 0;	//�������õ�Э�����ʹ���Э�鴦�����	
	BOOL CheckUpdateSec(BOOL bForceUpdateSec = TRUE);
	long GetValueTimestmap(long ltime)	{			// ������ʱ������������ת��Ϊ���յĻص�ʱ��
		if ((ltime <= 0) || m_config.m_bLocalTime)
		{
			ltime = (long)::time(NULL);
		}
		
		ltime += m_config.m_nTimeOffset;
		return ltime;
	}

protected: //���麯��
	virtual void	OnInitConfig();				// ��ʼ��������Ϣ
	virtual BOOL	OnConfig();					// ���ú����ý���,����TRUE��ʾ�޸�������
	virtual void	OnBeginConfig();			// ��ʼ����֮ǰ���ã����ؿ��Լ�������ֵ�Ƿ���Ҫ���³�ʼ��
	virtual void	OnEndConfig();				// ������ɣ���׼������������Ϣ֮ǰ���ã����ؿ��Լ��������Ƿ���Ч�������Ч������ǿ������ΪĬ�ϵ���Чֵ
	virtual void	Serialize(CArchive& ar);	// ���л����棬��ȡ����
	virtual DRVRET	OnStart();					// ׼������
	virtual void	OnStop();					// ֹͣ���к��������
	virtual void	OnDisconnect();				// �Ͽ�����
	virtual UINT	DoRun();					// ��������й��̣���RunThread�е���

private:
	FILETIME		m_ftStartTime;				// ��ʼʱ��
	FILETIME		m_ftEndTime;				// ��ֹʱ��

	long GetElapsedTime() {	// ������뼶ʱ��
		ULONGLONG qst = *((ULONGLONG*)(&m_ftStartTime));
		ULONGLONG qet = *((ULONGLONG*)(&m_ftEndTime));
		return (long)((qet - qst) / 10000);
	}
};

#endif // end of __TPRI_DDCLIB_H__
