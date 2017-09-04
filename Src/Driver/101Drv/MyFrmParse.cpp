// MyFrmParse.cpp: implementation of the CMyFrmParse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "101Drv.h"
#include "MyFrmParse.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyFrmParse::CMyFrmParse()
{

}

CMyFrmParse::~CMyFrmParse()
{

}

//
BOOL CMyFrmParse::CheckCrc32(BYTE *pd)//����16�ֽڱ�ͷУ��
{
	DWORD dwCRC32 = CCheckAlgorithm::TL_GetCRC32(pd,12);
	DWORD *pcrc = (DWORD *)(pd+12);
	if(*pcrc != dwCRC32)
		return FALSE;
	return TRUE;
}

/*
�ְ���������ʵ������Ķ��ְ��㷨

  ����ֵ��
	0:û�ҵ���ͷ��־
	1:�ҵ���ͷ��־,���Ȳ����޷���֤��ͷ
	2:��ͷУ�����,���ݳ��Ȳ���
	3:�������,
*/  
int CMyFrmParse::Parse()//�ְ�,����0��1��2��3
{
	int nsize = m_buf.GetDataSize();
	if(nsize < 4)//��ͷ����4
		return 0;
	BYTE *pc = m_buf.GetBuf();
	
	//�޻�˷ɨ��Ƚ�
	int np=0;//npָʾ��ǰ�ƶ��ֽ���
	BOOL bFindHead=FALSE;
	
lpFindHeadFlag:
	
	while(np <= (nsize - 1))
	{
		if(pc[np] != 0x68){
			np++;	continue;
		}
		
		if(pc[np+3] != 0x68){
			np += 3;	continue;
		}
		bFindHead = TRUE;
		break;
	}
	if(!bFindHead)	//û�ҵ�����ͷ��־
	{
		m_buf.LeftMove(np);
		return 0;
	}
	if((np +4) > nsize)
	{
		m_buf.LeftMove(np);//���Ȳ���
		return 1;
	}
	
// 	//��֤CRC32
// 	if(!CheckCrc32(pc+np))
// 	{
// 		np++;
// 		goto lpFindHeadFlag;
// 	}
// 	//
	BYTE *pf = pc+np;
	const BYTE bFrameSize = *(pf+1);
	DWORD dSize = bFrameSize;
	DWORD *pdw = 4+&dSize+2;
	//	DWORD *pdw = (DWORD *)(pf+4);
	if(*pdw > (1024 * 1024))//MAX_FRM_LEN) //���Ĺ����������Ǵ���ı���
	{
		np++;
		goto lpFindHeadFlag;
	}
	m_nFrmLen = (int)(*pdw);//���ñ��ĳ���
	if((np + m_nFrmLen) <= nsize)
	{
		m_Frm.ClearData();//��ձ���
		m_Frm.PutData(pc+np,m_nFrmLen);//���Ʊ���
		m_buf.LeftMove(np + m_nFrmLen);//������ʣ�µ��ֽ�����
		m_nStatus = 3;
		return 3;
	}
	
	m_Frm.ClearData();//��ձ���
	m_Frm.PutData(pc+np,nsize - np);//�������µ��ֽ�
	m_buf.ClearData();//��մ�������
	m_nStatus = 2;
	return 2;//��ͷ��ȷ
}

// �ֽ���ת��
short CMyFrmParse::SwitchBytes(short wBytes,BOOL bSwitchBytes)
{
	if (bSwitchBytes)
	{
		BYTE btVal[2];
		BYTE *p = (BYTE *)&wBytes;
		btVal[0] = p[1];
		btVal[1] = p[0];

		return *((WORD*)btVal);
	}
	else {
		return wBytes;
	}
}

float CMyFrmParse::SwitchShortToFloat(short wBytes,BOOL bSwitchBytes)
{
	BYTE btVal[2];
	BYTE *p = (BYTE *)&wBytes;

	if (bSwitchBytes)
	{
		btVal[0] = p[1];
		btVal[1] = p[0];
		return *((float*)btVal);
	}
	else {
		btVal[0] = p[0];
		btVal[1] = p[1];
		return *((float*)btVal);
	}
}

DWORD CMyFrmParse::SwitchBytes(DWORD wBytes,BOOL bSwitchBytes)
{
	if (bSwitchBytes)
	{
		BYTE btVal[4];
		BYTE *p = (BYTE *)&wBytes;
		btVal[0] = p[3];
		btVal[1] = p[2];
		btVal[2] = p[1];
		btVal[3] = p[0];
		return  *((DWORD*)btVal);
	}
	else {
		return wBytes;
	}
}

// �ֽ���ת��
float CMyFrmParse::SwitchBytes(float fltBytes,BOOL bSwitchBytes)
{

	if (bSwitchBytes)
	{
		BYTE btVal[4];
		BYTE *p = (BYTE *)&fltBytes;
		btVal[0] = p[3];
		btVal[1] = p[2];
		btVal[2] = p[1];
		btVal[3] = p[0];
		return *((float*)btVal);
	}
	else {
		return fltBytes;
	}
}