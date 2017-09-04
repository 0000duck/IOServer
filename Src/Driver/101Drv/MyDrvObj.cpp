/*****************************************************************
* CopyRight By �����ȹ��о�Ժ���޹�˾
* All rights reserved.
* �ļ�����: CDrvFactory.cpp 101��Լ�ӿ�����ʵ���ļ�
* ժ    Ҫ��������ʵ��101��Լ��
* ��ǰ�汾��1.0
* ��    �ߣ�ZCQ
* �������ڣ�2013.06.11
*****************************************************************/
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "101Drv.h"
#include "MyDrvObj.h"
//#include "General\DebugUtils.h>

#include <math.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////
//ʵ�־�̬�����๤��
CDrvObj* CDrvFactory::NewDrv()
{
	return new CMyDrvObj();
}

DRVRET CDrvFactory::GetDriverInfo(LPDRVINFO lpDrvInfo)
{
	memset(lpDrvInfo, 0, sizeof(T_DRVINFO));
	strcpy_s(lpDrvInfo->sname, "101��Լ�ӿ�����");
	strcpy_s(lpDrvInfo->sdes, "CopyRight By �����ȹ��о�Ժ���޹�˾");
	strcpy_s(lpDrvInfo->sver, "2.0.0.0"); //����汾
	lpDrvInfo->dwver = 0x0200;//�淶�汾
	lpDrvInfo->dwres = 0;
	return DRERR_OK;
}

//////////////////////////////////////////////////////////////////////
// CMyDrvObj class
//////////////////////////////////////////////////////////////////////

CMyDrvObj::CMyDrvObj()
{

	// �������ô��л��洢ʱ�ı�־ID����ò�ͬ����֮�����ò�ֵͬ��ʾ����
	SetSerializeID(0x10000007);

	// ����ͨѶЭ�����ͣ����֧�ֶ�Э�飬������� | �Ļ�������Ӷ�Э��������
	// SetComTypes(COM_TYPE_UDP | COM_TYPE_TCP);
	// Ȼ����CreateProtocol()�ӿ��и����û�ѡ���Э��������Э�鴦�������ʵ��;
	SetComTypes(COM_TYPE_TCP |COM_TYPE_COM );
	EnabledTestConnect();
	CConfigInfo::m_szHelpFile = _T("101�ӿ�����.chm");
	CConfigInfo::m_szParamHintText = _T("�豸��ַ;�����磺1;");

	CTagItem::SetCSVFormatName(_T("JinLing_101Drv"));
	SetConnectionFailedStartup(TRUE);
}

CMyDrvObj::~CMyDrvObj()
{

}

BOOL CMyDrvObj::TestConnect(CConfigInfo &config)
{
	return CDDCDrvObj::TestConnect(config);
}

DRVRET CMyDrvObj::GetErrMsg(CString& szMsg, DRVRET dwcode)
{	
	if (dwcode < 100)
		return CDrvObj::GetErrMsg(szMsg, dwcode);

	//>100�������Զ���ģ����û���Զ��������Ϣ���ɲ���ʵ�ָ÷���

	switch (dwcode)
	{
	case 1001:
		szMsg = _T("XXX����");
		break;
	case 1002:
		szMsg = _T("XXX����");
		break;
	default:
		return DRERR_NOERRCODE;
	}

	return DRERR_OK;
}

// ��CreateProtocol()������Э�鴦����ʵ������Ҫ��ʽɾ����
// ���������������ʵ�ʱ���Զ�ɾ��
CDDCDrvProtocol	*CMyDrvObj::CreateProtocol()
{
	
	CDDCDrvProtocol *pObj = NULL;

	if (m_config.m_wComType  == COM_TYPE_TCP )
	{
		pObj = new CMyTCPClientProtocol();
		AddInfoFmt("����TCP/IP ���ӷ�ʽ��");
	}
	else
	{
		pObj = new CMySerialProtocol();
		AddInfoFmt("����COM�������ӷ�ʽ��");
	}

	
	if (pObj)
	InitTagIdMap();

	return pObj;
}

//////////////////////////////////////////////////////////////////////
// CMySerialProtocol Э�鴦����ʵ��
//////////////////////////////////////////////////////////////////////
CMySerialProtocol::CMySerialProtocol()
{
	m_nLinkStateMachine = S_INIT;
	m_lStartTime = 0;
	m_nAddr      = 1;
	m_pBuffer = new BYTE[TCP_BUFFER_SIZE];
}

CMySerialProtocol::~CMySerialProtocol()
{
	delete[] m_pBuffer;
}

DRVRET CMySerialProtocol::WriteVar(LPDRREC lpval)
{
	return DRERR_NOTSUPPORT;
}

