#include "StdAfx.h"
#include "HeartThread.h"



CHeartThread::CHeartThread()
{

}
CHeartThread::~CHeartThread()
{

}

//����ΪSYSTEMTIME �ṹ���������16�ֽڵ�CRC32 ��20�ֽ�

int CHeartThread::DoOneJob(CJobItem* pjob)
{
	ASSERT(m_pconpool != NULL);
	BYTE *psend = NULL;
	int  nsend = 0;

	int nsize = 0;
	BYTE *pb = pjob->m_bin.GetBuf(nsize);
	if((pb == NULL) ||(nsize != 36)) //������
	{
		gGetDriveMgr()->m_nbakddcconnum = 0; //�ñ�DDC������
		m_frmsend.MakeFrm(OD_DDC_ERROR,0,0,OD_DDC_ERROR,(BYTE*)NULL,0);
		psend = m_frmsend.GetFrm(&nsend);
		m_pconpool->WriteSocket(pjob->m_dwid,psend,nsend,1000);
		return 0;
	}
	DWORD dwcrc32 = 0,*pdw = (DWORD*)(pb + 32);
	switch(pb[8])
	{
	case OD_DDC_HEART: //��������
		gGetDriveMgr()->m_lHeartTime = (long)::time(NULL);
		dwcrc32 = Utility::GetCRC32(pb+16,16);
		if(dwcrc32 != *pdw)
		{
			gGetDriveMgr()->m_nbakddcconnum = 0;//�ñ�DDC������
			m_frmsend.MakeFrm(OD_DDC_ERROR,0,0,OD_DDC_ERROR,(BYTE*)NULL,0);
		}
		else 
		{
			gGetDriveMgr()->m_nbakddcconnum = *((long*)(pb + 16));//��DDC�������Ӹ���
			//�����ж��Ƿ񽫹�������
			if((::gGetDriveMgr()->DDSConnectedNum()  < 1) && (gGetDriveMgr()->m_nbakddcconnum > 0))
				m_frmsend.MakeFrm(OD_DDC_HEART,0,0,0xFE,(BYTE*)NULL,0); //�뱸DDC����
			else
				m_frmsend.MakeFrm(OD_DDC_HEART,0,0,0x00,(BYTE*)NULL,0); //�뱸DDC��Ϣ			
		}
		break;
	default:
		m_frmsend.MakeFrm(OD_DDC_ERROR,0,0,OD_DDC_ERROR,(BYTE*)NULL,0);
		break;
	}
	psend = m_frmsend.GetFrm(&nsend);
	m_pconpool->WriteSocket(pjob->m_dwid,psend,nsend,1000);
	return 0;
}

