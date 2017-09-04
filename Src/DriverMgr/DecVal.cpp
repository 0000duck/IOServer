#include "StdAfx.h"
#include "DecVal.h"
#include "Expression/ExpressionTagNameParser.h"

CDecVal::CDecVal(void)
{
	int nHashSize = ::CalcHashTableSize(100000); // ��10w��ǩ��Ĺ�ģ���
	m_map.InitHashTable( nHashSize );
	m_mapd2t.InitHashTable( nHashSize );
	m_array.SetSize(0,4096);
	m_nposval = 0;
	m_nposvar = 0;
	m_id = 0;
	m_szDecName = "";
	m_bvardataok = FALSE;
};

CDecVal::CDecVal(long lid,int ntags)
{
	int nHashSize = ::CalcHashTableSize(ntags);
	m_map.InitHashTable(nHashSize);
	m_mapd2t.InitHashTable(nHashSize);//©����һ�䣬����DDC���ɺ���
	m_array.SetSize(0,ntags);
	m_id = lid;
	m_nposval = 0;
	m_nposvar = 0;

	m_szDecName = "";
};
CDecVal::~CDecVal(void)
{
	//ɾ����ǩ��
	m_map.RemoveAll();
	int i,n = m_array.GetSize();
	for(i=0;i<n;i++)
		delete m_array.GetAt(i);
	m_array.RemoveAll();

	//ɾ��ӳ��
	CString szkey;
	CDec2TagVal *pd2t, *pd2tNext;
	POSITION pos = m_mapd2t.GetStartPosition();
	while(pos != NULL)
	{
		m_mapd2t.GetNextAssoc(pos,szkey,pd2t);

		while(pd2t)
		{
			pd2tNext = pd2t->m_pNext;
			delete pd2t;

			pd2t = pd2tNext;
		}
	}
	m_mapd2t.RemoveAll();
}
CTagVal* CDecVal::GetTagVal(LPCTSTR lpszName)//ȡ��ǩֵָ��
{
	CTagVal* pt = NULL;
	if(m_map.Lookup(lpszName,pt))
		return pt;
	return NULL;
}; //ֻ�ڼ���ص���ʹ��
void CDecVal::Init(CDrvItem* pdec)//��ʼ��
{
	//��������ǩ
	CString szkey="",sztags = "",sztg="";
	//LPCTSTR pc;
	CDec2TagVal* pd2t;
	CTagVal* ptv;
	CTagItem* ptag=NULL,*ptg = NULL;

	m_szDecName = pdec->m_szDecName;//�豸����
	int i,ntags = pdec->m_array.GetSize();

	//��ɨ��û�й�ʽ�ı�ǩ
	for(i=0;i<ntags;i++)
	{
		ptag = (CTagItem*)pdec->m_array.GetAt(i);
		//�����Ƿ����
		if(m_map.Lookup(ptag->m_szName,ptv))
			continue;
		ptag->m_szexpcp.TrimLeft();
		ptag->m_szexpcp.TrimRight();

		if(!ptag->m_szexpcp.IsEmpty()) //�й�ʽ
			continue;

		//�½�CTagVal
		ptv = new CTagVal(ptag->m_szName,ptag->m_sznamedb,ptag->m_szexpcp,ptag->m_ntype,ptag->m_nCompType,ptag->m_szCompVal);
		if(ptv == NULL)
			continue;
		
		//����CDecVal
		m_map.SetAt(ptv->m_szName,ptv);
		m_array.Add(ptv);

		//����ӳ�䣬��һ��ӳ����������
		pd2t = NULL;
		if(!m_mapd2t.Lookup(ptag->m_sznamedrv,pd2t))
		{
			pd2t = new CDec2TagVal(ptag->m_sznamedrv);
			if(pd2t)
			{
				pd2t->m_tab.Add(ptv);
				m_mapd2t.SetAt(ptag->m_sznamedrv,pd2t);
			}
		}
		else if (pd2t) { // �Ѿ�����
			CDec2TagVal* pd2t2 = new CDec2TagVal(ptag->m_sznamedrv);
			if(pd2t2)
			{
				pd2t2->m_tab.Add(ptv);
				pd2t->AddItem(pd2t2);
			}
		}
	}

	//��ɨ���й�ʽ��
	for(i=0;i<ntags;i++)
	{
		ptag = (CTagItem*)pdec->m_array.GetAt(i);
		//�����Ƿ����
		if(m_map.Lookup(ptag->m_szName,ptv))
			continue;
		ptag->m_szexpcp.TrimLeft();
		ptag->m_szexpcp.TrimRight();

		if(ptag->m_szexpcp.IsEmpty()) //�޹�ʽ��ǰ���Ѿ�����
			continue;

		//�½�CTagVal
		ptv = new CTagVal(ptag->m_szName,ptag->m_sznamedb,ptag->m_szexpcp,ptag->m_ntype,ptag->m_nCompType,ptag->m_szCompVal);
		if(ptv == NULL)
			continue;
		
		//����CDecVal
		m_map.SetAt(ptv->m_szName,ptv);
		m_array.Add(ptv);

		//����ӳ�䣬��һ��ӳ����������
		pd2t = NULL;
		if(!m_mapd2t.Lookup(ptag->m_sznamedrv,pd2t))
		{
			pd2t = new CDec2TagVal(ptag->m_sznamedrv);
			if(pd2t)
			{
				pd2t->m_tab.Add(ptv);
				m_mapd2t.SetAt(ptag->m_sznamedrv,pd2t);
			}
		}
		else if (pd2t) { // �Ѿ�����
			CDec2TagVal* pd2t2 = new CDec2TagVal(ptag->m_sznamedrv);
			if(!pd2t2)
				continue;

			pd2t2->m_tab.Add(ptv);
			pd2t->AddItem(pd2t2);			

			pd2t = pd2t2;
		}

		CExpressionTagNameParser parser;
		parser.Parse(ptag->m_szexpcp);
		for (int j = 0; j < parser.GetCount(); j++)
		{
			sztg = parser.GetAt(j);

			// �ǲ��Ǳ�ǩ�Լ���
			if (sztg == _T("__this__"))
				sztg = ptag->m_szName;

			if(pdec->m_map.Lookup(sztg,ptg))
			{
				//����ӳ�䣬�ӵڶ�����ʼ
				/*
				if(!m_mapd2t.Lookup(ptg->m_sznamedrv,pd2t))
				{
					pd2t = new CDec2TagVal(ptg->m_sznamedrv);
					if(pd2t)
						m_mapd2t.SetAt(ptg->m_sznamedrv,pd2t);
				}
				if(pd2t)
					pd2t->m_tab.Add(ptv);
				*/
				if(pd2t)
					pd2t->m_tab.Add(ptv);
			}
		}
		//��������ʽ�еı�ǩ��
		/*
		sztags = g_parsetvtagname(ptag->m_szexpcp);
		pc = sztags;
		if(pc)
		{
			sztg = "";
			while(*pc != 0)
			{
				if(*pc != '\n')
					sztg += *pc;//sztg�Ǳ�ǩ��������������ǩ��
				else
				{
					//�ҵ�������ǩ����Ϊ��������
					if(pdec->m_map.Lookup(sztg,ptg))
					{
						//����ӳ��
						if(!m_mapd2t.Lookup(ptg->m_sznamedrv,pd2t))
						{
							pd2t = new CDec2TagVal(ptg->m_sznamedrv);
							if(pd2t)
								m_mapd2t.SetAt(ptg->m_sznamedrv,pd2t);
						}
						if(pd2t)
							pd2t->m_tab.Add(ptv);
					}
					sztg = ""; //��������һ��
				}
				pc++;
			}
		}
		*/
	}

#if 0
	POSITION pos = m_mapd2t.GetStartPosition();
	while(pos)
	{
		m_mapd2t.GetNextAssoc(pos, szkey, pd2t);
		if (pd2t->m_tab.GetSize() > 1)
		{
			for (int i = 1; i < pd2t->m_tab.GetSize(); i++)
			{
				GetLogger()->AddInfoFmt(_T("���ִ�����ǩ%s -> %s"), pd2t->m_szname, pd2t->m_tab[i]->m_szNamedb);
			}
		}
	}
#endif
}