/**************************************************************
// ��������
****************************************************************/
void CMySerialProtocol::DoRun()
{
	int nUpdateSec = m_pDrvObj->m_config.m_nUpdateSec/1000;
	long lEndTime = time(NULL);
	if (m_lStartTime != 0)
	{		
		if (lEndTime - m_lStartTime < nUpdateSec)
		{
			Sleep(10);
			return;
		}
	}

	if (!IsConnect())
	{
		if (!Connect())
			return;
	}

	m_nLinkStateMachine = S_RECIEVE_DATA;

	switch(m_nLinkStateMachine)
	{
	case S_INIT:                                    // ��ʼ״̬
	case S_LINK_REQ:                                // ����Զ����·{10 49 01 4A 16 }
		if (DoSend(S_LINK_REQ)){
			SetLinkStateMachine(S_LINK_OK);       
		}
		else
			SetLinkStateMachine(S_INIT);
		break;
	case S_LINK_OK:                                 // ȷ��Զ����·{10 0B 01 0C 16 }
		if (DoRead(S_LINK_OK)){
			SetLinkStateMachine(S_CALL_SYS_TIME);
		}
		else
			SetLinkStateMachine(S_INIT); 
		break;
	case S_RESET_REQ:                                // ����Զ����·{10 49 01 4A 16 }
		if (DoSend(S_RESET_REQ)){
			SetLinkStateMachine(S_RESET_OK);       
		}
		else
			SetLinkStateMachine(S_INIT);
		break;
	case S_RESET_OK:                                 // ȷ��Զ����·{10 0B 01 0C 16 }
		if (DoRead(S_RESET_OK)){
			SetLinkStateMachine(S_CALL_SYS_TIME);
		}
		else
			SetLinkStateMachine(S_INIT); 
		break;
	case S_CALL_SYS_TIME:                                 // ȷ��Զ����·{10 0B 01 0C 16 }
		if (DoSend(S_CALL_SYS_TIME)){
			SetLinkStateMachine(S_SYS_TIME_OK);
		}
		else
			SetLinkStateMachine(S_INIT); 
		break;
	case S_SYS_TIME_OK:      
		if (!DoRead(S_SYS_TIME_OK))
		{
			m_lStartTime = ::time(NULL);
		}
		SetLinkStateMachine(S_CALL_DATA_SECOND);
		break;
	case S_CALL_DATA_SECOND:     // ȷ��Զ����·{10 0B 01 0C 16 }
		if (DoSend(S_CALL_DATA_SECOND))
		{
			SetLinkStateMachine(S_RECIEVE_DATA);
		}
		else
			SetLinkStateMachine(S_INIT);
		break;
	case S_CALL_DATA_WHOLE:                         // ����
		if (DoSend(S_CALL_DATA_WHOLE))
		{
			SetLinkStateMachine(S_RECIEVE_DATA); 
		}
		break;
	case S_RECIEVE_DATA:                            // ��������
		DoRead(S_RECIEVE_DATA);
		SetLinkStateMachine(S_INIT); 
		break;
	default:
		break;
	}
}


/**************************************************
*��    �ܣ�����У���
*��    ����BYTE *pData ����
int iLen    �����ʼ��ַ����1��ʼ��
int iRight  �ұ���ʼ��ַ
*�� �� ֵ�� ����
*�� �� �ߣ�ZCQ
*�������ڣ�2013.06.12
**************************************************/
int CMySerialProtocol::CheckSum(BYTE *pData,int iLen,int iLeft,int iRight)
{
	if ( NULL == pData || iLeft < 1 || iRight < 1)
	{
		return -1;
	}

	if (iLen < max(iLeft,iRight) || iLen < 3)
	{
		return -1;
	}

	int iTemp = 0;
	for (int i = iLeft-1; i< iLen - iRight+1;i++)
	{
		iTemp += pData[i];
	}
	iTemp =  LOBYTE(iTemp);

	return iTemp;
}

/**************************************************
*��    �ܣ�����ֽڵ�ĳΪ0����1
*��    ����BYTE byData ����
BYTE byPos  λ�ã��׵�ַΪ0��
*�� �� ֵ��0����1 
*�� �� �ߣ�ZCQ
*�������ڣ�2013.06.12
**************************************************/
int CMySerialProtocol::ParseByte(BYTE byData,BYTE byPos)
{
	float fRadix = 2,fIndex = byPos;
	BYTE byResult = (BYTE)pow(fRadix,fIndex);

	if (byData & byResult)
	{
		return 1;
	}
	return 0;
}

