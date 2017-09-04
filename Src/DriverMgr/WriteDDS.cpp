#include "StdAfx.h"
#include "WriteDDS.h"



////////////////////////////////////////////////////////////////////////////////////
CWriteDDS::CMKVALC::CMKVALC()
{
	m_nsize = 0;
}

CWriteDDS::CMKVALC::~CMKVALC()
{
}

void CWriteDDS::CMKVALC::MakeCell(CTagVal* ptv)
{
	struct t_rs{
		BYTE uclen;
		BYTE ucf1;
		BYTE ucf2;
		BYTE uccomptype;
		float fcompval;

		long	ltime;
		short	snqa;
		WORD	wt;
		__int64 llval;
	}rs; //siseof() =  24 �����ٸ���ǩ��
	memset(m_sbuf,0,256);
	int nsl = ptv->m_szNamedb.GetLength();
	if(nsl > 79)
	{
		ptv->m_szNamedb = ptv->m_szNamedb.Left(79);
		nsl = 79;
	}
	rs.uclen = (BYTE)(24 + (nsl/4)*4 + 4);
	rs.ucf1 = 0xAA;
	if(ptv->m_bresend)
		rs.ucf2 = 0x5A; //5�����ط���
	else
		rs.ucf2 = 0x55;
	rs.uccomptype = (BYTE)ptv->m_nCompType;
	rs.fcompval = (float)ptv->m_dblcompval;
	rs.wt = (WORD)ptv->m_val.vt;
	rs.ltime = ptv->m_val.ltime;
	rs.snqa = ptv->m_val.snqa;
	rs.llval = ptv->m_val.llval;

	memcpy(m_sbuf,&rs,sizeof(struct t_rs));
	strcpy(&m_sbuf[24],ptv->m_szNamedb);
	m_nsize =  rs.uclen;
}

CWriteDDS::CWriteDDS()
{
	m_bRun = FALSE;
	m_bKill = FALSE;

	m_pddscon = NULL;
	m_pDecValMan = NULL;

	m_batmp.SetGrown(256 * 1024);//һ�η����㹻���ʺ�4000��

	m_breal = TRUE;
	m_dwPingTick = 0;
}

CWriteDDS::~CWriteDDS()
{

}
BOOL g_IsCashe();
int		CWriteDDS::WriteRealVal(BYTE &ucret,BYTE &ddsorder) //д������������ʵʱ
{
	ddsorder = 0;
	int nrecs = 0;

	if (m_pddscon->GetNetMode() == NET_MODE_UDP)
		nrecs = m_pDecValMan->GetSaveRecs(m_wrec, UDP_WDDS_BUF_SIZE);
	else
		nrecs = m_pDecValMan->GetSaveRecs(m_wrec,WDDS_BUF_SIZE);

	if(nrecs ==  0)
		return 0;

	m_batmp.ClearData();
	int i,n=0;
	BYTE *pb=NULL;
	CMKVALC mkc;
	for(i=0;i<nrecs;i++)
	{
#if 0
		if ((m_wrec[i].m_szNamedb.Compare(_T("2DCS.REAL_MW")) == 0) && ((m_wrec[i].m_val.GetFloat() < 100.0) || (m_wrec[i].m_val.GetFloat() > 400.0)))
		{
			GetLogger()->AddInfoFmt(_T("��ǩ<%s>��val:%s, time=%s, snqa=%d"), m_wrec[i].m_szNamedb, 
				m_wrec[i].m_val.GetValStr(), 
				m_wrec[i].m_val.GetTimeStr(),
				(int)m_wrec[i].m_val.snqa);
		}
#endif

		mkc.MakeCell(&m_wrec[i]);
		m_batmp.PutData(mkc.m_sbuf,mkc.m_nsize);
	}
	pb = m_batmp.GetBuf(n);
	m_sendfrm.MakeFrm(WDDS_OD_REALVAL,0,0,0,pb,n);
	pb = m_sendfrm.GetFrm(&n);

	if(!m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder)) //����ʧ�ܣ����뻺��
	{
		if(!g_IsCashe())
			return -1;
		for(i=0;i<nrecs;i++)
		{
			if(!m_wrec[i].m_bresend) //�����ط��Ĳż��뻺��
				gGetDriveMgr()->m_cashe.AddRealVal(m_wrec[i].m_szNamedb,m_wrec[i].m_nCompType,m_wrec[i].m_dblcompval,&m_wrec[i].m_val);
		}
		return -1;//����ʧ��
	}
	if(ucret != 0)//DDSдĿ�������ʧ��
	{
		if(m_pddscon->GetDDSNum() > 1) //�������һ��DDS������������������һ��DDS������
		{
			GetLogger()->AddError("д��������ʵʱʧ�ܣ��Ͽ������л�����һ��DDS!");
			m_pddscon->SetConnect(FALSE);
		}
		if(!g_IsCashe())
			return -1;
		for(i=0;i<nrecs;i++)
		{
			if(!m_wrec[i].m_bresend) //�����ط��Ĳż��뻺��
				gGetDriveMgr()->m_cashe.AddRealVal(m_wrec[i].m_szNamedb,m_wrec[i].m_nCompType,m_wrec[i].m_dblcompval,&m_wrec[i].m_val);
		}
		return -1;
	}

	// ����UDPģʽ����ʱ1���룬�Լ��ٶ���
	if ((m_pddscon->GetNetMode() == NET_MODE_UDP) && (CDriverMgr::m_nUdpSendDelay > 0))
	{
		Sleep(CDriverMgr::m_nUdpSendDelay);
	}

	return nrecs;
}


