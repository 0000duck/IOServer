// PiDBDrv.h: interface for the CPiDBDrv class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIDBDRV_H__FBA770FC_637F_4AD7_B9D8_A05A0A3410E1__INCLUDED_)
#define AFX_PIDBDRV_H__FBA770FC_637F_4AD7_B9D8_A05A0A3410E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define CACHE_ITEM_MAX		1000 // ��󻺴��ǩֵ
#define FLUSH_DELAY_TIME	0.2 // ��λΪ�룬�������ʱʱ��ֵ�ڻ�δ�ﵽ���������ύ������Ҳ�ᱻǿ�������ύ

// �����ҪSDK�����ͼ�飬����ú�
#define DATATYPE_CHECKED	1

#pragma warning(disable : 4819) // warning C4819: ���ļ����������ڵ�ǰ����ҳ(936)�б�ʾ���ַ����뽫���ļ�����Ϊ Unicode ��ʽ�Է�ֹ���ݶ�ʧ

#include "..\sdk\dbsdk.h"
#include "..\sdk\dbdef.h"
#include <piapix.h>
//#include <pilogex.h>
#include "..\..\Common\CriticalSection.h"
#include "..\sdk\DbSummary.h"
#include "..\..\Common\DBDrvConnectString.h"

// �Ƿ�ʹ�ð�ȫ�������ڶ����ӵ�PI������ʹ�ö��߳�
#define USE_SAFELOCK

#ifdef USE_SAFELOCK
	#define SAFELOCK(x) TPRI::CCriticalSection::Owner csLock(x)
	// �Ƿ���Ҫʹ��ȫ���̰߳�ȫ��
	#define USE_STATIC_SAFELOCK		1
#else
	#define SAFELOCK(x) (void)0
#endif


typedef struct t_tagval2		//��ǩֵ
{
	long	lid;		//��ǩID
	PIvaluetype pitype;
	TVVAL	m_val;		//ֵ
	
}TTAGVAL2,*LPTAGVAL2;

class CPIServerItem
{
public:
	CPIServerItem(){

	}
	CPIServerItem(const CPIServerItem &other) {
		this->operator =(other);
	}
	~CPIServerItem() {

	}

	CPIServerItem &operator=(const CPIServerItem &other) {
		if (this != &other)
		{
			m_szIP = other.m_szIP;
			m_szPort = other.m_szPort;
			m_szUserName = other.m_szUserName;
			m_szPassword = other.m_szPassword;
			m_nConnTimeout = other.m_nConnTimeout;
			m_nDATimeout = other.m_nDATimeout;
		}

		return *this;
	}

public:
	CString		m_szIP;		//RDB IP��ַ
	CString	    m_szPort;	//����˿�
	CString     m_szUserName ;  //�û�����
	CString     m_szPassword ;  //�û�����
	int	        m_nConnTimeout;	//���ӳ�ʱ
	int	        m_nDATimeout;	//DA���ӳ�ʱ
};


typedef CArray<CPIServerItem> CPIServerList;


typedef struct _server_info
{
	CString szServer;
	UINT uiPort;
	CString szDBName;
	CString szTable;
	CString szUser;
	CString szPsw;
}T_ServerInfo;

#define PI_CONN_MAXTRY			1	//20�����������ӳ��ԣ����Դ���������Լ�����
#define PI_STRING_BUFFER		1024
#define PI_STRING_BUFFER_MAX	4096 // ��������ȡ�ַ�������

class CSqlServerConnect;

typedef CMap<int,int, int,int> CMapCount;