/**************************************************
*��    �ܣ�֡��飨��������ʶ��У��͡������������ȵȣ�
*��    ����BYTE *pBuf  ����
int iLen    ����
char cCheckType �������
*�� �� ֵ��TRUE : �ɹ���FALSE�� ʧ��
*�� �� �ߣ�ZCQ
*�������ڣ�2013.06.12
**************************************************/
bool CMySerialProtocol::FrmBaseCheck(BYTE *pBuf, int iLen,char cCheckType)
{
	if ( NULL == pBuf || iLen < FRM_LENGTH_FIX)
	{
		return false;
	}

	if ( S_LINK_OK == cCheckType || S_RESET_OK  == cCheckType )
	{
		if (pBuf[0] != 0x10 ||  pBuf[3]!=CheckSum(pBuf,iLen,2,3) ||pBuf[4] != 0x16)
		{
			return false;
		}
	}
	else if ( S_CALL_SYS_TIME == cCheckType)
	{
		if (iLen < FRM_LENGTH_TIME)
		{
			return FALSE;
		}

		if (pBuf[0] != 0x68 || pBuf[3] != 0x68 || pBuf[iLen-2] != CheckSum(pBuf,iLen,5,3) ||pBuf[iLen -1] != 0x16)
		{
			return false;
		}
	}
	else if ( S_CALL_DATA_WHOLE == cCheckType || S_CALL_DATA_SECOND == cCheckType ||  S_CALL_DATA_FIRST == cCheckType ||  S_RECIEVE_DATA == cCheckType ) 
	{
		int iAckCount = pBuf[1] + 6;
		if (iAckCount < iLen)
		{
			return FALSE;
		}

		if (pBuf[0] != 0x68 || pBuf[3] != 0x68 || pBuf[iAckCount-2] != CheckSum(pBuf,iAckCount,5,3)||pBuf[iAckCount -1] != 0x16)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}


BOOL CMySerialProtocol::DoRead(int iTypeCode)
{
	int iSize = ReadData(m_pBuffer, TCP_BUFFER_SIZE);
	if (iSize <= 0)
	{
		Sleep(10);
		return FALSE;
	}

	if(!FrmBaseCheck(m_pBuffer,iSize,iTypeCode))
	{
		m_pDrvObj->AddError("��������֡У�����");
		return FALSE;
	}

	if ( S_LINK_OK == iTypeCode )
	{
		if (m_pBuffer[1] != 0x0b )
		{
			return FALSE;
		}
	}
	else if ( S_RESET_OK == iTypeCode)
	{
		if ( m_pBuffer[1] != 0x00 )
		{
			return FALSE;
		}
	}
	else if ( S_SYS_TIME_OK == iTypeCode)
	{
		//  PN�Ƿ���ȷ(PN =1 ������Уʱ) ;������Уʱ�����ñ���ʱ��
		if (1 ==ParseByte(m_pBuffer[8],6))
		{
			return FALSE;
		}

		// ��ȡ��վʱ�䣬�߸���λλ�������ʱ��(CP56Time2a)
		int iMilliSec = m_pBuffer[12] + (m_pBuffer[13] & 0x11);
		int nSecond = (m_pBuffer[13] >> 2);
		int nMin  = m_pBuffer[14];
		int nHour = m_pBuffer[15];
		int nDay = 0;
		BOOL bWeek = FALSE;
		if (bWeek)
		{
			nDay = (m_pBuffer[16] & 0x1F);
		}
		else
		{
			nDay = m_pBuffer[16];
		}
		int nMonth = m_pBuffer[17];
		int nYear  = m_pBuffer[18]+2000;
		COleDateTime sTime;
		sTime.SetDateTime(nYear, nMonth, nDay, nHour, nMin, nSecond);

		SYSTEMTIME systime;
		VariantTimeToSystemTime(sTime, &systime);
		CTime tm(systime);
		m_lStartTime  = tm.GetTime();
	}
	else if ( S_RECIEVE_DATA == iTypeCode)
	{
		DATAHEADER *pDATAHEADER = (LPDATAHEADER)m_pBuffer; 
		// ������Ӧ����֡FUN = 8 ���Ҵ�վ�ظ��϶�ȷ�� PN = 0 
		if ( (pDATAHEADER->btCtrl & 0x0F) == 8 && 0 == ParseByte(pDATAHEADER->btCot,6))
		{
			//  ң�ŵ㴦������( TI = 1 ����ʱ��ĵ�����Ϣ)
			if (pDATAHEADER->btType == 1)
			{
				if (!HandleDigitPoint((BYTE*)m_pBuffer,iSize,m_lStartTime))
				{
					m_pDrvObj->AddError(_T("ң�ŵ㴦��ʧ��"));
					return FALSE;
				}
			}
			// ң��㴦������ (TI = 9����ֵ, ��һ��ֵ)
			else if (pDATAHEADER->btType == 9)
			{
				if (!HandleAnologPoint((BYTE*)m_pBuffer,iSize,m_lStartTime))
				{
					m_pDrvObj->AddError(_T("ң�ŵ㴦��ʧ��"));
					return  FALSE;
				}
			}
			// ��ͼˢ��
			m_pDrvObj->FlushValue();
		}
	}
	return TRUE;
}

BOOL CMySerialProtocol::DoSend(int iTypeCode)
{
	int iMsgLenth = 0;
	BYTE byCS     = 0;
	BYTE pData[FRM_LENGTH_MAX] = {0};

	if (iTypeCode == S_LINK_REQ )
	{//����Զ����·{10 49 01 4A 16 }
		byCS = LOBYTE(0x49 + m_nAddr);
		pData[0] = 0x10;
		pData[1] = 0x49;
		pData[2] = m_nAddr;
		pData[3] = byCS;
		pData[4] = 0x16;
		iMsgLenth   = FRM_LENGTH_FIX;
	}
	else if ( iTypeCode == S_RESET_REQ )
	{//��λԶ����·{10 40 01 41 16 }
		byCS = LOBYTE(0x40 + m_nAddr);
		pData[0] = 0x10;
		pData[1] = 0x40;
		pData[2] = m_nAddr;
		pData[3] = byCS;
		pData[4] = 0x16;
		iMsgLenth   = FRM_LENGTH_FIX;
	}
	else if ( iTypeCode == S_CALL_DATA_FIRST )
	{//��λԶ����·{10 40 01 41 16 }
		byCS = LOBYTE(0x7A + m_nAddr);
		pData[0] = 0x10;
		pData[1] = 0x7A;
		pData[2] = m_nAddr;
		pData[3] = byCS;
		pData[4] = 0x16;
		iMsgLenth   = FRM_LENGTH_FIX;
	}
	else if ( iTypeCode == S_CALL_SYS_TIME )
	{//��λԶ����·{0x68,0x0F,0x0F,0x68,0x53,01,0x67,0x01,0x06,0x01,00,00,00,00,00,00,0x24,0x0B,0x05,nCS,0x16};
		byCS = LOBYTE(0x53+ m_nAddr +0x67+0x01+0x06+0x01+0x00+0x00+0x00+0x00+0x00+0x00+0x24+0x0B+0x05);
		BYTE buf[FRM_LENGTH_TIME] ={0x68,0x0F,0x0F,0x68,0x53,m_nAddr,0x67,0x01,0x06,0x01,00,00,00,00,00,00,0x24,0x0B,0x05,byCS,0x16};
		memcpy(pData,buf,FRM_LENGTH_TIME);
		iMsgLenth   = FRM_LENGTH_TIME;
	}
	else if ( iTypeCode == S_CALL_DATA_SECOND )
	{//�����û�����{10 40 01 41 16 }
		byCS = LOBYTE(0x7B + m_nAddr);
		pData[0] = 0x10;
		pData[1] = 0x7B;
		pData[2] = m_nAddr;
		pData[3] = byCS;
		pData[4] = 0x16;
		iMsgLenth   = FRM_LENGTH_FIX;
	}
	else if ( iTypeCode == S_CALL_DATA_WHOLE )
	{// ����  {68 09 09 68 88 01 64 01 0a 01 00 00 14 0d 16}
		byCS = LOBYTE(0x73+m_nAddr+0x64+0x01+0x06+0x01+0x00+0x00+0x14);
		BYTE buf[FRM_LENGTH_TIME] = {0x68,0x09,0x09,0x68,0x73,m_nAddr,0x64,0x01,0x06,0x01,0x00,0x00,0x14,byCS,0x16};
		memcpy(pData,buf,FRM_LENGTH_TIME);
		iMsgLenth   = FRM_LENGTH_TIME;
	}
	else
	{
		return FALSE;
	}

	// ��������
	int nsize = SendData(pData,iMsgLenth); 
	if (0 == nsize)
		return FALSE;

	return TRUE;
}

void CMySerialProtocol::GetAddress()
{
	CString sParam = m_config.m_szTcpUdpParam;
	sParam.Trim();
	int nFind = sParam.Find(';');
	//  ��ȡ�豸��ַ
	CString szt;
	if ((nFind > 0) || (!sParam.IsEmpty()))
	{
		szt = sParam.Left(nFind);
		m_nAddr  = atoi(szt);
		if (m_nAddr < 0 || m_nAddr > 256)
		{
			m_nAddr = 1;
		}
	}
}

/**************************************************
*��    �ܣ�������������� ��ң�ţ�
*��    ����long lTime ʱ��
*�� �� ֵ����
*�� �� �ߣ�ZCQ
*�������ڣ�2013.06.12
**************************************************/
bool CMySerialProtocol::HandleDigitPoint(BYTE *bufRev,int nSize,long lTime)
{

	BYTE *p  = bufRev;
	LPDATAHEADER pDataHeader = (LPDATAHEADER)p;

	//  SQ_TRUE = 1 ��ַ����
	if ( SQ_TRUE ==ParseByte(pDataHeader->btVSQ,7))
	{
		// ��ȡ��Ϣ�����ʼ��ַ������ǩ��ID
		BYTE btVal[2];
		ADDR *pADDR = (LPADDR)(p + sizeof(DATAHEADER));
		unsigned short usAddr= pADDR->shAddr;

		// ��ȡ��Ϣ��ĸ���
		int iItemCount = pDataHeader->btVSQ & 0x7F;

		// ��ȡ��Ϣ����������ַ
		LPDIGIT_AVALUE pValues = (LPDIGIT_AVALUE)(p + sizeof(DATAHEADER)+ sizeof(ADDR)) ;
		CTagItem *pTag = NULL;
		CString strID;

		// ѭ����ȡ���ݵ�ֵ
		for ( int k = 0; k < iItemCount; k++)
		{
			// ͨ��ID��ȡ��ǩ
			strID.Format(_T("%d"), usAddr + k - DIGIT_START_INDEX);
			pTag = m_pDrvObj->FindByID(strID);

			// �жϱ�ǩ�Ƿ����
			if (pTag)
			{
				// ��Ʒ�������ʵĵ�����Ϣ(SIQ)�����䵱ǰֵNT���θ�λ��
				float fValue = (float)ParseByte(pValues[k].bySIQ,1);

				if (m_pDrvObj->m_config.m_bLocalTime)
				{ 		
					m_pDrvObj->AddValueByID(strID, ::time(NULL), QA_OK, (float)fValue);
				}
				else
				{
					m_pDrvObj->AddValueByID(strID,lTime, QA_OK, (float)fValue);
				}
			}
		}
	}
	else // SQ = 0 ��ַ������ (û�б����ݲ�����)
	{
		// ��ȡ��Ϣ��ĸ���
		int iItemCount = pDataHeader->btVSQ & 0x7F;

		// ��ȡ��Ϣ����������ַ
		LPDIGIT_ADDR_AVALUE pValues = (LPDIGIT_ADDR_AVALUE)(p + sizeof(DATAHEADER)) ;
		CTagItem *pTag = NULL;
		CString strID;

		// ѭ����ȡ���ݵ�ֵ
		for ( int k = 0; k < iItemCount; k++)
		{
			// ͨ��ID��ȡ��ǩ
			strID.Format(_T("%d"),pValues[k].byID);
			pTag = m_pDrvObj->FindByID(strID);

			// �жϱ�ǩ�Ƿ����
			if (pTag)
			{
				// ��Ʒ�������ʵĵ�����Ϣ(SIQ)�����䵱ǰֵNT���θ�λ��
				float fValue = (float)ParseByte(pValues[k].bySIQ,1);

				if (m_pDrvObj->m_config.m_bLocalTime)
				{ 		
					m_pDrvObj->AddValueByID(strID, ::time(NULL), QA_OK, (float)fValue);
				}
				else
				{
					m_pDrvObj->AddValueByID(strID,lTime, QA_OK, (float)fValue);
				}
			}
		}
	}
	return true;
}

/**************************************************
*��    �ܣ�����ģ������� ��ң�⣩
*��    ����long lTime ʱ��
*�� �� ֵ����
*�� �� �ߣ�ZCQ
*�������ڣ�2013.06.12
**************************************************/
bool CMySerialProtocol::HandleAnologPoint(BYTE *bufRev,int nSize,long lTime)
{
	BYTE *p  = bufRev;
	LPDATAHEADER pDataHeader = (LPDATAHEADER)p;

	//  SQ_TRUE = 1 ��ַ����
	if ( SQ_TRUE == ParseByte(pDataHeader->btVSQ,7))
	{
		// ��ȡ��Ϣ��ĵ�ַ������ǩ��ID
		BYTE btVal[2];
		ADDR *pADDR = (LPADDR)(p + sizeof(DATAHEADER));
		unsigned short usAddr= pADDR->shAddr;

		// ��ȡ��Ϣ��ĸ���
		int iItemCount = pDataHeader->btVSQ & 0x7F;

		// ��ȡ��Ϣ����������ַ
		LPANALOG_AVALUE pValues = (LPANALOG_AVALUE)(p + sizeof(DATAHEADER)+ sizeof(ADDR)) ;
		CTagItem *pTag = NULL;
		CString  strID;

		// ѭ����ȡ���ݵ�ֵ
		for ( int k = 0; k < iItemCount; k++)
		{
			// ͨ��ID��ȡ��ǩ
			strID.Format(_T("%d"), usAddr + k - ANALOG_START_INDEX);
			pTag = m_pDrvObj->FindByID(strID);

			// �жϱ�ǩ�Ƿ����
			if (pTag)
			{
				// ���㸡������ֵ
				BYTE byValue[2];
				byValue[0] = pValues[k].btLowAddr;
				byValue[1] = pValues[k].btHighAddr;
				short fValue= *((short*)byValue);

				if (m_pDrvObj->m_config.m_bLocalTime)
				{ 		
					m_pDrvObj->AddValueByID(strID, ::time(NULL), QA_OK, (float)fValue);
				}
				else
				{
					m_pDrvObj->AddValueByID(strID,lTime, QA_OK, (float)fValue);
				}
			}
		}
	}
	else // SQ = 0 ��ַ������
	{
		// ��ȡ��Ϣ��ĸ���
		int iItemCount = pDataHeader->btVSQ & 0x7F;

		// ��ȡ��Ϣ����������ַ
		LPANALOG_ADDR_AVALUE pValues = (LPANALOG_ADDR_AVALUE)(p + sizeof(DATAHEADER)) ;
		CTagItem *pTag = NULL;
		CString strID;

		// ѭ����ȡ���ݵ�ֵ
		for ( int k = 0; k < iItemCount; k++)
		{
			// ͨ��ID��ȡ��ǩ
			strID.Format(_T("%d"), m_objBuffer.SwitchBytes(pValues[k].shID,FALSE));

			pTag = m_pDrvObj->FindByID(strID);
			// �жϱ�ǩ�Ƿ����
			if (pTag)
			{
				// ���㸡������ֵ
				BYTE byValue[2];
				byValue[0] = pValues[k].btLowAddr;
				byValue[1] = pValues[k].btHighAddr;
				short fValue= *((short*)byValue);

				if (m_pDrvObj->m_config.m_bLocalTime)
				{ 		
					m_pDrvObj->AddValueByID(strID,::time(NULL), QA_OK, (float)fValue);
				}
				else
				{
					m_pDrvObj->AddValueByID(strID,lTime, QA_OK, (float)fValue);
				}
			}
		}
	}
	return true;
}


/////////////////////////////////////////////////////////////////////*****************************************************************/
// CMyTCPClientProtocol Э�鴦����ʵ��
//////////////////////////////////////////////////////////////////////
CMyTCPClientProtocol::CMyTCPClientProtocol()
{
	m_nLinkStateMachine = S_INIT;
	m_lStartTime = 0;
	m_nAddr      = 1;
	m_pBuffer = new BYTE[TCP_BUFFER_SIZE];
}

CMyTCPClientProtocol::~CMyTCPClientProtocol()
{
	delete[] m_pBuffer;
}

DRVRET CMyTCPClientProtocol::WriteVar(LPDRREC lpval)
{
	return DRERR_NOTSUPPORT;
}

/**************************************************************
// ��������
****************************************************************/
void CMyTCPClientProtocol::DoRun()
{
	int nUpdateSec = m_pDrvObj->m_config.m_nUpdateSec/1000;
	long lEndTime = time(NULL);
	if (m_lStartTime != 0)
	{		
		if (lEndTime - m_lStartTime < nUpdateSec)
		{
			Sleep(10);
			return;
		}
	}

	if (!IsConnect())
	{
		if (!Connect())
			return;
	}

	m_nLinkStateMachine = S_RECIEVE_DATA;

	switch(m_nLinkStateMachine)
	{
	case S_INIT:                                    // ��ʼ״̬
	case S_LINK_REQ:                                // ����Զ����·{10 49 01 4A 16 }
		if (DoSend(S_LINK_REQ)){
			SetLinkStateMachine(S_LINK_OK);       
		}
		else
			SetLinkStateMachine(S_INIT);
		break;
	case S_LINK_OK:                                 // ȷ��Զ����·{10 0B 01 0C 16 }
		if (DoRead(S_LINK_OK)){
			SetLinkStateMachine(S_RESET_REQ);
		}
		else
			SetLinkStateMachine(S_INIT); 
		break;
	case S_RESET_REQ:                                // ����Զ����·{10 49 01 4A 16 }
		if (DoSend(S_RESET_REQ)){
			SetLinkStateMachine(S_RESET_OK);       
		}
		else
			SetLinkStateMachine(S_INIT);
		break;
	case S_RESET_OK:                                 // ȷ��Զ����·{10 0B 01 0C 16 }
		if (DoRead(S_RESET_OK)){
			SetLinkStateMachine(S_CALL_SYS_TIME);
		}
		else
			SetLinkStateMachine(S_INIT); 
		break;
	case S_CALL_SYS_TIME:                                 // ȷ��Զ����·{10 0B 01 0C 16 }
		if (DoSend(S_CALL_SYS_TIME)){
			SetLinkStateMachine(S_SYS_TIME_OK);
		}
		else
			SetLinkStateMachine(S_INIT); 
		break;
	case S_SYS_TIME_OK:      
		if (!DoRead(S_SYS_TIME_OK))
		{
			m_lStartTime = ::time(NULL);
		}
		SetLinkStateMachine(S_CALL_DATA_SECOND);
		break;
	case S_CALL_DATA_SECOND:     // ȷ��Զ����·{10 0B 01 0C 16 }
		if (DoSend(S_CALL_DATA_SECOND))
		{
			SetLinkStateMachine(S_RECIEVE_DATA);
		}
		else
			SetLinkStateMachine(S_INIT);
		break;
	case S_CALL_DATA_WHOLE:                         // ����
		if (DoSend(S_CALL_DATA_WHOLE))
		{
			SetLinkStateMachine(S_RECIEVE_DATA); 
		}
		break;
	case S_RECIEVE_DATA:                            // ��������
		DoRead(S_RECIEVE_DATA);
		SetLinkStateMachine(S_INIT); 
		break;
	default:
		break;
	}
}


/**************************************************
*��    �ܣ�����У���
*��    ����BYTE *pData ����
int iLen    �����ʼ��ַ����1��ʼ��
int iRight  �ұ���ʼ��ַ
*�� �� ֵ�� ����
*�� �� �ߣ�ZCQ
*�������ڣ�2013.06.12
**************************************************/
int CMyTCPClientProtocol::CheckSum(BYTE *pData,int iLen,int iLeft,int iRight)
{
	if ( NULL == pData || iLeft < 1 || iRight < 1)
	{
		return -1;
	}

	if (iLen < max(iLeft,iRight) || iLen < 3)
	{
		return -1;
	}

	int iTemp = 0;
	for (int i = iLeft-1; i< iLen - iRight+1;i++)
	{
		iTemp += pData[i];
	}
	iTemp =  LOBYTE(iTemp);

	return iTemp;
}

/**************************************************
*��    �ܣ�����ֽڵ�ĳΪ0����1
*��    ����BYTE byData ����
BYTE byPos  λ�ã��׵�ַΪ0��
*�� �� ֵ��0����1 
*�� �� �ߣ�ZCQ
*�������ڣ�2013.06.12
**************************************************/
int CMyTCPClientProtocol::ParseByte(BYTE byData,BYTE byPos)
{
	float fRadix = 2,fIndex = byPos;
	BYTE byResult = (BYTE)pow(fRadix,fIndex);

	if (byData & byResult)
	{
		return 1;
	}
	return 0;
}

/**************************************************
*��    �ܣ�֡��飨��������ʶ��У��͡������������ȵȣ�
*��    ����BYTE *pBuf  ����
int iLen    ����
char cCheckType �������
*�� �� ֵ��TRUE : �ɹ���FALSE�� ʧ��
*�� �� �ߣ�ZCQ
*�������ڣ�2013.06.12
**************************************************/
bool CMyTCPClientProtocol::FrmBaseCheck(BYTE *pBuf, int iLen,char cCheckType)
{
	if ( NULL == pBuf || iLen < FRM_LENGTH_FIX)
	{
		return false;
	}

	if ( S_LINK_OK == cCheckType || S_RESET_OK  == cCheckType )
	{
		if (pBuf[0] != 0x10 ||  pBuf[3]!=CheckSum(pBuf,iLen,2,3) ||pBuf[4] != 0x16)
		{
			return false;
		}
	}
	else if ( S_CALL_SYS_TIME == cCheckType)
	{
		if (iLen < FRM_LENGTH_TIME)
		{
			return FALSE;
		}

		if (pBuf[0] != 0x68 || pBuf[3] != 0x68 || pBuf[iLen-2] != CheckSum(pBuf,iLen,5,3) ||pBuf[iLen -1] != 0x16)
		{
			return false;
		}
	}
	else if ( S_CALL_DATA_WHOLE == cCheckType || S_CALL_DATA_SECOND == cCheckType ||  S_CALL_DATA_FIRST == cCheckType ||  S_RECIEVE_DATA == cCheckType ) 
	{
		int iAckCount = pBuf[1] +6;
		if (iAckCount < iLen)
		{
			return FALSE;
		}

		if (pBuf[0] != 0x68 || pBuf[3] != 0x68 || pBuf[iAckCount-2] != CheckSum(pBuf,iAckCount,5,3)||pBuf[iAckCount -1] != 0x16)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}


BOOL CMyTCPClientProtocol::DoRead(int iTypeCode)
{
	int iSize = ReadData(m_pBuffer, TCP_BUFFER_SIZE);
	if (iSize <= 0)
	{
		Sleep(10);
		return FALSE;
	}

	if(!FrmBaseCheck(m_pBuffer,iSize,iTypeCode))
	{
		m_pDrvObj->AddError("��������֡У�����");
		return FALSE;
	}

	if ( S_LINK_OK == iTypeCode )
	{
		if (m_pBuffer[1] != 0x0b )
		{
			return FALSE;
		}
	}
	else if ( S_RESET_OK == iTypeCode)
	{
		if ( m_pBuffer[1] != 0x00 )
		{
			return FALSE;
		}
	}
	else if ( S_SYS_TIME_OK == iTypeCode)
	{
		//  PN�Ƿ���ȷ(PN =1 ������Уʱ) ;������Уʱ�����ñ���ʱ��
		if (1 ==ParseByte(m_pBuffer[8],6))
		{
			return FALSE;
		}

		// ��ȡ��վʱ�䣬�߸���λλ�������ʱ��(CP56Time2a)
		int iMilliSec = m_pBuffer[12] + (m_pBuffer[13] & 0x11);
		int nSecond = (m_pBuffer[13] >> 2);
		int nMin  = m_pBuffer[14];
		int nHour = m_pBuffer[15];
		int nDay = 0;
		BOOL bWeek = FALSE;
		if (bWeek)
		{
			nDay = (m_pBuffer[16] & 0x1F);
		}
		else
		{
			nDay = m_pBuffer[16];
		}
		int nMonth = m_pBuffer[17];
		int nYear  = m_pBuffer[18]+2000;
		COleDateTime sTime;
		sTime.SetDateTime(nYear, nMonth, nDay, nHour, nMin, nSecond);

		SYSTEMTIME systime;
		VariantTimeToSystemTime(sTime, &systime);
		CTime tm(systime);
		m_lStartTime  = tm.GetTime();
	}
	else if ( S_RECIEVE_DATA == iTypeCode)
	{
		DATAHEADER *pDATAHEADER = (LPDATAHEADER)m_pBuffer; 
		// ������Ӧ����֡FUN = 8 ���Ҵ�վ�ظ��϶�ȷ�� PN = 0 
		if ( (pDATAHEADER->btCtrl & 0x0F) == 8 && 0 == ParseByte(pDATAHEADER->btCot,6))
		{
			//  ң�ŵ㴦������( TI = 1 ����ʱ��ĵ�����Ϣ)
			if (pDATAHEADER->btType == 1)
			{
				if (!HandleDigitPoint((BYTE*)m_pBuffer,iSize,m_lStartTime))
				{
					m_pDrvObj->AddError(_T("ң�ŵ㴦��ʧ��"));
					return FALSE;
				}
			}
			// ң��㴦������ (TI = 9����ֵ, ��һ��ֵ)
			else if (pDATAHEADER->btType == 9)
			{
				if (!HandleAnologPoint((BYTE*)m_pBuffer,iSize,m_lStartTime))
				{
					m_pDrvObj->AddError(_T("ң�ŵ㴦��ʧ��"));
					return  FALSE;
				}
			}
			// ��ͼˢ��
			m_pDrvObj->FlushValue();
		}
	}
	return TRUE;
}

BOOL CMyTCPClientProtocol::DoSend(int iTypeCode)
{
	int iMsgLenth = 0;
	BYTE byCS     = 0;
	BYTE pData[FRM_LENGTH_MAX] = {0};

	if (iTypeCode == S_LINK_REQ )
	{//����Զ����·{10 49 01 4A 16 }
		byCS = LOBYTE(0x49 + m_nAddr);
		pData[0] = 0x10;
		pData[1] = 0x49;
		pData[2] = m_nAddr;
		pData[3] = byCS;
		pData[4] = 0x16;
		iMsgLenth   = FRM_LENGTH_FIX;
	}
	else if ( iTypeCode == S_RESET_REQ )
	{//��λԶ����·{10 40 01 41 16 }
		byCS = LOBYTE(0x40 + m_nAddr);
		pData[0] = 0x10;
		pData[1] = 0x40;
		pData[2] = m_nAddr;
		pData[3] = byCS;
		pData[4] = 0x16;
		iMsgLenth   = FRM_LENGTH_FIX;
	}
	else if ( iTypeCode == S_CALL_DATA_FIRST )
	{//��λԶ����·{10 40 01 41 16 }
		byCS = LOBYTE(0x7A + m_nAddr);
		pData[0] = 0x10;
		pData[1] = 0x7A;
		pData[2] = m_nAddr;
		pData[3] = byCS;
		pData[4] = 0x16;
		iMsgLenth   = FRM_LENGTH_FIX;
	}
	else if ( iTypeCode == S_CALL_SYS_TIME )
	{//��λԶ����·{0x68,0x0F,0x0F,0x68,0x53,01,0x67,0x01,0x06,0x01,00,00,00,00,00,00,0x24,0x0B,0x05,nCS,0x16};
		byCS = LOBYTE(0x53+ m_nAddr +0x67+0x01+0x06+0x01+0x00+0x00+0x00+0x00+0x00+0x00+0x24+0x0B+0x05);
		BYTE buf[FRM_LENGTH_TIME] ={0x68,0x0F,0x0F,0x68,0x53,m_nAddr,0x67,0x01,0x06,0x01,00,00,00,00,00,00,0x24,0x0B,0x05,byCS,0x16};
        memcpy(pData,buf,FRM_LENGTH_TIME);
		iMsgLenth   = FRM_LENGTH_TIME;
	}
	else if ( iTypeCode == S_CALL_DATA_SECOND )
	{//�����û�����{10 40 01 41 16 }
		byCS = LOBYTE(0x7B + m_nAddr);
		pData[0] = 0x10;
		pData[1] = 0x7B;
		pData[2] = m_nAddr;
		pData[3] = byCS;
		pData[4] = 0x16;
		iMsgLenth   = FRM_LENGTH_FIX;
	}
	else if ( iTypeCode == S_CALL_DATA_WHOLE )
	{// ����  {68 09 09 68 88 01 64 01 0a 01 00 00 14 0d 16}
		byCS = LOBYTE(0x73+m_nAddr+0x64+0x01+0x06+0x01+0x00+0x00+0x14);
		BYTE buf[FRM_LENGTH_TIME] = {0x68,0x09,0x09,0x68,0x73,m_nAddr,0x64,0x01,0x06,0x01,0x00,0x00,0x14,byCS,0x16};
		memcpy(pData,buf,FRM_LENGTH_TIME);
		iMsgLenth   = FRM_LENGTH_TIME;
	}
	else
	{
		return FALSE;
	}

	// ��������
	int nsize = SendData(pData,iMsgLenth); 
	if (0 == nsize)
		return FALSE;

	return TRUE;
}

void CMyTCPClientProtocol::GetAddress()
{
	CString sParam = m_config.m_szTcpUdpParam;
	sParam.Trim();
	int nFind = sParam.Find(';');
	//  ��ȡ�豸��ַ
	CString szt;
	if ((nFind > 0) || (!sParam.IsEmpty()))
	{
		szt = sParam.Left(nFind);
		m_nAddr  = atoi(szt);
		if (m_nAddr < 0 || m_nAddr > 256)
		{
			m_nAddr = 1;
		}
	}
}

/**************************************************
*��    �ܣ�������������� ��ң�ţ�
*��    ����long lTime ʱ��
*�� �� ֵ����
*�� �� �ߣ�ZCQ
*�������ڣ�2013.06.12
**************************************************/
bool CMyTCPClientProtocol::HandleDigitPoint(BYTE *bufRev,int nSize,long lTime)
{

	BYTE *p  = bufRev;
	LPDATAHEADER pDataHeader = (LPDATAHEADER)p;

	//  SQ_TRUE = 1 ��ַ����
	if ( SQ_TRUE ==ParseByte(pDataHeader->btVSQ,7))
	{
		// ��ȡ��Ϣ�����ʼ��ַ������ǩ��ID
		BYTE btVal[2];
		ADDR *pADDR = (LPADDR)(p + sizeof(DATAHEADER));
		unsigned short usAddr= pADDR->shAddr;

		// ��ȡ��Ϣ��ĸ���
		int iItemCount = pDataHeader->btVSQ & 0x7F;

		// ��ȡ��Ϣ����������ַ
		LPDIGIT_AVALUE pValues = (LPDIGIT_AVALUE)(p + sizeof(DATAHEADER)+ sizeof(ADDR)) ;
		CTagItem *pTag = NULL;
		CString strID;

		// ѭ����ȡ���ݵ�ֵ
		for ( int k = 0; k < iItemCount; k++)
		{
			// ͨ��ID��ȡ��ǩ
			strID.Format(_T("%d"), usAddr + k - DIGIT_START_INDEX);
			pTag = m_pDrvObj->FindByID(strID);

			// �жϱ�ǩ�Ƿ����
			if (pTag)
			{
				// ��Ʒ�������ʵĵ�����Ϣ(SIQ)�����䵱ǰֵNT���θ�λ��
				float fValue = (float)ParseByte(pValues[k].bySIQ,1);

				if (m_pDrvObj->m_config.m_bLocalTime)
				{ 		
					m_pDrvObj->AddValueByID(strID, 0, QA_OK, (float)fValue);
				}
				else
				{
					m_pDrvObj->AddValueByID(strID,lTime, QA_OK, (float)fValue);
				}
			}
		}
	}
	else // SQ = 0 ��ַ������ (û�б����ݲ�����)
	{
		// ��ȡ��Ϣ��ĸ���
		int iItemCount = pDataHeader->btVSQ & 0x7F;

		// ��ȡ��Ϣ����������ַ
		LPDIGIT_ADDR_AVALUE pValues = (LPDIGIT_ADDR_AVALUE)(p + sizeof(DATAHEADER)) ;
		CTagItem *pTag = NULL;
		CString strID;

		// ѭ����ȡ���ݵ�ֵ
		for ( int k = 0; k < iItemCount; k++)
		{
			// ͨ��ID��ȡ��ǩ
			strID.Format(_T("%d"),pValues[k].byID);
			pTag = m_pDrvObj->FindByID(strID);

			// �жϱ�ǩ�Ƿ����
			if (pTag)
			{
				// ��Ʒ�������ʵĵ�����Ϣ(SIQ)�����䵱ǰֵNT���θ�λ��
				float fValue = (float)ParseByte(pValues[k].bySIQ,1);

				if (m_pDrvObj->m_config.m_bLocalTime)
				{ 		
					m_pDrvObj->AddValueByID(strID, 0, QA_OK, (float)fValue);
				}
				else
				{
					m_pDrvObj->AddValueByID(strID,lTime, QA_OK, (float)fValue);
				}
			}
		}
	}
	return true;
}

/**************************************************
*��    �ܣ�����ģ������� ��ң�⣩
*��    ����long lTime ʱ��
*�� �� ֵ����
*�� �� �ߣ�ZCQ
*�������ڣ�2013.06.12
**************************************************/
bool CMyTCPClientProtocol::HandleAnologPoint(BYTE *bufRev,int nSize,long lTime)
{
	BYTE *p  = bufRev;
	LPDATAHEADER pDataHeader = (LPDATAHEADER)p;

	//  SQ_TRUE = 1 ��ַ����
	if ( SQ_TRUE == ParseByte(pDataHeader->btVSQ,7))
	{
		// ��ȡ��Ϣ��ĵ�ַ������ǩ��ID
		BYTE btVal[2];
		ADDR *pADDR = (LPADDR)(p + sizeof(DATAHEADER));
		unsigned short usAddr= pADDR->shAddr;

		// ��ȡ��Ϣ��ĸ���
		int iItemCount = pDataHeader->btVSQ & 0x7F;

		// ��ȡ��Ϣ����������ַ
		LPANALOG_AVALUE pValues = (LPANALOG_AVALUE)(p + sizeof(DATAHEADER)+ sizeof(ADDR)) ;
		CTagItem *pTag = NULL;
		CString  strID;

		// ѭ����ȡ���ݵ�ֵ
		for ( int k = 0; k < iItemCount; k++)
		{
			// ͨ��ID��ȡ��ǩ
			strID.Format(_T("%d"), usAddr + k - ANALOG_START_INDEX);
			pTag = m_pDrvObj->FindByID(strID);

			// �жϱ�ǩ�Ƿ����
			if (pTag)
			{
				// ���㸡������ֵ
				BYTE byValue[2];
				byValue[0] = pValues[k].btLowAddr;
				byValue[1] = pValues[k].btHighAddr;
				short fValue= *((short*)byValue);

				if (m_pDrvObj->m_config.m_bLocalTime)
				{ 		
					m_pDrvObj->AddValueByID(strID, ::time(NULL), QA_OK, (float)fValue);
				}
				else
				{
					m_pDrvObj->AddValueByID(strID,lTime, QA_OK, (float)fValue);
				}
			}
		}
	}
	else // SQ = 0 ��ַ������
	{
		// ��ȡ��Ϣ��ĸ���
		int iItemCount = pDataHeader->btVSQ & 0x7F;

		// ��ȡ��Ϣ����������ַ
		LPANALOG_ADDR_AVALUE pValues = (LPANALOG_ADDR_AVALUE)(p + sizeof(DATAHEADER)) ;
		CTagItem *pTag = NULL;
		CString strID;

		// ѭ����ȡ���ݵ�ֵ
		for ( int k = 0; k < iItemCount; k++)
		{
			strID.Format(_T("%d"), pValues[k].shID);
			// ͨ��ID��ȡ��ǩ
			pTag = m_pDrvObj->FindByID(strID);
			// �жϱ�ǩ�Ƿ����
			if (pTag)
			{
				// ���㸡������ֵ
				BYTE byValue[2];
				byValue[0] = pValues[k].btLowAddr;
				byValue[1] = pValues[k].btHighAddr;
				short fValue= *((short*)byValue);

				if (m_pDrvObj->m_config.m_bLocalTime)
				{ 		
					m_pDrvObj->AddValueByID(strID,::time(NULL), QA_OK, (float)fValue);
				}
				else
				{
					m_pDrvObj->AddValueByID(strID,lTime, QA_OK, (float)fValue);
				}
				m_pDrvObj->AddDebugFmt(_T("ң��� �� %s ,ֵ: %d"), strID,fValue);
			}
		}
	}
	return true;
}

#ifdef DEBUG_2
int CMySerialProtocol::TestMsg(BYTE *byDataType)
{

	int iCount = 48;
	BYTE bufTest[] = {0x68,0x25,0x00,0x68,0x0a,0x12,0x10,0x14,0x00,0x00,0x0c,0x1c,0x0a,0xdd,0x07,0x00,0x01,0x01,0x01,0x00,0x00,0x01,0x01,0x02,0xa4,0xca,0x22,0x00,0x00,0x02,0x03,0xac,0x26,0x00,0x00,0x02,0x03,0x04,0x04,0x44,0x16,0x00,0x00,0x04,0xc3,0x64,0x16};
	memcpy(byDataType,bufTest,iCount);
	return iCount;
}
#endif