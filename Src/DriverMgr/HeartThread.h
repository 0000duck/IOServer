#pragma once
#include "asyntcp/asyntcp.h"

//����Ϊ˫ǰ��TCP��������
#define OD_DDC_ERROR 0x07 //��������
#define OD_DDC_LOGIN 0x08 //��¼����
#define OD_DDC_HEART 0x09 //��������
//��ҵ�����߳�

class CHeartThread :
	public CAsynThread
{
public:
	CHeartThread();
	virtual ~CHeartThread();
protected:
	CMakeFrm	m_frmsend;

	//��Ҫ���ص�
protected:
	virtual int DoOneJob(CJobItem* pjob); //����һ����ҵ
};

