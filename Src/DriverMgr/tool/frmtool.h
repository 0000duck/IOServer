// ����������
#pragma once

#define FRM_SYN_WORD	0XA99AA99A
#define FRM_SYN_BYTE0	0x9A
#define FRM_SYN_BYTE1	0xA9
#define FRM_SYN_BYTE2	0x9A
#define FRM_SYN_BYTE3	0xA9
/*
	�����
  */
#define MAX_FRM_LEN		(MAX_BYTEARRAYSIZE - 128)	//����ĳ��� 4M

class CMakeFrm
{
public:
	CMakeFrm(){};
	~CMakeFrm(){};
public:
	BOOL MakeFrm(BYTE ucOrder,BYTE ucSubOrder,BYTE ucEndFlag,BYTE ucErrCode,BYTE *pData,int nDataSize)
	{
		if((nDataSize + 16) > MAX_FRM_LEN)
			return FALSE;

		m_ByteArray.ClearData();

		BYTE ucbuf[32];
		DWORD *pdw = (DWORD *)ucbuf;
		pdw[0] = FRM_SYN_WORD;
		pdw[1] = 16 + nDataSize;
		ucbuf[8] = ucOrder;
		ucbuf[9] = ucSubOrder;
		ucbuf[10] = ucEndFlag;
		ucbuf[11] = ucErrCode;
		pdw[3] = Utility::GetCRC32(ucbuf,12);

		m_ByteArray.PutData(ucbuf,16);

		m_ByteArray.PutData(pData,nDataSize);
		return TRUE;
	}; //���

	BOOL MakeFrm(BYTE ucOrder,BYTE ucSubOrder,BYTE ucEndFlag,BYTE ucErrCode,LPCTSTR pData,int nDataSize)
	{
		return MakeFrm(ucOrder,ucSubOrder,ucEndFlag,ucErrCode,(BYTE *)pData,nDataSize);
	}; //���

	BYTE *GetFrm(int *nsize)
	{
		*nsize = m_ByteArray.GetDataSize();
		if(*nsize > 0)
			return m_ByteArray.GetBuf();
		return NULL;
	}; //ȡ����

	static BOOL MakeFrm(CMyByteArray *pba,BYTE ucOrder,BYTE ucSubOrder,BYTE ucEndFlag,BYTE ucErrCode,BYTE *pData,int nDataSize)
	{
		if((nDataSize + 16) > MAX_FRM_LEN)
			return FALSE;

		pba->ClearData();

		BYTE ucbuf[32];
		DWORD *pdw = (DWORD *)ucbuf;
		pdw[0] = FRM_SYN_WORD;
		pdw[1] = 16 + nDataSize;
		ucbuf[8] = ucOrder;
		ucbuf[9] = ucSubOrder;
		ucbuf[10] = ucEndFlag;
		ucbuf[11] = ucErrCode;
		pdw[3] = Utility::GetCRC32(ucbuf,12);

		pba->PutData(ucbuf,16);
		pba->PutData(pData,nDataSize);
		return TRUE;
	}; //���
protected:
	CMyByteArray m_ByteArray; //������ݵĶ�̬����

};

/*------------------------------------------------------------------------------------

  ���Ľ��� CFrmParse,ÿ���ͻ����Ӻ�ʹ���һ���ö���
	0:û�ҵ���ͷ��־
	1:�ҵ���ͷ��־,���Ȳ����޷���֤��ͷ
	2:��ͷУ�����,���ݳ��Ȳ���
	3:�������,
  */
#define FRMP_NOHEAD		0	//û�ҵ���ͷ
#define FRMP_HEADOK		1	//�ҵ���ͷ
#define FRMP_CHKOK		2	//У����ȷ
#define FRMP_FRMOK		3	//�������
class CFrmParse //���Ľ���,ֻ�Ƿְ�
{
public:
	CFrmParse()
	{
		m_nStatus = FRMP_NOHEAD;
		m_nFrmLen = 0;	
	};
	~CFrmParse(){};
public:
	CMyByteArray	m_Frm;	//�ѷ�����ı��ģ�����ͷ
public:
	int		PutData(BYTE *buf,int nsize)//����δ��������
	{
		if((buf == NULL)||(nsize == 0))
			return Parse();

		if(m_nStatus == 2) //��ͷ����
		{
			int n=m_Frm.GetDataSize();
			if( ( n + nsize) < m_nFrmLen)
			{
				m_Frm.PutData(buf,nsize);
				return 2;
			}
			else 
			{
				
				m_Frm.PutData(buf,m_nFrmLen - n);
				m_buf.PutData(buf + (m_nFrmLen - n),nsize - (m_nFrmLen - n));//ʣ�µķŵ���������
				m_nStatus = 3;
				return 3;
			}
		}
		m_buf.PutData(buf,nsize);//���뻺����
		return Parse();
	};

	int		GetStatus(){return m_nStatus;};
	void	SetStatus(int nst){m_nStatus = nst;};
	int		Parse()//�ְ�,����m_nStatus
	{
		int nsize=m_buf.GetDataSize();
		if(nsize < 16)
			return 0;
		BYTE *pc = m_buf.GetBuf();
		
		//�޻�˷ɨ��Ƚ�
		int np=0;//npָʾ��ǰ�ƶ��ֽ���
		BOOL bFindHead=FALSE;

	lpFindHeadFlag:
		while(np <= (nsize - 4))
		{
			if(pc[np] != FRM_SYN_BYTE0){
				np++;	continue;
			}
			if(pc[np+1] != FRM_SYN_BYTE1){
				np++;		continue;
			}
			if(pc[np+2] != FRM_SYN_BYTE2){
				np += 2;	continue;
			}
			if(pc[np+3] != FRM_SYN_BYTE3){
				np += 3;	continue;
			}
			bFindHead = TRUE;
			break;
		}
		if(!bFindHead)	//û�ҵ�����ͷ��־
		{
			m_buf.LeftMove(np);
			return FRMP_NOHEAD;
		}
		if((np +16) > nsize)
		{
			m_buf.LeftMove(np);//���Ȳ���
			return FRMP_HEADOK;
		}

		//��֤CRC32
		if(!CheckCrc32(pc+np))
		{
			np++;
			goto lpFindHeadFlag;
		}
		//
		BYTE *pf = pc+np;
		DWORD *pdw = (DWORD *)(pf + 4);
		if(*pdw > MAX_FRM_LEN) //���Ĺ����������Ǵ���ı���
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
			m_nStatus = FRMP_FRMOK;
			return FRMP_FRMOK;
		}

		m_Frm.ClearData();//��ձ���
		m_Frm.PutData(pc+np,nsize - np);//�������µ��ֽ�
		m_buf.ClearData();//��մ�������
		m_nStatus = FRMP_CHKOK;
		return FRMP_CHKOK;//��ͷ��ȷ
	};
	static	BOOL CheckCrc32(BYTE *pd) //����16�ֽڱ�ͷУ��
	{
		DWORD dwCRC32 = Utility::GetCRC32(pd,12);
		DWORD *pcrc = (DWORD *)(pd+12);
		if(*pcrc != dwCRC32)
			return FALSE;
		return TRUE;
	};
protected:
	int				m_nFrmLen;		//���ĳ���
	int				m_nStatus;		//0:�ޣ�1����ͷ��ʼ��־�ҵ���2����ͷ��ɣ�3���������
	CMyByteArray	m_buf;			//�������ֽ�
	
};