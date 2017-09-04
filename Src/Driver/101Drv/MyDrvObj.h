/*****************************************************************
* CopyRight By �����ȹ��о�Ժ���޹�˾
* All rights reserved.
* �ļ�����: CDrvFactory.h 101��Լ�ӿ�����ʵ���ļ�
* ժ    Ҫ��������ʵ��101��Լ��
* ��ǰ�汾��1.0
* ��    �ߣ�ZCQ
* �������ڣ�2013.06.11
*****************************************************************/
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYDRVOBJ_H__9D0CA9F6_F8AE_48C3_97F0_7623BFA62115__INCLUDED_)
#define AFX_MYDRVOBJ_H__9D0CA9F6_F8AE_48C3_97F0_7623BFA62115__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\SDK\ddclib.h"
#include "MyFrmParse.h"

typedef enum emPROTOCOL  //SQֵ
{
	PROTOCOL_TCP,
	PROTOCOL_COM     
}PROTOCOL;

#define TCP_BUFFER_SIZE		    (1024 * 2) // 2K

#define  RTU_REFRESH_CIRCLE     60     //����ٻ�һ�����ݣ��룩
#define  ANALOG_START_INDEX     1793   //ң��㿪ʼ����
#define  DIGIT_START_INDEX      1793   //ң�ŵ㿪ʼ����
#define  TAG_NAME_FORMAT        "TESTPOINT"   //ң��㿪ʼ����

#define  FRM_LENGTH_MAX   255
#define  FRM_LENGTH_VAR   15
#define  FRM_LENGTH_TIME  21
#define  FRM_LENGTH_FIX   5
#define  FRM_LENGTH_E5    1

// ����ִ��״̬
enum STATE_OPER{
	S_INIT,			              // ��ʼ��״̬
	S_LINK_REQ,                   // ����Զ����·{10 49 01 4A 16 }
	S_LINK_OK,		              // ȷ��Զ����·{10 0B 01 0C 16 }
	S_RESET_REQ,                  // ��λԶ����·{10 40 01 41 16 }
	S_RESET_OK,	                  // ȷ��Զ����·{10 00 01 01 16 }
	S_CALL_SYS_TIME,		      // ��ȡϵͳʱ�� {0x68,0x0F,0x0F,0x68,0x53,nDomain,0x67,0x01,0x06,0x01,00,00,00,00,00,00,0x24,0x0B,0x05,nCS,0x16};
	S_SYS_TIME_OK,                // ��ȡϵͳʱ��
	S_CALL_DATA_FIRST,		      // �ٻ�һ������ {10 5A 01 5B 16}
	S_CALL_DATA_SECOND,		      // �ٻ��������� {10 5B 01 5C 16}
	S_CALL_DATA_WHOLE,		      // ����  {68 09 09 68 88 01 64 01 0a 01 00 00 14 0d 16}
	S_RECIEVE_DATA 	              // ���ݽ���
};

typedef enum emFrmType  //֡����
{
	FRM_FIX,
	FRM_TIME,
	FRM_CONFIRM,
	FRM_VARI
}FrmType; 

typedef enum emSQ  //SQֵ
{
	SQ_FALSE,
	SQ_TRUE     
}SQ;

#pragma pack(1)

typedef struct tagFixedFrm//�̶�֡���ṹ
{
	BYTE byLaunch;      //�����ַ���10H)
	BYTE byControl;     //������C)
	BYTE byLA;          //��·��ַ��A��
	BYTE byCheckSum;    //֡У��ͣ�CS��-- ���ȡģ256
	BYTE btProtect;     //�����ַ���16H��
}FixedFrm, *LPFixedFrm;   // ����ֽ�


typedef struct tagDATAHEADER //��ͷ�ṹ
{
	BYTE btFlag;        //���ı�ʶ
	BYTE btFrameLen;    //֡��
	BYTE btFrameLen2;   //ȷ��֡��
	BYTE btFlag2;       //ȷ�ϱ��ı�ʶ
	BYTE btCtrl;        //������C
	BYTE btAddr;        //��ַ��
	BYTE btType;        //���ͱ�ʶTI
	BYTE btVSQ;         //�ɱ�ṹ�޶���
	BYTE btCot;         //����ԭ��
	BYTE btComAddr;       //������ַ
}DATAHEADER, *LPDATAHEADER;

typedef struct tagADDR   //��Ϣ���ַ
{
	short shAddr;      //��Ϣ���λ��ַ     
}ADDR, *LPADDR;

typedef struct tagDIGIT_ADDR_AVALUE //������ ��ַ������
{
	BYTE byID;     
	BYTE bySIQ;      
}DIGIT_ADDR_AVALUE, *LPDIGIT_ADDR_AVALUE;

