// MyDrvObj.h: interface for the CMyDrvObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYDRVOBJ_H__9D0CA9F6_F8AE_48C3_97F0_7623BFA62115__INCLUDED_)
#define AFX_MYDRVOBJ_H__9D0CA9F6_F8AE_48C3_97F0_7623BFA62115__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\..\SDK\ddclib.h"
#include "MyFrmParse.h"
#include "Tool.h"

#define  BREAK_PERIOD  300
// #define  HAVE_ELECTR    

#pragma pack(1)
typedef struct tagAPCI 
{
	BYTE byHead[6];
}APCI, *LPAPCI;

typedef struct tagASDUHEAD
{
	BYTE byType;      //����
	BYTE byWord;      //�޶���
	BYTE byReason[2]; //����ԭ��
	BYTE byAddr[2];   //������ַ
}ASDUHEAD, *LPASDUHEAD;

//������Ϣ ��Ʒ������ ����ʱ�� ��ַ������
typedef struct tagIDENTPT1_0  
{
	BYTE byAddr[3]; //��Ϣ�����ַ
	BYTE byVal;     //������Ϣ
}IDENTPT1_0, *LPIDENTPT1_0;

//������Ϣ ��Ʒ������ ����ʱ�� ��ַ����
typedef struct tagIDENTPT1_1  
{
	BYTE byVal; //������Ϣ
}IDENTPT1_1, *LPIDENTPT1_1;

//˫����Ϣ ��Ʒ������ ����ʱ�� ��ַ������
typedef struct tagIDENTPT3_0
{
	BYTE byAddr[3]; //��Ϣ�����ַ
	BYTE byVal;	    //˫����Ϣ
}IDENTPT3_0,*LPIDENTPT3_0;

//˫����Ϣ ��Ʒ������ ����ʱ�� ��ַ����
typedef struct tagIDENTPT3_1
{
	BYTE byVal;	//˫����Ϣ
}IDENTPT3_1,*LPIDENTPT3_1;

//��һ��ֵ ����ʱ�� ��ַ������
typedef struct tagIDENTPT9_0 
{
	BYTE byAddr[3]; //��Ϣ�����ַ
	BYTE byVal[2];  //��һ��ֵ
	BYTE byQulity;  //Ʒ������
}IDENTPT9_0,*LPIDENTPT9_0;

//��һ��ֵ ����ʱ�� ��ַ����
typedef struct tagIDENTPT9_1
{
	BYTE byVal[2];  //��һ��ֵ
	BYTE byQulity;  //Ʒ������
}IDENTPT9_1,*LPIDENTPT9_1;

//��Ȼ�ֵ ����ʱ�� ��ַ������
typedef struct tagIDENTPT11_0
{
	BYTE byAddr[3]; //��Ϣ�����ַ
 	BYTE byVal[2];  //��Ȼ�ֵ
	BYTE byQulity;  //Ʒ������
}IDENTPT11_0,*LPIDENTPT11_0;

//��Ȼ�ֵ ����ʱ�� ��ַ����
typedef struct tagIDENTPT11_1
{
	BYTE byVal[2];  //��Ȼ�ֵ
	BYTE byQulity;  //Ʒ������
}IDENTPT11_1,*LPIDENTPT11_1;

//�̸����� ����ʱ�� ��ַ������
typedef struct tagIDENTPT13_0
{
	BYTE byAddr[3]; //��Ϣ�����ַ
	BYTE byVal[4];  //�̸�����
	BYTE byQulity;  //Ʒ������
}IDENTPT13_0,*LPIDENTPT13_0;

//�̸����� ����ʱ�� ��ַ����
typedef struct tagIDENTPT13_1
{
	BYTE byVal[4]; //�̸�����
	BYTE byQulity; //Ʒ������
}IDENTPT13_1,*LPIDENTPT13_1;

//����Ʒ�������Ĺ�һ��ֵ ����ʱ�� ��ַ������
typedef struct tagIDENTPT21_0
{
	BYTE byAddr[3]; //��Ϣ�����ַ
	BYTE byVal[2];  //����Ʒ�������Ĺ�һ��ֵ
}IDENTPT21_0,*LPIDENTPT21_0;

//����Ʒ�������Ĺ�һ��ֵ ����ʱ�� ��ַ����
typedef struct tagIDENTPT21_1
{
	BYTE byVal[2]; //����Ʒ�������Ĺ�һ��ֵ
}IDENTPT21_1,*LPIDENTPT21_1;

//������Ϣ ��Ʒ������ ��ʱ�� ��ַ������
typedef struct tagIDENTPT30_0
{
	BYTE byAddr[3]; //��Ϣ�����ַ
	BYTE byVal;     //������Ϣ
	BYTE byTime[7]; //ʱ��
}IDENTPT30_0, *LPIDENTPT30_0;

//������Ϣ ��Ʒ������ ��ʱ�� ��ַ����
typedef struct tagIDENTPT30_1 
{
	BYTE byVal;		//������Ϣ 
	BYTE byTime[7]; //ʱ��
}IDENTPT30_1, *LPIDENTPT30_1;

//˫����Ϣ ��Ʒ������ ��ʱ�� ��ַ������
typedef struct tagIDENTPT31_0
{
	BYTE byAddr[3]; //��Ϣ�����ַ
	BYTE byVal;	    //˫����Ϣ
	BYTE byTime[7]; //ʱ��
}IDENTPT31_0,*LPIDENTPT31_0;

//˫����Ϣ ��Ʒ������ ��ʱ�� ��ַ����
typedef struct tagIDENTPT31_1
{
	BYTE byVal;	    //˫����Ϣ
	BYTE byTime[7]; //ʱ��
}IDENTPT31_1,*LPIDENTPT31_1;

