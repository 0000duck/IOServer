#include "StdAfx.h"
#include "DecValMan.h"



/////////////////////////////////////////////////////////////
CDecValMan::CDecValMan()
{
	m_mapdec.InitHashTable(37);
	m_arraydec.SetSize(0,32);
	m_ndecpos = 0; //ȡ�������ݵ��豸λ��
	m_ndecposvar = 0;
}
CDecValMan::~CDecValMan()
{
	m_mapdec.RemoveAll();
	int i,n = m_arraydec.GetSize();
	for(i=0;i<n;i++)
		delete m_arraydec.GetAt(i);
	m_arraydec.RemoveAll();
}

void CDecValMan::AddDec(CDrvItem* pdec) //����豸
{
	CSafeLock lock(&m_Lock); //����

	DelDec(pdec->m_lID); //��ɾ��
	CDecVal* pdv = new CDecVal(pdec->m_lID,pdec->m_array.GetSize());
	pdv->m_szDecName = pdec->m_szDecName;
	if(pdv == NULL)
		return;
	pdv->Init(pdec);
	m_mapdec.SetAt(pdv->m_id,pdv);
	m_arraydec.Add(pdv);
}

void CDecValMan::DelDec(long lid )//ɾ���豸
{
	CDecVal* pdv= NULL;
	if(!m_mapdec.Lookup(lid,pdv))
		return;
	m_mapdec.RemoveKey(lid);
	int i=0,n=m_arraydec.GetSize();
	for(i=0;i<n;i++)
	{
		if(m_arraydec.GetAt(i)->m_id == lid)
		{
			m_arraydec.RemoveAt(i);
			delete pdv;
			return;
		}
	}
}
void CDecValMan::DelDecVal(long lid)//ɾ���豸
{
	CSafeLock lock(&m_Lock); //����
	DelDec(lid);
}
CString CDecValMan::GetDecName(long lid)
{
	CSafeLock lock(&m_Lock); //����
	CDecVal *pdv = NULL;
	if(!m_mapdec.Lookup(lid,pdv))
		return CString("");
	char sname[32];

	strncpy(sname,pdv->m_szDecName,31);
	return CString(sname);
}
void CDecValMan::OnDecDataOK(long ldecid,T_DRREC recs[],int nsize) //����豸ֵ�������ڻص�������ʹ�ã�����
{
	CSafeLock lock(&m_Lock);

	//����CDecVal
	CDecVal *pdv = NULL;
	if(!m_mapdec.Lookup(ldecid,pdv))
		return;
	pdv->OnDecDataOK(recs,nsize);
}

void CDecValMan::OnSoeOk(long ldecid,LPSOE psoe) //SOE�¼��ص���������
{
	CSafeLock lock(&m_Lock);
	//����CDecVal
	CDecVal *pdv = NULL;
	if(!m_mapdec.Lookup(ldecid,pdv))
		return;
	strcpy(psoe->m_ssrc,pdv->GetDecName()); //��ֵ����
	m_soeQueue.Add(psoe);
}

DWORD CDecValMan::OnWriteHistoryCB(long ldecid,T_DRREC recs[],int nsize)
{
	CSafeLock lock(&m_Lock);

	//����CDecVal
	CDecVal *pdv = NULL;
	if(!m_mapdec.Lookup(ldecid,pdv))
		return 0;

	return pdv->OnWriteHistoryCB(recs, nsize);
}

DWORD CDecValMan::OnStreamMessageCB(long ldecid, const char* lpszTagName, DWORD	dwCommandID, BYTE* pBuffer,	int	nSize)
{
	int nPacketSize = 16 + 80 + sizeof(DWORD) + nSize + 16;

	if (nPacketSize > MAX_FRM_LEN)
		return DBERR_PACKET_TO_LONG;

	CSafeLock lock(&m_Lock);

	//����CDecVal
	CDecVal *pdv = NULL;
	if(!m_mapdec.Lookup(ldecid,pdv))
		return 0;

	CMyByteArray* pMessagePacket = m_smsgQueue.Malloc(pdv->GetDecName(), lpszTagName, dwCommandID, pBuffer, nSize);
	if (pMessagePacket)
	{
		if (m_smsgQueue.Add(pMessagePacket))
		{
			return DRERR_OK;
		}
		else 
		{
			m_smsgQueue.Free(pMessagePacket);
			return DBERR_QUEUE_FULL;
		}
	}
	else
		return DBERR_MEMORY;
}

