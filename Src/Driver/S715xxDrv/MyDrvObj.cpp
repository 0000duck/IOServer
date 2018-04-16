// MyDrvObj.cpp: implementation of the CMyDrvObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "S715xxDrv.h"
#include "MyDrvObj.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//BOOL LessStar( const LPTDataBlock &v1, const LPTDataBlock &v2)
//{
//	return v1->nStart < v2->nStart; //����
//}


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
	strcpy(lpDrvInfo->sdes, "CopyRight By �����Ƽ�");
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
	SetComTypes(COM_TYPE_TCP);
#else  
	SetComTypes(COM_TYPE_COM);
#endif 
	
	EnabledTestConnect();

	CConfigInfo::m_szHelpFile = _T("�ܺ����ݴ���ӿ�����.chm");
	CConfigInfo::m_szParamHintText = _T("�豸��ַ;��ʼ��ţ���ֹ���;   *���磺1;1,20;*");

	CTagItem::SetCSVFormatName(_T("S715xxDrv"));
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


	return DRERR_OK;
}

void	CMyDrvObj::OnStop()
{
	CDDCDrvObj::OnStop();
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
	//(((*(CDDCDrvObj*)(&*this))).m_config).m_szTcpUdpParam;
	std::string DrvName = m_config.m_szTcpUdpParam.GetBuffer();
	m_config.m_szTcpUdpParam.ReleaseBuffer();

	std::string DrvIP = m_config.m_szIP.GetBuffer();
	m_config.m_szIP.ReleaseBuffer();
	CDDCDrvProtocol *pObj = NULL;
	pObj = new CMyProtocol(DrvName, DrvIP);

	if (pObj)
		InitTagIdMap();


	return pObj;
}

//////////////////////////////////////////////////////////////////////
// CMyProtocol Э�鴦����ʵ��
//////////////////////////////////////////////////////////////////////
CMyProtocol::CMyProtocol(std::string DevName, std::string DevIP)
{
	strDrvName = DevName;
	m_nBlockNum = 0;
	m_lStartTime = 0;
	m_lTime = 0;
	bInitPeriodTable = FALSE;
	strSvrIP = DevIP;
	m_pS7Client = new TS7Client();
	m_vectorDataBlocks.clear();
	BTVALUE[0] = 0x01;
	BTVALUE[1] = 0x02;
	BTVALUE[2] = 0x04;
	BTVALUE[3] = 0x08;
	BTVALUE[4] = 0x10;
	BTVALUE[5] = 0x20;
	BTVALUE[6] = 0x40;
	BTVALUE[7] = 0x80;
	ParamInit();
}

CMyProtocol::~CMyProtocol()
{
	if(NULL != m_pS7Client)
	{
		m_pS7Client->Disconnect();
		delete m_pS7Client;
		m_pS7Client = NULL;
	}

	CVectorTDataBlock::iterator itor = m_vectorDataBlocks.begin();
	while(itor != m_vectorDataBlocks.end())
	{
		LPTDataBlock lpTDataBlock = *itor;
		if(lpTDataBlock)
		{
			if(lpTDataBlock->pbyBuf)
			{
				delete []lpTDataBlock->pbyBuf;
				lpTDataBlock->pbyBuf = NULL;
			}

			delete lpTDataBlock;
			lpTDataBlock = NULL;
		}
		itor++;
	}
	m_vectorDataBlocks.clear();
}

BOOL CMyProtocol::ParsePerTable()
{
	CString strID;
	CTagItem *pTagItem = NULL;
	CString strKey("");
	std::string stTemp;
	char cTemp[32];
	BOOL bRet=FALSE;
	PointInfo tGetPoint;
	ParamPointInfo tGetPDetail;


	POSITION pos = m_pDrvObj->m_mapTagIDs.GetStartPosition();
	while(pos)
	{
		m_pDrvObj->m_mapTagIDs.GetNextAssoc(pos, strKey, pTagItem);

		tGetPoint.szID = pTagItem->m_szID.GetBuffer();
		pTagItem->m_szID.ReleaseBuffer();
		tGetPoint.szName = pTagItem->m_szName.GetBuffer();
		pTagItem->m_szName.ReleaseBuffer();
		tGetPoint.szNote = pTagItem->m_szNote.GetBuffer();
		pTagItem->m_szNote.ReleaseBuffer();
		tGetPoint.utype = pTagItem->m_wType;

		bRet = GetPointDetail(tGetPoint, tGetPDetail);
		if(bRet)
		{
			tGetPDetail.szID = tGetPoint.szID;
			InsertVarTable(tGetPDetail);
		}
	}
	return TRUE;
}