class CPiDBDrv : public CDrvObj  
{
public:
	CPiDBDrv();
	virtual ~CPiDBDrv();
	
#ifdef USE_SAFELOCK
#ifdef USE_STATIC_SAFELOCK
	static TPRI::CCriticalSection m_csLock;
#else
	TPRI::CCriticalSection m_csLock;
#endif
#endif

public:
	DWORD ErrRetCode(DWORD dwcode); //����ֵ������ת����PI������ת��Ϊ����������
	DWORD ErrDataCode(DWORD dwcode);//���ݴ������װ����PI������ת��Ϊ����������
	DWORD DataTypeFrom(PIvaluetype pttype); //ʵʱ������ת��Ϊ��������
	PIvaluetype DataTypeTo(DWORD dwtype);	//��������ת��Ϊʵʱ������
	void Time2PiTimestamp(long lTime, PITIMESTAMP *pTimestamp);
	long PiTimestamp2Time(PITIMESTAMP *pTimestamp);
	short StateToQuality(int32 nIstat);
	int32 QualityToState(short nQuality);
	BOOL SetTVVAL(LPVVAL pvval, int32 digicode, int32 pttype, float64 drval, int32 ival, 
		void *bval, uint32 bsize, int32 istat, int16 flags, long ltime, short ierror);//wzq 20101128
	BOOL SetTVVAL(LPVVAL pvval, int32 digicode, int32 pttype, float rval, int32 istat, long ltime);
	BOOL SetTVVAL(LPVVAL pvval, short snerr);//wzq
	//����ʹ�õ�protected����
protected:
	int				m_nPIServerIndex;
	CPIServerList	m_arrPIServerList;
	T_ServerInfo	m_tServerInfo;

	TVVAL		m_vStrValue;	// ����String\Blob���ͷ���
	CArray<char, char>	m_arrStrValue;	// m_vStrValue�Ķ�̬������

	CArray<int32, int32> m_arrPoint;
	CArray<float64, float64> m_arrDrval;
	CArray<int32, int32> m_arrIval;
	CArray<int32, int32> m_arrIstat;
	CArray<int16, int16> m_arrFlags;
	CArray<PITIMESTAMP, PITIMESTAMP> m_arrTimestamp;
	CArray<int32, int32> m_arrErrors;
	CArray<int, int> m_arrIndex;
	CArray<void *, void *> m_arrBval;
	CArray<uint32, uint32> m_arrBsize;

	CArray<float, float> m_arrHisFRvals;
	CArray<int32, int32> m_arrHisTimestamp;
	CArray<int32, int32> m_arrHisIstat;

	CDbSummary	m_summary;
	CMapCount  m_mapCount;

	
	void LoadSQLConfig(CDBDrvConnectString &objConnect, T_ServerInfo& tServerInfo);
	void SaveSQLConfig(CDBDrvConnectString &objConnect, const T_ServerInfo& tServerInfo);

	void LoadConfig(CDBDrvConnectString &objConnect, CPIServerList *pPIServerList = NULL);
	void SaveConfig(CDBDrvConnectString &objConnect, CPIServerList *pPIServerList = NULL);
	CPIServerItem &GetPIServer() 
	{
		return m_arrPIServerList[m_nPIServerIndex];
	}

	BOOL CheckValueType(PIvaluetype pttype, DWORD dwtype);
	int32 InitWriteTagValues(TTAGVAL vals[], int nsize);
	int32 InitWriteTagValues(TTAGVAL2 vals[], int nsize);
	int DoDeleteHisVal(int32 nPointID, long ltimes, long ltimee);
	//�������ݿ�;
	BOOL ConnectSQL();
	int IsConnectSQL();
	int WriteSQL(LPT_WeightData pWeightData);
	int UpdateFlag(int szDevice, long ltime);
protected:
	BOOL SetActiveServer() {
		/*
		  piut_setservernode - ��Ҫ����PI������
		  pilg_setservernode - �������е�PI���������ӽ����л�

		  ����ʹ��pilg_setservernode()������Ҫ����pilog32.lib������֤sdkreg.dll��pilog32.dll�汾��Ӧ
		 */
		//return (pilg_setservernode(GetPIServer().m_szIP) == 0);
		return (piut_setservernode(GetPIServer().m_szIP) == 0);
	}
	
protected:
	CSqlServerConnect* m_pSQLServer;
	BOOL		m_bWritePIServer;
	BOOL		m_bWriteSQLServer;

	BOOL		m_bKillThread;
	CWinThread	*m_pThread;
	int			m_nStrRealVals;
	TTAGVAL2	m_arrStrRealVals[CACHE_ITEM_MAX];
	int			m_nStrHisVals;
	TTAGVAL2	m_arrStrHisVals[CACHE_ITEM_MAX];
	TPRI::CCriticalSection m_csBatchLock;