int CWriteDDS::WriteRealVar(BYTE &ucret,BYTE &ddsorder)	//д�ɱ䳤��¼string,blob ʵʱ
{
	ddsorder = 0;

	if(!m_pDecValMan->GetSaveVarVal(&m_varval))
		return 0; //�޿ɱ��������ݷ���
	//���������API���ݶη���
	BYTE ucend = 0;
	struct t_rs
	{
		char	sname[80];
		long	ltime;
		short	snqa;
		WORD	wt;
		long	lvlen;//����

	}rs;//sizeof() = 96

	memset(&rs,0,sizeof(struct t_rs));
	strcpy(rs.sname,m_varval.m_szNamedb);
	rs.ltime = m_varval.m_val.ltime;
	rs.snqa = m_varval.m_val.snqa;
	rs.wt = (WORD)m_varval.m_val.vt;
	rs.lvlen = m_varval.m_val.lvlen;

	m_batmp.ClearData();
	m_batmp.PutData(&rs,sizeof(struct t_rs));//д��̶�����
	if(rs.lvlen > 0)
	{
		m_batmp.PutData(m_varval.m_val.pstr,rs.lvlen); //д��ɱ䲿��
		if(rs.wt == DT_STRING)
			m_batmp.PutData(&ucend,1);//�ַ���ĩβ��0
	}

	int n = 0;
	BYTE *pb = NULL;
	pb = m_batmp.GetBuf(n);
	m_sendfrm.MakeFrm(WDDS_OD_REALVAR,0,0,0,pb,n);
	pb = m_sendfrm.GetFrm(&n);

	if(!m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder)) //����ʧ�ܣ����뻺��
	{
		if(!g_IsCashe())
			return -1;
		gGetDriveMgr()->m_cashe.AddRealVar(m_varval.m_szNamedb,&m_varval.m_val);//���뻺��
		return -1;
	}
	if(ucret != 0) //дĿ�������ʧ��
	{
		if(m_pddscon->GetDDSNum() > 1) //�������һ��DDS������������������һ��DDS������
		{
			GetLogger()->AddError("д�䳤����ʵʱʧ�ܣ��Ͽ������л�����һ��DDS!");
			m_pddscon->SetConnect(FALSE);
		}
		if(!g_IsCashe())
			return -1;
		gGetDriveMgr()->m_cashe.AddRealVar(m_varval.m_szNamedb,&m_varval.m_val);//���뻺��
		return -1;
	}
	return 1; //����д��һ����¼
}