typedef struct tagDIGIT_AVALUE //������ ��ַ����
{   
	BYTE bySIQ;      
}DIGIT_AVALUE, *LPDIGIT_AVALUE;

typedef struct tagANALOG_ADDR_AVALUE//����ֵ-�̸����� ��ַ������
{
	short shID; 
	BYTE btLowAddr;      //ģ��ֵ��λ��ַ
	BYTE btHighAddr;     //ģ��ֵ��λ��ַ
	BYTE btQDS;			 //Ʒ��������
}ANALOG_ADDR_AVALUE, *LPANALOG_ADDR_AVALUE;

typedef struct tagANALOG_AVALUE//����ֵ-�̸����� ��ַ����
{
	BYTE btLowAddr;      //ģ��ֵ��λ��ַ
	BYTE btHighAddr;     //ģ��ֵ��λ��ַ
	BYTE btQDS;			 //Ʒ��������
}ANALOG_AVALUE, *LPANALOG_AVALUE;


typedef struct tagCONFIRMFRAME//ȷ��֡�ṹ
{
	BYTE btFlags[FIXEDPACKETLENGTH];
}CONFIRMFRAME,*LPCONFIRMFRAME;

#pragma pack()

class CMyDrvObj : public CDDCDrvObj  
{
public:
	CMyDrvObj();
	virtual ~CMyDrvObj();
public:
	virtual BOOL TestConnect(CConfigInfo &config);

protected:
	virtual CDDCDrvProtocol	*CreateProtocol();
	virtual DRVRET GetErrMsg(CString& szMsg,DRVRET dwcode);
public:
};

//////////////////////////CMySerialProtocol//////////////////////////////////////////////////////
class CMySerialProtocol : public CSerialProtocol
{

public:
	CMySerialProtocol();
	virtual ~CMySerialProtocol();

protected:

	virtual DRVRET  WriteVar(LPDRREC lpval);
	virtual void	DoRun();
	BOOL DoRead(int iTypeCode);
	BOOL DoSend(int iTypeCode);
	void GetAddress();
	int  TestMsg(BYTE *byDataType); // ���Ա��� ���ֳ���ȡ
	bool FrmBaseCheck(BYTE *pBuf, int iLen,char cCheckType);
	int  CheckSum(BYTE *pData,int iLen,int iLeft,int iRight);
	int  ParseByte(BYTE byData,BYTE byPos);
	bool HandleAnologPoint(BYTE *bufRev,int nSize,long lTime);
	bool HandleDigitPoint(BYTE *bufRev,int nSize,long lTime);

	void SetLinkStateMachine(int nState) 
	{
		m_nLinkStateMachine = nState;
		m_pDrvObj->AddInfoFmt(_T("״̬����%d"),nState);
	}

	long    m_lStartTime;//��ʼʱ��
	int     m_nAddr;     //��ʼ��ַ
	int m_nLinkStateMachine; // ��·״̬��

	BYTE		*m_pBuffer;
	CMyFrmParse  m_objBuffer;
private:

};


#define DEBUG_2
class CMyTCPClientProtocol : public CTCPClientProtocol
{
public:
	CMyTCPClientProtocol();
	virtual ~CMyTCPClientProtocol();

protected:

	virtual DRVRET  WriteVar(LPDRREC lpval);
	virtual void	DoRun();
	BOOL DoRead(int iTypeCode);
	BOOL DoSend(int iTypeCode);
	void GetAddress();
	int  TestMsg(BYTE *byDataType); // ���Ա��� ���ֳ���ȡ
	bool FrmBaseCheck(BYTE *pBuf, int iLen,char cCheckType);
	int  CheckSum(BYTE *pData,int iLen,int iLeft,int iRight);
	int  ParseByte(BYTE byData,BYTE byPos);
	bool HandleAnologPoint(BYTE *bufRev,int nSize,long lTime);
	bool HandleDigitPoint(BYTE *bufRev,int nSize,long lTime);

	void SetLinkStateMachine(int nState) 
	{
		m_nLinkStateMachine = nState;
		m_pDrvObj->AddInfoFmt(_T("״̬����%d"),nState);
	}

	long    m_lStartTime;//��ʼʱ��
	int     m_nAddr;     //��ʼ��ַ   

	int m_nLinkStateMachine; // ��·״̬��
	BYTE		*m_pBuffer;
	CMyFrmParse  m_objBuffer;
private:

};

#endif // !defined(AFX_MYDRVOBJ_H__9D0CA9F6_F8AE_48C3_97F0_7623BFA62115__INCLUDED_)