	void InitBatchParams();
	void ClearTagVals(LPTAGVAL2 pvals, int nsize);
	DWORD AddBatchTag(LPTAGVAL pval, BOOL bReal = TRUE);
	DWORD FlushBatchTags(BOOL bReal = TRUE);
	void StartBatchThread();
	void StopBatchThread();
	static	UINT RunBatchThread(void *pParam);// �����߳�

	//���غ���
public:
	void CheckConnect();

	virtual DWORD ExecCommand(
		DWORD dwCommand,		// �������
		void *pParams,		// �����������ָ��
		DWORD dwParamSize,	// ���������С
		void *pResults,		// ����ִ�н������ָ�룬�����������ڴ棬���ʹ���Э�̱�֤��С��������ִ�н��Ҫ��
		DWORD dwResultSize	// ����ִ�н�����ݴ�С
		);
	//����
	virtual	void  OnDestroy(); //ɾ��ǰ����
	virtual DWORD Connect(const char* sConnectStr);
	virtual DWORD DisConnect();
	virtual DWORD Config(const char* sConnectStr,char* sBuf, int  nBufLen); //����
	
	//��ǩ
	virtual DWORD GetTagAttr(LPDRVTAGATTR ptag); //ȡ����
	virtual DWORD AddTag(
			TDRVTAGATTR* ptag
		) ; //��ӱ�ǩ
	virtual DWORD DelTag(
			const char* stagname
		) ; //ɾ����ǩ
	virtual DWORD EditTag(
			TDRVTAGATTR* ptag
		) ; //�޸ı�ǩ,�ѱ�ǩ��Ϊ����

	//дʵʱ
	virtual DWORD WriteReal(
		TTAGVAL vals[],
		int nsize
		); //дʵʱ
	virtual DWORD WriteHis(
		TTAGVAL vals[],
		int nsize
		);	//д��ʷ
	virtual DWORD WriteSoe(
		TSOE    	soes[], //�¼��ṹ����
		int			 nsize  //�¼�����
	);
	
	//��ʵʱ
	virtual DWORD GetRealVal(
		TTAGVAL vals[],
		int		nsize
		) ;//��ʵʱ,ֻ�ʺϻ�����������
	virtual DWORD GetRealStr(
		LPCTSTR sTagName,
		TVVAL** ppv
		) ; //ȡʵʱ�ַ�������,��ǩ����ptv->m_sname��
	virtual DWORD GetRealBlob(
		LPCTSTR sTagName,
		TVVAL** ppv
		) ;//ȡʵʱblob����,��ǩ����ptv->m_sname��

	//����ʷ
	virtual DWORD GetHisVal(
		const char* stagname,	//��ǩ��
		long ltimes,			//��ʼʱ��
		long ltimee,			//����ʱ��
		long ldsec,				//���ʱ����
		long lflag,				//ʵʱ���־��
		TVVAL vals[],			//���ݻش�ռ�
		int	 nsize,				//�ռ��С
		int* pnvals				//ʵ�����ݸ���
		) ;  //��һ����ǩ����ʷ��ֻ��Ի�����������
	/*
	virtual DWORD GetHisVal2(
		TTAGVAL vals[],		//��ǩ���ͻ���ֵ�ռ�
		int		nsize,		//��ǩ����
		long	ltime		//ʱ��
		) ;  //�����ǩͬһʱ����ʷ��ֻ��Ի�����������
	*/
	virtual DWORD GetHisStr(
		DWORD dwrst,
		LPCTSTR sTagName,
		long ltimes,
		TVVAL** ppv
		) ; //ȡһ���ַ������ͱ�ǩ��ʷֵ
	virtual DWORD GetHisBlob(
		DWORD dwrst,
		LPCTSTR sTagName,
		long ltimes,
		TVVAL** ppv
		) ;//ȡһ��blob���ͱ�ǩ��ʷֵ
	
