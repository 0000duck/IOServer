//cashe.cpp
#include "stdafx.h"
#include "cashe.h"
#include <math.h>

//void g_AddLog(long ltime,LPCTSTR lpszTxt);
static long g_lLastDiskErrTime = 0;

#include  <io.h>

////////////////////////////////////////////////////////////////////////

CTvObj::CTvObj()
{
	m_szname="";//��ǩ��
	m_ntype = DT_VOID; //��������
	m_ncomptype = TA_COMPNO;//ѹ������
	m_fcompval = 0;//ѹ������
}
CTvObj::~CTvObj()
{

}

////////////////////////////////////////////////////////////////////////
CTv_digital::CTv_digital()
{
	m_ntype = DT_DIGITAL;
	m_valreal.ResetType(m_ntype);

	m_llastsave = 0;//�ϴδ洢
	m_bdataok = FALSE; //������Ч
}

CTv_digital::~CTv_digital()
{

}

BOOL CTv_digital::AddReal(LPVVAL pval,CBaseVal* pbaseval)
{
	CVarVal *pv = (CVarVal *)pval;
	if(m_valreal.ltime == 0)
	{
		m_valreal = *pv;
		pbaseval->Clear();
		pbaseval->m_szName = m_szname;
		pbaseval->m_val = m_valreal;
		m_llastsave = pval->ltime;
		m_bdataok = FALSE;//����Ҫ�ٴ��������
		return TRUE;
	}

	if(m_valreal.ltime >= pval->ltime)
		return FALSE;	
	if((pval->snqa == m_valreal.snqa) && (pv->GetInt() == m_valreal.GetInt())) //ֵ��������ͬ
	{
		//����ʱ��
		m_valreal.ltime = pval->ltime;
		if((m_valreal.ltime - m_llastsave) > 600) //10���Ӵ���һ��
		{
			pbaseval->Clear();
			pbaseval->m_szName = m_szname;
			pbaseval->m_val = m_valreal;
			m_llastsave = pval->ltime;
			m_bdataok = FALSE;//����Ҫ�ٴ��������
			return TRUE;
		}
		m_bdataok = TRUE; //������Ч����ʱδ��
		return FALSE;
	}
	//��ͬ
	m_valreal = *pv;//����
	pbaseval->Clear();
	pbaseval->m_szName = m_szname;
	pbaseval->m_val = m_valreal;
	m_llastsave = pval->ltime;
	m_bdataok = FALSE;//����Ҫ�ٴ��������
	return TRUE;
}

////////////////////////////////////////////////////////////////////////
CTv_int32::CTv_int32()
{
	m_ntype = DT_INT32;
	m_valreal.ResetType(m_ntype);

	m_llastsave = 0;//�ϴδ洢
	m_bdataok = FALSE; //������Ч
}

CTv_int32::~CTv_int32()
{

}

BOOL CTv_int32::AddReal(LPVVAL pval,CBaseVal* pbaseval)//���ʵʱֵ,��ѹ����Ҫѹ��
{
	CVarVal *pv = (CVarVal *)pval;
	if(m_valreal.ltime == 0)
	{
		m_valreal = *pv;
		pbaseval->Clear();
		pbaseval->m_szName = m_szname;
		pbaseval->m_val = m_valreal;
		m_llastsave = pval->ltime;
		m_bdataok = FALSE;//����Ҫ�ٴ��������
		return TRUE;
	}

	if(m_valreal.ltime >= pval->ltime)
		return FALSE;	
	if((pval->snqa == m_valreal.snqa) && (pv->GetInt() == m_valreal.GetInt())) //ֵ��������ͬ
	{
		//����ʱ��
		m_valreal.ltime = pval->ltime;
		if((m_valreal.ltime - m_llastsave) > 600) //10���Ӵ���һ��
		{
			pbaseval->Clear();
			pbaseval->m_szName = m_szname;
			pbaseval->m_val = m_valreal;
			m_llastsave = pval->ltime;
			m_bdataok = FALSE;//����Ҫ�ٴ��������
			return TRUE;
		}
		m_bdataok = TRUE; //������Ч����ʱδ��
		return FALSE;
	}
	//��ͬ
	m_valreal = *pv;//����
	pbaseval->Clear();
	pbaseval->m_szName = m_szname;
	pbaseval->m_val = m_valreal;
	m_llastsave = pval->ltime;
	m_bdataok = FALSE;//����Ҫ�ٴ��������
	return TRUE;
}

////////////////////////////////////////////////////////////////////////
CTv_int64::CTv_int64()
{
	m_ntype = DT_INT64;
	m_valreal.ResetType(m_ntype);

	m_llastsave = 0;//�ϴδ洢
	m_bdataok = FALSE; //������Ч
}

CTv_int64::~CTv_int64()
{

}

