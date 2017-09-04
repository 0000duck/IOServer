// MyFrmParse.cpp: implementation of the CMyFrmParse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyFrmParse.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyFrmParse::CMyFrmParse()
{

}

CMyFrmParse::~CMyFrmParse()
{

}

/*
�ְ���������ʵ������Ķ��ְ��㷨

  ����ֵ��
	0:û�ҵ���ͷ��־
	1:�ҵ���ͷ��־,���Ȳ����޷���֤��ͷ
	2:��ͷУ�����,���ݳ��Ȳ���
	3:�������,
  */
//////////////////////////////////////////////////////////////////////////
int CMyFrmParse::Parse()//�ְ�,����0��1��2��3
{
	int nsize=m_buf.GetDataSize();

	if (nsize == LEN_E5 )
	{
		return Parse_E5();
	}
	else if (nsize ==  LEN_FIX_FRM )
	{ 
        return Parse_Fix();
	} 
	else
	{
		return Parse_Vari();
	}
}

BYTE    CMyFrmParse::GetByte(BYTE btData, char cType)
{
	BYTE btRet = 0;
	if (cType == GET_BYTE_HIGH)
	{
		btRet = btData &0xF0;
		btRet = btRet>>4;
	}
	else
	{
		btRet = btData &0x0F;
	}
	return   btRet;
}

BYTE  CMyFrmParse::CalaCS(BYTE *pData, int iSize)
{
	BYTE byCS = 0;
	if (pData == NULL || iSize <= 0)
	{       return byCS;      }

	for (int i = 0 ; i < iSize; i++ )
	{
		byCS += pData[i];
	}

	return byCS;
}

int CMyFrmParse::Parse_E5() //�ְ�,����0��1��2��3
{
	int nsize=m_buf.GetDataSize();
	if(nsize < LEN_E5)//��ͷ����
		return 0;
	BYTE *pc = m_buf.GetBuf();

	m_nFrmLen = LEN_E5;
	if (pc[0] == 0xE5)
	{
		m_Frm.ClearData();//��ձ���
		m_Frm.PutData(pc, m_nFrmLen);//���Ʊ���
		m_buf.LeftMove(m_nFrmLen);//������ʣ�µ��ֽ�����
		m_nStatus = FRAME_STATUS_OK;
		return m_nStatus;
	}
	m_Frm.ClearData();//��ձ���
	m_Frm.PutData(pc,nsize);//�������µ��ֽ�
	m_buf.ClearData();//��մ�������
	m_nStatus = FRAME_STATUS_HEAD_NO_ENOUGH;
	return m_nStatus;//��ͷ��ȷ
}

int CMyFrmParse::Parse_Fix()//�ְ�,����0��1��2��3
{
	int nsize=m_buf.GetDataSize();
	if(nsize < LEN_FIX_FRM )//��ͷ����
		return 0;

	BYTE *pc = m_buf.GetBuf();
	//�޻�˷ɨ��Ƚ�
	int np=0;//npָʾ��ǰ�ƶ��ֽ���
	BOOL bFindHead=FALSE;

lpFindHeadFlag:
	while(np <= (nsize - LEN_FIX_FRM))
	{
		if(pc[np] != 0x10){
			np++;	continue;}

		if(pc[np+5] != 0x16){
			np++;		continue;}

		bFindHead = TRUE;
		break;
	}

	if(!bFindHead)	{  //û�ҵ�����ͷ��־
		m_buf.LeftMove(np);
		return 0;   }

	if((np + LEN_FIX_FRM ) > nsize){
		m_buf.LeftMove(np);//���Ȳ���
		return 1;  }

	//��֤CS
	BYTE *pf = pc+np;
	FIXFRM *pFIXFRM = (LPFIXFRM)pf;

	BYTE byCS = CalaCS(&pFIXFRM->btCtrl, 3);
	if(byCS != pFIXFRM->pEndFrm.btCS){
		np++;
		goto lpFindHeadFlag;      }

	m_nFrmLen = LEN_FIX_FRM;//���ñ��ĳ���
	if ((np + m_nFrmLen) <= nsize){
		m_Frm.ClearData();//��ձ���
		m_Frm.PutData(pc + np, m_nFrmLen);//���Ʊ���
		m_buf.LeftMove(np + m_nFrmLen);//������ʣ�µ��ֽ�����

		m_nStatus = FRAME_STATUS_OK;
		return m_nStatus;}

	m_Frm.ClearData();//��ձ���
	m_Frm.PutData(pc + np,nsize - np);//�������µ��ֽ�
	m_buf.ClearData();//��մ�������
	m_nStatus = FRAME_STATUS_HEAD_NO_ENOUGH;
	return m_nStatus;//��ͷ��ȷ
}

