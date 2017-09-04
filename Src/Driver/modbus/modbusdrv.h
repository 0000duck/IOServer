//////////////////////////////////////////////////////////////
// modbusdrv.h
#pragma once

#include <afxtempl.h>
#include <afxmt.h>
#include <afxsock.h>		// MFC socket extensions

#include "../sdk/drvsdk.h"
#include "../sdk/sdktool.h"
//modbus����������
#define RESULT_OPENCOMPORTERR			1005	// ��COM��
#define RESULT_NOCOMTYPEERR				1006	// ��֧�ֵ�ͨ�ŷ�ʽ
#define RESULT_TCPCONNECTSERVERERR		1007	// ���ӷ���������

#include "..\sdk\DrvVersion.h"
#define DRVCFG_VERSION		(DRVCFG_VERSION_USER + 3)	// ��һ��

#define MODBUS_REGISTER_MAX	100			// �Ĵ������48��

// �Ƿ�����ʱ����
// ���ڸ��๫˾������糧��Modbus������һ��ʱ��󣬻�������ݴ��ҵ����⣬�Ͽ������ɽ��������
//#define _ENABLED_RECONNECT_TIMEOUT
#define _RECONNECT_TIMEOUT				(3600 * 1) // ��Сʱ

//------------------------------------------------------------------------
// ������Ͷ���

#define MAXMODFRMLEN  256

typedef struct tagMODFRM {	// ͨ��֡
	BYTE	cbuf[MAXMODFRMLEN];//
	int		nlen;

}MODFRM, *LPMODFRM;

class CRandomFrmBuf //������Ͷ��У�ѭ������,ȡ�����ٴ���
{
protected:
	MODFRM	frms[256];
	int     nmaxsize;//���ռ� 256֡;
	int		nhead;//ͷ
	int		ntail;//β
public:
	CRandomFrmBuf()
	{
		nmaxsize = 256;
		nhead =0;
		ntail = 0;
	};
	~CRandomFrmBuf(){};

	// nhead == ntail ��
	// (ntail + 1) % nmaxsize == nhead ��

	void Clear() //���
	{
		nhead = ntail;
	};

	BOOL AddFrm(BYTE *s,int nlen)//���,β��ͷ��,ֻ��β������������
	{
		int nh,nt;
		nh = nhead;
		nt = ntail;

		if( ((nt + 1) % nmaxsize) == nh )//��
			return FALSE;
		if((nlen <= 0)||(nlen > MAXMODFRMLEN))
			return FALSE;

		memcpy(frms[nt].cbuf,s,nlen);
		frms[nt].nlen = nlen;

		nt = (nt +1)%nmaxsize;
		ntail = nt;
		return TRUE;
	};

	int GetFrm(BYTE *buf,int nbuflen)//ȡ��,��ͷȡ��ֻ��ͷ������������
	{
		int nh,nt;
		nh = nhead;
		nt = ntail;
		if(nh == nt)//��
			return 0;

		int nret = frms[nh].nlen;
		if((nret >0) && (nret <= nbuflen))
			memcpy(buf,frms[nh].cbuf,nret);
		else 
			nret = 0;

		nh = (nh + 1)%nmaxsize;
		nhead = nh;
		
		return nret;
	};
};

//------------------------------------------------------------------------
// ѭ������֡����
class CLoopFrmBuf //ѭ������֡����,�ṹ����,ֻȡ��ɾ
{
protected:
	MODFRM frms[256];
	int     nmaxsize;//���ռ� 256֡;
	int     nsize;//��ǰ֡��
	int		npos;//��ǰλ��
public:
	CLoopFrmBuf()
	{
		nmaxsize = 256;
		nsize=0;
		npos = 0;
	};
	~CLoopFrmBuf(){};

	void Clear()
	{
		nsize=0;
		npos = 0;
	};

	BOOL AddFrm(BYTE *s,int nlen)
	{
		if((nlen > MAXMODFRMLEN)||(nlen <= 0))
			return FALSE;
		if(nsize >= nmaxsize)
			return FALSE;
		memcpy(frms[nsize].cbuf,s,nlen);
		frms[nsize].nlen = nlen;
		nsize++;
		return TRUE;
	};