	//������
	virtual DWORD GetSoe(
		DWORD dwrst,
		long	ltimes, //��ʼʱ��
		long    lmsecs,	//��ʼ����
		const char* ssrc, //��Դ,NULL��ʾȫ��
		const char* sdec, //DDClient�豸,NULL��ʾȫ��
		long lclsid,       //�¼�����,0��ʾȫ��
		TSOE**  lppSoe
	); //����һ��SOE
	virtual DWORD GetSumVal(
		const char* stagname,	//��ǩ��
		long ltimes,			//��ʼʱ��
		long ltimee,			//����ʱ��
		int  nSumType,			//ͳ�Ʒ�ʽ
		long ldsec,				//�ֶ�ʱ�䣬��0��ʾȫ����
		TVVAL vals[],			//���ݻش�ռ�
		int	 nsize,				//�ռ��С
		int* pnvals				//ʵ�ʸ���
	);  //��ͳ��ֵ��ֻ��Ի�����������

	//ɾ��
	virtual DWORD  DelSoe(
		long ltimes, //��ʼʱ��
		long lmsecs, //��ʼ����
		long timee,  //����ʱ��
		long lmsece, // ��������
		const char* ssrc, //��Դ,NULL��ʾȫ��
		const char* sdec, //DDClient�豸,NULL��ʾȫ��
		long lclsid       //�¼�����,0��ʾȫ��
	 ) ;//ɾ��SOE�¼�
	virtual DWORD DelHisVal(
		const char* stagname, //��ǩ��
		long	 ltimes,    //��ʼʱ��
		long	 ltimee		//����ʱ��
		) ;
	virtual DWORD  DelHisVal2(
		const char stagnames[][80], //��ǩ������
		long	ltimes[],			//��Ӧÿ����ǩ��ʱ��
		int		nerrcodes[],			//�����룬����
		int		nsize				//��¼����
		) ;	
	virtual DWORD  GetMultiPointHisVal(
		char sTagNames[][80], 
		long lTimes[], 
		long lMode,
		TVVAL tvs[], 
		int nsize
		);	
	virtual DWORD  GetDiSumVal(
		LPCTSTR lpszTagName, 
		long lts, 
		long lte, 
		long lds, 
		long lval,
		long lpSumCount[], 
		long lpSumTimes[], 
		int nsize, 
		int* pnrecsize
		);	
	virtual DWORD GetHisValFilter(
		const char* stagname,	//��ǩ��
		long ltimes,			//��ʼʱ��
		long ltimee,			//����ʱ��
		long ldsec,				//���ʱ����
		long lflag,				//ʵʱ���־��
		const char* expression,	//���������ַ���
		TVVAL vals[],			//���ݻش�ռ�
		int	 nsize,				//�ռ��С
		int* pnvals				//ʵ�����ݸ���
		);//���������˵Ķ���ʷ

	virtual DWORD TagSearch(
		const char* szTagNameMask,			// ��ǩ�����룬*��ʾȫ����?��ʾ����һ���ַ�
		const char* szTagDescrMask,			// ��ǩ�������룬*��ʾȫ����?��ʾ����һ���ַ�
		int nDataTypeMask,					// �����������룬-1��ʾȫ��
		LPTAGSEARCHRESULT pSearchResults,	// ���ر�ǩ��������ı�ǩ��������
		DWORD* pdwSearchResultCount,		// ����pSearchResults�����С�������ر�ǩ����������������������������������Ҫ���������������ʾ������ɣ�
											// ���*pdwSearchResultCount==0�������������Ƿ�֧�ֱ�ǩ������DBERR_OK��ʾ֧�֣�DBERR_NOTSUPPORT��ʾ��֧��
		DWORD_PTR* pdwSearchSession,		// ���δ���0�����ر�ǩ����Session
		float* pfltPerProgress				// ����������ɰٷ��ʣ���Чֵ0.0-100.0���������ֵС��0����ʾ��֧����������
		);

	virtual DWORD WriteStreamMessage(
		const char* lpszDeviceName,			// DDClient�豸ʵ����
		const char* lpszTagName,			// ���ݿ��ǩ��
		DWORD dwCommandID,					// ��Ϣָ��ID
		BYTE* pBuffer,						// ����Ϣ��
		int nSize							// ����Ϣ���С
		);
};

#endif // !defined(AFX_PIDBDRV_H__FBA770FC_637F_4AD7_B9D8_A05A0A3410E1__INCLUDED_)
