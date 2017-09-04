// MyFrmParse.h: interface for the CMyFrmParse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYFRMPARSE_H__1AD52466_5898_4762_B5F7_DA665B9B6484__INCLUDED_)
#define AFX_MYFRMPARSE_H__1AD52466_5898_4762_B5F7_DA665B9B6484__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\..\SDK\ddclib.h"

// ����ִ��״̬
enum STATE_OPER{
	S_INIT,			                     // ��ʼ��״̬

	S_TIME_REQ,                         // ��½����
	S_TIME_REQ_OK,		              // ��½ȷ��

	S_REQ_LINK,                     //  ������·
	S_REQ_LINK_ACK,           //  ������·Ӧ��

	S_RESENT_LINK,              //  ��λ��·
	S_RESENT_LINK_ACK,   //  ��λ��·Ӧ��

	S_DATA_REQ,                                  // ��ȡ����
	S_DATA_REQ_OK,	                          // �յ�����OK

	S_LEVER1_DATA,              //  ����һ���û���������
	S_LEVER2_DATA,              //  ���Ͷ����û���������

	S_LOGOFF_REQ,                           // �˳�����
	S_LOGOFF_OK, 	                         // �˳�ȷ��

	S_CHEK_PSW_REQ,                       // У����������
	S_CHEK_PSW_OK 	                      // У������ȷ��
};

// ������
enum TYPE_CODE{
	TC_ELEC_0X86            = 0x86,	      // l.	��ʱ��ʱ�������ݿ��������
};

enum SEND_MSG_LEN{
	LEN_FIX_FRM            = 6,	       // �̶�֡��
	LEN_SET_TIME          = 16,          // �˳�״̬
	LEN_E5                        = 1 		    // E5Ӧ��
};

#define  REQ_SUCCESS        0x0
#define  SEND_MAX_FRM   0xAA
#define  DEVISE_FOCTOR  10000
//*************************************DIFINE****************************************
#pragma pack(push)
#pragma pack(1)

typedef struct tagENDFRM //����֡�ṹ
{
	BYTE btCS;                //CSУ��
	BYTE btEFlag;            //������ʶ
}ENDFRM, *LPENDFRM;

typedef struct tagFIXFRM //�̶�֡�ṹ
{
	BYTE btSFlag;            //���ı�ʶ
	BYTE btCtrl;               //������
	WORD     wAddr ;      //��ַ��
	ENDFRM pEndFrm;   //����֡�ṹ
}FIXFRM, *LPFIXFRM;

typedef struct tagVARIHEADER //��ͷ�ṹ
{
	BYTE btSFlag1;           //�����ַ�
	BYTE btLen;               //֡��
	BYTE btLen2;             //ȷ��֡��
	BYTE btSFlag2;           //�����ַ�
	BYTE btCtrl;              //������
	WORD  wAddr ;            //��ַ��
}VARIHEADER, *LPVARIHEADER;

typedef struct tagASDUHEADER //ASDU��ͷ�ṹ
{
	BYTE btTI;                 //���ͱ�ʶ
	BYTE btVSQ;            //֡��
	BYTE btCOT;             //ȷ��֡��
	WORD   wElecAddr;   //������Զ���ն��豸��ַ
	BYTE     byItemNum ;  //������������Ŀ
}ASDUHEADER, *LPASDUHEADER;

typedef struct tagTIME_A     //ʱ����Ϣ a�������꣩
{
	BYTE  btMinute ;                      //��
	BYTE  btHour ;                         //ʱ
	BYTE  btDate;                          //��
	BYTE  btMonth;                       //��
	BYTE  btYear;                          //��
}TIME_A, *LPTIME_A;

typedef struct tagTIME_B     //ʱ����Ϣ b���������꣩
{
	BYTE  btMS ;                           //����
	BYTE  btSecond ;                     //��
	BYTE  btMinute ;                      //��
	BYTE  btHour ;                         //ʱ
	BYTE  btDate;                          //��
	BYTE  btMonth;                       //��
	BYTE  btYear;                          //��
}TIME_B, *LPTIME_B;

typedef struct tagITEM_DATA     //��ʱ��ĵ�����Ϣ����֡---��������
{
	DWORD      dwValue;                       //ֵ  
	BYTE       bySqua;                        //����
}ITEM_DATA, *LPITEM_DATA;

#define  ELE_NUM  5
typedef struct tagELEC_DATA     //��������
{
	BYTE      wElecTI;                      //�������ݱ�ʶ
	ITEM_DATA  Value[ELE_NUM];                      //����������ֵ0
	BYTE       btCheckSum;                  // ����λ
}ELEC_DATA, *LPELEC_DATA;

typedef struct tagINFO_Addr //��ʱ�����ʶ��
{
	WORD  wAddr;                         //��ʱ�����ʶ��
}INFO_Addr, *LPINFO_Addr;

typedef struct tagELECNO //��ʱ�����ʶ��
{
	BYTE  btElecNo;                         //��ʱ�����ʶ��
}ELECNO, *LPELECNO;



#pragma pack()
//*************************************DIFINE****************************************
#define TCP_BUFFER_SIZE		 1024*8  // 

enum GET_BYTE{
	GET_BYTE_HIGH ,	       // ��λ
	GET_BYTE_LOW  ,          // ��λ
};

class CMyFrmParse : public CFrmParse  
{
public:
	CMyFrmParse();
	virtual ~CMyFrmParse();
	virtual  int   Parse();//�ְ�,����m_nStatus
	virtual  int   Parse_E5();//�ְ�,����m_nStatus
	virtual  int   Parse_Fix();//�ְ�,����m_nStatus
	virtual  int   Parse_Vari();//�ְ�,����m_nStatus

	static BYTE        CalaCS(BYTE *pData, int iSize);
	static BYTE        GetByte(BYTE btData, char cType);
	static short   SwitchBytes(short wBytes,BOOL bSwitchBytes);
	static float    SwitchBytes(float fltBytes,BOOL bSwitchBytes);
	static DWORD   SwitchBytes(DWORD wBytes,BOOL bSwitchBytes);
	static DWORD   SwitchBytes(BYTE *pBytes,BOOL bSwitchBytes);
};

#endif // !defined(AFX_MYFRMPARSE_H__1AD52466_5898_4762_B5F7_DA665B9B6484__INCLUDED_)