	int GetNextFrm(BYTE *buf,int nbuflen)//�����ֽ���
	{
		if(nsize == 0)
			return 0;
		if(npos == nsize)
			npos=0;
		if(frms[npos].nlen > nbuflen)
			return 0;
		memcpy(buf,frms[npos].cbuf,frms[npos].nlen);
		int nret = frms[npos].nlen;
		npos = (npos + 1)%nsize;
		return nret;
	};
};

//------------------------------------------------------------------------
// modbus������
class CModItem//
{
public:
	enum datatype { SHORTV = 2,WORDV = 3,LONGV = 4,DWORDV = 5,FLOATV = 6 };//��ECD_DT_XXXX��ͬ

	BYTE    m_ucorder;	//modbus����ţ���һ���� 
	WORD	m_waddr;	//�Ĵ�����ַ���ڶ�����
	char	m_uctype;	// �ɼ�ֵ����datatype��DRT_DIGITAL,

	CString m_sname;	//����
	CString m_szdes;	//����
	CString m_szunit;	//��ע

	CModItem(){
		m_sname = "";
		m_ucorder = 4;	
		m_waddr = 0;		
		m_uctype = SHORTV; 
		m_szdes = "";
		m_szunit = "";
	};
	CModItem(LPCTSTR lpszname,LPCTSTR lpszname2,BYTE ucorder,WORD waddr,BYTE uctype,LPCTSTR lpszdes,LPCTSTR lpszunit){
		m_sname = lpszname;
		m_ucorder = ucorder;	
		m_waddr = waddr;
		m_uctype = uctype; 
		m_szdes = lpszdes;
		m_szunit = lpszunit;
	};
	~CModItem(){};
	virtual void	Serialize(CArchive& ar)
	{
		if(ar.IsStoring()){
			ar << m_waddr ;
			ar << m_ucorder ;	
			ar << m_uctype ; 
			ar << m_sname ;	
			ar << m_szdes;
			ar << m_szunit ;	
		}
		else{
			ar >> m_waddr ;
			ar >> m_ucorder ;	
			ar >> m_uctype ; 
			ar >> m_sname ;
			ar >> m_szdes;
			ar >> m_szunit ;	
		}
	};
	void Copy(CModItem *pi)
	{
		m_sname = pi->m_sname;
		m_szdes = pi->m_szdes;
		m_ucorder = pi->m_ucorder;	
		m_waddr = pi->m_waddr;		
		m_uctype = pi->m_uctype; 
		m_szunit  = pi->m_szunit;
	};
	void SetType(LPCTSTR lpszType)
	{
		CString szType = lpszType;
		szType.MakeUpper();
		if(szType == "SHORT")
			m_uctype = SHORTV;
		else if((szType == "WORD")||(szType == "BIT"))
			m_uctype = WORDV;
		else if(szType == "LONG")
			m_uctype = LONGV;
		else if(szType == "DWORD")
			m_uctype = DWORDV;
		else
			m_uctype = FLOATV;
	};
	CString GetType()
	{
		CString szt;
		if(m_uctype == SHORTV)
			szt = "SHORT";
		else if(m_uctype == WORDV)
		{
			if((m_ucorder == 1)||(m_ucorder == 2)||(m_ucorder == 5))
				szt = "BIT";
			else
				szt = "WORD";
		}
		else if(m_uctype == LONGV)
			szt = "LONG";
		else if(m_uctype == DWORDV)
			szt = "DWORD";
		else
			szt  ="FLOAT";
		return szt;
	};
	int GetCVLen(){
		if(m_uctype < LONGV)
			return 2;
		return 4;
	};
	
};//������Ķ���ֻ�ܶ��壬�����������⽨�����ݽṹ��
typedef CModItem*	LPMODITEM;

//----------------------------------------------------------------------------------
//  MODBUS�豸
class CModDec//MODBUS�豸
{
public:
	CString m_szname;//�豸����
	BYTE	m_ucaddr;//�豸��ַ
	