int	CWriteDDS::WriteStreamMessage(BYTE &ucret,BYTE &ddsorder)	//д����Ϣ
{
	ddsorder = 0;
	CMyByteArray* pMessagePacket = m_pDecValMan->GetSaveStreamMessage();
	if (!pMessagePacket)
		return 0;

	// WDDS_OD_STREAM_MESSAGE

	int n = 0;
	BYTE *pb = pMessagePacket->GetBuf(n);

	if(!m_pddscon->DoWrite(pb, n, WDDSTIMEOUT, ucret, ddsorder)) // ����ʧ�ܣ����뻺��
	{
		if(!g_IsCashe())
		{
			m_pDecValMan->FreeStreamMessage(pMessagePacket);
			return -1;
		}

		//gGetDriveMgr()->m_cashe.AddSoe(&m_soe);
		m_pDecValMan->FreeStreamMessage(pMessagePacket);
		return -1;
	}
	if(ucret != 0)//DDSдĿ�������ʧ��
	{
		if(m_pddscon->GetDDSNum() > 1) //�������һ��DDS������������������һ��DDS������
		{
			GetLogger()->AddError("д����Ϣʧ�ܣ��Ͽ������л�����һ��DDS!");
			m_pddscon->SetConnect(FALSE);
		}
		if(!g_IsCashe())
		{
			m_pDecValMan->FreeStreamMessage(pMessagePacket);
			return -1;
		}

		//gGetDriveMgr()->m_cashe.AddSoe(&m_soe);
		m_pDecValMan->FreeStreamMessage(pMessagePacket);

		return -1;
	}

	m_pDecValMan->FreeStreamMessage(pMessagePacket);

	return 1; //����д��һ����¼
}

int	CWriteDDS::WriteRealSoe(BYTE &ucret,BYTE &ddsorder)	//дSOE ʵʱ
{
	ddsorder = 0;
	if(!m_pDecValMan->GetSaveSoe(&m_soe))
		return 0;
	//WDDS_OD_SOE
	m_batmp.ClearData();

	m_batmp.PutData(&m_soe.m_ltime,4);
	m_batmp.PutData(&m_soe.m_lmsec,4);
	m_batmp.PutData(m_soe.m_ssrc,64);
	m_batmp.PutData(m_soe.m_sdec,64);
	m_batmp.PutData(m_soe.m_sdes,256);
	m_batmp.PutData(m_soe.m_stxtargs,256);
	m_batmp.PutData(&m_soe.m_lclsid,4);
	m_batmp.PutData(&m_soe.m_lbinarglen,4);

	if(m_soe.m_lbinarglen > 0)
		m_batmp.PutData(m_soe.m_pbinargs,m_soe.m_lbinarglen);

	int n = 0;
	BYTE *pb = NULL;
	pb = m_batmp.GetBuf(n);
	m_sendfrm.MakeFrm(WDDS_OD_SOE,0,0,0,pb,n);
	pb = m_sendfrm.GetFrm(&n);

	if(!m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder)) //����ʧ�ܣ����뻺��
	{
		if(!g_IsCashe())
			return -1;
		gGetDriveMgr()->m_cashe.AddSoe(&m_soe);
		return -1;
	}
	if(ucret != 0)//DDSдĿ�������ʧ��
	{
		if(m_pddscon->GetDDSNum() > 1) //�������һ��DDS������������������һ��DDS������
		{
			GetLogger()->AddError("дSOEʵʱʧ�ܣ��Ͽ������л�����һ��DDS!");
			m_pddscon->SetConnect(FALSE);
		}
		if(!g_IsCashe())
			return -1;
		gGetDriveMgr()->m_cashe.AddSoe(&m_soe);
		return -1;
	}
	return 1; //����д��һ����¼
}

