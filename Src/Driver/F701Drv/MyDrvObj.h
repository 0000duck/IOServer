/*****************************************************************
* CopyRight By �����Ƽ�
* All rights reserved.
* �ļ�����: CDrvFactory.h F701
* ժ    Ҫ��������ʵ��F701��Լ��
* ��ǰ�汾��1.0
* ��    �ߣ�
* �������ڣ�2017.05.01
*****************************************************************/
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYDRVOBJ_H__9D0CA9F6_F8AE_48C3_97F0_7623BFA62115__INCLUDED_)
#define AFX_MYDRVOBJ_H__9D0CA9F6_F8AE_48C3_97F0_7623BFA62115__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\SDK\ddclib.h"
#include "MyFrmParse.h"
#include <vector>
using namespace std;

typedef struct _t_lastvalue
{
	int iLastCount;
	int iLastSetValue;
	float fLastRealValue;
	float fLastSuttleValue;
	float fLastTotalValue;
	int	lLastUpper;
	BYTE bClearFlag;
}T_LastValue;



#pragma pack()

class CMyDrvObj : public CDDCDrvObj  
{
public:
	CMyDrvObj();
	virtual ~CMyDrvObj();
public:
	virtual BOOL TestConnect(CConfigInfo &config);

protected:
	virtual CDDCDrvProtocol	*CreateProtocol();
	virtual DRVRET GetErrMsg(CString& szMsg,DRVRET dwcode);
public:
};

//////////////////////////CMySerialProtocol//////////////////////////////////////////////////////
class CMySerialProtocol : public CSerialProtocol
{

public:
	CMySerialProtocol();
	virtual ~CMySerialProtocol();

protected:
	virtual BOOL	Connect();
	virtual void	Disconnect();
	virtual void	DoRun();
	int		CheckSum(BYTE *pData,int iLen,int iLeft,int iRight);

	BOOL	ReadDevice(const string& szDevice, int iIndex);
	BOOL	WriteDeviceFailed(const string& szDevice,int iIndex);
	BOOL	StartDevice(const string& szDevice);
	void	StopDevice(const string& szDevice);
	//ë��
	int	GetTough(float& fValue);
	//����
	int	GetSuttle(float& fValue);
	//Ƥ��
	int   GetTare(float& fValue);
	//״̬1
	BOOL   GetStatus1();
	//״̬4
	int   GetStatus4();
	//�趨ֵ
	int   GetSetttingValue(const char* chNo, long& nValue);
	//�������
	int		ClearCount();
	int		GetUpper(long& iData);
	//�ۼƴ���
	int   GetCount(long& nValue);
	//�ۼ�����
	int   GetTotal(float& fValue);

	//��¼��0 ��־λ
	int  RecordClearFlag(string szDevice, int iIndex);
	long    m_lStartTime;//��ʼʱ��
	int     m_nAddr;     //��ʼ��ַ
	BOOL	m_bNewConnected;
	long	m_lLastCount;
private:
	vector<long>		m_vecTypes;
	vector<string>	m_arrDevices;
	map<string,int>		m_mapTolerance;
	string	szDevice;
	CString m_szSendTag;
	string	m_szIndex;
	long	m_lTimeSection[3];
	long	m_lTimeSpan;
	vector<T_LastValue*> m_vecLastValue;
};

#endif // !defined(AFX_MYDRVOBJ_H__9D0CA9F6_F8AE_48C3_97F0_7623BFA62115__INCLUDED_)
