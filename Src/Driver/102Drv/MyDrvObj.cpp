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
	strcpy(lpDrvInfo->sname, "�ܺ����ݴ���ӿ�����");
	strcpy(lpDrvInfo->sdes, "CopyRight By ����Ѷ��");
	strcpy(lpDrvInfo->sver, "2.0.0.0"); //����汾
	lpDrvInfo->dwver = 0x0200;//�淶�汾
	lpDrvInfo->dwres = 0;
	return DRERR_OK;
}

//////////////////////////////////////////////////////////////////////
// CMyDrvObj class
//////////////////////////////////////////////////////////////////////

CMyDrvObj::CMyDrvObj()
:m_bListenThreadRun(FALSE)
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

	CConfigInfo::m_szHelpFile = _T("�ܺ����ݴ���ӿ�����.chm");
	CConfigInfo::m_szParamHintText = _T("�豸��ַ;��ʼ��ţ���ֹ���;   *���磺1;1,20;*");

	CTagItem::SetCSVFormatName(_T("102Drv"));
	CTagItem::AddCustomField(CUSTOMFIELD_TYPE_DOUBLE, 0, _T("ϵ��"), _T("1"), 100, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	CTagItem::AddCustomField(CUSTOMFIELD_TYPE_DOUBLE, 1, _T("ƫ��"), _T("0"), 100, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	
}

CMyDrvObj::~CMyDrvObj()
{

}

BOOL CMyDrvObj::TestConnect(CConfigInfo &config)
{
	return CDDCDrvObj::TestConnect(config);
}

// ׼������
DRVRET	CMyDrvObj::OnStart()				
{	
 	CDDCDrvObj::OnStart();
	
	StartThread();

	return DRERR_OK;
}

void	CMyDrvObj::OnStop()
{
	m_bListenThreadRun = FALSE;
	CDDCDrvObj::OnStop();
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
		Sleep(10);
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
	m_lStartTime = 0;
	m_lTime = 0;

	m_mapConnect.clear();
}

CMyProtocol::~CMyProtocol()
{
	Release();
}

BOOL  CMyProtocol::Release()
{
	CSafeLock lock(&m_Lock);
	mapSocket::iterator it = m_mapConnect.begin();
	while(it != m_mapConnect.end())
	{
		SOCKET socket = it->second;
		if(socket != INVALID_SOCKET)
			::closesocket(socket);
		//m_mapConnect.erase(it);
		it ++;
	}
	m_mapConnect.clear();

	return TRUE;
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
/*
	static BOOL bSend =FALSE;
	CSafeLock lock(&m_Lock); 
	if (m_tmAutoBreak.CheckUpdatePeriod(2*60*1000))
	{
		Disconnect();
		return  FALSE;
	}
*/
	return  TRUE;
}

// �ͻ��������߳� ֻ����һ���߳����ӡ��������̣߳���ر����߳�
void CMyProtocol::ListenClientEx()
{
	int	nClientAddrLen = sizeof(SOCKADDR);//�ͻ���ַ����
	struct sockaddr_in	addrClient;

	CString szErr;
	if(SOCKET_ERROR == ::listen(m_hSocket, 60)) 
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
	pairaddr.first  = addrClient.sin_addr.S_un.S_addr;
	pairaddr.second = addrClient.sin_port;
	//��������
	m_mapConnect[pairaddr] = hConnect;

	char * ipaddr= inet_ntoa(addrClient.sin_addr);

	szErr.Format("����������[%d]��%s<%d>,������������ %d",hConnect,ipaddr,addrClient.sin_port, m_mapConnect.size());
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
	CSafeLock lock(&m_Lock); 

	mapSocket::iterator it = m_mapConnect.begin();

	CString strMsg ,strIPinfo;


	int iIndexSockct = 0;
	while(it != m_mapConnect.end())
	{
		BYTE	pBuffer[BUF_LENGTH_TCP] = {0},pBufferTemp[256] = {0},*pBuffer2 = NULL;;
		SOCKET hConnect = it->second;
		int iSize  = Read(hConnect, pBuffer, BUF_LENGTH_TCP,1000);

		pairAddr pairaddr = it->first;
		in_addr inaddr;
		inaddr.s_addr= pairaddr.first;
		char * ipaddr= inet_ntoa(inaddr);
		strIPinfo.Format("�ͻ���%d[%d]��%s<%d>",++iIndexSockct,hConnect,ipaddr,pairaddr.second);

		if(iSize == -1 || hConnect == INVALID_SOCKET)
		{
			strMsg.Format("%s�Ͽ�����,������������ %d",strIPinfo , m_mapConnect.size()-1);
			m_pDrvObj->AddDebug(strMsg);

			::closesocket(hConnect);
			m_mapConnect.erase(it++);
			hConnect = INVALID_SOCKET;
			continue;
		}

		++it;

		if (iSize <= 0){
			Sleep(10);
			continue;}

		int iIndex = 0;
		for( iIndex = 0; iIndex <iSize && iIndex <255; iIndex++ )
		{
			if(pBuffer[iIndex]  == '<')
			{
				pBuffer2 = pBuffer+iIndex;
				break;
			}
		}

		memcpy(pBufferTemp, pBuffer,iIndex);  //�������ݵ���̬�ڴ��� ��ȻҲ������������ʽ�����������ط�.
		pBufferTemp[3]  = 0xdc;

		BOOL bSend = FALSE;
		long lTime = ::time(NULL);
		CString strDeviceID,strDeviceTime;
		TiXmlDocument xmlDoc;
		xmlDoc.Parse((const char *)pBuffer2);
		TiXmlNode* node  = xmlDoc.FirstChild("root");

		strMsg.Format(_T("%s���յ����ģ�%s"),strIPinfo,pBuffer2);
		//m_pDrvObj->AddDebugFmt(strMsg);

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
						szValue    = element->GetText();
					else if(szCol.Compare("gateway_id")== 0)
						strDeviceID = element->GetText();
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
					szValue = element->GetText();
				else if(szValue.Compare("md5")== 0)
				{
					bSend = TRUE; 
					element->SetAttribute("operation","result");

					TiXmlNode * element_old =  element->FirstChild();
					element->RemoveChild(element_old);//�ҵ���ɾ���˽ڵ�

					TiXmlElement* element_new = new TiXmlElement("result");
					TiXmlText *text =new TiXmlText("pass");
					element_new->LinkEndChild(text);
					element->LinkEndChild(element_new);
			
					AddTypeXML(eType,"result");
					m_pDrvObj->AddDebugFmt(_T("%s,�յ��ɼ���md5�����֤���ط��ַ���pass"),strIPinfo);
				}
				else if(szValue.Compare("result") == 0)
					szValue = element->GetText();
				else if(szValue.Compare("request") == 0)
				{
					bSend = TRUE; 
					element->SetAttribute("operation","sequence");

					TiXmlElement* element_new = new TiXmlElement("sequence");
					TiXmlText *text =new TiXmlText("AAAAAA");
					element_new->LinkEndChild(text);
					element->LinkEndChild(element_new);

					AddTypeXML(eType,"sequence");
					m_pDrvObj->AddDebugFmt(_T("%s,�յ��ɼ���request�������ӣ��ط��ַ���AAAAAA"),strIPinfo);
				}
			}
			else if(szCol.Compare("heart_beat")== 0) // ����/Уʱ���ݰ�
			{
				szValue = element->Attribute("operation");

				if(szValue.Compare("time") == 0)
					szValue = element->GetText();
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
					m_pDrvObj->AddDebugFmt(_T("%s,�յ��ɼ���notify�������ط�Уʱ���ݰ�%s"),strIPinfo,strDeviceTime);
				}
			}
			else if(szCol.Compare("data")== 0) // �ܺ�Զ�����ݰ�
			{
				xmlDoc.SaveFile("Rev_Data.txt");//ת�����ַ���  
				szValue = element->Attribute("operation");

				if(szValue.Compare("query") == 0)
					szValue = element->GetText();
				else if(szCol.Compare("reply")== 0)
					szValue = element->GetText();
				else if(szValue.Compare("report") == 0)
					szValue = element->GetText();
				else if(szValue.Compare("continuous")== 0)
					szValue = element->GetText();
				else if(szValue.Compare("continuous_ack")== 0)
					szValue = element->GetText();
				// 
				for( TiXmlNode *RowNode =  pNode->FirstChild();RowNode; RowNode = RowNode->NextSibling())
				{
					element = RowNode->ToElement();
					szCol = element->Value();
					if(szCol.Compare("sequence") == 0)
						szValue = element->GetText();
					else if(szCol.Compare("parser")== 0)
						szValue = element->GetText();
					else if(szCol.Compare("time") == 0)
					{
						strDeviceTime = element->GetText();
						lTime   = StrTimeToLong(strDeviceTime);
					}
					else if(szCol.Compare("total")== 0)
						szValue = element->GetText();
					else if(szCol.Compare("current") == 0)
						szValue = element->GetText();
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
				
						m_pDrvObj->AddDebugFmt(_T("%s,��ǩID��%s,ʱ���ǣ�%s,ֵ��%f "),strIPinfo,strID,strDeviceTime,fValue);

						CTagItem * pTag = m_pDrvObj->FindByID(strID);
						if (!pTag)             
						{
							m_pDrvObj->AddDebugFmt(_T("%s,��ǩID��%s  ������"),strIPinfo,strID);
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
		
		if(!bSend)	continue; 

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

		int iLen =  str.GetLength() + 1;
		BYTE* RPstr = new BYTE[iLen];    
		memcpy(RPstr, str.GetBuffer(),iLen);  //�������ݵ���̬�ڴ��� ��ȻҲ������������ʽ�����������ط�.
		CString strValue(RPstr);

		strMsg.Format(_T("%s,���ͱ��ģ�%s"),strIPinfo,RPstr);
		m_pDrvObj->AddDebug(strMsg);

		int nsize = Write(hConnect, RPstr, iLen,1000);
		if (0 == nsize)
			continue;

		Sleep(10);
	}
}