int		CWriteDDS::WriteCasheVal(BYTE &ucret,BYTE &ddsorder)//д�����������ͻ���
{
	if (!g_IsCashe())
		return 0;
	if(!m_pddscon->IsConnect())//û������
		return 0;
	//��д�ڴ滺��
	int nrecs = 0;

	if (m_pddscon->GetNetMode() == NET_MODE_UDP)
		nrecs = ::gGetDriveMgr()->m_cashe.GetLastRecs(m_wrec,UDP_WDDSCACHE_BUF_SIZE);
	else
		nrecs = ::gGetDriveMgr()->m_cashe.GetLastRecs(m_wrec,WDDSCACHE_BUF_SIZE);

	if(nrecs >  0)
	{
		m_batmp.ClearData();
		int i,n=0;
		BYTE *pb=NULL;
		CMKVALC mkc;
		for(i=0;i<nrecs;i++)
		{
			mkc.MakeCell(&m_wrec[i]);
			m_batmp.PutData(mkc.m_sbuf,mkc.m_nsize);
		}
		pb = m_batmp.GetBuf(n);
		m_sendfrm.MakeFrm(WDDS_OD_CASHEVAL,0,0,0,pb,n);
		pb = m_sendfrm.GetFrm(&n);

		if(m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder)) //���ͳɹ�
		{
			if(ucret == 0) //�ɹ��ķ��͵���Ŀ�������
			{
				::gGetDriveMgr()->m_cashe.GetPosOk();//ȷ�ϻ��淢��

				TRACE("WriteCasheVal recs= %d ok\n",nrecs);
				return nrecs;
			}
			else //DDSдĿ�������ʧ��
			{
				if(m_pddscon->GetDDSNum() > 1) //�������һ��DDS������������������һ��DDS������
				{
					GetLogger()->AddError("д�������ͻ���ʧ�ܣ��Ͽ������л�����һ��DDS!");
					m_pddscon->SetConnect(FALSE);
				}
				return -1; //д��Ŀ�������ʧ��
			}
		}
		return -1;
	}

	//�ڴ滺��û���ˣ��Ӵ��̶�����̻���
	if(!::gGetDriveMgr()->m_cashe.LoadFromDisk())
		return 0;

	if (m_pddscon->GetNetMode() == NET_MODE_UDP)
		nrecs = ::gGetDriveMgr()->m_cashe.GetLastRecs(m_wrec,UDP_WDDS_BUF_SIZE);
	else
		nrecs = ::gGetDriveMgr()->m_cashe.GetLastRecs(m_wrec,WDDS_BUF_SIZE);

	if(nrecs >  0)
	{
		m_batmp.ClearData();
		int i,n=0;
		BYTE *pb=NULL;
		CMKVALC mkc;
		for(i=0;i<nrecs;i++)
		{
			mkc.MakeCell(&m_wrec[i]);
			m_batmp.PutData(mkc.m_sbuf,mkc.m_nsize);
		}
		pb = m_batmp.GetBuf(n);
		m_sendfrm.MakeFrm(WDDS_OD_CASHEVAL,0,0,0,pb,n);
		pb = m_sendfrm.GetFrm(&n);

		if(m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder)) //���ͳɹ�
		{
			if(ucret == 0) //�ɹ��ķ��͵���Ŀ�������
			{
				::gGetDriveMgr()->m_cashe.GetPosOk();//ȷ�ϻ��淢��
				TRACE("WriteCasheVal recs= %d ok\n",nrecs);
				return nrecs;
			}
			else //DDSдĿ�������ʧ��
			{
				if(m_pddscon->GetDDSNum() > 1) //�������һ��DDS������������������һ��DDS������
				{
					GetLogger()->AddError("д�������ͻ���ʧ�ܣ��Ͽ������л�����һ��DDS!");
					m_pddscon->SetConnect(FALSE);
				}
				return -1; //д��Ŀ�������ʧ��
			}
		}
		return -1;
	}
	return 0;
}