//��һ��ֵ ��Ʒ������ ��ʱ�� ��ַ������
typedef struct tagIDENTPT34_0
{
	BYTE byAddr[3]; //��Ϣ�����ַ
	BYTE byVal[2];  //��һ��ֵ
	BYTE byQulity;  //Ʒ������
	BYTE byTime[7]; //ʱ��
}IDENTPT34_0,*LPIDENTPT34_0;

//��һ��ֵ ��Ʒ������ ��ʱ�� ��ַ����
typedef struct tagIDENTPT34_1
{
	BYTE byVal[2];  //��һ��ֵ
	BYTE byQulity;  //Ʒ������
	BYTE byTime[7]; //ʱ��
}IDENTPT34_1,*LPIDENTPT34_1;

//��Ȼ�ֵ ��Ʒ������ ��ʱ�� ��ַ������
typedef struct tagIDENTPT35_0
{
	BYTE byAddr[3]; //��Ϣ�����ַ
	BYTE byVal[2];  //��Ȼ�ֵ
	BYTE byQulity;  //Ʒ������
	BYTE byTime[7]; //ʱ��
}IDENTPT35_0,*LPIDENTPT35_0;

//��Ȼ�ֵ ��Ʒ������ ��ʱ�� ��ַ����
typedef struct tagIDENTPT35_1
{
	BYTE byVal[2];  //��Ȼ�ֵ
	BYTE byQulity;  //Ʒ������
	BYTE byTime[7]; //ʱ��
}IDENTPT35_1,*LPIDENTPT35_1;

//�̸����� ��Ʒ������ ��ʱ�� ��ַ������
typedef struct tagIDENTPT36_0
{
	BYTE byAddr[3]; //��Ϣ�����ַ
	BYTE byVal[4];  //�̸�����
	BYTE byQulity;  //Ʒ������
	BYTE byTime[7]; //ʱ��
}IDENTPT36_0,*LPIDENTPT36_0;

//�̸����� ��Ʒ������ ��ʱ�� ��ַ����
typedef struct tagIDENTPT36_1
{
	BYTE byVal[4];  //�̸�����
	BYTE byQulity;  //Ʒ������
	BYTE byTime[7]; //ʱ��
}IDENTPT36_1,*LPIDENTPT36_1;

//������ ��Ʒ������ ��ʱ�� ��ַ������
typedef struct tagIDENTPT37_0
{
	BYTE byAddr[3]; //��Ϣ�����ַ
	int iValue;  //������
	BYTE byQulity;  //Ʒ������
	BYTE byTime[7]; //ʱ��
}IDENTPT37_0,*LPIDENTPT37_0;

//�̸����� ��Ʒ������ ��ʱ�� ��ַ����
typedef struct tagIDENTPT37_1
{
	int  iValue;  //������
	BYTE byQulity;  //Ʒ������
	BYTE byTime[7]; //ʱ��
}IDENTPT37_1,*LPIDENTPT37_1;


//���� ��Ʒ������ ��ַ������
typedef struct tagIDENTPT15_0
{
	BYTE byAddr[3]; //��Ϣ�����ַ
	BYTE byVal[4];  //������
	BYTE byQulity;  //Ʒ������
}IDENTPT15_0,*LPIDENTPT15_0;

//���� ��Ʒ������ ��ַ����
typedef struct tagIDENTPT15_1
{
	BYTE byVal[4];  //������
	BYTE byQulity;  //Ʒ������
}IDENTPT15_1,*LPIDENTPT15_1;
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
};

class CMyTCPClientProtocol : public CTCPClientProtocol
{
public:
	CMyTCPClientProtocol();
	virtual ~CMyTCPClientProtocol();
	
	virtual BOOL	Connect();
protected:
	BOOL AskPower(BYTE byCmd);
	virtual DRVRET  WriteVar(LPDRREC lpval);
	virtual void	DoRun();
	virtual void	Disconnect();
	void DoRead();
	void GetAddress();
	BOOL TestLink();		//������·
	BOOL AffirmTestLink();  //ȷ����·
	BOOL StartDT();		    //����
	BOOL AffirmStartDT();   //ȷ������
	BOOL Summon();			//���ٻ�
	BOOL AffirmSummon();    //ȷ�����ٻ�
	BOOL AffirmRecv();		//ȷ���յ�����
	BOOL CloseSummon();     //����ر�

	// ���÷�����IP
	BOOL m_bUseIP2;
	CString m_szIP2;
	WORD	m_wPort2;
	long	m_lDiStartAddr;
	long	m_lAiStartAddr;
	long	m_lPowerStartAddr;

	WORD m_nADPUAddr;       //ADPU��ַ
	BOOL m_bEnd;
	long lStartTime;
	BOOL m_bLinkOK;		    //��·��ͨ
	BOOL m_bSendTest;       //�Ƿ��Ͳ���ȷ��֡
	BOOL m_bSendCall;       //�Ƿ������ٻ�
	WORD m_wSendFrameNum;   //����֡��
	WORD m_wReceiveFrameNum;//����֡��
	BOOL m_bCanAskPowerAgain;//���������������
	BOOL m_bCanAskPower;    //���������������
	BOOL m_bCanRecvPower;   //���Ի�õ���

	CMyFrmParse m_FrmParseStream;

	CTimeSpanDif       m_tmAutoBreak;          // �Զ��Ͽ����ڣ�����Ӧ�Ը������绷��
private:
	
};

#endif // !defined(AFX_MYDRVOBJ_H__9D0CA9F6_F8AE_48C3_97F0_7623BFA62115__INCLUDED_)