	int		m_ncrc16seq;//crc16�ֽ�˳��
	int		m_nwordseq;//���ֽ�����˳��
	int		m_ndwordseq;//4�ֽ�����˳��
	int		m_nfloatseq;//FLOAT�ֽ�˳��

	CLoopFrmBuf m_sendfrm;//Ԥ�õĹ̶����Ͱ�

protected:
	LPMODITEM* m_pbuf;
	long	m_ndatas;//ʵ�����ݶ�����
	long	m_nmaxitems;
protected:
	void	InitSendFrm0102(int norder);//��ʼ���̶�����3,4�����
	void	InitSendFrm0304(int norder);//��ʼ���̶�����1,2�����
public:

	CModDec(){
		m_pbuf = NULL;
		m_ndatas=0;
		m_nmaxitems=0;
		m_ncrc16seq = 12;//Ĭ�ϵ��ֽ���ǰ
		m_nwordseq = 21;//���ֽ�����˳��,Ĭ�ϸ��ֽ���ǰ
		m_ndwordseq = 4321;//4�ֽ�����˳��Ĭ��4321
		m_nfloatseq = 2134;//FLOAT�ֽ�˳��,Ĭ��2134
		m_szname = "";
	};

	~CModDec(){
		if(m_pbuf == NULL)
			return;
		LPMODITEM pi;
		int i;
		for(i=0;i<m_ndatas;i++)
		{
			pi =  m_pbuf[i];
			delete pi;
		}
		free(m_pbuf);
		m_pbuf = NULL;
	};
	virtual void	Serialize(CArchive& ar)
	{
		int i,n;
		CModItem item;
		if(ar.IsStoring()){			
			ar << m_szname;
			ar << m_ucaddr;
			ar << m_ncrc16seq;
			ar << m_nwordseq;
			ar << m_ndwordseq;
			ar << m_nfloatseq;

			ar << m_ndatas;
			for(i=0;i<m_ndatas;i++)
				m_pbuf[i]->Serialize(ar);
		}
		else{
			DeleteObjects();

			ar >> m_szname;
			ar >> m_ucaddr;
			ar >> m_ncrc16seq;
			ar >> m_nwordseq;
			ar >> m_ndwordseq;
			ar >> m_nfloatseq;

			ar >> n;
			for(i=0;i<n;i++)
			{
				item.Serialize(ar);
				Add(&item);
			}
		}
	};
	int	GetItems()//ȥ�������
	{
		return m_ndatas;
	};

	LPMODITEM GetAt(int nIndex) //ȡ����
	{
		if((nIndex < 0)||(nIndex >= m_ndatas))
			return FALSE;
		return m_pbuf[nIndex];
	};

	BOOL  Add(LPMODITEM pi){//���
		if(	m_ndatas >= m_nmaxitems)
		{
			LPMODITEM* pnew = (LPMODITEM*)malloc( (m_nmaxitems + 256) * sizeof(LPMODITEM));//ÿ������256��
			if(pnew == NULL)
				return FALSE;
			if(m_pbuf != NULL)
			{
				memcpy(pnew,m_pbuf,m_nmaxitems * sizeof(LPMODITEM));
				free(m_pbuf);
			}
			m_pbuf = pnew;
			m_nmaxitems+=256;
		}
		LPMODITEM pt = new CModItem;
		if( pt == NULL)
			return FALSE;
		pt->Copy(pi);
		m_pbuf[m_ndatas]=pt;
		m_ndatas++;
		return TRUE;
	};

	static int Comp(const void *p1,const void *p2 )//�Ƚ�
	{
		LPMODITEM pi1 ,pi2;

		pi1 = *((LPMODITEM *)p1);
		pi2 = *((LPMODITEM *)p2);
		
		if(pi2->m_ucorder > pi1->m_ucorder)
			return -1;
		if(pi2->m_ucorder < pi1->m_ucorder)
			return 1;

		if(pi2->m_waddr > pi1->m_waddr)
			return -1;
		if(pi2->m_waddr < pi1->m_waddr)
			return 1;
		return 0;
	};

