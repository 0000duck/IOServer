#pragma once

#define SOEBUF_SIZE 128 //SOE���пռ��С

class CSOEBUF  //SOE������,����ѭ������
{
public:
	CSOEBUF();
	~CSOEBUF();
protected:
	int m_nhead,m_ntail; //δ��ͷ�� m_nhead == m_ntail��ʾ��
	TSOE m_soe[SOEBUF_SIZE];
public:
	BOOL Add(LPSOE psoe);
	BOOL Get(CSoe* psoe);
};