BOOL CTv_int64::AddReal(LPVVAL pval,CBaseVal* pbaseval)//���ʵʱֵ,��ѹ����Ҫѹ��
{
	CVarVal *pv = (CVarVal *)pval;
	if(m_valreal.ltime == 0)
	{
		m_valreal = *pv;
		pbaseval->Clear();
		pbaseval->m_szName = m_szname;
		pbaseval->m_val = m_valreal;
		m_llastsave = pval->ltime;
		m_bdataok = FALSE;//����Ҫ�ٴ��������
		return TRUE;
	}

	if(m_valreal.ltime >= pval->ltime)
		return FALSE;	
	if((pval->snqa == m_valreal.snqa) && (pv->GetInt64() == m_valreal.GetInt64())) //ֵ��������ͬ
	{
		//����ʱ��
		m_valreal.ltime = pval->ltime;
		if((m_valreal.ltime - m_llastsave) > 600) //10���Ӵ���һ��
		{
			pbaseval->Clear();
			pbaseval->m_szName = m_szname;
			pbaseval->m_val = m_valreal;
			m_llastsave = pval->ltime;
			m_bdataok = FALSE;//����Ҫ�ٴ��������
			return TRUE;
		}
		m_bdataok = TRUE; //������Ч����ʱδ��
		return FALSE;
	}
	//��ͬ
	m_valreal = *pv;//����
	pbaseval->Clear();
	pbaseval->m_szName = m_szname;
	pbaseval->m_val = m_valreal;
	m_llastsave = pval->ltime;
	m_bdataok = FALSE;//����Ҫ�ٴ��������
	return TRUE;
}

////////////////////////////////////////////////////////////////////////
CTv_string::CTv_string()
{
	m_ntype = DT_STRING;
	m_valreal.ResetType(m_ntype);

}

CTv_string::~CTv_string()
{

}