BOOL CDecValMan::GetSaveSoe(CSoe *psoe) //ȡSOE�¼����ڷ���
{
	CSafeLock lock(&m_Lock);
	return m_soeQueue.Get(psoe);
}

//������ʾ��

BOOL CDecValMan::GetTagVal(long ldecid,LPCTSTR lpszName,CVarVal &val)//ȡ��ʾֵ
{
	val.SetVoid();
	/* 
	 * ȥ������������߱�ǩ��ʱ��ǩ�б����Ӧ�ٶȣ�
	 * ��������ʵ��������״̬���ǲ�����ɾ�����޸ĵģ�����ȥ���������̰߳�ȫӰ�첻��
	 */
	//CSafeLock lock(&m_Lock);
	CDecVal *pdv = NULL;
	if(!m_mapdec.Lookup(ldecid,pdv))
		return FALSE;
	CTagVal* pv;
	if(!pdv->m_map.Lookup(lpszName,pv))
		return FALSE;

	/*
	 * �ڲ�����������£���ÿ����ǩ��ֵʹ������������֤��д�̰߳�ȫ
	 */
	CSpinLockGuard lock(&pv->m_lock);
	val = pv->m_val; //��ֵ
	return TRUE;
}

BOOL CDecValMan::GetTagViewTime(long ldecid,LPCTSTR lpszName,long *pltime)
{
	CSafeLock lock(&m_Lock);
	CDecVal *pdv = NULL;
	if(!m_mapdec.Lookup(ldecid,pdv))
		return FALSE;
	CTagVal* pv;
	if(!pdv->m_map.Lookup(lpszName,pv))
		return FALSE;
	*pltime = pv->m_val.ltime;
	return TRUE;
}
BOOL CDecValMan::GetTagViewQa(long ldecid,LPCTSTR lpszName,long *pnqa)
{
	CSafeLock lock(&m_Lock);
	CDecVal *pdv = NULL;
	if(!m_mapdec.Lookup(ldecid,pdv))
		return FALSE;
	CTagVal* pv;
	if(!pdv->m_map.Lookup(lpszName,pv))
		return FALSE;
	*pnqa = pv->m_val.snqa;
	return TRUE;
}
BOOL CDecValMan::GetTagViewVal(long ldecid,LPCTSTR lpszName,CString &szVal)
{
	CSafeLock lock(&m_Lock);
	CDecVal *pdv = NULL;
	if(!m_mapdec.Lookup(ldecid,pdv))
		return FALSE;
	CTagVal* pv;
	if(!pdv->m_map.Lookup(lpszName,pv))
		return FALSE;
	szVal = pv->m_val.GetValStr();
	return TRUE;
}

int  CDecValMan::GetSaveRecs(CTagVal* lprecs,int nsize)//ȡ�������� //debug20100430
{
	CSafeLock lock(&m_Lock);
	CDecVal *pdv = NULL;
	
	int ndo = 0,n = m_arraydec.GetSize();
	if(n == 0)
		return 0;

	if(m_ndecpos >= n)
		m_ndecpos = 0;
	int nt,nh = m_ndecpos;
	do{
		pdv = m_arraydec.GetAt(m_ndecpos);
		nt = pdv->GetSaveRecs(&(lprecs[ndo]),nsize - ndo);
		ndo += nt;
		m_ndecpos = (m_ndecpos + 1) % n;
	}while((nh != m_ndecpos) && (ndo < nsize));
	return ndo;
}

BOOL CDecValMan::GetSaveVarVal(CTagVal* lprecs)//ȡ��������(string blob)
{
	CSafeLock lock(&m_Lock);
	CDecVal *pdv = NULL;
	
	int n = m_arraydec.GetSize();
	if(n == 0)
		return FALSE;

	if(m_ndecposvar >= n)
		m_ndecposvar = 0;
	int nh = m_ndecposvar;
	do{
		pdv = m_arraydec.GetAt(m_ndecposvar);
		if(pdv->GetSaveVarVal(lprecs))
			return TRUE;
		m_ndecposvar = (m_ndecposvar + 1) % n;
	}while(nh != m_ndecposvar);
	return FALSE;
}
