#include "StdAfx.h"
#include "TagItem.h"
#include "drvdef.h"
#include "sdktool.h"
#include "define.h"

CTagItem::CTagItem()
{
	m_ntype = DRT_FLOAT32;	//��������
	m_szName="";	//��ǩ��
	m_szdes="";		//����
	m_szunit="";	//��λ
	m_sznamedb="";	//��ʵʱ���ж�Ӧ�ı�ǩ��
	m_sznamedrv="";//��Ӧ�����б�ǩ
	m_szexpcp="";	//�ɼ�ֵת��������ֵת����ʽ
	m_szexppc="";	//����ֵת��Ϊ�ɼ�ֵ��ʽ,�ݲ���

	m_nCompType = CASHE_COMP_NONE;//ѹ����ʽ
	m_szCompVal = "";//ѹ����

	//	m_szres = "";	//����
	m_bSel  = FALSE;

	m_nErrStatus = 0;

	m_ldecid = 0;//�豸ID modify811
}

CTagItem::~CTagItem()
{

}

void		CTagItem::CopyTo(CTagItem *pt)
{
	pt->m_ntype = m_ntype;	
	pt->m_szName=m_szName;	
	pt->m_szdes=m_szdes;		
	pt->m_szunit=m_szunit;	
	pt->m_sznamedb=m_sznamedb;	
	pt->m_sznamedrv=m_sznamedrv;
	pt->m_szexpcp=m_szexpcp;	
	pt->m_szexppc=m_szexppc;	
	pt->m_nCompType = m_nCompType;	
	pt->m_szCompVal = m_szCompVal;
}

int		CTagItem::Compare(CSortItem *pOb)	//����Ƚ�
{
	int nret = 0;
	if(pOb == NULL)
		return nret;
	CTagItem *pi = (CTagItem *)pOb;

	switch(m_nSortWho)
	{
	case TGF_NAME:  
		nret = strcmp(m_szName,pi->m_szName);
		break;
	case TGF_NAMEDB: 
		nret = strcmp(m_sznamedb,pi->m_sznamedb);
		break;
	case TGF_DES: 
		nret = strcmp(m_szdes,pi->m_szdes);
		break;
	case TGF_UNIT: 
		nret = strcmp(m_szunit,pi->m_szunit);
		break;
	case TGF_TYPE: 
		if(m_ntype > pi->m_ntype)
			nret = 1;
		else if(m_ntype < pi->m_ntype)
			nret = -1;
		break;
	case TGF_NAMEDRV: 
		nret = strcmp(m_sznamedrv,pi->m_sznamedrv);
		break;
		//{{ modify811
	case TGF_QA:
		{
			CVarVal v1,v2;
			//gGetDriveMgr()->m_DecValMan.GetTagVal(m_ldecid,m_szname,v1);
			//gGetDriveMgr()->m_DecValMan.GetTagVal(m_ldecid,pi->m_szname,v2);
			if(v1.snqa > v2.snqa)
				nret = 1;
			else if(v1.snqa < v2.snqa)
				nret = -1;
		}
		break;
	case TGF_TIME:
		{
			CVarVal v1,v2;
			//gGetDriveMgr()->m_DecValMan.GetTagVal(m_ldecid,m_szname,v1);
			//gGetDriveMgr()->m_DecValMan.GetTagVal(m_ldecid,pi->m_szname,v2);
			if(v1.ltime > v2.ltime)
				nret = 1;
			else if(v1.ltime < v2.ltime)
				nret = -1;
		}
		break;
		//}}
	}
	if(!m_bAsc)
		nret *= -1;
	return nret;	
}

void	CTagItem::parsecomp(LPCTSTR lpsztxt)//�ֽ�ѹ������
{
	const char* pc = lpsztxt;
	if(pc == NULL)
	{
		m_nCompType = CASHE_COMP_NONE;//ѹ����ʽ
		m_szCompVal = "";//ѹ����
		return;
	}
	CString sztype = "",szval = "";
	int npos = 0;
	while(*pc != 0)
	{
		if(*pc != '\n')
		{
			if(npos == 0)
				sztype += *pc;
			else if(npos == 1)
				szval += *pc;
		}
		else
			npos++;
		pc++;
	}
	m_nCompType = atoi(sztype);//ѹ����ʽ
	m_szCompVal = szval;//ѹ����
}

void	CTagItem::Serialize(CArchive& ar)	//����
{
	CString szcom = "";
	if(ar.IsLoading())
	{
		ar >> m_ntype;	
		ar >> m_szName;	
		ar >> m_szdes;	
		ar >> m_szunit;	
		ar >> m_sznamedb;	
		ar >> m_sznamedrv;
		ar >> m_szexpcp;
		ar >> m_szexppc;

		ar >> szcom;	//ѹ������
		parsecomp(szcom);
	}
	else
	{
		ar << m_ntype;	
		ar << m_szName;	
		ar << m_szdes;	
		ar << m_szunit;	
		ar << m_sznamedb;	
		ar << m_sznamedrv;
		ar << m_szexpcp;
		ar << m_szexppc;

		szcom.Format("%d\n%s\n",m_nCompType,m_szCompVal);
		ar << szcom;	//ѹ������
	}
}

CString CTagItem::GetTypeStr(int ntype)
{
	if(ntype == DRT_DIGITAL)
		return  CString("digital");
	else if(ntype == DRT_INT32)
		return  CString("int32");
	else if(ntype == DRT_FLOAT32)
		return  CString("float32");
	else if(ntype == DRT_INT64)
		return  CString("int64");
	else if(ntype == DRT_FLOAT64)
		return  CString("float64");
	else if(ntype == DRT_STRING)
		return  CString("string");
	else if(ntype == DRT_BLOB)
		return  CString("blob");
	return CString("void");
}

int CTagItem::GetTypeVal(LPCTSTR lpszType)
{
	if(!_stricmp("digital",lpszType))
		return DRT_DIGITAL;
	else if(!_stricmp("int32",lpszType))
		return DRT_INT32;
	else if(!_stricmp("float32",lpszType))
		return DRT_FLOAT32;
	else if(!_stricmp("int64",lpszType))
		return DRT_INT64;
	else if(!_stricmp("float64",lpszType))
		return DRT_FLOAT64;
	else if(!_stricmp("string",lpszType))
		return DRT_STRING;
	else if(!_stricmp("blob",lpszType))
		return DRT_BLOB;
	return DRT_VOID;
}

CString CTagItem::GetCompTypeStr(int ntype)
{
	if(ntype == CASHE_COMP_NONE)
		return  CString("no");
	else if(ntype == CASHE_COMP_PER)
		return  CString("per");
	else if(ntype == CASHE_COMP_VAL)
		return  CString("val");
	return CString("no");
}
int		CTagItem::GetCompTypeVal(LPCTSTR lpszType)
{
	if(!_stricmp("no",lpszType))
		return CASHE_COMP_NONE;
	else if(!_stricmp("per",lpszType))
		return CASHE_COMP_PER;
	else if(!_stricmp("val",lpszType))
		return CASHE_COMP_VAL;
	return CASHE_COMP_NONE;
}