int CMyProtocol::InsertVarTable(const ParamPointInfo &PTDetail)
{
	auto itor = m_VarTotalMap.find(PTDetail.uAddStart);
	if(itor != m_VarTotalMap.end())
	{//�ҵ�����λ�ã���������
		itor._Ptr->_Myval.second.push_back(PTDetail);
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

int CMyProtocol::GetPointLenth(const PointInfo &Info, ParamPointInfo &retDate)
{
	int iRet = 1;
	switch(Info.utype)
	{
	case DB_DIGITAL:
		retDate.iTypeSize = 1;
		retDate.iDateType = DB_DIGITAL;
		retDate.iBitPos = atoi(Info.szNote.c_str()) - 1;
		break;
	case DB_INT32:
		if(atoi(Info.szNote.c_str()) == 1)
		{
			retDate.iTypeSize = 1;
			retDate.iDateType = DB_STRING;
			retDate.iBitPos = 0;
		}
		else if(atoi(Info.szNote.c_str()) == 2)
		{
			retDate.iTypeSize = 2;
			retDate.iDateType = DB_BLOB;
			retDate.iBitPos = 0;
		}
		else
		{
			retDate.iTypeSize = 4;
			retDate.iDateType = DB_INT32;
			retDate.iBitPos = 0;
		}
		break;
	case DB_FLOATE32:
		retDate.iTypeSize = 4;
		retDate.iDateType = DB_FLOATE32;
		retDate.iBitPos = 0;
		break;
	case DB_BLOB:
	case DB_STRING:
	case DB_INT64:
	case DB_FLOATE64:
		retDate.iTypeSize = 0;
		retDate.iDateType = DB_KNOW;
		retDate.iBitPos = 0;
		iRet = 0;
		break;
	default:
		retDate.iTypeSize = 0;
		retDate.iDateType = DB_KNOW;
		retDate.iBitPos = 0;
		iRet = 0;
		break;
	}

	return iRet;
}

BOOL CMyProtocol::GetPointDetail(const PointInfo &getPoint, ParamPointInfo &retPoint)
{
	int iStartAddr = 0, iEndAddr = 0;
	int iRet = 0;
	iRet = GetPointLenth(getPoint, retPoint);
	if(iRet == 0)
	{
		m_pDrvObj->AddInfo(_T("����������ʧ��"));
		return FALSE;
	}
	int PTOffset = atoi(getPoint.szID.c_str());
	for(auto itor = m_vectorDataBlocks.begin(); itor != m_vectorDataBlocks.end(); itor++)
	{	
		iStartAddr = (*itor)->nStart;
		iEndAddr =  (*itor)->nStart + (*itor)->nSize;
		if(PTOffset >= iStartAddr && PTOffset < iEndAddr)
		{
			retPoint.uAddStart = (*itor)->nStart;
			retPoint.uLKOffset = PTOffset - iStartAddr;
			return TRUE;
		}
	}	
	m_pDrvObj->AddInfo(_T("�㲻�ڵ�ַ��Χ"));
	return FALSE;
}

DRVRET CMyProtocol::WriteVar(LPDRREC lpval)
{
	return DRERR_NOTSUPPORT;
}



BOOL CMyProtocol::ParamInit()
{
	//strDrvName = m_config.m_szTcpUdpParam.GetBuffer();
	//m_config.m_szTcpUdpParam.ReleaseBuffer();
	TCHAR tcsExeDirPath[MAX_PATH + 1] = {0};
    GetModuleFileName(NULL, tcsExeDirPath, sizeof(tcsExeDirPath) - 1);
    *(strrchr(tcsExeDirPath, '\\') + 1) = 0x00;
	strConfigPath = tcsExeDirPath;
	strConfigPath += strDrvName + ".ini";
	char pszBuf[256];
	char cSection[256];
	memset(cSection, 0, 255);
	sprintf(cSection, "Common");
	m_nRack = GetPrivateProfileInt(cSection, "Rack", 0, strConfigPath.c_str());
	m_nSlot = GetPrivateProfileInt(cSection, "Slot", 0, strConfigPath.c_str());
	m_bEndian = GetPrivateProfileInt(cSection, "Endian", 1, strConfigPath.c_str());
	m_nBlockCount =  GetPrivateProfileInt(cSection, "DBBlockCount", 0, strConfigPath.c_str());

	memset(pszBuf, 0, 255);//�ñ�����һ�����ö����ַ��
	GetPrivateProfileString(cSection, "Type", "", pszBuf, 255,  strConfigPath.c_str());

     m_nBlockNum = GetPrivateProfileInt(cSection , "DataCount", 0, strConfigPath.c_str());
	if(m_nBlockNum < 1)
	{
		//m_pDrvObj->AddInfo(_T("��ȡ���ݿ���Ϊ0"));
		return FALSE;
	}
	//else
	//{
	//	sprintf(cSection, "�����ļ�Ϊ %s ���� %d ����", strConfigPath.c_str(), m_nBlockNum);
	//	m_pDrvObj->AddInfo(_T(cSection));
	//}
	for(int i = 0; i < m_nBlockNum; i++)
	{
		memset(cSection, 0, 255);
		sprintf(cSection, "DataAddr_%d", i+1);
		LPTDataBlock lpTDataBlock = new TDataBlock;
		lpTDataBlock->nStart = GetPrivateProfileInt(cSection, "StartAddr", 0, strConfigPath.c_str());
		lpTDataBlock->nSize = GetPrivateProfileInt(cSection, "Size", 0, strConfigPath.c_str());
		lpTDataBlock->nInterval = GetPrivateProfileInt(cSection, "Period", 0, strConfigPath.c_str());
		lpTDataBlock->nBlock = GetPrivateProfileInt(cSection, "Block", 0, strConfigPath.c_str());
		lpTDataBlock->lUpDateTime = 0;
		lpTDataBlock->szType = pszBuf;
		if(lpTDataBlock->nSize > 0)
		{
			lpTDataBlock->pbyBuf = new BYTE[lpTDataBlock->nSize + 10];
			memset(lpTDataBlock->pbyBuf, 0, lpTDataBlock->nSize + 10);
		}
		else
		{
			lpTDataBlock->pbyBuf = NULL;
		}
		m_vectorDataBlocks.push_back(lpTDataBlock);
		//std::sort(m_vectorDataBlocks.begin(), m_vectorDataBlocks.end(), LessStar);
		RegVarTable mt_RegVarTable;
		m_VarTotalMap.insert(std::pair<int, RegVarTable>(lpTDataBlock->nStart, mt_RegVarTable));
	}
	return TRUE;
}

BOOL CMyProtocol::Connect()
{//���ӵײ��豸
	
	BOOL bRet = FALSE;
	if(m_nBlockNum < 1)
	{
		m_pDrvObj->AddInfo(_T("��ȡ���ݿ���Ϊ0,���������ļ�"));
		return FALSE;
	}
	//else
	//{
	//	sprintf(ss, "�����ļ�Ϊ %s ���� %d ����", strConfigPath.c_str(), m_nBlockNum);
	//	m_pDrvObj->AddInfo(_T(ss));
	//}

	if(bInitPeriodTable == FALSE)
	{
		bRet = ParsePerTable();
		if(bRet)
		{
			bInitPeriodTable = TRUE;
		}
	}
//	return TRUE;

	bRet = ConnectDev();

	return bRet;
}

BOOL CMyProtocol::ConnectDev()
{
	int nRet = 0;
	char szLog[512];
	string szErr;
	if(IsConnect())
	{
		m_pS7Client->Disconnect();
	}
	nRet = m_pS7Client->ConnectTo(strSvrIP.c_str(), m_nRack, m_nSlot);
	if(nRet != 0)
	{
		szErr = CliErrorText(nRet);
		sprintf(szLog, "%s Connect<%s>: Rack<%d>:Slot<%d> Error Infor:<%s>",strDrvName.c_str(), strSvrIP.c_str(), m_nRack, m_nSlot, szErr.c_str());
		if(m_pDrvObj)
			m_pDrvObj->AddInfo(_T(szLog));
		return FALSE;
	}
	else
	{
		sprintf(szLog, "%s Connect<%s>: Rack<%d>:Slot<%d> Success",strDrvName.c_str(), strSvrIP.c_str(), m_nRack, m_nSlot);
		if(m_pDrvObj)
			m_pDrvObj->AddInfo(_T(szLog));
	}
	return TRUE;
}

int CMyProtocol::ReConnect()
{
	if(IsConnect())
	{
		m_pS7Client->Disconnect();
	}

	int nRet = m_pS7Client->Connect();

	if(m_pS7Client->Connected())
	{
		return nRet;
	}
	return nRet;
}

BOOL CMyProtocol::IsCanDo()
{
	char szInfo[255];
	int nRet = 0;
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

		nRet = ReConnect();
		if(nRet != 0)
		{
			sprintf(szInfo, "ʵ���� %s �������� %s", strDrvName.c_str(), CliErrorText(nRet).c_str()); 
			if(m_pDrvObj)
				m_pDrvObj->AddInfo(_T(szInfo));
			return FALSE;
		}
		else
		{
			sprintf(szInfo, "ʵ���� %s �����ɹ�", strDrvName.c_str()); 
			if(m_pDrvObj)
				m_pDrvObj->AddInfo(_T(szInfo));
		}
	}
	return  TRUE;
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

BOOL CMyProtocol::IsConnect()
{
	return m_pS7Client->Connected();
}

/**************************************************************
// ��������
****************************************************************/
void CMyProtocol::DoRun()
{
	int nRet = 0; 
	char szInfo[255];
	BYTE btBuffer[64];
	CString strID;
	CTagItem *pTagItem = NULL;
	CString strKey("");
	BYTE bZZM;
	WORD usZZM;
	int iZZM;
	long lValue;
	UINT uZZM;
	float fValue;
	RegVarTable getValue;

	if (!IsCanDo())  
	{  
		int aa = 0;
		return;  
	}
	long lTime =::time(NULL);
	LPTDataBlock lpTDataBlock;
	for(auto itor = m_vectorDataBlocks.begin(); itor != m_vectorDataBlocks.end(); itor++)
	{
		lpTDataBlock = *itor;
		VarTotalMAPItor mapItor = m_VarTotalMap.find(lpTDataBlock->nStart );
		if(mapItor != m_VarTotalMap.end())
		{
			if(lpTDataBlock->nInterval > lTime - lpTDataBlock->lUpDateTime)
			{
				 continue;
			}
			lpTDataBlock->lUpDateTime = lTime;
			//��ȡ�����������
			nRet = GetS715DevData(lpTDataBlock);
			if(nRet == 0)
			{
				getValue.clear();
				getValue = mapItor->second;
				int isize = getValue.size();
				for(int i = 0; i< isize; i++)
				{
					memset(btBuffer, 0, 64);
					BYTE *pVal = lpTDataBlock->pbyBuf + getValue._Myfirst[i].uLKOffset;
					memcpy(btBuffer, pVal, getValue._Myfirst[i].iTypeSize );

					if(getValue[i].iTypeSize > 1 && m_bEndian )
					{
						SwapBuffer(btBuffer, getValue._Myfirst[i].iTypeSize);
					}
					
					strID = getValue[i].szID.c_str();

					switch(getValue._Myfirst[i].iDateType)
					{
						case DB_DIGITAL:
							iZZM = getValue._Myfirst[i].iBitPos;
							lValue = (btBuffer[0] & BTVALUE[iZZM]) == BTVALUE[iZZM] ? 1 : 0;							
							m_pDrvObj->AddValueByID(strID, lTime, QA_OK, lValue);
							break;
						case DB_INT32:
							iZZM=*(int *)btBuffer;
							lValue = iZZM;
							m_pDrvObj->AddValueByID(strID, lTime, QA_OK, lValue);
							break;
						case DB_BLOB:
							usZZM=*(WORD *)btBuffer;
							lValue = usZZM;
							m_pDrvObj->AddValueByID(strID, lTime, QA_OK, lValue);
							break;
						case DB_FLOATE32:
							fValue=*(float *)btBuffer;
							m_pDrvObj->AddValueByID(strID, lTime, QA_OK, fValue);
							break;
						case DB_STRING:
							lValue=btBuffer[0];
							m_pDrvObj->AddValueByID(strID, lTime, QA_OK, lValue);
							break;
						case DB_INT64:
						case DB_FLOATE64:
							break;
						default:
							break;
					}					
				}
			}
			else
			{
				sprintf(szInfo, "��ȡS715���͵� %s ��������ʧ�� ret %s", strDrvName.c_str(), CliErrorText(nRet).c_str());
			}
		}		
	}
	//m_pDrvObj->AddDebug("aaaaaaaaaaaa");
	m_pDrvObj->FlushValue();

	//POSITION pos = m_pDrvObj->m_mapTagIDs.GetStartPosition();	
	//float fValue;
	//long lTime =::time(NULL);
	//while(pos)
	//{
	//	m_pDrvObj->m_mapTagIDs.GetNextAssoc(pos, strKey, pTagItem);
	//	

	//	sprintf(szInfo, "hsp m_szID %s szName %s szNote %s utype %d\n", szID.c_str(),szName.c_str(), szNote.c_str(), utype);
	//	OutputDebugString(szInfo);
	//	//�ڴ˴������ǩ
	//	int aa = 0;
	//	m_pDrvObj->AddValueByID(strID, lTime, QA_OK,  (float)fValue);
	//	Sleep(1000);
	//}	

	//Sleep(5000);
}

int CMyProtocol::GetS715DevData(LPTDataBlock TDataBlock)
{
	char szInfo[255];
	int nRet = 0;
	memset(TDataBlock->pbyBuf, 0,TDataBlock->nSize+10 );
	if(TDataBlock->szType.compare("M") == 0)
	{
		nRet = m_pS7Client->MBRead(TDataBlock->nStart, TDataBlock->nSize, TDataBlock->pbyBuf);
	}
	else if(TDataBlock->szType.compare("DB") == 0)
	{
		nRet = m_pS7Client->DBRead(TDataBlock->nBlock, TDataBlock->nStart, TDataBlock->nSize, TDataBlock->pbyBuf);
	}
	else if(TDataBlock->szType.compare("Q") == 0)
	{
		nRet = m_pS7Client->ABRead(TDataBlock->nStart, TDataBlock->nSize, TDataBlock->pbyBuf);
	}
	else if(TDataBlock->szType.compare("I") == 0)
	{
		nRet = m_pS7Client->EBRead(TDataBlock->nStart, TDataBlock->nSize, TDataBlock->pbyBuf);
	}
	else
	{
		sprintf(szInfo, "��ȡ�豸�������ʹ��� %s\n", TDataBlock->szType.c_str());
	}

	if( nRet != 0)
	{
		sprintf(szInfo, "��ȡ�豸�������ݴ��� %s\n", CliErrorText(nRet).c_str());
		if(m_pDrvObj)
				m_pDrvObj->AddInfo(_T(szInfo));
		m_pS7Client->Disconnect();
	}
	return nRet;
}

void CMyProtocol::Disconnect()
{
	if(NULL != m_pS7Client)
	{
		m_pS7Client->Disconnect();

		delete m_pS7Client;
		m_pS7Client = NULL;
	}
}

void CMyProtocol::SwapBuffer(void *pdata, int size)
{
		BYTE *pby = (BYTE*)pdata; 
		BYTE by;
		int iSizeEnd = size / 2;
		for (int i=0; i<iSizeEnd; i++)
		{
			by = pby[i];
			pby[i] = pby[size - i - 1];
			pby[size - i - 1] = by;
		}
}