void CDecVal::OnDecDataOK(T_DRREC recs[],int nsize) //����豸ֵ�������ڻص�������ʹ�ã�����
{
	//���ҵ�CDec2TagVal
	CDec2TagVal *pd2t;
	int i;
	CString szdrvname;
	for(i=0;i<nsize;i++)
	{
		recs[i].sname[79] = 0;
		if(!m_mapd2t.Lookup(recs[i].sname,pd2t))
			continue;

#if 0
		if ((strcmp(recs[i].sname, "REAL_MW") == 0) && ((recs[i].val.fval < 100.0) || (recs[i].val.fval > 400.0)))
		{
			CTime tf(recs[i].val.ltime);
			GetLogger()->AddInfoFmt(_T("CDecVal::OnDecDataOK(%d, %d)�յ���ǩ<REAL_MW>��ֵ = %f, ʱ�� = %s"), nsize, i, (double)recs[i].val.fval, tf.Format(_T("%Y-%m-%d %H:%M:%S")));
		}

#endif

		while(pd2t)
		{
			pd2t->OnDrvTagDataOk(this,&recs[i]);
			pd2t = pd2t->GetNext();
		}
	}
}

DWORD CDecVal::OnWriteHistoryCB(T_DRREC recs[],int nsize)
{
	return 0;
}

