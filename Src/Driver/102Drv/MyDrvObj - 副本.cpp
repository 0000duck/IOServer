// MyDrvObj.cpp: implementation of the CMyDrvObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "102Drv.h"
#include "MyDrvObj.h"


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
	strcpy(lpDrvInfo->sname, "102�ӿ�����");
	strcpy(lpDrvInfo->sdes, "CopyRight By �����ȹ��о�Ժ���޹�˾");
	strcpy(lpDrvInfo->sver, "2.0.0.0"); //����汾
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
	SetSerializeID(0x10000011);

	// ����ͨѶЭ�����ͣ����֧�ֶ�Э�飬������� | �Ļ�������Ӷ�Э��������
	// SetComTypes(COM_TYPE_UDP | COM_TYPE_TCP);
	// Ȼ����CreateProtocol()�ӿ��и����û�ѡ���Э��������Э�鴦�������ʵ��

#ifdef USE_PROTOCOL_TCP 
	SetComTypes(COM_TYPE_TCPSERV);
#else  
	SetComTypes(COM_TYPE_COM);
#endif 
	
	EnabledTestConnect();

	CConfigInfo::m_szHelpFile = _T("102�ӿ�����.chm");
	CConfigInfo::m_szParamHintText = _T("�豸��ַ;��ʼ��ţ���ֹ���;   *���磺1;1,20;*");

	CTagItem::SetCSVFormatName(_T("102Drv"));
	CTagItem::AddCustomField(CUSTOMFIELD_TYPE_DOUBLE, 0, _T("ϵ��"), _T("0.0001"), 100, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	CTagItem::AddCustomField(CUSTOMFIELD_TYPE_DOUBLE, 1, _T("ƫ��"), _T("0"), 100, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	
}

CMyDrvObj::~CMyDrvObj()
{

}

BOOL CMyDrvObj::TestConnect(CConfigInfo &config)
{
	return CDDCDrvObj::TestConnect(config);
}

// �ͻ��������߳� ֻ����һ���߳����ӡ��������̣߳���ر����߳�
UINT CMyDrvObj::OnListenThread(LPVOID pParam)
{
	CMyDrvObj* pDrvObj = reinterpret_cast<CMyDrvObj*>(pParam);
	CMyProtocol* pSvc = (CMyProtocol *) pDrvObj->m_pProtocol;
	if (NULL == pDrvObj)
	{
		return 0;
	}

	while(pDrvObj->m_bListenThreadRun/* !pDrvObj->IsKillThread()*/ && NULL != pSvc )
	{
		pSvc->ListenClientEx();
		//Sleep(10);
	}
	return 1;
}

BOOL CMyDrvObj::StartThread()
{
	if (m_bListenThreadRun)
		return m_bListenThreadRun;

	m_bListenThreadRun = TRUE;

	// �ͻ��������߳� ֻ����һ���߳����ӡ��������̣߳���ر����߳�

	AfxBeginThread(OnListenThread, this, THREAD_PRIORITY_NORMAL);

	return m_bListenThreadRun;
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
	pObj = new CMyProtocol();

	if (pObj)
		InitTagIdMap();

	return pObj;
}

//////////////////////////////////////////////////////////////////////
// CMyProtocol Э�鴦����ʵ��
//////////////////////////////////////////////////////////////////////
CMyProtocol::CMyProtocol()
{
    m_nLinkStateMachine = S_INIT;
	m_lStartTime = 0;
	m_nAddr      =  	  1;
	m_iEndElecCode  = 255;
	m_iStartElecCode = 1;
	//m_pBuffer = new BYTE[TCP_BUFFER_SIZE];
	m_lTime = 0;
	m_bReSend = TRUE;
	m_strPsw    = "";
}

CMyProtocol::~CMyProtocol()
{
	// free(m_pBuffer);
}

DRVRET CMyProtocol::WriteVar(LPDRREC lpval)
{
	return DRERR_NOTSUPPORT;
}


BOOL CMyProtocol::IsCanDo()
{
	int nUpdateSec = m_pDrvObj->m_config.m_nUpdateSec/1000;
	long lEndTime = time(NULL);
	if (m_lStartTime != 0)
	{		
		if (lEndTime - m_lStartTime < nUpdateSec)
		{
			Sleep(10);
			return  FALSE;
		}
	}

	if (!IsConnect())
	{
		m_pDrvObj->AddDebugFmt(_T("��������ʧ�ܣ�������������ò���"));
		if (!Connect())
			return  FALSE;
	}

	GetAddress();

	static BOOL bSend =FALSE;
	CSafeLock lock(&m_Lock); 
	if (m_tmAutoBreak.CheckUpdatePeriod(2*60*1000))
	{
		Disconnect();
		m_bReSend =TRUE;
		SetLinkStateMachine(S_INIT); 
		return  FALSE;
	}

	return  TRUE;
}

// �ͻ��������߳� ֻ����һ���߳����ӡ��������̣߳���ر����߳�
void CMyProtocol::ListenClientEx()
{
	int	nClientAddrLen = sizeof(SOCKADDR);//�ͻ���ַ����
	struct sockaddr_in	addrClient;

	CString szErr;
	if(SOCKET_ERROR == ::listen(m_hSocket, 5)) 
	{
		szErr.Format("�����˿�[%d]ʧ��! Err = %d",m_pDrvObj->m_config.m_wPort,WSAGetLastError());
		m_pDrvObj->AddError(szErr);
		Sleep(100);
	}
	//////////////////////////////////////////////////////////////////////////
	// ��סǰһ���ͻ��ˣ�����ʱʹ��
	//memcpy(&addrClient,&(m_addrClient),nClientAddrLen);

	SOCKET hConnect = ::accept(m_hSocket, (struct sockaddr *)(&(addrClient)), &nClientAddrLen);


	if(INVALID_SOCKET == hConnect)
	{
		return;
	}
	pairAddr pairaddr;
	pairaddr.first = addrClient.sin_addr.S_un.S_addr;
	pairaddr.second = addrClient.sin_port;
	//��������
	m_mapConnect[pairaddr] = hConnect;

	szErr.Format("���������� %d", m_mapConnect.size());
	m_pDrvObj->AddDebug(szErr);
}

long CMyProtocol::StrTimeToLong(CString strTime)
{
	long lTime =::time(NULL);

	if(strTime.IsEmpty() || strTime.GetLength()< 14)
		return lTime;

	int iYear,iMonth,iDate,iHour,iMinite,iSec;

	iYear   =   atoi(strTime.Left(4));
	iMonth  =   atoi(strTime.Mid(4,2));
	iDate   =   atoi(strTime.Mid(6,2));
	iHour   =   atoi(strTime.Mid(8,2));
	iMinite =   atoi(strTime.Mid(10,2));
	iSec    =   atoi(strTime.Mid(12,2));

	CTime tm(iYear,iMonth,iDate,iHour,iMinite,iSec);
	lTime = (long)tm.GetTime();
	return lTime;
}

CString  CMyProtocol::LongTimeToStr(long lTime)
{
	CTime tm(lTime);
	return tm.Format("%Y%m%d%H%M%S");
}

void  CMyProtocol::AddTypeXML(TiXmlElement* eType,CString str)
{
	TiXmlElement* Temp = eType->ToElement();
	//�滻
	TiXmlNode * TType = Temp->FirstChild();
	TiXmlText newText(str);
	Temp->ReplaceChild(TType,newText);
}
/**************************************************************
// ��������
****************************************************************/
void CMyProtocol::DoRun()
{
	if (!IsCanDo())   {  return;   }
	
	// �����ͻ������ӣ�������ֻ�������һ���ͻ�������
	if (!IsAccept())
	{
		ListenClient();
		Sleep(10);
		return;
	}

	CSafeLock lock(&m_Lock); 
	BYTE	pBuffer[TCP_BUFFER_SIZE] = {0},pBufferTemp[256] = {0};
	BYTE	*pBuffer2 = NULL;

	int iSize = ReadData(pBuffer, TCP_BUFFER_SIZE);
	if (iSize <= 0){
		Sleep(10);
		return;}

	int iFact = 0;
	for(int i = 0; i <iSize; i++ )
	{
		if(pBuffer[i]  == '<')
		{
			pBuffer2 = pBuffer+i;
			break;
		}
		iFact++;
	}

	memcpy(pBufferTemp, pBuffer,iFact);  //�������ݵ���̬�ڴ��� ��ȻҲ������������ʽ�����������ط�.

	BOOL bSend = FALSE;
	long lTime = ::time(NULL);
	CString strDeviceID,strDeviceTime;
	TiXmlDocument xmlDoc;
	xmlDoc.Parse((const char *)pBuffer2);
	TiXmlNode* node  = xmlDoc.FirstChild("root");

	m_pDrvObj->AddDebugFmt(_T("�յ����ģ�%s"),pBuffer2);
	TiXmlElement* eType    = NULL;

	for(TiXmlNode *pNode =  node->FirstChild();pNode; pNode = pNode->NextSibling())
	{
		TiXmlElement* element = pNode->ToElement();
		
		CString szCol = element->Value(),szValue;
		if(szCol.Compare("common") == 0)
		{
			szValue = element->GetText();
			for( TiXmlNode *RowNode =  pNode->FirstChild();RowNode; RowNode = RowNode->NextSibling())
			{
				element = RowNode->ToElement();
				szCol = element->Value();
				if(szCol.Compare("building_id") == 0)
				{
					szValue    = element->GetText();
				}
				else if(szCol.Compare("gateway_id")== 0)
				{
					strDeviceID = element->GetText();
				}
				else if(szCol.Compare("type") == 0)
				{
					szValue     = element->GetText();
					eType       =   element;
				}
			}
		}
		else if(szCol.Compare("id_validate")== 0) // �����֤
		{
			szValue = element->Attribute("operation");

		    if(szValue.Compare("sequence") == 0)
			{
				szValue = element->GetText();
			}
			else if(szValue.Compare("md5")== 0)
			{
				bSend = TRUE; 
				element->SetAttribute("operation","result");
				TiXmlNode * element_old =  element->FirstChild();
				TiXmlText text("pass");
				element->ReplaceChild(element_old,text);

				AddTypeXML(eType,"result");
				m_pDrvObj->AddDebugFmt(_T("�յ��ɼ���md5�����֤���ط��ַ���pass"));
			}
			else if(szValue.Compare("result") == 0)
			{
				szValue = element->GetText();
			}
			else if(szValue.Compare("request") == 0)
			{
				bSend = TRUE; 
				element->SetAttribute("operation","sequence");

				TiXmlElement* element_new = new TiXmlElement("sequence");
				TiXmlText *text =new TiXmlText("AAAAAA");
				element_new->LinkEndChild(text);
				element->LinkEndChild(element_new);

				AddTypeXML(eType,"sequence");
				m_pDrvObj->AddDebugFmt(_T("�յ��ɼ���sequence�������ӣ��ط��ַ���AAAAAA"));
			}
		}
		else if(szCol.Compare("heart_beat")== 0) // ����/Уʱ���ݰ�
		{
			szValue = element->Attribute("operation");

		    if(szValue.Compare("time") == 0)
			{
				szValue = element->GetText();
			}
			else if(szValue.Compare("notify")== 0)
			{
				bSend = TRUE; 
				element->SetAttribute("operation","time");

				strDeviceTime = LongTimeToStr(::time(NULL));
				TiXmlElement* element_new = new TiXmlElement("time");
				TiXmlText *text =new TiXmlText(strDeviceTime);
				element_new->LinkEndChild(text);
				element->LinkEndChild(element_new);

				AddTypeXML(eType,"time");
				m_pDrvObj->AddDebugFmt(_T("�յ��ɼ���notify�������ط�Уʱ���ݰ�%s"),strDeviceTime);
			}
		}
		else if(szCol.Compare("data")== 0) // �ܺ�Զ�����ݰ�
		{
			szValue = element->Attribute("operation");

		    if(szValue.Compare("query") == 0)
			{
				szValue = element->GetText();
			}
			else if(szCol.Compare("reply")== 0)
			{
				szValue = element->GetText();
			}
			else if(szValue.Compare("report") == 0)
			{
				szValue = element->GetText();
			}
			else if(szValue.Compare("continuous")== 0)
			{
				szValue = element->GetText();
			}
			else if(szValue.Compare("continuous_ack")== 0)
			{
				szValue = element->GetText();
			}
			// 
			for( TiXmlNode *RowNode =  pNode->FirstChild();RowNode; RowNode = RowNode->NextSibling())
			{
				element = RowNode->ToElement();
				szCol = element->Value();
				if(szCol.Compare("sequence") == 0)
				{
					szValue = element->GetText();
				}
				else if(szCol.Compare("parser")== 0)
				{
					szValue = element->GetText();
				}
				else if(szCol.Compare("time") == 0)
				{
					strDeviceTime = element->GetText();
					lTime   = StrTimeToLong(strDeviceTime);
				}
				else if(szCol.Compare("total")== 0)
				{
					szValue = element->GetText();
				}
				else if(szCol.Compare("current") == 0)
				{
					szValue = element->GetText();
				}
			}

			CString strMeterID,strFuncID,strID;
			float fValue = 0;
			// ����
		    for (TiXmlNode *RowNode =  pNode->FirstChild("meter"); RowNode; RowNode = RowNode->NextSibling("meter"))
			{
				element = RowNode->ToElement();
				strMeterID = element->Attribute("id");
		
				for (TiXmlNode *RowNodeT =  RowNode->FirstChild("function"); RowNodeT; RowNodeT = RowNodeT->NextSibling("function"))
				{
					element = RowNodeT->ToElement();
					strFuncID = element->Attribute("id");
					szValue = element->Attribute("coding");
					szValue = element->Attribute("error");
					szValue = element->GetText();
					fValue = atof(szValue);

					strID.Format("%s_%s_%s",strDeviceID,strMeterID,strFuncID);
			
					m_pDrvObj->AddDebugFmt(_T("��ǩID��%s,ʱ���ǣ�%s,ֵ��%f "),strID,strDeviceTime,fValue);

					CTagItem * pTag = m_pDrvObj->FindByID(strID);
					if (!pTag)             
					{
						m_pDrvObj->AddDebugFmt(_T("��ǩID��%s  ������"),strID);
						continue;
					}

					if (m_pDrvObj->m_config.m_bLocalTime) 		
						m_pDrvObj->AddValueByID(strID, 0, QA_OK, (float)fValue);
					else
						m_pDrvObj->AddValueByID(strID, lTime, QA_OK,  (float)fValue);

				}
			}
			//
		}
	}

	m_pDrvObj->FlushValue();
	
	if(!bSend)	return; 

	TiXmlPrinter Pstr;  
	xmlDoc.Accept(&Pstr);//ת�����ַ���  

	CString str(pBufferTemp);
	str = str + Pstr.CStr();

	str.Replace("          ","");
	str.Replace("         ","");
	str.Replace("        ","");
	str.Replace("       ","");
	str.Replace("      ","");
	str.Replace("     ","");
	str.Replace("    ","");
	str.Replace("   ","");
	str.Replace("  ","");
	//const char *TempPstr = Pstr.CStr();//��ָ��ȡ��    
	//int len = strlen(TempPstr);//���㳤��      
	//if(len == 0) return;  

	int iLen =  str.GetLength() + 1;
	BYTE* RPstr = new BYTE[iLen];    
	memcpy(RPstr, str.GetBuffer(),iLen);  //�������ݵ���̬�ڴ��� ��ȻҲ������������ʽ�����������ط�.
	CString strValue(RPstr);

	int nsize = SendData(RPstr,iLen); 

	m_pDrvObj->AddDebugFmt(_T("���ͱ��ģ�%s"),RPstr);

	if (0 == nsize)
		return;

	Sleep(10);
}


void CMyProtocol::DoRead()
{
;
}

BOOL CMyProtocol::DoSend(int iTypeCode)
{
	
	BYTE sendBuf[SEND_MAX_FRM] = {0x68,0x0,0x0,0x68,0};
	int iMsgLenth = 0;

	if (iTypeCode == S_REQ_LINK || 
		iTypeCode == S_RESENT_LINK || 
		iTypeCode == S_LEVER1_DATA  || 
		iTypeCode == S_LEVER2_DATA)
	{
		LPFIXFRM  pFIXFRM = (LPFIXFRM)sendBuf;
		pFIXFRM->btSFlag  = 0x10;

		if  (iTypeCode == S_REQ_LINK)
		{  
			pFIXFRM->btCtrl     = 0x49;
		}
		else if  (iTypeCode == S_RESENT_LINK)
			pFIXFRM->btCtrl     = 0x60;
		else if  (iTypeCode == S_LEVER1_DATA)
		{
			if (m_bReSend)
				  pFIXFRM->btCtrl     = 0x5A;
			else
			      pFIXFRM->btCtrl     = 0x7A;
			m_bReSend = !m_bReSend;
		}
		else if  (iTypeCode == S_LEVER2_DATA)
		{
			if (m_bReSend)
				pFIXFRM->btCtrl     = 0x5B;
			else
				pFIXFRM->btCtrl     = 0x7B;
			m_bReSend = !m_bReSend;
		}

		pFIXFRM->wAddr   = m_nAddr;
		pFIXFRM->pEndFrm.btCS = CMyFrmParse::CalaCS(&pFIXFRM->btCtrl,3);
		pFIXFRM->pEndFrm.btEFlag  = 0x16;
		pFIXFRM->btSFlag  = 0x10;
		// ����
		iMsgLenth   =  LEN_FIX_FRM;
	}
	else if (iTypeCode == S_CHEK_PSW_REQ )
	{// 
		VARIHEADER *pVARIHEADER = (LPVARIHEADER)sendBuf;
		pVARIHEADER->btCtrl     = 0x43;
		pVARIHEADER->wAddr   = 0xFF;
		LPASDUHEADER    pASDUHEADER = (LPASDUHEADER)(sendBuf +sizeof(VARIHEADER));
		pASDUHEADER->btTI     =0x7D;
		pASDUHEADER->btVSQ = 0x01;
		pASDUHEADER->btCOT = 0x05;
		pASDUHEADER->wElecAddr = 0x01;

		BYTE  *pPsw = (sendBuf +sizeof(VARIHEADER) +sizeof(ASDUHEADER) );
		pPsw[0] = m_strPsw.GetLength();
		memcpy((pPsw+1),m_strPsw,m_strPsw.GetLength());

		LPENDFRM pENDFRM = (LPENDFRM)(sendBuf +sizeof(VARIHEADER)  + sizeof(ASDUHEADER) + m_strPsw.GetLength() +1);
		pENDFRM->btCS = CMyFrmParse::CalaCS(&pVARIHEADER->btCtrl,9+ m_strPsw.GetLength() +1);
		pENDFRM->btEFlag = 0x16;

		iMsgLenth   =  sizeof(VARIHEADER)  + sizeof(ASDUHEADER) + m_strPsw.GetLength() +1 + sizeof(ENDFRM);

		pVARIHEADER->btLen   = iMsgLenth -6;
		pVARIHEADER->btLen2 = 0;
	}
	else if (iTypeCode == S_DATA_REQ )
	{// 
		VARIHEADER *pVARIHEADER = (LPVARIHEADER)sendBuf;
		pVARIHEADER->btCtrl =  0x53;//0x73;
		pVARIHEADER->btLen  = 0x0B;
		pVARIHEADER->btLen2 = 0x0B;
		pVARIHEADER->wAddr = m_nAddr;
		LPASDUHEADER    pASDUHEADER = (LPASDUHEADER)(sendBuf +sizeof(VARIHEADER));
		pASDUHEADER->btTI     = 0xE6;//0x78;
		pASDUHEADER->btVSQ = 0x01;
		pASDUHEADER->btCOT = 0x06;
		pASDUHEADER->wElecAddr  = 0x01;
		pASDUHEADER->byItemNum = 0x9F;// ����102

		BYTE *pData =(BYTE *)(sendBuf +sizeof(VARIHEADER)+sizeof(ASDUHEADER));
		pData[0] = m_iStartElecCode; pData[1] = m_iEndElecCode; // ������ʼ����ֹ���
/*
		TIME_A *pTIME_A_From = (LPTIME_A)(sendBuf +sizeof(VARIHEADER)+sizeof(ASDUHEADER) + 2);
	
		long lTime = ::time(NULL);
		LongToTime(lTime - 60*5,pTIME_A_From);
		OutMsg((BYTE *)pTIME_A_From, sizeof(TIME_A),"ʱ��From",0);
		TIME_A *pTIME_A_To = (LPTIME_A)(sendBuf +sizeof(VARIHEADER)+sizeof(ASDUHEADER) + 2 +sizeof(TIME_A));
		LongToTime(lTime,pTIME_A_To);
		OutMsg((BYTE *)pTIME_A_To, sizeof(TIME_A),"ʱ��To",0);
*/

		LPENDFRM pENDFRM = (LPENDFRM)(sendBuf +sizeof(VARIHEADER)+sizeof(ASDUHEADER) + 2) ;
		pENDFRM->btCS = CMyFrmParse::CalaCS(&pVARIHEADER->btCtrl,11);
		pENDFRM->btEFlag = 0x16;
		iMsgLenth   =  17;
	}
	else if (iTypeCode == S_TIME_REQ )
	{// 
		VARIHEADER *pVARIHEADER = (LPVARIHEADER)sendBuf;
		pVARIHEADER->btCtrl = 0x73;
		pVARIHEADER->btLen = 0x09;
		pVARIHEADER->btLen2 = 0x09;
		pVARIHEADER->wAddr = m_nAddr;
		LPASDUHEADER    pASDUHEADER = (LPASDUHEADER)(sendBuf +sizeof(VARIHEADER));
		pASDUHEADER->btTI     = 0x67;
		pASDUHEADER->btCOT = 0x05;
		
		LPENDFRM pENDFRM = (LPENDFRM)(sendBuf +sizeof(VARIHEADER)  + sizeof(ASDUHEADER));
		pENDFRM->btCS = CMyFrmParse::CalaCS(&pVARIHEADER->btCtrl,9);
		pENDFRM->btEFlag = 0x16;
		iMsgLenth   =  15;
	}
	else
	{
		return FALSE;
	}

	CSafeLock lock(&m_Lock); 
	int nsize = SendData(sendBuf,iMsgLenth); 
	if (0 == nsize)
		return FALSE;

    Sleep(20);
	return TRUE;
}

void     CMyProtocol::PackData(BYTE *pData, int iSize)
{
	if (pData == NULL || iSize < 1)  {       return;             }

	LPASDUHEADER    pASDUHEADER = (LPASDUHEADER)(pData +sizeof(VARIHEADER));
	LPVARIHEADER     pVARIHEADER  = (LPVARIHEADER)pData;
	int    iNum = pASDUHEADER->btVSQ &0x7F;
	BYTE *pMsg = NULL;

	CWin32Heap stringHeap( HEAP_NO_SERIALIZE, 0, 0 );
	CAtlStringMgr stringMgr( &stringHeap );
	CString szID(&stringMgr);

	if ( 0x02 == pASDUHEADER->btTI)
	{// ��������

		ELEC_DATA *pELEC_DATA =  (ELEC_DATA *)(pData +sizeof(VARIHEADER) +sizeof(ASDUHEADER));

		TIME_A *pTIME_A = (LPTIME_A)(pData +sizeof(VARIHEADER) +sizeof(ASDUHEADER) + sizeof(ELEC_DATA)*iNum);
		long lTime =TimeAToLong(pTIME_A);

		if (lTime <=0) { return;  }

		for (int k = 0; k < iNum ; k++)
		{
			pELEC_DATA =  (ELEC_DATA *)(pData +sizeof(VARIHEADER) +sizeof(ASDUHEADER) + sizeof(ELEC_DATA)*k);

			int iT = pELEC_DATA->wElecTI;
			for (int i= 0 ; i < ELE_NUM; i++)
			{
				szID.Format("%d_%d",iT,i);
				//OutMsg((BYTE*)pELEC_DATA,sizeof(ELEC_DATA),szID,(float)pELEC_DATA[i].fValue);
				CTagItem * pTag = m_pDrvObj->FindByID(szID);
				if (!pTag)             
				{
					m_pDrvObj->AddDebugFmt(_T("��ǩID��%s  ������"),szID);
					continue;
				}

				float fValue = pELEC_DATA->Value[i].dwValue; /** pTag->m_dblField1 + pTag->m_dblField2*/;
				if (m_pDrvObj->m_config.m_bLocalTime) 		
					m_pDrvObj->AddValueByID(szID, 0, QA_OK, (float)fValue);
				else
					m_pDrvObj->AddValueByID(szID, lTime, QA_OK,  (float)fValue);

			}
		}
	}

	m_pDrvObj->FlushValue();
}

long     CMyProtocol::TimeAToLong(TIME_A   *pTIME_A)
{
	long lTime = 0 ;
	if (pTIME_A == NULL )  {       return              lTime;             }

	int btYear,btMonth,btDate,btHour,btMinute;
	btYear       = (pTIME_A->btYear &0x7F) + 2000;
	btMonth    = pTIME_A->btMonth & 0x0F;
	btDate       = pTIME_A->btDate& 0x1F;
	btHour      = pTIME_A->btHour& 0x1F;
	btMinute   = pTIME_A->btMinute& 0x3F;

	if (btYear >= 1900 && (btMonth >= 1 && btMonth <= 12) && (btDate >= 1 && btDate <= 31) && ( btHour >= 0 && btHour <= 23 ) && ( btMinute >= 0 && btMinute <= 59))
	{
		CTime tm(btYear,btMonth,btDate,btHour,btMinute,0);
		lTime =  tm.GetTime();
	}
	else
	{
		m_pDrvObj->AddDebugFmt(_T(" TimeAToLong ERROR!"));
		lTime =  0;  
		m_bReSend =FALSE;
		SetLinkStateMachine(S_INIT); 
	}

	try  
	{ 
	    CTime tm(btYear,btMonth,btDate,btHour,btMinute,0);
		lTime =  tm.GetTime();
	}
	catch(...)  
	{  
		m_pDrvObj->AddDebugFmt(_T(" TimeAToLong ERROR!"));
		lTime =  0;  
		Disconnect(); 
		m_bReSend =TRUE;
		SetLinkStateMachine(S_INIT); 
	}
	return lTime;
}

long     CMyProtocol::TimeBToLong(TIME_B   *pTIME_B)
{
	long lTime = 0;
	if (pTIME_B == NULL )  {       return              lTime;             }

	int btYear,btMonth,btDate,btHour,btMinute,btSecond;
	btYear       = pTIME_B->btYear + 2000;
	btMonth    = pTIME_B->btMonth & 0x0F;
	btDate       = pTIME_B->btDate& 0x1F;
	btHour      = pTIME_B->btHour& 0x1F;
	btMinute   = pTIME_B->btMinute& 0x3F;
	btSecond  = (pTIME_B->btSecond& 0xFC)>>2;

	if (btYear >= 1900 && (btMonth >= 1 && btMonth <= 12) && (btDate >= 1 && btDate <= 31) && ( btHour >= 0 && btHour <= 23 ) && ( btMinute >= 0 && btMinute <= 59))
	{
		CTime tm(btYear,btMonth,btDate,btHour,btMinute,0);
		lTime =  tm.GetTime();
	}
	else
	{
		m_pDrvObj->AddDebugFmt(_T(" TimeAToLong ERROR!"));
		lTime =  0;  
		m_bReSend =TRUE;
		SetLinkStateMachine(S_INIT); 
	}

	try  
	{ 
		CTime tm(btYear,btMonth,btDate,btHour,btMinute,btSecond);
		lTime =  tm.GetTime();
	}
	catch(...)  
	{  
		m_pDrvObj->AddDebugFmt(_T(" TimeBToLong ERROR!"));
		lTime =  0;  
		Disconnect(); 
		m_bReSend =TRUE;
		SetLinkStateMachine(S_INIT); 
	}

	return lTime;
}

void     CMyProtocol::LongToTime(long lTime,TIME_A *pTIME_A)
{
	if (!pTIME_A) {  return;     }

	if (lTime <= 0 ) {  lTime = ::time(NULL);     }

	CTime tm(lTime);
	pTIME_A->btYear    = tm.GetYear() - 2000;
	pTIME_A->btMonth = tm.GetMonth();
	pTIME_A->btDate    = tm.GetDay();
	pTIME_A->btHour   = tm.GetHour();
	pTIME_A->btMinute= tm.GetMinute();

// 	BYTE btDay = 0;
// 	if (TRUE)//ʱ�������
// 	{
// 		BYTE bBegin = tm.GetDayOfWeek();
// 		BYTE bWeekDayBegin = 0;
// 		if(bBegin <= 3)
// 			bWeekDayBegin = (bBegin << 6);
// 		else
// 			bWeekDayBegin = (bBegin << 5);
// 
// 		BYTE bb1 = tm.GetDay();
// 		btDay = bWeekDayBegin | bb1;
// 	}
// 	else
// 	{
// 		btDay = tm.GetDay();
// 	}
// 
// 	pTIME_A->btDate = btDay;
}

void CMyProtocol::GetAddress()
{
	CWin32Heap stringHeap( HEAP_NO_SERIALIZE, 0, 0 );
	CAtlStringMgr stringMgr( &stringHeap );
	CString sParam(&stringMgr);

#ifdef USE_PROTOCOL_TCP 
	sParam= m_config.m_szTcpUdpParam;
#else  
   sParam	= m_config.m_szComParam;
#endif 

	sParam.Trim();
	if (sParam.IsEmpty()){
		return; }

	int nFind = sParam.Find(';');
	//  ��ȡ�豸��ַ
	CString szt;
	if ((nFind > 0) || (!sParam.IsEmpty()))
	{
		szt = sParam.Left(nFind);
		int nAddr = atoi(szt);
		if (nAddr > 0 && nAddr < 65535)
			m_nAddr = nAddr;
	}

	sParam = sParam.Right(sParam.GetLength()-nFind-1);
	nFind = sParam.Find(';');
	// ������ʼ�ͽ������
	if (nFind > 0 && !sParam.IsEmpty())
	{
		int nTemp = sParam.Find(',');
		szt = sParam.Left(nFind);
		if (nTemp > 0&& !szt.IsEmpty())
		{
			m_iEndElecCode   = atoi(szt.Right(szt.GetLength()-nTemp-1));
			m_iStartElecCode  = atoi(szt.Left(nTemp));
		}
	}

	sParam = sParam.Right(sParam.GetLength()-nFind-1);
	nFind = sParam.Find(';');
	// ��������
	if (nFind > 0 && !sParam.IsEmpty())
	{
		szt = sParam.Left(nFind);
		m_strPsw = szt;
	}
}

#define  MSG_MAX_SIZE   256
void  CMyProtocol:: OutMsg(BYTE *pData, int iSize,CString strID,float fValue)
{
	return;

	if (iSize > MSG_MAX_SIZE){
		iSize = MSG_MAX_SIZE;}

	BYTE Msg[MSG_MAX_SIZE];
	
	CWin32Heap stringHeap( HEAP_NO_SERIALIZE, 0, 0 );
	CAtlStringMgr stringMgr( &stringHeap );
	CString strMsg(&stringMgr), strTemp(&stringMgr);

	memcpy(Msg,pData,iSize);
	for (int m = 0 ;m < iSize;m++)
	{
		strTemp.Format("%02x",Msg[m]);
		strMsg += strTemp + " ";
	}

	m_pDrvObj->AddDebugFmt(_T("��ǩID��%s ,���ģ�%s, ֵ: %f"),strID,strMsg,fValue);
}