BOOL CTv_string::AddReal(LPVVAL pval,CBaseVal* pbaseval)//���ʵʱֵ,��ѹ����Ҫѹ��
{
	if((m_valreal.ltime >= pval->ltime)||(pval->vt != DT_STRING))
		return FALSE;

	CVarVal *pv = (CVarVal *)pval;
	m_valreal = *pv;//����
	pbaseval->Clear();
	pbaseval->m_szName = m_szname;
	pbaseval->m_val = m_valreal;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////
CTv_blob::CTv_blob()
{
	m_ntype = DT_BLOB;
	m_valreal.ResetType(m_ntype);

}

CTv_blob::~CTv_blob()
{

}

BOOL CTv_blob::AddReal(LPVVAL pval,CBaseVal* pbaseval)//���ʵʱֵ,��ѹ����Ҫѹ��
{
	if((m_valreal.ltime >= pval->ltime)||(pval->vt != DT_BLOB))
		return FALSE;

	CVarVal *pv = (CVarVal *)pval;
	m_valreal = *pv;//����
	pbaseval->Clear();
	pbaseval->m_szName = m_szname;
	pbaseval->m_val = m_valreal;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////
CTv_float32::CTv_float32()
{
	m_ntype = DT_FLOAT32;
	m_valreal.ResetType(m_ntype);

	int i;
	for(i=0;i<6;i++)
		m_valtmp[i].Init(DT_FLOAT32); //��ʼ������

	m_nrecs = 0;
}

CTv_float32::~CTv_float32()
{

}

BOOL CTv_float32::GetLastHisVal(CBaseVal* pbaseval)//ȡ������ʷֵ
{
	if(m_nrecs < 3)
		return FALSE;
	pbaseval->Clear();
	pbaseval->m_szName = m_szname;
	pbaseval->m_val = m_valtmp[m_nrecs - 1];

	return TRUE;
}

BOOL CTv_float32::AddReal(LPVVAL pval,CBaseVal* pbaseval)//���ʵʱֵ,��ѹ����Ҫѹ��
{
	if((m_valreal.ltime >= pval->ltime)||(pval->vt != DT_FLOAT32))
		return FALSE;

	CVarVal *pv = (CVarVal *)pval;
	m_valreal = *pv;//����

	//ѹ��
	if(m_ncomptype == TA_COMPNO)//��ѹ��
	{
		pbaseval->Clear();
		pbaseval->m_szName = m_szname;
		pbaseval->m_val = m_valreal;
		m_llastsave = pval->ltime;
		return TRUE;
	}
	//����ѹ����
	m_valtmp[m_nrecs] = m_valreal;
	m_nrecs++;

	if(m_nrecs < 3)
	{
		if(m_nrecs == 1) //��һ��������������
		{
			pbaseval->Clear();
			pbaseval->m_szName = m_szname;
			pbaseval->m_val = m_valreal;
			m_llastsave = pval->ltime;
			return TRUE;
		}
		return FALSE; //����ѹ��
	}
	//ѹ��
	int i;
	double dblx = (m_valtmp[m_nrecs-1].GetFloat() - m_valtmp[0].GetFloat())/
		(m_valtmp[m_nrecs-1].ltime - m_valtmp[0].ltime);
	double dblt=0,dbltv,dbldv;
	for(i=1;i<m_nrecs - 1;i++)
	{
		dbltv = m_valtmp[i].GetFloat();
		dblt = m_valtmp[i-1].GetFloat() + dblx * (m_valtmp[i].ltime - m_valtmp[i-1].ltime);
		if(m_ncomptype == TA_COMPPER) //�ٷֱȾ���
			dbldv = fabs(100 * (dblt - dbltv)/dbltv);
		else //���Ծ���
			dbldv = fabs(m_valtmp[i-1].GetFloat() - dbltv); //��������
		if(dbldv > m_fcompval)//��������
		{
			//����m_nrecs - 2��
			pbaseval->Clear();
			pbaseval->m_szName = m_szname;
			pbaseval->m_val = m_valtmp[m_nrecs - 2];
			m_llastsave = pval->ltime;

			//���¿�ʼѹ��,���Ѵ��̵���Ϊ���
			m_valtmp[0] = m_valtmp[m_nrecs - 2];
			m_valtmp[1] = m_valtmp[m_nrecs - 1];
			m_nrecs = 2;

			return TRUE; //����Ҫ���̵�����ֵ
		}
	}
	
	if(m_nrecs > 4) //5���㼰��������ƫ����С�ĵ�
	{
		double dblmin = 0,dbltt;
		int nminpos = -1;
		for(i=1;i<m_nrecs - 1;i++) //Ѱ��ƫ����С��λ��
		{
			dbltv = m_valtmp[i].GetFloat();
			dblt = m_valtmp[i-1].GetFloat() + dblx * (m_valtmp[i].ltime - m_valtmp[i-1].ltime);
			if(m_ncomptype == TA_COMPPER) //�ٷֱȾ���
				dbltt = fabs(100 * (dblt - dbltv)/dbltv);
			else //���Ծ���
				dbltt = fabs(m_valtmp[i-1].GetFloat() - dbltv);
			if(i == 1)
			{
				dblmin = dbltt;
				nminpos = i;
			}
			else 
			{
				if(dblmin > dbltt)
				{
					dblmin = dbltt;
					nminpos = i;
				}
			}
		}
		//����
		if(nminpos > 0)
		{
			for(i=nminpos;i<m_nrecs - 1;i++)
				m_valtmp[i] = m_valtmp[i+1];
			m_nrecs--;
		}
	}
	if(((m_valreal.ltime - m_llastsave) > 600) && (m_nrecs > 2)) //10���Ӵ���һ��
	{
		//����m_nrecs - 2��
		pbaseval->Clear();
		pbaseval->m_szName = m_szname;
		pbaseval->m_val = m_valtmp[m_nrecs - 2];
		m_llastsave = pval->ltime;

		//���¿�ʼѹ��,���Ѵ��̵���Ϊ���
		m_valtmp[0] = m_valtmp[m_nrecs - 2];
		m_valtmp[1] = m_valtmp[m_nrecs - 1];
		m_nrecs = 2;

		return TRUE;
	}
	return FALSE;
}
////////////////////////////////////////////////////////////////////////
CTv_float64::CTv_float64()
{
	m_ntype = DT_FLOAT64;
	m_valreal.ResetType(m_ntype);

	int i;
	for(i=0;i<6;i++)
		m_valtmp[i].Init(DT_FLOAT64); //��ʼ������

	m_nrecs = 0;
}

CTv_float64::~CTv_float64()
{

}

BOOL CTv_float64::GetLastHisVal(CBaseVal* pbaseval)//ȡ������ʷֵ
{
	if(m_nrecs < 3)
		return FALSE;
	pbaseval->Clear();
	pbaseval->m_szName = m_szname;
	pbaseval->m_val = m_valtmp[m_nrecs - 1];

	return TRUE;
}


BOOL CTv_float64::AddReal(LPVVAL pval,CBaseVal* pbaseval)//���ʵʱֵ,��ѹ����Ҫѹ��
{

	if((m_valreal.ltime >= pval->ltime)||(pval->vt != DT_FLOAT64))
		return FALSE;

	CVarVal *pv = (CVarVal *)pval;
	m_valreal = *pv;//����

	//ѹ��
	if(m_ncomptype == TA_COMPNO)//��ѹ��
	{
		pbaseval->Clear();
		pbaseval->m_szName = m_szname;
		pbaseval->m_val = m_valreal;
		m_llastsave = pval->ltime;
		return TRUE;
	}
	//����ѹ����
	m_valtmp[m_nrecs] = m_valreal;
	m_nrecs++;

	if(m_nrecs < 3)
	{
		if(m_nrecs == 1) //��һ��������������
		{
			pbaseval->Clear();
			pbaseval->m_szName = m_szname;
			pbaseval->m_val = m_valreal;
			m_llastsave = pval->ltime;
			return TRUE;
		}
		return FALSE; //����ѹ��
	}
	//ѹ��
	int i;
	double dblx = (m_valtmp[m_nrecs-1].GetDouble() - m_valtmp[0].GetDouble())/
		(m_valtmp[m_nrecs-1].ltime - m_valtmp[0].ltime);
	double dblt=0,dbltv,dbldv;
	for(i=1;i<m_nrecs - 1;i++)
	{
		dbltv = m_valtmp[i].GetDouble();
		dblt = m_valtmp[i-1].GetDouble() + dblx * (m_valtmp[i].ltime - m_valtmp[i-1].ltime);
		if(m_ncomptype == TA_COMPPER) //�ٷֱȾ���
			dbldv = fabs(100 * (dblt - dbltv)/dbltv);
		else //���Ծ���
			dbldv = fabs(m_valtmp[i-1].GetDouble() - dbltv); //��������
		if(dbldv > m_fcompval)//��������
		{
			//����m_nrecs - 2��
			pbaseval->Clear();
			pbaseval->m_szName = m_szname;
			pbaseval->m_val = m_valtmp[m_nrecs - 2];
			m_llastsave = pval->ltime;

			//���¿�ʼѹ��,���Ѵ��̵���Ϊ���
			m_valtmp[0] = m_valtmp[m_nrecs - 2];
			m_valtmp[1] = m_valtmp[m_nrecs - 1];
			m_nrecs = 2;

			return TRUE; //����Ҫ���̵�����ֵ
		}
	}
	
	if(m_nrecs > 4) //5���㼰��������ƫ����С�ĵ�
	{
		double dblmin = 0,dbltt;
		int nminpos = -1;
		for(i=1;i<m_nrecs - 1;i++) //Ѱ��ƫ����С��λ��
		{
			dbltv = m_valtmp[i].GetDouble();
			dblt = m_valtmp[i-1].GetDouble() + dblx * (m_valtmp[i].ltime - m_valtmp[i-1].ltime);
			if(m_ncomptype == TA_COMPPER) //�ٷֱȾ���
				dbltt = fabs(100 * (dblt - dbltv)/dbltv);
			else //���Ծ���
				dbltt = fabs(m_valtmp[i-1].GetDouble() - dbltv);
			if(i == 1)
			{
				dblmin = dbltt;
				nminpos = i;
			}
			else 
			{
				if(dblmin > dbltt)
				{
					dblmin = dbltt;
					nminpos = i;
				}
			}
		}
		//����
		if(nminpos > 0)
		{
			for(i=nminpos;i<m_nrecs - 1;i++)
				m_valtmp[i] = m_valtmp[i+1];
			m_nrecs--;
		}
	}
	if(((m_valreal.ltime - m_llastsave) > 600) && (m_nrecs > 2)) //10���Ӵ���һ��
	{
		//����m_nrecs - 2��
		pbaseval->Clear();
		pbaseval->m_szName = m_szname;
		pbaseval->m_val = m_valtmp[m_nrecs - 2];
		m_llastsave = pval->ltime;

		//���¿�ʼѹ��,���Ѵ��̵���Ϊ���
		m_valtmp[0] = m_valtmp[m_nrecs - 2];
		m_valtmp[1] = m_valtmp[m_nrecs - 1];
		m_nrecs = 2;

		return TRUE;
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////
CCompCashe::CCompCashe()
{
	m_map.InitHashTable(77777);
}
CCompCashe::~CCompCashe()
{
	Clear();
}

void CCompCashe::Clear()//���
{
	CString szkey;
	CTvObj* pi;
	POSITION pos = m_map.GetStartPosition();
	while(pos)
	{
		m_map.GetNextAssoc(pos,szkey,pi);
		delete pi;
	}
	m_map.RemoveAll();
}

BOOL CCompCashe::AddReal(LPCTSTR sname,int ncomptype,double dblcompval,LPVVAL pval,CBaseVal* pbaseval)//���ʵʱֵ
{
	//�Ȳ���m_map���Ƿ��б�ǩ
	CTvObj* ptv = NULL;
	if(!m_map.Lookup(sname,ptv)) //û�У�����һ��
	{
		if(pval->vt == DT_DIGITAL)
			ptv = new CTv_digital;
		else if(pval->vt == DT_INT32)
			ptv = new CTv_int32;
		else if(pval->vt == DT_INT64)
			ptv = new CTv_int64;
		else if(pval->vt == DT_FLOAT32)
			ptv = new CTv_float32;
		else if(pval->vt == DT_FLOAT64)
			ptv = new CTv_float64;
		else 
			return FALSE;

		if(ptv == NULL)
			return FALSE;

		ptv->m_szname = sname;
		m_map.SetAt(sname,ptv);//���浽MAP��
	}
	//�������������ɸ���
	ptv->m_ncomptype = ncomptype;
	ptv->m_fcompval = dblcompval;

	return ptv->AddReal(pval,pbaseval);
}

void CCompCashe::SaveLastVal(CMemCashe* pmc)//�洢���һ��ֵ���˳�ʱʹ��
{
	CBaseVal bv;
	CString szkey;
	CTvObj* pi;
	POSITION pos = m_map.GetStartPosition();
	while(pos!=NULL)
	{
		m_map.GetNextAssoc(pos,szkey,pi);
		if(pi->GetLastHisVal(&bv))
			pmc->AddTagVal(&bv);
	}
}


//////////////////////////////////////////////////////////////////////////
CMemCashe::CMemCashe()
{
	m_pvals = NULL;
	m_dwNextID = 1;
	m_nsize = 0;
	m_nGetPos = 0;
	m_pvals = (struct t_caval*)calloc(MEM_CASHE_SIZE,sizeof(struct t_caval)); //�����ڴ�

	m_mapname.InitHashTable(77777);
	m_mapid.InitHashTable(77777);

	m_szDiskCasheFile = "";//���̻����ļ�
}

CMemCashe::~CMemCashe()
{
	if(m_pvals)
		free(m_pvals);
	m_pvals = NULL;

	ClearMap();
}

void CMemCashe::ClearMap()//���MAP
{
	DWORD dwid;
	LPNAMEID pid = NULL;
	POSITION pos = m_mapid.GetStartPosition();
	while(pos)
	{
		m_mapid.GetNextAssoc(pos,dwid,pid);
		delete pid;
	}
	m_mapid.RemoveAll();
	m_mapname.RemoveAll();
}

DWORD CMemCashe::GetID(LPCTSTR lpszName)
{
	LPNAMEID pid = NULL;
	if(m_mapname.Lookup(lpszName,pid))
		return pid->dwid;
	return 0;
}

CString  CMemCashe::GetName(DWORD dwid)
{
	CString szname = "";
	LPNAMEID p=NULL;
	if(m_mapid.Lookup(dwid,p))
		szname = p->sname;
	return szname;
}

DWORD CMemCashe::MakeID(LPCTSTR lpszName)
{
	DWORD dwid = GetID(lpszName);
	if(dwid != 0)
		return dwid;
	LPNAMEID pid = new TNAMEID;
	if(pid == NULL)
		return 0;
	memset(pid,0,sizeof(TNAMEID));
	strncpy(pid->sname,lpszName,79);
	pid->dwid = m_dwNextID++;

	m_mapname.SetAt(pid->sname,pid);
	m_mapid.SetAt(pid->dwid,pid);
	return pid->dwid;
}

BOOL CMemCashe::IsFull()
{
	if(!m_pvals)
		return TRUE;
	return (m_nsize >= MEM_CASHE_SIZE);
}

BOOL CMemCashe::AddTagVal(CBaseVal* pbaseval)//�������ֵ
{
	if(IsFull())
		return FALSE;
	DWORD dwid = MakeID(pbaseval->m_szName);
	if(dwid == 0)
		return FALSE;

	m_pvals[m_nsize].dwid = dwid;
	m_pvals[m_nsize].wtype = (WORD)pbaseval->m_val.vt; //����
	m_pvals[m_nsize].llval = pbaseval->m_val.llval; //ֵ
	m_pvals[m_nsize].ltime = pbaseval->m_val.ltime; //ʱ��
	m_pvals[m_nsize].snqa = pbaseval->m_val.snqa;	//����
	m_pvals[m_nsize].dwres = 0;	//����
	m_nsize++;
	return TRUE;
}

void CMemCashe::Reset()		//��λ�����³�ʼ��
{
	ClearMap();
	m_dwNextID = 1;
	m_nsize = 0;
	m_nGetPos = 0;
	m_szDiskCasheFile = "";
}

BOOL CMemCashe::SaveToDisk(LPCTSTR lpszPath) //���浽����
{
	CString szdisk="";
	szdisk = lpszPath;
	szdisk = szdisk.Left(3);
	if(Utility::GetDiskSpace(szdisk) < 500)  //���̿ռ䲻��
	{
		long ltime = (long)::time(NULL);
		if((ltime - g_lLastDiskErrTime) > 300) //5���ӱ���һ��
		{
			GetLogger()->AddError("���̿ռ�С��500M����ֹ����д��!");
			g_lLastDiskErrTime = ltime;
		}
		return FALSE;
	}


	CString szFile="",szf="";
	SYSTEMTIME syst;
	::GetLocalTime(&syst);
	szFile.Format("%04d%02d%02d-%02d%02d%02d-%03d.cah",syst.wYear,syst.wMonth,syst.wDay,
		syst.wHour,syst.wMinute,syst.wSecond,syst.wMilliseconds);
	szf = lpszPath;
	szf += szFile;

	if(-1 == _access( lpszPath, 0 ))//Ŀ¼������
	{
		TRACE("CMemCashe::SaveToDisk() Ŀ¼������!\n");
		if(!Utility::CreateDir(lpszPath))
		{
			TRACE("CMemCashe::SaveToDisk() ����Ŀ¼ʧ��!\n");
			return FALSE;//����Ŀ¼ʧ��!
		}
	}

	

	CFile fl;
	//�����ļ� 
	if(!fl.Open(szf,CFile::modeCreate|CFile::modeWrite))
			return FALSE;
	TCAFHEAD th;
	th.dwflag = 0xDF23AD79;
	th.ldovals = 0;
	th.lnameids = m_mapid.GetCount();
	th.lvals = m_nsize;
	try
	{
		
		fl.Write(&th,sizeof(TCAFHEAD)); //д��ͷ��

		//д��name-id��
		DWORD dwid;
		LPNAMEID pid = NULL;
		POSITION pos = m_mapid.GetStartPosition();
		while(pos)
		{
			m_mapid.GetNextAssoc(pos,dwid,pid);
			fl.Write(pid,sizeof(TNAMEID));
		}
		//д�����ݼ�¼
		int i;
		for(i=0;i<m_nsize;i++)
			fl.Write(&m_pvals[i],sizeof(struct t_caval));
	}
	catch ( CFileException *ex ) 
	{
	   ex->Delete();
	   fl.Close(); //�ر��ļ�
	   ::DeleteFile(szf);//ɾ��������ļ�
	   return FALSE;
	}
	GetLogger()->AddInfoFmt("д�뻺��<%s>,lnameids = %d,lvals = %d",szf,th.lnameids,th.lvals);

	return TRUE;
}

BOOL CMemCashe::LoadFromDisk(LPCTSTR lpszPath)//�Ӵ��̶�ȡ���µ�,����ɾ����������ǰ�ļ�
{
	if(m_nsize > 0) //�����ڴ滺��
		return FALSE;

	if(!m_szDiskCasheFile.IsEmpty())
	{
		GetLogger()->AddInfoFmt("<%s>�������!",m_szDiskCasheFile);
	}
	Reset(); //��λ
	int nfiles = 0;
	CFileFind ff;
    CString strDir, strFile,szret="";

    strDir = lpszPath;
    strDir += "*.cah";

    BOOL bFind = ff.FindFile(strDir);
    while ( bFind )
    {
        bFind = ff.FindNextFile();
        if ( ff.IsDots() )
            continue;
		if ( ff.IsDirectory() )
			continue;

		nfiles++;
		strFile = ff.GetFileName ();
        if(szret.IsEmpty())
		{
			szret = strFile;
			continue;
		}
		if(_stricmp(strFile,szret) > 0) //��
			szret = strFile;
    }
    ff.Close();
    if(szret.IsEmpty())
		return FALSE;

	//strFile ���򿪵�ȫ·���ļ�
	strFile = lpszPath;
	strFile += szret;

	CFile fl;
	//���ļ� 
	if(!fl.Open(strFile,CFile::modeRead))
			return FALSE;

	m_szDiskCasheFile = strFile;
	
	GetLogger()->AddInfoFmt("��ʼ���ͻ���<%s>",m_szDiskCasheFile);

	TCAFHEAD th;
	memset(&th,0,sizeof(TCAFHEAD));

	int i;
	try
	{
		
		if(sizeof(TCAFHEAD) != fl.Read(&th,sizeof(TCAFHEAD)) )//����ͷ��
		{
			fl.Close();
			::DeleteFile(strFile);
			return FALSE;
		}
		if(th.dwflag != 0xDF23AD79)
		{
			fl.Close();
			::DeleteFile(strFile);
			return FALSE;
		}
		//����name-id��
		TNAMEID tid,*ptid;
		for(i=0;i<th.lnameids;i++)
		{
			if(sizeof(TNAMEID) != fl.Read(&tid,sizeof(TNAMEID)) )
			{
				fl.Close();
				::DeleteFile(strFile);
				Reset();
				return FALSE;
			}
			ptid = new TNAMEID;
			if(ptid)
			{
				memcpy(ptid,&tid,sizeof(TNAMEID));
				m_mapid.SetAt(ptid->dwid,ptid);
				m_mapname.SetAt(ptid->sname,ptid);
			}
		}

		//�������ݼ�¼
		int i;
		for(i=0;i<th.lvals && MEM_CASHE_SIZE;i++)
		{
			if(sizeof(struct t_caval) != fl.Read(&m_pvals[i],sizeof(struct t_caval)) )
				break;
			m_nsize++;
		}
	}
	catch ( CFileException *ex ) 
	{
	   ex->Delete();
	   fl.Close(); //�ر��ļ�
	   ::DeleteFile(strFile);//ɾ��������ļ�
	   return FALSE;
	}
	fl.Close(); //�ر��ļ�
	::DeleteFile(strFile);//ɾ���ļ�
	return TRUE;
}

int  CMemCashe::GetLastRecs(CTagVal* lprecs,int nsize)//ȡ���µ�cashe��������cashe����,���ظ���
{
	int i,n=0;
	CString szname = "";
	struct t_caval*  ptca = NULL;
	for(i = m_nsize-1; i>=0 && n < nsize;i--)
	{
		szname = GetName(m_pvals[i].dwid);
		if(szname.IsEmpty())
			continue;
	
		lprecs[n].m_szNamedb = szname; //Ŀ���������ǩ��
		lprecs[n].m_nType  = m_pvals[i].wtype;
		lprecs[n].m_nCompType = 0;//cashe���ݲ�����ѹ����ֱ����д����
		lprecs[n].m_dblcompval = 0;//cashe���ݲ�����ѹ����ֱ����д����
		lprecs[n].m_val.ResetType(m_pvals[i].wtype);
		lprecs[n].m_val.llval = m_pvals[i].llval;
		lprecs[n].m_val.ltime = m_pvals[i].ltime;
		lprecs[n].m_val.snqa = m_pvals[i].snqa;
		lprecs[n].m_val.snerr = 0;
		n++;
	}
	m_nGetPos = i;
	if(m_nGetPos < 0)
		m_nGetPos = 0;
	return n;
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
CPfCashe::CPfCashe()
{
	m_szPath = "";
}
CPfCashe::~CPfCashe()
{

}

CString CPfCashe::GetLastFile(int &n)//��ȡ���µ�ҳ���ļ����������ļ�����
{
	n = 0;
	CFileFind ff;
    CString strDir, strFile,szret="";

    strDir = m_szPath;
    strDir += "*.pgf";

    BOOL bFind = ff.FindFile(strDir);
    while ( bFind )
    {
        bFind = ff.FindNextFile();
        if ( ff.IsDots() )
            continue;
		if ( ff.IsDirectory() )
			continue;

		n++;
		strFile = ff.GetFileName ();
        if(szret.IsEmpty())
		{
			szret = strFile;
			continue;
		}
		if(_stricmp(strFile,szret) > 0) //��
			szret = strFile;
    }
    ff.Close();
    return szret;
}

BOOL CPfCashe::Init(LPCTSTR lpszPath)//��ʼ�����������ļ���û���򴴽�
{
	int nfiles = 0;
	m_szPath = lpszPath;
	Utility::MakePath(m_szPath);
	Utility::CreateDir(m_szPath); //����Ŀ¼
	CString szFile = GetLastFile(nfiles);
	if(szFile.IsEmpty())
	{
		SYSTEMTIME syst;
		::GetLocalTime(&syst);
		szFile.Format("%04d%02d%02d-%02d%02d%02d.pgf",syst.wYear,syst.wMonth,syst.wDay,
			syst.wHour,syst.wMinute,syst.wSecond);
		if(PFR_OK != m_pf.Create(m_szPath + szFile)) //����ҳ���ļ�
			return FALSE;
		return TRUE;
	}
	if(PFR_OK != m_pf.Open(m_szPath + szFile)) //��ҳ���ļ�
		return FALSE;
	GetLogger()->AddInfoFmt("��ǰ�ɱ�����ҳ���ļ�<%s>!",m_szPath + szFile);
	return TRUE;
}	

//void g_AddLog(long ltime,LPCTSTR lpszTxt);
BOOL CPfCashe::SaveRec(CPfRecVal* pvar)//д��
{
	CString szdisk="";
	szdisk = m_szPath;
	szdisk = szdisk.Left(3);
	if(Utility::GetDiskSpace(szdisk) < 500)  //���̿ռ䲻��
	{
		long ltime = (long)::time(NULL);
		if((ltime - g_lLastDiskErrTime) > 300) //5���ӱ���һ��
		{
			GetLogger()->AddError("���̿ռ�С��500M����ֹ����д��!");
			g_lLastDiskErrTime = ltime;
		}
		return FALSE;
	}

	if(PFR_OK == m_pf.WriteRec(pvar)) //�ɹ�
		return TRUE;

	m_pf.Close(); //�ر�

	//������ҳ���ļ�
	CString szFile;
	SYSTEMTIME syst;
	::GetLocalTime(&syst);
	szFile.Format("%04d%02d%02d-%02d%02d%02d.pgf",syst.wYear,syst.wMonth,syst.wDay,
		syst.wHour,syst.wMinute,syst.wSecond);
	if(PFR_OK != m_pf.Create(m_szPath + szFile)) //����ҳ���ļ�
		return FALSE;

	//д�뵽�µ�
	if(PFR_OK == m_pf.WriteRec(pvar)) //�ɹ�
		return TRUE;

	return FALSE;
}

void CPfCashe::OpenNew()
{
	int nfiles = 0;
	CString szFile = GetLastFile(nfiles);
	if(nfiles == 1)
	{
		m_pf.EmptyFile();
		
		return; //ֻ�е�ǰ��һ��
	}
	m_pf.CloseAndDelFile(); //�رպ�ɾ����ǰ

	// ���µ����һ���ļ�
	szFile = GetLastFile(nfiles);
	if (szFile.IsEmpty())
	{
		SYSTEMTIME syst;
		::GetLocalTime(&syst);
		szFile.Format("%04d%02d%02d-%02d%02d%02d.pgf",syst.wYear,syst.wMonth,syst.wDay,
			syst.wHour,syst.wMinute,syst.wSecond);
	}

	m_pf.Open(m_szPath + szFile);
	GetLogger()->AddInfoFmt("���µĿɱ�����ҳ���ļ�<%s>!",m_szPath + szFile);
}

////////////////////////////////////////////////////////////////////////
CLocalCashe::CLocalCashe()
{
	m_szPath = "";
}

CLocalCashe::~CLocalCashe()
{
	m_compcashe.SaveLastVal(&m_memcashe); //�洢���һ��,��ѹ���������в�������ֵ������ǿ�Ʊ������ָ

	if(!m_memcashe.IsEmpty())
	{
		m_memcashe.SaveToDisk(m_szPath); //�洢������
		m_memcashe.Reset();
	}
}

void CLocalCashe::AddRealVal(LPCTSTR sname,int ncomptype,double dblcompval,LPVVAL pval)//��ӻ�������ʵʱֵ
{
	CSafeLock lock(&m_lock); //����

	CBaseVal bv;
	if(m_compcashe.AddReal(sname,ncomptype,dblcompval,pval,&bv))
	{
		m_memcashe.AddTagVal(&bv);
		if(m_memcashe.IsFull())
		{
			m_memcashe.SaveToDisk(m_szPath);
			m_memcashe.Reset();
		}
	}
}

void CLocalCashe::AddRealVar(LPCTSTR sname,CVarVal *pval)//дString,Blob ��cashe
{
	CSafeLock lock(&m_lock); //����

	if((pval->vt != DT_STRING)&&(pval->vt != DT_BLOB))
		return;
	CPfRecVal rv;
	rv.m_ltime = pval->ltime;
	rv.m_ntype = (pval->vt == DT_STRING)? PFILE_STRING : PFILE_BLOB;
	rv.m_snqa = pval->snqa;
	rv.m_szdes.Format("%s",sname);
	rv.SetData(pval->pstr,pval->lvlen);

	if(!m_pfcashe.SaveRec(&rv))
		TRACE("CLocalCashe::AddRealVar Err!\n");
}

void CLocalCashe::AddSoe(CSoe *psoe)//дSOE��cashe
{
	CSafeLock lock(&m_lock); //����

	CPfRecVal rv;
	rv.m_ltime = psoe->m_ltime;
	rv.m_ntype = PFILE_SOE;
	rv.m_snqa = (short)psoe->m_lmsec; //��������ĺ���
	rv.m_dwres = psoe->m_lclsid;	  // m_dwres����ķ���

	rv.m_szdes.Format("%s",psoe->m_ssrc); //�����������Դ
	
	CMyByteArray ua;
	ua.PutData(psoe,sizeof(TSOE) - 4);
	ua.PutData(psoe->m_pbinargs,psoe->m_lbinarglen);

	int n=0;
	BYTE *pb = ua.GetBuf(n);
	rv.SetData(pb,n);

	if(!m_pfcashe.SaveRec(&rv))
		TRACE("CLocalCashe::AddSoe Err!\n");
}
void CLocalCashe::SetPath(LPCTSTR lpszPath){
	m_szPath = lpszPath;
	m_pfcashe.Init(m_szPath + "PageFile\\"); //ҳ���ļ�
}; //����Ŀ¼


int  CLocalCashe::GetLastRecs(CTagVal* lprecs,int nsize)//ȡ���µ�cashe��������cashe����,���ظ���
{
	CSafeLock lock(&m_lock); //����
	return m_memcashe.GetLastRecs(lprecs,nsize);
}

void CLocalCashe::GetPosOk()//ȷ��ȥ���ͻ���
{
	CSafeLock lock(&m_lock); //����
	m_memcashe.GetPosOk();
}

//h���ڶ����ͳɹ���ɾ����¼��
int	CLocalCashe::GetLastPfRec(PF_HANDLE &h, CPfRecVal* prec)
{
	CSafeLock lock(&m_lock); //����
	return m_pfcashe.GetLastRec(h,prec);
};

int	CLocalCashe::GetFirstPfRec(PF_HANDLE &h,CPfRecVal* prec)
{
	CSafeLock lock(&m_lock); //����
	return m_pfcashe.GetFirstRec(h,prec);
}
void CLocalCashe::DeletePfRec(PF_HANDLE h)
{
	CSafeLock lock(&m_lock); //����
	m_pfcashe.DeleteRec(h);//ɾ����¼
}

//���µ�ҳ���ļ�,���ڻ���ҳ���ļ�������ɺ��һ���µ�
void CLocalCashe::OpenNewPgeFile()
{
	CSafeLock lock(&m_lock); //����
	m_pfcashe.OpenNew();
}

//�Ӵ��̶�ȡ���µ�
BOOL CLocalCashe::LoadFromDisk()
{
	CSafeLock lock(&m_lock); //����
	return m_memcashe.LoadFromDisk(m_szPath);
}