int		CWriteDDS::WriteCasheVar(BYTE &ucret,BYTE &ddsorder)	//д�ɱ䳤��¼string,blob ����
{
	if (!g_IsCashe())
		return 0;
	ddsorder = 0;
	if(!m_pddscon->IsConnect())//û������
		return 0;
	PF_HANDLE h = -1;
	CPfRecVal rec; //��¼
	CLocalCashe* plc = &::gGetDriveMgr()->m_cashe;
	//if(PFR_OK != plc->GetFirstPfRec(h,&rec)) //û�м�¼��
	if(PFR_OK != plc->GetLastPfRec(h,&rec)) //û�м�¼��
	{
		int nfiles = 0;
		plc->OpenNewPgeFile();
		return 0;
	}
	//����
	if((rec.m_ntype == PFILE_STRING)||(rec.m_ntype == PFILE_BLOB)) //string,blob
	{
		//���������API���ݶη���
		BYTE ucend = 0;
		struct t_rs
		{
			char	sname[80];
			long	ltime;
			short	snqa;
			WORD	wt;
			long	lvlen;//����

		}rs;//sizeof() = 96

		memset(&rs,0,sizeof(struct t_rs));
		strcpy(rs.sname,rec.m_szdes);
		rs.ltime = rec.m_ltime;
		rs.snqa = rec.m_snqa;
		if(rec.m_ntype == PFILE_STRING)
			rs.wt = DT_STRING;
		else
			rs.wt = DT_BLOB;
		rs.lvlen = rec.GetDataSize();

		m_batmp.ClearData();
		m_batmp.PutData(&rs,sizeof(struct t_rs));//д��̶�����
		if(rs.lvlen > 0)
		{
			m_batmp.PutData(rec.GetBuf(),rs.lvlen); //д��ɱ䲿��
			if(rs.wt == DT_STRING)
				m_batmp.PutData(&ucend,1);//�ַ���ĩβ��0
		}

		int n = 0;
		BYTE *pb = NULL;
		pb = m_batmp.GetBuf(n);
		m_sendfrm.MakeFrm(WDDS_OD_CASHEVAR,0,0,0,pb,n);
		pb = m_sendfrm.GetFrm(&n);

		if(!m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder)) //����ʧ��
			return -1;

		if(ucret != 0) //DDSдĿ�������ʧ��
		{
			if(m_pddscon->GetDDSNum() > 1) //�������һ��DDS������������������һ��DDS������
			{
				GetLogger()->AddError("д�ɱ����ͻ���ʧ�ܣ��Ͽ������л�����һ��DDS!");
				m_pddscon->SetConnect(FALSE);
			}
			return -1; //����д��һ����¼
		}

		plc->DeletePfRec(h);//���ͳɹ���ɾ����¼
		TRACE("write var cashe ok!\n");
		return 1;
	}
	else if(rec.m_ntype == PFILE_SOE) //����SOE
	{
		m_batmp.ClearData();
		int nrecsize  = rec.GetDataSize();
		if(nrecsize < sizeof(TSOE) - 4) //�����SOE��¼
		{
			TRACE("Get From pagefile soe Err1!");
			plc->DeletePfRec(h);
			return 0;
		}

		BYTE *pbd = (BYTE*)rec.GetBuf();
		TSOE* psoe = (TSOE*)pbd;
		if((DWORD)nrecsize != sizeof(TSOE) - 4 + psoe->m_lbinarglen)//�����SOE��¼
		{
			TRACE("Get From pagefile soe Err2!");
			plc->DeletePfRec(h);
			return 0;
		}
		m_batmp.PutData(&psoe->m_ltime,4);
		m_batmp.PutData(&psoe->m_lmsec,4);
		m_batmp.PutData(psoe->m_ssrc,64);
		m_batmp.PutData(psoe->m_sdec,64);
		m_batmp.PutData(psoe->m_sdes,256);
		m_batmp.PutData(psoe->m_stxtargs,256);
		m_batmp.PutData(&psoe->m_lclsid,4);
		m_batmp.PutData(&psoe->m_lbinarglen,4);

		m_batmp.PutData(pbd + sizeof(TSOE) - 4,psoe->m_lbinarglen);

		int n = 0;
		BYTE *pb = NULL;
		pb = m_batmp.GetBuf(n);
		m_sendfrm.MakeFrm(WDDS_OD_SOE,0,0,0,pb,n);
		pb = m_sendfrm.GetFrm(&n);

		if(!m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder)) //����ʧ��
			return -1;
		if(ucret != 0)//DDSдĿ�������ʧ��
		{
			if(m_pddscon->GetDDSNum() > 1) //�������һ��DDS������������������һ��DDS������
			{
				GetLogger()->AddError("дSOE����ʧ�ܣ��Ͽ������л�����һ��DDS!");
				m_pddscon->SetConnect(FALSE);
			}
			return -1;
		}

		plc->DeletePfRec(h);//���ͳɹ���ɾ����¼
		return 1;
	}
	else
	{
		TRACE("err cashe var rec!\n");
		plc->DeletePfRec(h);
		return 0;
	}
	return 0;
}

