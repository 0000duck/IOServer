#pragma once
#include "DDSConnect.h"
#include "DecValMan.h"
#include "tool/frmtool.h"

//��� wddscache 
#define WDDS_BUF_SIZE		2000			//дDDS�Ļ��������������ݻ�������С
#define WDDSCACHE_BUF_SIZE (WDDS_BUF_SIZE/2)//д��������������ͻ����С
#define WDDSTIMEOUT			25000			//25�볬ʱ

#define UDP_WDDS_BUF_SIZE		100
#define UDP_WDDSCACHE_BUF_SIZE	100


class CWriteDDS
{
protected:
	class CMKVALC //�����������ͷ��Ͱ���֯
	{
	public:
		CMKVALC();
		~CMKVALC();
		char m_sbuf[256];
		int m_nsize;
		void MakeCell(CTagVal* ptv);
	};
public:
	CWriteDDS();
	~CWriteDDS();

	BOOL m_breal;//�Ƿ���ʵʱ, wddscache
public:
	CDDSConnect* m_pddscon;//DDS����
	CDecValMan* m_pDecValMan;//�豸���ݹ���
public:
	void Start();
	void Stop(BOOL bWaitForThreadExit);
	UINT DoRun();//�߳�����
protected:
	CTagVal		m_wrec[WDDS_BUF_SIZE];	//д���ü�¼������,������������
	CTagVal		m_varval;//string ��BLOB�Ļ�������һ��дһ������ֻ��Ҫһ������������
	CSoe		m_soe;	 //soe��������һ��дһ����
	DWORD		m_dwPingTick;

	//�̹߳���
	BOOL	m_bRun; //����
	BOOL	m_bKill;//�����߳�

	//ͨ�Ż���
	CMakeFrm	m_sendfrm;//���ͱ���
	CMyByteArray m_batmp;//��ʱ���ݶ�

protected:
	//����<0 ��ʾ���ϣ�>=0��ʾд�ļ�¼��
	int		WriteRealVal(BYTE &ucret,BYTE &ddsorder); //д������������ʵʱ
	int		WriteCasheVal(BYTE &ucret,BYTE &ddsorder);//д�����������ͻ���

	int		WriteRealVar(BYTE &ucret,BYTE &ddsorder);	//д�ɱ䳤��¼string,blob ʵʱ

	int		WriteCasheVar(BYTE &ucret,BYTE &ddsorder);	//д�ɱ䳤��¼string,blob ����

	int		WriteStreamMessage(BYTE &ucret,BYTE &ddsorder);	//д����Ϣ

	int		WriteRealSoe(BYTE &ucret,BYTE &ddsorder);	//дSOE

	static  UINT ThreadDDS(LPVOID lparam); //�߳�

	void	SendDdsTags();//���ͱ�ǩ

	int    SendEmpty(BYTE &ucret,BYTE &ddsorder); //���Ϳ�����
};

