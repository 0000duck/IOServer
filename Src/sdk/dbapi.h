#if !defined _DPAPI2_H_I
#define _DPAPI2_H_I

//��������
#ifndef DT_VOID
#define DT_VOID		0	//������
#define DT_DIGITAL	1	//������	,Ҳ��4�ֽ�����		
#define DT_INT32	2	//4�ֽ�����
#define DT_FLOAT32	3	//4�ֽڸ�����
#define DT_INT64	4	//8�ֽ�����
#define DT_FLOAT64	5	//8�ֽڸ�����
#define DT_STRING	6	//�ַ���
#define DT_BLOB		7	//���������ݿ�
#endif

// ���������������
#define DBP_QUALITY_GOOD					0x00	// GOOD
#define DBP_QUALITY_BAD						0x01	// ��ֵ����û���ض�ԭ��
#define DBP_QUALITY_BAD_CONFIG_ERROR		0x02	// ���������ô������磬��ǩ�����еķ�����������ɾ����
#define DBP_QUALITY_BAD_NOT_CONNECTED		0x03	// ����Ҫ�����ӵ�����Ŀ�꣬���޷��ɹ����ӡ�������˵���ڸ�ʱ����ϵ�ֵ�����ţ���Ϊ��һ������Դδ�ṩ
#define DBP_QUALITY_BAD_DEVICE_FAIL			0x04	// ��⵽�豸����
#define DBP_QUALITY_BAD_SENSOR_FAIL			0x05	// ��⵽����������
#define DBP_QUALITY_BAD_LAST				0x06	// �豸���������ͨѶʧ�ܣ����ǣ������Чֵ�ǿ��õġ���ע�⣬ʱ���ǵ������һ����Чֵ��ʱ�꣬���������������ʱ��
#define DBP_QUALITY_BAD_COMM_FAIL			0x07	// �豸���������ͨѶʧ�ܣ�����û�������õ���Чֵ
#define DBP_QUALITY_BAD_OUT_OF_SERVICE		0x08	// �ñ�ǩ���鱻����Ϊ�ǻ����ʾ�ñ�ǩ����ĳ��ԭ�򲻱�����������


#define DBP_QUALITY_UNCERTAIN				0xE0	// ֵ��Ч�Բ�ȷ������û���ض�ԭ��
#define DBP_QUALITY_UNCERTAIN_LAST			0xE1	// �Ѿ�ֹͣд������ֵ����ֵӦ����Ϊ����ȥֵ������ע�⣬��״̬��ʾΪ�ڹ涨ʱ���ڱ�ǩδ�����¡���Bad/Last Usable Value��ʾΪ��⵽ͨѶ������
#define DBP_QUALITY_UNCERTAIN_SENSOR		0xE2	// ֵ�������������̷�Χ���ߴ������ڲ����Ϊ������׼ȷ
#define DBP_QUALITY_UNCERTAIN_EU_EXCEEDED	0xE3	// ����ֵ�����˸�ֵ����ķ�Χ����ע�⣬��״̬������ζ�Ÿ�ֵ�ǳ��޵�
#define DBP_QUALITY_UNCERTAIN_SUB_NORMAL	0xE4	// ��ֵ���Զ��Դ��������״̬ΪGOOD������Դ�������ڹ涨ֵ

#define DBP_QUALITY_LOLO_LIMIT				0xF9	// �͵ͱ�
#define DBP_QUALITY_LO_LIMIT				0xFA	// �ͱ�
#define DBP_QUALITY_HI_LIMIT				0xFB	// �߱�
#define DBP_QUALITY_HIHI_LIMIT				0xFC	// �߸߱�
#define DBP_QUALITY_NOT_INIT				0xFD	// �ű���������δ����ʼ��
#define DBP_QUALITY_SHUTDOWN				0xFE	// �������ر�
#define DBP_QUALITY_UNKOWN					0xFF	// δ֪����

// ����ʷstr,��ʷblob,SOE��ʱ�������
#define  RST_CUR			1		//��ǰʱ�괦
#define  RST_AFTER			2		//��ǰ֮��һ��
#define  RST_DEFAULT		(RST_CUR | RST_AFTER) //��ǰ��֮��һ��

//Ȩ��
#define DS_POWERMAN		0x00000001 //�˺Ź���
#define DS_SYSMAN		0x00000002 //ϵͳ����,���ݿ������ǩ����
#define DS_WRITE		0x00000004 //дȨ��
#define DS_READ			0x00000008 //��Ȩ��

typedef void* DBPH; //���
typedef struct t_dbparg  //������������Ӳ���
{
	char	sip[16];    	//������IP
	char	suser[16];		//��¼�˺�
	char	spass[16];  	//��¼����
	WORD	wport;	//�˿�
	short	snres;	//����
	char	sres[4];		//����
}TDBPARG,*LPDBPARG;	//sizeof() = 56