	void  sort()//����
	{
		qsort(m_pbuf, m_ndatas,sizeof(LPMODITEM),Comp);
	};
	void DeleteObjects()//ɾ�����ж���
	{
		if(m_pbuf == NULL)
			return;
		LPMODITEM pi;
		int i;
		for(i=0;i<m_ndatas;i++)
		{
			pi =  m_pbuf[i];
			delete pi;
		}
		free(m_pbuf);
		m_pbuf = NULL;
		m_ndatas=0;
		m_nmaxitems=0;
	};
	LPMODITEM FindItem(LPCTSTR lpszname)//����
	{
		LPMODITEM pi;
		int i;
		for(i=0;i<m_ndatas;i++)
		{
			pi =  m_pbuf[i];
			if(pi->m_sname == lpszname)
				return pi;
		}
		return NULL;
	};

	BOOL	DelItem(LPCTSTR lpszname)//ɾ��
	{
		LPMODITEM pi;
		int i;
		for(i=0;i<m_ndatas;i++)
		{
			pi =  m_pbuf[i];
			if(pi->m_sname == lpszname)
			{
				delete pi;
				m_pbuf[i] = NULL;
				while(i<m_ndatas - 1)
				{
					m_pbuf[i] = m_pbuf[i+1];
					i++;
				}
				m_ndatas--;
				return TRUE;
			}
		}
		return FALSE;
	};

	void	InitSendFrm();//��ʼ����ʱ����
};

//------------------------------------------------------------------------
// MODBUS����
#define COM_TYPE_485		0
#define COM_TYPE_TCP		1

#define MOD_TYPE_RTU		0
#define MOD_TYPE_ASCII		1

#define MAX_MODBUSDEC  128	//����豸����

class CModBusCfg//MODBUS����
{
protected:
	int m_nsize;//�豸����
	CModDec	*m_pdec[MAX_MODBUSDEC];
	BOOL m_bModify;
public:

	//ͨ�Ų���
	int		m_nComType;				// 0:COM485, 1:TCP
	int		m_nModType;				//modbusģʽ MOD_TYPE_RTU,MOD_TYPE_ASCII,�Ժ���
	WORD    m_wtcpport;				//TCP�˿�,��׼��502
	CString	m_szIP;					//������IP;

	CString m_szPort;				// �˿�������"\\\\.\\com1","\\\\.\\com2"
	DWORD	m_dwBaudRate;			//������,CBR_1200,CBR_2400,CBR_4800,CBR_9600,
									//CBR_14400,CBR_19200,CBR_38400,CBR_57600,CBR_115200,
									//CBR_14400,CBR_19200,CBR_38400,CBR_57600,CBR_115200,
	BYTE	m_cByteSize;			//����λ number of bits/byte, 4-8
	BYTE	m_cParity;				//0-4��Ӧno,odd,even,mark,space 
	BYTE	m_cStopBits;			// 0,1,2 ��Ӧ 1, 1.5, 2
	HANDLE  m_hComDev;				//ͨ�Ŷ˿ھ��

	BOOL	m_bTcpCrc16;			//TCP�Ƿ��CRC16
	int		m_nTimeOut;				//����ʱ
	//{{�������� by heqing 2010-7-28
	BOOL	m_bSaveLog;				//�Ƿ񱣴������־
	UINT	m_nUpdateRate;			//ˢ������
	//}}
	CString m_szRes;				//�����Ժ�ʹ��
	CModBusCfg(){
		m_nComType  = COM_TYPE_485;		//485
		m_nModType =  MOD_TYPE_RTU;		//RTU

		m_wtcpport  = 502;
		m_szIP = "127.0.0.1";

		m_szPort="\\\\.\\com1";			// �˿�������"\\\\.\\com1","\\\\.\\com2"
		m_dwBaudRate=CBR_9600;			//������,CBR_1200,CBR_2400,CBR_4800,CBR_9600,
										//CBR_14400,CBR_19200,CBR_38400,CBR_57600,CBR_115200,
		m_cByteSize=8;					//����λ number of bits/byte, 4-8
		m_cParity=0;					//0-4��Ӧno,odd,even,mark,space 
		m_cStopBits=0;					// 0,1,2 ��Ӧ 1, 1.5, 2

		m_nsize = 0;
		for(int i=0;i<MAX_MODBUSDEC;i++)
			m_pdec[i]= NULL;

		m_bModify = FALSE;
		m_bTcpCrc16 = FALSE;
		m_nTimeOut = 200;
		m_szRes = "";

		m_bSaveLog = FALSE;
		m_nUpdateRate = 1000;
	};
	~CModBusCfg(){
		int i;
		for(i=0;i<m_nsize;i++)
			delete m_pdec[i];
	};
	void SetModify(BOOL bModify){m_bModify = bModify;};
	BOOL IsModify(){return m_bModify;};
	CString GetComMsg()
	{
		CString szt;
		if(m_nComType == COM_TYPE_485)
		{
			szt.Format("%s,%d,%d",m_szPort,m_dwBaudRate,m_cByteSize);
			if(m_cStopBits == 0)
				szt+=",1";
			else if(m_cStopBits == 1)
				szt+=",1.5";
			else if(m_cStopBits == 2)
				szt+=",2";
			if(m_cParity == 0)
				szt+=",��У��";
			else if(m_cParity == 1)
				szt+=",odd";
			else if(m_cParity == 2)
				szt+=",even";
			else if(m_cParity == 3)
				szt+=",mark";
			else szt+=",space";
		}
		else
			szt.Format("%s;%d", m_szIP,m_wtcpport);
		return szt;
	};
	BOOL	Add(CModDec *pi)
	{
		if(m_nsize >= MAX_MODBUSDEC)
			return FALSE;
		m_pdec[m_nsize] = pi;
		m_nsize++;
		return TRUE;
	};

	BOOL Delete(LPCTSTR lpszname)//ɾ������
	{
		int i;
		for(i=0;i<m_nsize;i++)
		{
			if(m_pdec[i]->m_szname == lpszname)
			{
				delete m_pdec[i];
				m_pdec[i] = NULL;
				while(i< (m_nsize -1))
				{
					m_pdec[i] = m_pdec[i+1];
					i++;
				}
				m_nsize--;
				return TRUE;
			}
		}
		return FALSE;
	};
	BOOL Delete(BYTE ucaddr)//ɾ������
	{
		int i;
		for(i=0;i<m_nsize;i++)
		{
			if(m_pdec[i]->m_ucaddr == ucaddr)
			{
				delete m_pdec[i];
				m_pdec[i] = NULL;
				while(i< (m_nsize -1))
				{
					m_pdec[i] = m_pdec[i+1];
					i++;
				}
				m_nsize--;
				return TRUE;
			}
		}
		return FALSE;
	};
	void DeleteObjects()//ɾ�����ж���
	{
		int i;
		for(i=0;i<m_nsize;i++)
		{
			delete m_pdec[i];
			m_pdec[i] = NULL;
		}
		m_nsize = 0;
	}

	int	GetSize()//ȡ�豸����
	{
		return m_nsize;
	};

	CModDec	*GetAt(int nIndex)//ȡ����
	{
		if((nIndex < 0)||(nIndex >= m_nsize))
			return NULL;
		return m_pdec[nIndex];
	};

	CModDec	*GetDecByAddr(BYTE ucaddr)
	{
		int i;
		for(i=0;i<m_nsize;i++)
		{
			if(m_pdec[i]->m_ucaddr == ucaddr)
				return m_pdec[i];
		}
		return NULL;
	};

