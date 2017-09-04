//drvdef.h
#pragma once
#include <vector>

#define DRT_VOID				0	//������
#define DRT_DIGITAL				1	//������	,Ҳ��4�ֽ�����		
#define DRT_INT32				2	//4�ֽ�����
#define DRT_FLOAT32				3	//4�ֽڸ�����
#define DRT_INT64				4	//8�ֽ�����
#define DRT_FLOAT64				5	//8�ֽڸ�����
#define DRT_STRING				6	//�ַ���
#define DRT_BLOB				7	//�����

// Ŀ�������������
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

// Ϊ�˱�֤��ǰ�ϵ���Ŀ������������
#define QA_OK								DBP_QUALITY_GOOD
#define QA_ERR								DBP_QUALITY_BAD

//����״̬
#define DRSTATUS_STOP			0	//��������״̬ ֹͣ
#define DRSTATUS_RUN			1	//��������״̬ ����

typedef long DRVRET;	//����ֵ����

//������룬<0 ������,0-100����Ϊ�����淶�̶�������룬>=100Ϊ�����Զ���������
#define DRERR_OK			0		//�ɹ�
#define DRERR_HANDLE		1		//����ľ��
#define DRERR_CONFIGDATA	2		//�������ݴ���
#define DRERR_CONFIGBUF		3		//�������ݻ���������
#define DRERR_CREATE		4		//������������ʧ��
#define DRERR_NOMORETAG		5		//�޸����ǩ
#define DRERR_NOERRCODE		6		//�޴˴������
#define DRERR_NOTSUPPORT	7		//��֧��
#define DBERR_NOMOREREC		8		//û�м�¼
#define DBERR_NOTAG			9		//�޴˱�ǩ
#define DBERR_TYPE			10		//�����޷�ת�������
#define DBERR_TIME			11		//ʱ�����
#define DBERR_NOVAL			12		//û������

#define DBERR_MEMORY		13		// �ڴ治��
#define DBERR_PACKET_TO_LONG 14		// ���ĳ���
#define DBERR_QUEUE_FULL	15		// ��������

typedef struct DR_TAG  //����ö��ʹ��
{
	char sname[80];	//����
	char sdes[80];	//����
	char sunit[16];	//��λ
	WORD wtype;		//��������
	WORD waccess;	//��ȡ��ʽ��0ֻ����1��д
	char cr[4];		//����
}T_DRVTAG,*LPDRVTAG; //sizeof = 168

typedef struct _DR_TAG  //����ö��ʹ��
{
	CString szName;	//����
	CString szDes;	//����
	CString szUnit;	//��λ
	WORD	wType;		//��������
	WORD	wAccess;	//��ȡ��ʽ��0ֻ����1��д
	char cr[4];		//����
}T_DRVTAG1,*LPDRVTAG1; //sizeof = 168

typedef  struct  DR_HANDLE  //�������
{
	void *pObject;		//����������������д
	long lid;			//�����ṩ������ID,������Ҫ���ġ�
	long lParam1;		//����ʹ�ò�����������Ҫ����
	long lParam2;		//����ʹ�ò�����������Ҫ����
}T_DRHANDLE,*DRHANDLE; 	//sizeof() = 16

typedef struct DR_INFO //������Ϣ
{
	char sname[32]; 	//��������
	char sdes[128];		//����
	char sver[16];		//��������汾
	DWORD dwver;		//�����淶�汾����ѭ�������淶�汾
	DWORD dwres;		//����
	char sres[72];		//����
}T_DRVINFO,*LPDRVINFO;	//sizeof() == 256

#ifndef DEFINE_STRUCT_TVVAL
typedef struct  t_var		//����ṹ
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

typedef struct DR_REC		//���ݼ�¼
{
	char	sname[80];		//��ǩ��
	TVVAL	val;			//ֵ
}T_DRREC,*LPDRREC;			//sizeof() = 104

#ifndef DEFINE_STRUCT_TSOE
typedef struct  t_soe		//�¼��ṹ
{
	long	m_ltime;		//ʱ��
	long	m_lmsec;		//����
	char	m_ssrc[64];		//��Դ
	char	m_sdec[64];		//�����豸
	char	m_sdes[256];	//�¼�����
	char	m_stxtargs[256];//�ı�����
	long	m_lclsid;		//�¼�����
	char	m_sres[4];		//����
	long	m_lbinarglen;	//��������
	void*	m_pbinargs;		//�¼�����
}TSOE,*LPSOE; //sizof() = 664
#define DEFINE_STRUCT_TSOE
#endif

//�ص�������
typedef DWORD (WINAPI * DRCB_OnDataOk)(	//���ݻص�����
					DRHANDLE handle,	//���
					T_DRREC  recs[],		//��¼��
					int nrecs			//��¼����
);

typedef DWORD  (WINAPI * DRCB_OnLogOk)( //��־�ص�����
					DRHANDLE handle,	//���
					const char *plog,	//��־�ַ�����0�������ַ���
					DWORD dwLogLevel,	//��־����
					DWORD dwLogOptType	//��־��������
);

typedef DWORD  (WINAPI * DRCB_OnSoeOk)( //SOE�¼��ص�����,ÿ�λص�һ���¼�
					DRHANDLE handle,	//���
					LPSOE psoe			//�¼�
);

//////////////////////////////////////////////////////////////////////////
// 2015-09-22�������ݻص��ӿ�
//////////////////////////////////////////////////////////////////////////

// ��ʷ���ݻص�
typedef DWORD (WINAPI * DRCB_OnWriteHistoryCB)(
	DRHANDLE handle,		// ���
	T_DRREC  recs[],		// ��¼��
	int nrecs				// ��¼����
	);

// ͨ�����ݱ��Ļص�
typedef DWORD (WINAPI * DRCB_OnStreamMessageCB)(
	DRHANDLE handle,				// ���
	const char* lpszTagName,		// ��ǩ����
	DWORD	dwCommandID,			// ����ָ��ID
	BYTE*	pBuffer,				// �������ݻ���
	int		nSize					// ���ĳ���
	);