//��ǩ����
#define TAG_FLAG_DDC	0 //�ֳ����ݱ�ǩ(ʵʱ���ǩ) ������"ddctag"
#define TAG_FLAG_DEF	1 //�����ǩ(�ֹ���ǩ)       ������"deftag"
#define TAG_FLAG_PRE	2 //Ԥ�����ǩ(Ԥ�����ǩ)   ������"pretag"
#define TAG_FLAG_CUR	3 //���߱�ǩ                 ������"curtag"	
#define TAG_FLAG_CALC	4 //�����ǩ				 ������"calctag"

typedef struct t_tagitem		//�����������ǩ����
{
	char	m_sname[80];		//��ǩ��
	char	m_sdbname[16];		//���ݿ�ʵ����
	char	m_sdbtagname[80];	//���ݿ��ǩ��
	char	m_sdes[80];			//����
	char	m_sunit[16];		//��λ
	DWORD   m_dwid;				//��ǩID
	WORD	m_wtype;			//��������
	short   m_errcode;			//�������
	long	m_ltagflag;			//��ǩ��־,
	DWORD   m_dwres;			//��������д0
}TTAGITEM,*LPTAGITEM; //sizeof() = 288

#ifndef DEFINE_STRUCT_TVVAL
typedef struct  t_vval		//����ṹ
{
	DWORD	vt;		//����
	long	lvlen;	//�ɱ䲿�ֳ���,��pstr,pblob,psoe��Ч,������֪���ȵĻ�������������0
					//�ַ������Ȳ�������β��0�ַ�
	union 	
	{
		long  	lval;		//�洢didital��int32
		float  	fval;		//�洢float32
		__int64 llval;		//�洢int64
		double 	dblval;		//�洢float64
		char*	pstr;		//�ַ�����ָ��
		void*	pblob;		//�洢pblob�Ĵ洢���׵�ַ
	};
	long  ltime;			//ʱ��
	short snqa;				//����
	short snerr;			//�������
}TVVAL,*LPVVAL; //sizeof() = 24 

#define DEFINE_STRUCT_TVVAL
#endif

#ifndef DEFINE_STRUCT_TSOE
typedef struct  t_soe		//�¼��ṹ
{
	long	m_ltime;			//ʱ��
	long	m_lmsec;			//����
	char	m_ssrc[64];			//��Դ
	char	m_sdec[64];			//�����豸
	char	m_sdes[256];		//�¼�����
	char	m_stxtargs[256];	//�ı�����
	long	m_lclsid;			//�¼�����
	char	m_sres[4];			//����
	long	m_lbinarglen;		//��������
	void*	m_pbinargs;			//�¼�����
}TSOE,*LPSOE; //sizof() = 664
#define DEFINE_STRUCT_TSOE
#endif


typedef struct t_ServerInfo
{
	DWORD dwVersion;			// ����������汾�ţ��汾��V2.5.4.0��ʾΪ0x02050400
	long lNowTime;				// ��ǰʱ��
	long lStartupTime;			// ����ʱ��
	DWORD dwConnectCount;		// ��ǰ������
	DWORD dwTagCount;			// ���б�ǩ��
	float fCPUUsage;			// CPUռ��
	DWORD dwHandleCount;		// �����
	DWORD dwThreadsCount;		// �߳���
	DWORD dwMemoryBytes;		// �ڴ�ʹ��
	DWORD dwMemoryBytesPeak;	// �߷��ڴ�ʹ��
	DWORD dwVirtualBytes;		// �����ڴ��С
	DWORD dwVirtualBytesPeak;	// �߷������ڴ��С
	DWORD dwReserves[52];
}SERVERINFO, *LPSERVERINFO; // sizeof()=256

//ö�ٺ���һ�δ���һ����ǩ��ԭ���Ǵ���һ����ǩ����������һ������ntags
typedef void (WINAPI *Enum_TagAttr)(LPTAGITEM ptag, int ntags, long lparam); //ö�ٱ�ǩ����

extern "C"{
	DWORD  __stdcall DBP_GetVersion();

	DBPH  __stdcall DBP_Open( 
		TDBPARG  conargs[],
		int nsize
		);

	DBPH  __stdcall DBP_Open2( 
		const char *szIP, 
		WORD wPort,
		const char *szUserName,
		const char *szPassword,
		BOOL bUseGroup
		);

	DWORD __stdcall DBP_Close( 
		DBPH h
		);

	DWORD __stdcall DBP_Connect( 
		DBPH h
		);

	DWORD __stdcall DBP_DisConnect( 
		DBPH h
		);

	DWORD __stdcall DBP_IsConnect( 
		DBPH h
		);

	DWORD __stdcall DBP_GetTagAttr(
		DBPH  h, 
		TTAGITEM tags[],
		int nsize
		);

	DWORD __stdcall DBP_GetSnapshot(
		DBPH  h, 
		char sTagNames[][80],
		TVVAL tvs[],
		int nsize
		);

	DWORD __stdcall DBP_GetHisVal(
		DBPH  h, 
		const char* lpszTagName,
		long lts,
		long lte,
		long linsertsec,
		long lflag,
		TVVAL pvals[],
		int nsize,
		int* pnrecsize
		);

	DWORD __stdcall DBP_GetHisVal2(
		DBPH  h,
		long ltime,
		char sTagNames[][80],
		TVVAL tvs[],
		int nsize
		);//�������ʷ����

	DWORD __stdcall DBP_WriteReal(
		DBPH  h, 
		char sTagNames[][80],
		TVVAL tvs[],
		int nsize
		);

	DWORD __stdcall DBP_WriteHis(
		DBPH  h, 
		char sTagNames[][80],
		TVVAL tvs[],
		int nsize
		);

	DWORD __stdcall DBP_WriteRealStr(
		DBPH  h, 
		const char* sName,
		TVVAL* ptv
		);

	DWORD __stdcall DBP_WriteRealBlob(
		DBPH  h, 
		const char* sName,
		TVVAL* ptv
		);

	DWORD __stdcall DBP_WriteHisStr(
		DBPH  h, 
		const char* sName,
		TVVAL* ptv
		);

	DWORD __stdcall DBP_WriteHisBlob(
		DBPH  h, 
		const char* sName,
		TVVAL* ptv
		);

	DWORD __stdcall DBP_WriteSoe(
		DBPH  h, 
		TSOE* psoe
		);

	DWORD __stdcall DBP_DelHis(
		DBPH  h, 
		const char* sName,
		long lts,
		long lte
		);

	DWORD __stdcall DBP_DelHis2(
		DBPH  h, 
		const char sNames[][80],
		long lts[],
		long lte[],
		short snerr[],
		int nsize
		);

	DWORD __stdcall DBP_DelSoe(
		DBPH  h, 
		long ltimes, 
		long lmsecs, 
		long ltimee,  
		long lmsece, 
		const char* ssrc, 
		const char* sdec, 
		long lclsid       
		);

	DWORD __stdcall DBP_GetSumVal(
		DBPH  h, 
		const char*  sName,
		long lts,
		long lte,
		long ltype,
		long lds,
		TVVAL pvals[],
		int nsize,
		int* pnrecsize
		);

	DWORD __stdcall DBP_GetRealStr(
		DBPH  h, 
		const char*  sName,
		TVVAL** ppval
		);

	DWORD __stdcall DBP_GetRealBlob(
		DBPH  h, 
		const char*  sName,
		TVVAL** ppval
		);

	DWORD __stdcall DBP_GetHisStr(
		DBPH  h, 
		const char*  sName,
		long ltime,
		unsigned long dwstyle,
		TVVAL** ppval
		);

	DWORD __stdcall DBP_GetHisBlob(
		DBPH  h, 
		const char*  sName,
		long ltime,
		unsigned long dwstyle,
		TVVAL** ppval
		);

	DWORD __stdcall DBP_GetSoe(
		DBPH  h, 
		long lts, 
		long lms, 
		const char* ssrc, 
		const char* sdec, 
		long lclsid,       
		DWORD dwStyle,		
		TSOE** ppsoe
		);

	DWORD __stdcall DBP_QueryTag(
		DBPH  h,
		LPCTSTR sfltname,
		LPCTSTR sfltdes,
		int ntype,
		Enum_TagAttr pfEnum,
		long lparam
		); //��ѯ��ǩ

	DWORD __stdcall DBP_QueryTag2(
		DBPH  h,
		LPCTSTR sfltname,
		LPCTSTR sfltdes,
		int ntype,
		int ntagtype,
		Enum_TagAttr pfEnum,
		long lparam); //��ѯ��ǩ

	DWORD __stdcall DBP_GetSvrConInfo(
		DBPH  h,
		LPDBPARG pconarg,
		int* pnNum);//ȡ�������ͻ�����

	DWORD __stdcall DBP_SendIdel(
		DBPH  h,
		long* plsrvoder);//���Ϳ��У����������Ƿ�������

	DWORD __stdcall DBP_InportTags(
		DBPH  h,
		const char* lpsztags,
		int nsize,
		BOOL bEnd); //�����ǩ��dbp������

	DWORD __stdcall DBP_GetGroups(
		DBPH  h,
		TDBPARG  conargs[],
		int *pnsize); //��ȡDBPȺ������

	DWORD __stdcall DBP_SetGroups(
		DBPH  h,
		TDBPARG  conargs[],
		int nsize); //����DBPȺ������

	DWORD __stdcall DBP_GetServerInfo(
		DBPH h, 
		LPSERVERINFO lpServerInfo
		);
}
#endif //_DPAPI2_H_I