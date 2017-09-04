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
int CMyFrmParse::Parse()//�ְ�,����0��1��2��3
{
	int nsize=m_buf.GetDataSize();
	if(nsize < 2)//��ͷ����2
		return 0;
	BYTE *pc = m_buf.GetBuf();
	
	//�޻�˷ɨ��Ƚ�
	int np=0;//npָʾ��ǰ�ƶ��ֽ���
	BOOL bFindHead=FALSE;

lpFindHeadFlag:
	while(np <= (nsize - 2))
	{
		if(pc[np] != 0x68){
			np++;	continue;
		}
		if(pc[np+1] > 0xFD){
			np++;		continue;
		}
	
		bFindHead = TRUE;
		break;
	}
	if(!bFindHead)	//û�ҵ�����ͷ��־
	{
		m_buf.LeftMove(np);
		return 0;
	}
	if((np + 2) > nsize)
	{
		m_buf.LeftMove(np);//���Ȳ���
		return 1;
	}

	//��֤CRC32
/*	if(!CheckCrc32(pc+np))
	{
		np++;
		goto lpFindHeadFlag;
	}*/
	//
/*	BYTE *pf = pc+np;
	DWORD *pdw = (DWORD *)(pf);
	if(*pdw > (1024 * 1024))//MAX_FRM_LEN) //���Ĺ����������Ǵ���ı���
	{
		np++;
		goto lpFindHeadFlag;
	}
	m_nFrmLen = (int)(*pdw);//���ñ��ĳ���*/

	int ii = *(pc +2);
	int nFrameType = (ii & 0x03);
	if (nFrameType == 3) //U֡
	{
		m_nFrmLen = 6;
	}
	else
	{
		int nFrameType = (ii & 0x01);
		if (nFrameType == 0) //I֡
		{
			m_nFrmLen = 2 + *(pc +1);
		}
		else if (nFrameType == 1) //S֡
		{
			m_nFrmLen = 6;
		}
	}
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