int    CWriteDDS::SendEmpty(BYTE &ucret,BYTE &ddsorder) //���Ϳ�����
{
	//if (theApp.m_Config.m_bWaitTcpAck)
	if (CDriverMgr::m_bWaitTcpAck || (m_pddscon->GetNetMode() == NET_MODE_UDP))
	{
		ddsorder = 0;
		if(!m_pddscon->IsConnect())//û������
			return 0;
		BYTE *pb;
		int n=0;
		// NET_MODE_UDPģʽ�·���Pingָ��
		if (m_pddscon->GetNetMode() == NET_MODE_UDP)
		{
			DWORD dwTick = ::GetTickCount();
			if ((dwTick - m_dwPingTick) > 10000)
			{
				m_sendfrm.MakeFrm(WDDS_OD_PING,0,0,0,(BYTE*)NULL,0);
				pb = m_sendfrm.GetFrm(&n);
				m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder);

				m_dwPingTick = dwTick;
			}

			ucret = 0;
			ddsorder = 0;
			return 0;
		}
		else {
			m_sendfrm.MakeFrm(WDDS_OD_EMPTY,0,0,0,(BYTE*)NULL,0);
			pb = m_sendfrm.GetFrm(&n);
			if(!m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder))
				return 0;
			return 1;
		}
	}
	else { // ����Ӧ���汾��֧�ֱ�ǩͬ��
		ucret = 0;
		ddsorder = 0;
		return 0;
	}
}

void CWriteDDS::SendDdsTags()//���ͱ�ǩ
{
	int nindex = -1;
	CDriverMgr*  pDriverMgr = gGetDriveMgr();
	CMyByteArray ua;
	BYTE *pb,ucret = 0,ddsorder = 0;
	int n;
	ua.SetGrown(64 * 1024);//64����
	while(pDriverMgr->m_ddsTagTab.GetSendData(ua,nindex))
	{
		if(nindex == 0) //�ޱ�ǩ
		{
			m_sendfrm.MakeFrm(WDDS_OD_TAGS,0,0,0,(BYTE*)NULL,0);
			pb = m_sendfrm.GetFrm(&n);

			m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder);
			return;
		}
		pb  = ua.GetBuf(n);
		m_sendfrm.MakeFrm(WDDS_OD_TAGS,0,1,0,pb,n);
		pb = m_sendfrm.GetFrm(&n);
		if(!m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder)) //����ʧ���˳�
			return; 
	}
	//���һ�ν�����
	m_sendfrm.MakeFrm(WDDS_OD_TAGS,0,0,0,(BYTE*)NULL,0);
	pb = m_sendfrm.GetFrm(&n);
	m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder);
}