	virtual void	Serialize(CArchive& ar, DWORD dwVersion)
	{
		int i,n;
		CModDec *pi;
		if(ar.IsStoring()){		
			ar << m_nComType;	
			ar << m_nModType;
			ar << m_wtcpport;
			ar << m_szIP;
			ar << m_szPort;			
			ar << m_dwBaudRate;								
			ar << m_cByteSize;					
			ar << m_cParity;					
			ar << m_cStopBits;	
			ar << m_bTcpCrc16;
			ar << m_nTimeOut;
			ar << m_nUpdateRate;
			ar << m_bSaveLog;
			ar << m_szRes;

			ar << m_nsize;
			for(i=0;i<m_nsize;i++)
				m_pdec[i]->Serialize(ar);
		}
		else{
			ar >> m_nComType;
			ar >> m_nModType;
			ar >> m_wtcpport;
			ar >> m_szIP;
			ar >> m_szPort;			
			ar >> m_dwBaudRate;								
			ar >> m_cByteSize;					
			ar >> m_cParity;					
			ar >> m_cStopBits;	
			ar >> m_bTcpCrc16;
			ar >> m_nTimeOut;
			if (dwVersion >= DRVCFG_VERSION_USER + 2)
			{
				ar >> m_nUpdateRate;
			}
			if (dwVersion >= DRVCFG_VERSION)
			{
				ar >> m_bSaveLog;
			}
			ar >> m_szRes;

			DeleteObjects();
			ar >> n;
			for(i=0;((i<n) &&(i<MAX_MODBUSDEC));i++)
			{	
				pi = new CModDec;
				pi->Serialize(ar);
				m_pdec[m_nsize] = pi;
				m_nsize++;
			}
		}
	};

	int GetCfgSize()//ȡ�����ֽ���
	{
		CMemFile mf(4096);
		CArchive ar(&mf,CArchive::store);
		Serialize(ar, DRVCFG_VERSION);
		ar.Close();
		int nl = (int)mf.GetLength();
		mf.Close();
		return nl;
	};
};

//------------------------------------------------------------------------
//
class CModItemIndex //����CMAP��CModItem���������ڿ��ٲ���
{
public:
	CModItemIndex(){};
	CModItemIndex(CModDec *pdec,CModItem *pitem)
	{
		m_szID = pdec->m_szname + "." + pitem->m_sname;
		m_pdec = pdec;
		m_pitem = pitem;
	};

	~CModItemIndex(){};

	CString		m_szID;		//�������豸�� + "." + ��������
	CModDec		*m_pdec;	//�豸ָ��
	CModItem	*m_pitem;	//������ָ��
};
typedef CMap<CString,LPCTSTR, CModItemIndex*, CModItemIndex*> CModItemMap; //CModItem����MAP

class CModDrv;

//------------------------------------------------------------------------
//
class CModCom : public CObject//modbusͨ�Ż���,��������485��TCP�Ӵ˻��༯�ɶ�����ͬһ�ӿ�
{
public:
	CModCom(){};
	~CModCom(){};

	CModDrv* m_pDrv;
public:

	//���涨��Ĵ��麯�����������غ������
	virtual BOOL IsPortOpen() = 0;//�˿��Ƿ��
	virtual BOOL OpenPort() = 0;//�򿪶˿�
	virtual void ClosePort() = 0;//�رն˿�

	virtual int WriteFrm(BYTE *pb,int nsize) = 0;//����modbusͨ�Ű�,���ط��͵��ֽ���
				//���͵�ͨ�Ű�Ϊ��׼��MODBUS���������TCPͨ�ţ���Ҫ��ͨ��ģ���Լ�����ͷ��6�ֽ�

	virtual int ReadFrm(BYTE *pbuf,int nbufsize,int ncrc16seq) = 0;//����modbusͨ�Ű�,���ؽ�������֤�����ֽ���
				//��׼MODBUS���ݰ��������TCP������Ҫȥ��ǰ���6���ֽ�
};