int CMyFrmParse::Parse_Vari()//�ְ�,����0��1��2��3
{
	int nsize=m_buf.GetDataSize();

	int iMinMsgSize = sizeof(VARIHEADER) + sizeof(ENDFRM);
	if(nsize < iMinMsgSize )//��ͷ����
		return 0;
	BYTE *pc = m_buf.GetBuf();

	//�޻�˷ɨ��Ƚ�
	int np=0;//npָʾ��ǰ�ƶ��ֽ���
	BOOL bFindHead=FALSE;

lpFindHeadFlag:
	while(np <= (nsize - LEN_FIX_FRM))
	{
		if(pc[np] != 0x68)  {
			np++;	continue;}

		if(pc[np+3] != 0x68) {
			np++;		continue;}

		bFindHead = TRUE;
		break;
	}

	if(!bFindHead){ 	//û�ҵ�����ͷ��־
		m_buf.LeftMove(np);
		return 0;   }

	if((np + iMinMsgSize ) > nsize)  {
		m_buf.LeftMove(np);//���Ȳ���
		return 1;   }

	BYTE *pf = pc+np;
	VARIHEADER *pVARIHEADER = (LPVARIHEADER)pf;

	// ����У�����
	if (pVARIHEADER->btLen  != pVARIHEADER->btLen2) {
		np++;
		goto lpFindHeadFlag;  }

	int iFactLen = pVARIHEADER->btLen + 6;
	if((np + iFactLen ) > nsize)  {
		m_buf.LeftMove(np);   //ʵ�ʳ��Ȳ���
		return 1;   }

	//��֤CS
	BYTE byCS = CalaCS(&pVARIHEADER->btCtrl, pVARIHEADER->btLen);
	ENDFRM *pENDFRM = (LPENDFRM)(pf +  pVARIHEADER->btLen +4);
	if(byCS != pENDFRM->btCS){
		np++;
		goto lpFindHeadFlag; }

	m_nFrmLen = iFactLen;   //���ñ��ĳ���
	if ((np + m_nFrmLen) <= nsize)
	{
		m_Frm.ClearData();//��ձ���
		m_Frm.PutData(pc + np, m_nFrmLen);//���Ʊ���
		m_buf.LeftMove(np + m_nFrmLen);//������ʣ�µ��ֽ�����

		m_nStatus = FRAME_STATUS_OK;
		return m_nStatus;
	}

	m_Frm.ClearData();//��ձ���
	m_Frm.PutData(pc + np,nsize - np);//�������µ��ֽ�
	m_buf.ClearData();//��մ�������
	m_nStatus = FRAME_STATUS_DATA_NO_ENOUGH;
	return m_nStatus;//��ͷ��ȷ
}
//////////////////////////////////////////////////////////////////////////

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

DWORD CMyFrmParse::SwitchBytes(BYTE * pBytes,BOOL bSwitchBytes)
{
	if ( pBytes == NULL )
	{
		return 0;
	}

	BYTE btVal[4];

	if (bSwitchBytes)
	{
		btVal[0] = pBytes[0];
		btVal[1] = pBytes[1];
		btVal[2] = pBytes[2];
		btVal[3] = pBytes[3];
	}
	else {
		btVal[0] = pBytes[3];
		btVal[1] = pBytes[2];
		btVal[2] = pBytes[1];
		btVal[3] = pBytes[0];
	}

	return *((DWORD*)btVal);
}