//1�ֽڰ�ʵʱ����ֿ��汾 wddscache
UINT	CWriteDDS::DoRun()//�߳�����
{
	DWORD dwtk=0,dwtkold=0;
	m_bKill = FALSE;
	m_bRun = TRUE;
	BYTE ddsorder = 0;
	BYTE ucret = 0xFF;

	CEvent evt(FALSE,TRUE);
	BOOL bSleep = TRUE;

	while(!m_bKill)
	{
		dwtk = ::GetTickCount(); //ȡ������

		ddsorder = 0;
		ucret = 0;
		bSleep = TRUE;

		if(!::gGetDriveMgr()->IsCanWork()) //����Ҫ����������Ҫ���ͻ���
		{	
			if(!m_breal)//���淢������
			{
				if(WriteCasheVal(ucret,ddsorder)!=0)	//д��������cashe
					bSleep = FALSE;

				if(WriteCasheVar(ucret,ddsorder)!=0)	//д�ɱ�����cashe
					bSleep = FALSE;
			}
			if(bSleep) //���У����Ϳ�����
			{
				if((dwtk - dwtkold) > 1000) //1���ӷ���һ�ο�����
				{
					dwtkold = dwtk;
					if(SendEmpty(ucret,ddsorder) != 0) //���Ϳ�����
					{
						if(ddsorder == 0xFF)//��Ҫ���ͱ�ǩ
						{
							SendDdsTags();
							ddsorder = 0;
							ucret = 0;
						}
					}
				}
				//WaitForSingleObject(evt.m_hObject,1);//��1���� ,���ﲻ��Ҫ�¼�����������ʱ��
			}
			else //�����У���ʱǿ�Ʒ��Ϳ���
			{
				if((dwtk - dwtkold) > 3000) //3���ӷ���һ�ο�����
				{
					dwtkold = dwtk;
					if(SendEmpty(ucret,ddsorder) != 0) //���Ϳ�����
					{
						if(ddsorder == 0xFF)//��Ҫ���ͱ�ǩ
						{
							SendDdsTags();
							ddsorder = 0;
							ucret = 0;
						}
					}
				}
			}
			WaitForSingleObject(evt.m_hObject,1);//��1���� ,���ﲻ��Ҫ�¼�����������ʱ��
			continue;
		}

		if(m_breal)//����ʵʱ
		{
			if(WriteRealVal(ucret,ddsorder) > 0)	//д��������ʵʱ
				bSleep = FALSE;

			WaitForSingleObject(evt.m_hObject,1);//��1���� ,���ﲻ��Ҫ�¼�����������ʱ��

			if(WriteRealVar(ucret,ddsorder) > 0)	//д�ɱ�����ʵʱ
				bSleep = FALSE;

			// ��������Ϣָ��
			if (WriteStreamMessage(ucret, ddsorder))
				bSleep = FALSE;

			WaitForSingleObject(evt.m_hObject,1);//��1���� ,���ﲻ��Ҫ�¼�����������ʱ��

			if(WriteRealSoe(ucret,ddsorder) > 0)	//дʵʱSOE
				bSleep = FALSE;

			WaitForSingleObject(evt.m_hObject,1);//��1���� ,���ﲻ��Ҫ�¼�����������ʱ��
		}
		else//���ͻ���
		{
			if(WriteCasheVal(ucret,ddsorder) > 0)	//д��������cashe
				bSleep = FALSE;

			WaitForSingleObject(evt.m_hObject,1);//��1���� ,���ﲻ��Ҫ�¼�����������ʱ��

			if(WriteCasheVar(ucret,ddsorder) > 0)	//д�ɱ�����cashe
				bSleep = FALSE;

			WaitForSingleObject(evt.m_hObject,1);//��1���� ,���ﲻ��Ҫ�¼�����������ʱ��
		}
		if(bSleep) //���У����Ϳ�����
		{
			if((dwtk - dwtkold) > 1000) //1���ӷ���һ�ο�����
			{
				dwtkold = dwtk;
				if(SendEmpty(ucret,ddsorder) != 0) //���Ϳ�����
				{
					if(ddsorder == 0xFF)//��Ҫ���ͱ�ǩ
					{
						SendDdsTags();
						ddsorder = 0;
						ucret = 0;
					}
				}
			}
			//WaitForSingleObject(evt.m_hObject,1);//��1���� ,���ﲻ��Ҫ�¼�����������ʱ��
		}
		else //�����У���ʱǿ�Ʒ��Ϳ���
		{
			if((dwtk - dwtkold) > 3000) //3���ӷ���һ�ο�����
			{
				dwtkold = dwtk;
				if(SendEmpty(ucret,ddsorder) != 0) //���Ϳ�����
				{
					if(ddsorder == 0xFF)//��Ҫ���ͱ�ǩ
					{
						SendDdsTags();
						ddsorder = 0;
						ucret = 0;
					}
				}
			}
		}
	}

	m_bKill = FALSE;
	m_bRun = FALSE;	
	TRACE("CWriteDDS Thread Exit\n");
	return 0;
}

UINT CWriteDDS::ThreadDDS(LPVOID lparam) //�߳�
{
	CWriteDDS* pwd = (CWriteDDS*)lparam;
	return pwd->DoRun();
}

void CWriteDDS::Start()
{
	if(m_bRun)
		return;
	m_pddscon->Start(); //����DDS�����߳�
	AfxBeginThread(ThreadDDS,this,THREAD_PRIORITY_NORMAL);//���������߳�
}

void CWriteDDS::Stop(BOOL bWaitForThreadExit)
{
	m_bKill = TRUE;

	if (bWaitForThreadExit)
	{
		while(m_bRun)
			Sleep(100);
		m_pddscon->Stop(); //ֹͣDDS�����߳�
	}
}