int  CDecVal::GetSaveRecs(CTagVal* lprecs,int nsize)//ȡ��������
{
	int n = m_array.GetSize();
	if(n == 0)
		return 0;
	if(m_nposval >= n)
		m_nposval = 0;
	int ndo=0,nh = m_nposval;
	CTagVal* pv=NULL;
	long ltime = (long)::time(NULL);
	int nCounter = 0;
	do{
		++nCounter;
		if ((nCounter % 10000) == 0)
		{
			Sleep(1);
			nCounter = 0;
		}
		pv = m_array.GetAt(m_nposval);

		if(pv->m_val.ltime > 0 )
		{
			if((pv->m_nType >DT_VOID) && (pv->m_nType < DT_STRING)) 
			{
				if(pv->m_nUpdate)
				{
					*lprecs = *pv;
					pv->m_nUpdate = 0;

					pv->m_lsendtime = ltime;
					lprecs->m_bresend = FALSE;

					ndo++;
					lprecs++;
				}
				else
				{
					if((pv->m_lsendtime > 0)&&((ltime - pv->m_lsendtime) > 300) )//5����ǿ���ط�
					{
						pv->m_lsendtime = ltime;
						*lprecs = *pv;
						lprecs->m_bresend = TRUE;
						ndo++;
						lprecs++;
					}
				}
			}	
		}
		m_nposval = (m_nposval + 1) % n;
	}while((nh != m_nposval) && (ndo < nsize));
	return ndo;
}

BOOL CDecVal::GetSaveVarVal(CTagVal* lprecs)//ȡ���̵Ŀɱ����ݣ�string blob
{
	if(!m_bvardataok)
		return FALSE;
	int n = m_array.GetSize();
	if(n == 0)
		return FALSE;
	if(m_nposvar >= n)
		m_nposvar = 0;
	int nh = m_nposvar;
	CTagVal* pv=NULL;
	long ltime = (long)::time(NULL);
	int nCounter = 0;
	do{
		++nCounter;
		if ((nCounter % 10000) == 0)
		{
			Sleep(1);
			nCounter = 0;
		}
		pv = m_array.GetAt(m_nposvar);
		if(pv->m_val.ltime > 0 )
		{
			if((pv->m_nType == DT_STRING) || (pv->m_nType == DT_BLOB))
			{
				if(pv->m_nUpdate)
				{
					*lprecs = *pv;
					pv->m_nUpdate = 0;

					pv->m_lsendtime = ltime;
					lprecs->m_bresend = FALSE;

					m_nposvar = (m_nposvar + 1) % n;
					return TRUE;
				}
				else
				{
					if((pv->m_lsendtime > 0)&&((ltime - pv->m_lsendtime) > 300) )//5����ǿ���ط�
					{
						pv->m_lsendtime = ltime;
						*lprecs = *pv;
						lprecs->m_bresend = TRUE;

						m_nposvar = (m_nposvar + 1) % n;
						return TRUE;
					}
				}
			}	
		}
		m_nposvar = (m_nposvar + 1) % n;
	}while(nh != m_nposvar);

	m_bvardataok = FALSE;
	return FALSE;
}