//------------------------------------------------------------------------
//
class CModCom485 : public CModCom //485ͨ�ŷ�ʽ
{
public:
	CModCom485();
	~CModCom485();
protected:
	//ͨ�Ų���
	CString m_szPort;				// �˿�������"\\\\.\\com1","\\\\.\\com2"
	DWORD	m_dwBaudRate;			//������,CBR_1200,CBR_2400,CBR_4800,CBR_9600,
									//CBR_14400,CBR_19200,CBR_38400,CBR_57600,CBR_115200,
									//CBR_14400,CBR_19200,CBR_38400,CBR_57600,CBR_115200,
	BYTE	m_cByteSize;			//����λ number of bits/byte, 4-8
	BYTE	m_cParity;				//0-4��Ӧno,odd,even,mark,space 
	BYTE	m_cStopBits;			// 0,1,2 ��Ӧ 1, 1.5, 2
	HANDLE  m_hComDev;				//ͨ�Ŷ˿ھ��
	
	int		m_nTimeOut;				//��ʱ
	int		m_nerrnum;//�������,���ڼ�⴮�ڴ����Ը�λ��
public:
	
	void InitComPort(
		LPCTSTR lpszComName,// �˿�������"\\\\.\\com1","\\\\.\\com2"
		DWORD dwBaudRate,	// ������,
		BYTE cByteSize,		// ����λ number of bits/byte, 4-8
		BYTE cParity,		// 0-4=no,odd,even,mark,space 
		BYTE cStopBits,		// 0,1,2 = 1, 1.5, 2 )
		int  n_TimeOut
	);//��ʼ��ͨ�ſ�

	//{{�������صĺ���
	virtual BOOL IsPortOpen();//�˿��Ƿ��
	virtual BOOL OpenPort();//�򿪶˿�
	virtual void ClosePort();//�رն˿�

	virtual int WriteFrm(BYTE *pb,int nsize);
	virtual int ReadFrm(BYTE *pbuf,int nbufsize,int ncrc16seq);	
	//}}

protected:
	BOOL	SendData(BYTE *pc,int nSize);
	int		ReadData(BYTE *pc,int nSize);
};

//------------------------------------------------------------------------
// 
class CModComTcp : public CModCom //TCPͨ�ŷ�ʽ
{
public:
	CModComTcp();
	~CModComTcp();
protected:
	//ͨ�Ų���
	CString m_szSvrIp;	//������IP
	int		m_nSvrPort;	//����˿�
	BOOL	m_bTcpCrc16;//�Ƿ�ʹ��CRC16
	int		m_nTimeOut; //����ʱ
	SOCKET	m_sClient;
public:
	
	void	InitArgs(LPCTSTR lpszip,int nport,BOOL bTcpCrc16,int nTimeOut)//��ʼ������
	{
		m_szSvrIp = lpszip;
		m_nSvrPort = nport;
		m_bTcpCrc16 = bTcpCrc16;
		m_nTimeOut = nTimeOut;
	};

	//{{�������صĺ���
	virtual BOOL IsPortOpen();//�˿��Ƿ��
	virtual BOOL OpenPort();//�򿪶˿�
	virtual void ClosePort();//�رն˿�

	virtual int WriteFrm(BYTE *pb,int nsize);
	virtual int ReadFrm(BYTE *pbuf,int nbufsize,int ncrc16seq);
	//}}

protected:
	int		Read(SOCKET s,BYTE *pbuf,int size,int ndelaymsec);
	int		Write(SOCKET s,BYTE *pbuf,int size,int ndelaymsec);
};

class CModDrv : public CDrvObj//Modbus ������,�����CEcdDrv�̳ж���
{
protected:
	//{{�������е�����
	int		m_ndecsize;				//�豸����
	CModDec	*m_pdec[MAX_MODBUSDEC];	//�豸��������
	//}}
#ifdef _ENABLED_RECONNECT_TIMEOUT
	time_t	m_tmLastReconnectTime;
#endif
public:
	CModDrv();
	~CModDrv();

	void WriteDebugLog(BOOL nReadMode, BYTE *pb, int nsize); // дDebug��־
protected:
	WORD m_wVersion;
	//{{�������е�����
	CModCom				*m_pmodcom;		//modbusͨ����
	CModBusCfg			m_cfg;			//����
	CModItemMap			m_mapitem;		//�������ݵ㶨��MAP
	CRandomFrmBuf		m_randfrm;		//������Ͱ�,���ڻ���5,6�������з��Ͱ�
	//}}
	void	TransCfg();//�������ļ�ת��Ϊ���ж���

	//�������ػ���ĺ���
public: //�麯��
	virtual DRVRET  GetErrMsg(CString& szMsg,DRVRET dwcode);			//��ȡ������Ϣ
	virtual DRVRET  WriteVar(LPDRREC lpval);			//д����
	virtual DRVRET  GetTagDef(int nindex,LPDRVTAG  lptag);
					
protected: //���麯��
	virtual BOOL	OnConfig();					//���ú����ý���,����TRUE��ʾ�޸�������
	virtual void	Serialize(CArchive& ar);	//���л����棬��ȡ����
	virtual DRVRET	OnStart();					//׼������
	virtual void	OnStop();					//ֹͣ���к��������
	virtual UINT	DoRun();					//��������й��̣���RunThread�е���

protected:
	void	DoReadFrm(BYTE ucod,WORD wregaddr,CModDec *pdec,BYTE *pb,int nsize);//������յ������ݰ������ݰ�����֤
	
	void	Send05(CModDec *pdec,WORD regaddr,int nVal);//����05����
	void	Send06(CModDec *pdec,WORD regaddr,WORD wVal);//����06����
	void	Send16(CModDec *pdec,WORD regaddr,BYTE *pb,int nsize);//����16����

	void	Send16(CModDec *pdec,WORD regaddr,DWORD dwVal);//����16����
	void	Send16(CModDec *pdec,WORD regaddr,float fVal);//����16����

public:
	//{{ �ֽ�˳��ת��
	static	short	GetShort(BYTE *pb,int nseq);//
	static	WORD	GetWord(BYTE *pb,int nseq);//
	static	long	GetLong(BYTE *pb,int nseq);//
	static	DWORD	GetDWord(BYTE *pb,int nseq);//
	static	float	GetFloat(BYTE *pb,int nseq);//

	static  void	SetWord(void* pv,BYTE *pb,int nseq);
	static  void	SetDWord(void* pv,BYTE *pb,int nseq);
	static  void	SetFloat(void* pv,BYTE *pb,int nseq);
	static  WORD	GetRegAddr(BYTE *pb,int nsize)//ȡ�Ĵ�����ַ
	{
		if(nsize < 4)
			return 0;
		WORD wAddr = 0;
		wAddr = pb[2];
		wAddr <<= 8 ;
		wAddr += pb[3];

		return wAddr;
	};
	//}}
	BOOL	Adddec(CModDec *pi)
	{
		if(m_ndecsize >= MAX_MODBUSDEC)
			return FALSE;
		m_pdec[m_ndecsize] = pi;
		m_ndecsize++;
		return TRUE;
	};

	void DeleteAllDec()//ɾ�����ж���
	{
		int i;
		for(i=0;i<m_ndecsize;i++)
		{
			delete m_pdec[i];
			m_pdec[i] = NULL;
		}
		m_ndecsize = 0;
	};

	int	GetSize()//ȡ�豸����
	{
		return m_ndecsize;
	};

	CModDec	*GetAt(int nIndex)//ȡ����
	{
		if((nIndex < 0)||(nIndex >= m_ndecsize))
			return NULL;
		return m_pdec[nIndex];
	};

	CModDec	*GetDecByAddr(BYTE ucaddr)
	{
		int i;
		for(i=0;i<m_ndecsize;i++)
		{
			if(m_pdec[i]->m_ucaddr == ucaddr)
				return m_pdec[i];
		}
		return NULL;
	};
	FILETIME		m_ftStartTime;				// ��ʼʱ��
	FILETIME		m_ftEndTime;				// ��ֹʱ��
	
	long GetElapsedTime() {	// ������뼶ʱ��
		ULONGLONG qst = *((ULONGLONG*)(&m_ftStartTime));
		ULONGLONG qet = *((ULONGLONG*)(&m_ftEndTime));
		return (long)((qet - qst) / 10000);
	}
};
