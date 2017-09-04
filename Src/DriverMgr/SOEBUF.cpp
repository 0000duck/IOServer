#include "StdAfx.h"
#include "SOEBUF.h"


CSOEBUF::CSOEBUF(void)
{
	m_nhead = 0;
	m_ntail = 0;
	memset(m_soe,0,SOEBUF_SIZE * sizeof(TSOE)); //��ʼ��
}

CSOEBUF::~CSOEBUF(void)
{
	int i;
	for(i=0;i<SOEBUF_SIZE;i++)
		((CSoe*)&m_soe[i])->Clear();
}

BOOL CSOEBUF::Add(LPSOE psoe)
{
	if( ((m_ntail + 1) % SOEBUF_SIZE) == m_nhead)
		return FALSE; //��
	//����β��
	*((CSoe*)&m_soe[m_ntail]) = *((CSoe*)psoe);//��ֵ
	m_ntail = (m_ntail + 1) % SOEBUF_SIZE;
	return TRUE;
};
BOOL CSOEBUF::Get(CSoe* psoe)
{
	if(m_nhead == m_ntail) //��
		return FALSE;
	//��ͷ��
	*psoe = *((CSoe*)&m_soe[m_nhead]);
	m_nhead = (m_nhead + 1) % SOEBUF_SIZE;
	return TRUE;
};