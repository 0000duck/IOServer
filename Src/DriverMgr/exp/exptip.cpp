#include "stdafx.h"
#include "expdef.h"
#include <math.h>
BOOL CExpItem::ChangeType(int ndt)//�ı���������
{
	if(m_ndt == ndt)
		return TRUE;
	if(ndt == DT_LONG)
	{
		switch(m_ndt)
		{
			case DT_LLONG:
				m_val.lVal = (long)m_val.llVal;
				m_ndt = ndt;
				break;
			case DT_FLOAT:
				m_val.lVal = (long )m_val.fltVal;
				m_ndt = ndt;
				break;
			case DT_DOUBLE:
				m_val.lVal = (long )m_val.dblVal;
				m_ndt = ndt;
				break;
			default:
				return FALSE;
				break;
		}
		return TRUE;
	}
	else if(ndt == DT_FLOAT)
	{
		switch(m_ndt)
		{
			case DT_LLONG:
				m_val.fltVal = (float)m_val.llVal;
				m_ndt = ndt;
				break;
			case DT_LONG:
				m_val.fltVal = (float )m_val.lVal;
				m_ndt = ndt;
				break;
			case DT_DOUBLE:
				m_val.fltVal = (float )m_val.dblVal;
				m_ndt = ndt;
				break;
			default:
				return FALSE;
				break;
		}
		return TRUE;
	}
	else if(ndt == DT_DOUBLE)
	{
		switch(m_ndt)
		{
			case DT_LLONG:
				m_val.dblVal = (double)m_val.llVal;
				m_ndt = ndt;
				break;
			case DT_LONG:
				m_val.dblVal = (double )m_val.lVal;
				m_ndt = ndt;
				break;
			case DT_FLOAT:
				m_val.dblVal = (double )m_val.fltVal;
				m_ndt = ndt;
				break;
			default:
				return FALSE;
				break;
		}
		return TRUE;
	}

	else if(ndt == DT_LLONG)
	{
		switch(m_ndt)
		{
			case DT_DOUBLE:
				m_val.llVal = (__int64)m_val.dblVal;
				m_ndt = ndt;
				break;
			case DT_LONG:
				m_val.llVal = (__int64 )m_val.lVal;
				m_ndt = ndt;
				break;
			case DT_FLOAT:
				m_val.llVal = (__int64 )m_val.fltVal;
				m_ndt = ndt;
				break;
			default:
				return FALSE;
				break;
		}
		return TRUE;
	}
	return FALSE;
}

CExpItem *CExpItem::Add(CExpItem *ps)// +
{
	CExpItem *pr = new CExpItem();
	if(pr == NULL)
		return NULL;
	//ת��Ϊ��ͬ����,����ת
	if(m_ndt > ps->m_ndt){
		ps->ChangeType(m_ndt);
		pr->m_ndt = m_ndt;
	}
	else if(m_ndt < ps->m_ndt){
		ChangeType(ps->m_ndt);
		pr->m_ndt = ps->m_ndt;
	}
	else
		pr->m_ndt = m_ndt;

	if(m_ndt == DT_LONG)
		pr->m_val.lVal = m_val.lVal + ps->m_val.lVal;
	else if(m_ndt == DT_LLONG)
		pr->m_val.llVal = m_val.llVal + ps->m_val.llVal;
	else if(m_ndt == DT_FLOAT)
		pr->m_val.fltVal = m_val.fltVal + ps->m_val.fltVal;
	else if(m_ndt == DT_DOUBLE)
		pr->m_val.dblVal = m_val.dblVal + ps->m_val.dblVal;
	else{
		delete pr;
		pr = NULL;
	}
	return pr;

}
CExpItem *CExpItem::Dec(CExpItem *ps)// -
{
	CExpItem *pr = new CExpItem();
	if(pr == NULL)
		return NULL;
	//ת��Ϊ��ͬ����,����ת
	if(m_ndt > ps->m_ndt){
		ps->ChangeType(m_ndt);
		pr->m_ndt = m_ndt;
	}
	else if(m_ndt < ps->m_ndt){
		ChangeType(ps->m_ndt);
		pr->m_ndt = ps->m_ndt;
	}
	else
		pr->m_ndt = m_ndt;

	if(m_ndt == DT_LONG)
		pr->m_val.lVal = m_val.lVal - ps->m_val.lVal;
	else if(m_ndt == DT_LLONG)
		pr->m_val.llVal = m_val.llVal - ps->m_val.llVal;
	else if(m_ndt == DT_FLOAT)
		pr->m_val.fltVal = m_val.fltVal - ps->m_val.fltVal;
	else if(m_ndt == DT_DOUBLE)
		pr->m_val.dblVal = m_val.dblVal - ps->m_val.dblVal;
	else{
		delete pr;
		pr = NULL;
	}
	return pr;
}
CExpItem *CExpItem::Mut(CExpItem *ps)// *
{
	CExpItem *pr = new CExpItem();
	if(pr == NULL)
		return NULL;
	//ת��Ϊ��ͬ����,����ת
	if(m_ndt > ps->m_ndt){
		ps->ChangeType(m_ndt);
		pr->m_ndt = m_ndt;
	}
	else if(m_ndt < ps->m_ndt){
		ChangeType(ps->m_ndt);
		pr->m_ndt = ps->m_ndt;
	}
	else
		pr->m_ndt = m_ndt;

	if(m_ndt == DT_LONG)
		pr->m_val.lVal = m_val.lVal * ps->m_val.lVal;
	else if(m_ndt == DT_LLONG)
		pr->m_val.llVal = m_val.llVal * ps->m_val.llVal;
	else if(m_ndt == DT_FLOAT)
		pr->m_val.fltVal = m_val.fltVal * ps->m_val.fltVal;
	else if(m_ndt == DT_DOUBLE)
		pr->m_val.dblVal = m_val.dblVal * ps->m_val.dblVal;
	else{
		delete pr;
		pr = NULL;
	}
	return pr;
}
CExpItem *CExpItem::Div(CExpItem *ps)// /
{
	CExpItem *pr = new CExpItem();
	if(pr == NULL)
		return NULL;
	//ת��Ϊ��ͬ����,����ת
	if(m_ndt > ps->m_ndt){
		ps->ChangeType(m_ndt);
		pr->m_ndt = m_ndt;
	}
	else if(m_ndt < ps->m_ndt){
		ChangeType(ps->m_ndt);
		pr->m_ndt = ps->m_ndt;
	}
	else
		pr->m_ndt = m_ndt;

	if(m_ndt == DT_LONG){
		if(ps->m_val.lVal == 0){
			delete pr;
			pr = NULL;
		}
		else
			pr->m_val.lVal = m_val.lVal / ps->m_val.lVal;
	}
	else if(m_ndt == DT_LLONG){
		if(ps->m_val.llVal == 0){
			delete pr;
			pr = NULL;
		}
		else
			pr->m_val.llVal = m_val.llVal / ps->m_val.llVal;
	}
	else if(m_ndt == DT_FLOAT){
		if(fabs(ps->m_val.fltVal) < 0.00001){
			delete pr;
			pr = NULL;
		}
		else
			pr->m_val.fltVal = m_val.fltVal / ps->m_val.fltVal;
	}
	else if(m_ndt == DT_DOUBLE){
		if(fabs(ps->m_val.dblVal) < 0.0000000001){
			delete pr;
			pr = NULL;
		}
		else
			pr->m_val.dblVal = m_val.dblVal / ps->m_val.dblVal;
	}
	else{
		delete pr;
		pr = NULL;
	}
	return pr;
}

CExpItem *CExpItem::Mod(CExpItem *ps)// %
{
	CExpItem *pr = new CExpItem();
	if(pr == NULL)
		return NULL;
	//ת��Ϊ��ͬ����,����ת
	if(m_ndt > ps->m_ndt){
		ps->ChangeType(m_ndt);
		pr->m_ndt = m_ndt;
	}
	else if(m_ndt < ps->m_ndt){
		ChangeType(ps->m_ndt);
		pr->m_ndt = ps->m_ndt;
	}
	else
		pr->m_ndt = m_ndt;

	if(m_ndt == DT_LONG){
		if(ps->m_val.lVal == 0){
			delete pr;
			pr = NULL;
		}
		else
			pr->m_val.lVal = m_val.lVal % ps->m_val.lVal;
	}
	else if(m_ndt == DT_LLONG){
		if(ps->m_val.llVal == 0){
			delete pr;
			pr = NULL;
		}
		else
			pr->m_val.llVal = m_val.llVal % ps->m_val.llVal;
	}
	else if(m_ndt == DT_FLOAT){
		if(((long)(ps->m_val.fltVal)) == 0){
			delete pr;
			pr = NULL;
		}
		else
			pr->m_val.fltVal = (float)(((long)m_val.fltVal) % ((long)ps->m_val.fltVal));
	}
	else if(m_ndt == DT_DOUBLE){
		if(((__int64)(ps->m_val.dblVal)) == 0){
			delete pr;
			pr = NULL;
		}
		else
			pr->m_val.dblVal = (double)(((__int64)m_val.dblVal) % ((__int64)ps->m_val.dblVal));
	}
	else{
		delete pr;
		pr = NULL;
	}
	return pr;
}
CExpItem *CExpItem::LeftMove(CExpItem *ps)// <<
{
	CExpItem *pr = new CExpItem();
	if(pr == NULL)
		return NULL;
	//ת��Ϊ��ͬ����,����ת
	if(m_ndt > ps->m_ndt){
		ps->ChangeType(m_ndt);
		pr->m_ndt = m_ndt;
	}
	else if(m_ndt < ps->m_ndt){
		ChangeType(ps->m_ndt);
		pr->m_ndt = ps->m_ndt;
	}
	else
		pr->m_ndt = m_ndt;

	if(m_ndt == DT_LONG)
		pr->m_val.lVal = m_val.lVal << ps->m_val.lVal;
	else if(m_ndt == DT_LLONG)
		pr->m_val.llVal = m_val.llVal << ps->m_val.llVal;
	else if(m_ndt == DT_FLOAT)
		pr->m_val.fltVal = (float)((long)m_val.fltVal << (long)ps->m_val.fltVal);
	else if(m_ndt == DT_DOUBLE)
		pr->m_val.dblVal = (double)((__int64)m_val.dblVal << (long)ps->m_val.dblVal);
	else{
		delete pr;
		pr = NULL;
	}
	return pr;
	
}
CExpItem *CExpItem::RightMove(CExpItem *ps)// >>
{
	CExpItem *pr = new CExpItem();
	if(pr == NULL)
		return NULL;
	//ת��Ϊ��ͬ����,����ת
	if(m_ndt > ps->m_ndt){
		ps->ChangeType(m_ndt);
		pr->m_ndt = m_ndt;
	}
	else if(m_ndt < ps->m_ndt){
		ChangeType(ps->m_ndt);
		pr->m_ndt = ps->m_ndt;
	}
	else
		pr->m_ndt = m_ndt;

	if(m_ndt == DT_LONG)
		pr->m_val.lVal = m_val.lVal >> ps->m_val.lVal;
	else if(m_ndt == DT_LLONG)
		pr->m_val.llVal = m_val.llVal >> ps->m_val.llVal;
	else if(m_ndt == DT_FLOAT)
		pr->m_val.fltVal = (float)((long)m_val.fltVal >> (long)ps->m_val.fltVal);
	else if(m_ndt == DT_DOUBLE)
		pr->m_val.dblVal = (double)((__int64)m_val.dblVal >> (long)ps->m_val.dblVal);
	else{
		delete pr;
		pr = NULL;
	}
	return pr;
	
}
CExpItem *CExpItem::BitAnd(CExpItem *ps)// &
{
	CExpItem *pr = new CExpItem();
	if(pr == NULL)
		return NULL;
	//ת��Ϊ��ͬ����,����ת
	if(m_ndt > ps->m_ndt){
		ps->ChangeType(m_ndt);
		pr->m_ndt = m_ndt;
	}
	else if(m_ndt < ps->m_ndt){
		ChangeType(ps->m_ndt);
		pr->m_ndt = ps->m_ndt;
	}
	else
		pr->m_ndt = m_ndt;

	if(m_ndt == DT_LONG)
		pr->m_val.lVal = m_val.lVal & ps->m_val.lVal;
	else if(m_ndt == DT_LLONG)
		pr->m_val.llVal = m_val.llVal & ps->m_val.llVal;
	else if(m_ndt == DT_FLOAT)
		pr->m_val.fltVal = (float)((long)m_val.fltVal & (long)ps->m_val.fltVal);
	else if(m_ndt == DT_DOUBLE)
		pr->m_val.dblVal = (double)((__int64)m_val.dblVal & (__int64)ps->m_val.dblVal);
	else{
		delete pr;
		pr = NULL;
	}
	return pr;
}
CExpItem *CExpItem::BitOr(CExpItem *ps)// |
{
	CExpItem *pr = new CExpItem();
	if(pr == NULL)
		return NULL;
	//ת��Ϊ��ͬ����,����ת
	if(m_ndt > ps->m_ndt){
		ps->ChangeType(m_ndt);
		pr->m_ndt = m_ndt;
	}
	else if(m_ndt < ps->m_ndt){
		ChangeType(ps->m_ndt);
		pr->m_ndt = ps->m_ndt;
	}
	else
		pr->m_ndt = m_ndt;

	if(m_ndt == DT_LONG)
		pr->m_val.lVal = m_val.lVal | ps->m_val.lVal;
	else if(m_ndt == DT_LLONG)
		pr->m_val.llVal = m_val.llVal | ps->m_val.llVal;
	else if(m_ndt == DT_FLOAT)
		pr->m_val.fltVal = (float)((long)m_val.fltVal | (long)ps->m_val.fltVal);
	else if(m_ndt == DT_DOUBLE)
		pr->m_val.dblVal = (double)((__int64)m_val.dblVal | (__int64)ps->m_val.dblVal);
	else{
		delete pr;
		pr = NULL;
	}
	return pr;
}

BOOL exp_is16(LPCTSTR lpszVal,CString &sz16)//�Ƿ���16������,�����Ѵ�д
{
	CString szs = lpszVal;
	int i,n=szs.GetLength();
	char c;

	sz16="";

	if(n>2)
	{
		if((szs[0] == '0')&&(szs[1] == 'X')) //������16���� 0X��﷽ʽ
		{
			CString s16="";
			for(i=2;i<n;i++)
			{
				c=szs[i];
				if( ((c >= '0') && (c <='9')) || ((c >='A')&&(c<='F')) )
					sz16+=c;
				else
				{
					return FALSE;
				}
			}
			return TRUE;
		}
	}
	if(n>=2)
	{
		if(szs[n-1] == 'H')
		{
			CString s16="";
			for(i=0;i<n-1;i++)
			{
				c=szs[i];
				if( ((c >= '0') && (c <='9')) || ((c >='A')&&(c<='F')) )
					sz16+=c;
				else
				{
					return FALSE;
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}

BOOL exp_isstr(LPCTSTR lpszVal)//�Ƿ����ַ���
{
	char c;
	CString szs = lpszVal;
	int n=szs.GetLength();
	for(int i=0;i<n;i++)
	{
		c= szs[i];
		if(((c < '0')||(c > '9')) && (c != '.'))
			return TRUE;
	}
	return FALSE;
}

DWORD exp_str2hex16(LPCTSTR lpsVal)
{
	DWORD dwr=0,dwt;
	
	CString szs=lpsVal;
	szs.MakeUpper();
	int n=szs.GetLength();
	int i,j=0;
	char c;
	
	for(i=n-1;i>=0;i--)
	{
		c=szs[i];
		if((c >='0') && ( c<='9'))
			c -= '0';
		else
			c = 10 + c - 'A';
		dwt=c;
		dwt = dwt << (4 * j);
		j++;
		dwr |= dwt;
	}
	return dwr;
}

int CExpItem::SetData(LPCTSTR lpszVal,EXP_GetExternVar pfun,void *pParam1,void *pParam2)//�ַ������ƣ�ת��Ϊ��ֵ
{
	CString szs = lpszVal;
	szs.MakeUpper();

	CString sz16="";
	if(exp_is16(szs,sz16))
	{
		m_val.lVal = exp_str2hex16(sz16);
		m_ndt = DT_LONG;
		m_szid="";
		return DT_LONG;
	}
	else if(exp_isstr(lpszVal))//ʵʱ�����
	{
		//����TV()�����Ų���
		if(szs.Left(2) == "TV")
		{
			szs = lpszVal;
			szs = szs.Right(szs.GetLength() - 3);
			szs = szs.Left(szs.GetLength() - 1);
		}
		m_szid = szs;//lpszVal;
		if(pfun == NULL)
			return -1;
		if(0 != pfun(this,pParam1,pParam2))
			return -1;
		return m_ndt;
	}
	if(-1 != szs.Find('.'))
	{
		m_val.fltVal = (float)(atof(lpszVal));
		m_ndt = DT_FLOAT;
		m_szid="";
		return DT_FLOAT;
	}
	else
	{
		m_val.lVal = atol(lpszVal);
		m_ndt = DT_LONG;
		m_szid="";
		return DT_LONG;
	}
	return -1;
}

BOOL CExpItem::GetVal(CString &szr)//ȡֵ
{
	
	if(m_ndt == DT_LONG)
		szr.Format("%d",m_val.lVal);
	else if(m_ndt == DT_LLONG)
		szr.Format("%I64d",m_val.llVal);
	else if(m_ndt == DT_FLOAT)
		szr.Format("%G",m_val.fltVal);
	else if(m_ndt == DT_DOUBLE)
		szr.Format("%G",m_val.dblVal);
	else 
		return FALSE;

	return TRUE;
}


//���ȼ��ӵ͵���
// & |
// << >>
// + -
// * / %
// - ȡ����
// ()
// �Ѹ���ת��Ϊ��^
// ���ʵʱ�������Ҫ������Щ�ַ�
// ��������szD��

//20090728����tv()����
int g_Mid2Behind(LPCTSTR lpszS,CString &szD,CString &szErr)//����->���� 
{
	CArrayStackTemp <char, 512> stk;//��ջ
	
	char b0 = TRUE;//�ϴ������

	CString szs = lpszS;
	int ns = szs.GetLength();
	int i;
	char c,cc,c2;
	CString szt="";
	szD="";
	for(i=0;i<ns;i++)
	{
		c=szs[i];
		if((c== 0x0d)||(c==0x0a)||(c==' ')||(c=='\t'))//��Ȼ�ָ���
		{
			if(szt.IsEmpty())
				continue;
		}
		if(
			(c!='+')&&
			(c!='-')&&
			(c!='*')&&
			(c!='/')&&
			(c!='%')&&
			(c!='<')&&
			(c!='>')&&
			(c!='(')&&
			(c!=')')&&
			(c!='&')&&
			(c!='|')
		 ) //�������ָ�
		{
			szt+=c;
			continue;
		}

		if((c=='&') || (c=='|'))
		{
			if(!szt.IsEmpty())
			{
				szD+=szt;
				szD+=' ';
				szt = "";
				b0=FALSE;
			}
			do
			{ 
				if(!stk.Pop(cc)) 
					cc=0;
				if((cc!=0)&&(cc!='(')) 
					szD += cc;
			}while((cc!=0)&&(cc!='('));
			if((cc=='('))// ||(c=='^'))
				stk.Push(cc); // ���

			stk.Push(c);//������ѹջ
			b0=TRUE;
		}

		else if((c=='<') || (c=='>'))
		{
			if(!szt.IsEmpty())
			{
				szD+=szt;
				szD+=' ';
				szt = "";
				b0=FALSE;
			}

			if(i<ns)
			{
				i++;
				c2=szs[i];
				if(c!=c2)
				{
					szErr.Format("������%c",c);
					return 1;
				}
			}
			do
			{ 
				if(!stk.Pop(cc)) 
					cc=0;
				if((cc!=0)&&(cc!='(')&&(cc!='&')&&(cc!='|')) 
					szD += cc;
			}while((cc!=0)&&(cc!='(')&&(cc!='&')&&(cc!='|'));
			if((cc=='(') || (cc=='&') ||(cc=='|'))
				stk.Push(cc); // ���

			stk.Push(c);//������ѹջ ��λ����һ���ַ�
			b0=TRUE;
		}
		else if((c=='+') || (c=='-'))
		{
			if(!szt.IsEmpty())//������ֱ����ӵ�����ַ���
			{
				szD+=szt;
				szD+=' ';
				szt = "";
				b0=FALSE;
			}
			if((b0 == TRUE)&&(c=='-'))//�ϴ�����������򱾴��Ǹ���
				c='^';
			else
			{
				do
				{ 
					if(!stk.Pop(cc)) 
						cc=0;
					if((cc!=0)&&(cc!='(')&&(cc!='&')&&(cc!='|')&&(cc!='<')&&(cc!='>')) 
						szD += cc;
				}while((cc!=0)&&(cc!='(')&&(cc!='&')&&(cc!='|')&&(cc!='<')&&(cc!='>'));
		
				if((cc=='(') || (cc=='&') ||(cc=='|')||(cc=='<')||(cc=='>'))
					stk.Push(cc); // ���
			}
			stk.Push(c);//������ѹջ
			if(c == '^')
				b0=FALSE;
			else
				b0=TRUE;
		}
		else if((c=='*') || (c=='/')||(c=='%'))
		{
			if(!szt.IsEmpty())//������ֱ����ӵ�����ַ���
			{
				szD+=szt;
				szD+=' ';
				szt = "";
				b0=FALSE;
			}
			do
			{ 
				if(!stk.Pop(cc)) 
					cc=0;
				if((cc!=0)&&(cc!='(')&&(cc!='&')&&(cc!='|')&&(cc!='<')&&(cc!='>')&&(cc!='+')&&(cc!='-')) 
					szD += cc;
			}while((cc!=0)&&(cc!='(')&&(cc!='&')&&(cc!='|')&&(cc!='<')&&(cc!='>')&&(cc!='+')&&(cc!='-'));
			//����ַ��Ǹ���������������pop����������
			//ֱ�����������ţ�opening parenthesis�������ȼ��ϵ͵Ĳ���������ͬһ
			//���ȼ����ҽ�Ϸ��š������������ѹ�루push����ջ��

			if((cc=='(') || (cc=='&') ||(cc=='|')||(cc=='<')||(cc=='>')||(cc=='+')||(cc=='-'))
				stk.Push(cc); // ���

			stk.Push(c);//������ѹջ
			b0=TRUE;
		}
		else if(c == '(') //����ַ��Ǹ������ţ�����ѹ���ջ��
		{
			if(_stricmp("tv",szt) == 0) //���������⴦��
			{
				szt +='(';
				i++;
				int nlk=1;//�����Ÿ���
				int nrk=0;//�����Ÿ���
				while(i<ns)
				{
					szt += szs[i];
					if(szs[i] == ')')
					{
						nrk++;
						if(nrk == nlk)//����ƥ�䣬����
							break;
					}
					i++;
				}
			}
			else
			{
				if(!szt.IsEmpty())//������ֱ����ӵ�����ַ���
				{
					szD+=szt;
					szD+=' ';
					szt = "";
					b0=FALSE;
				}
				stk.Push(c);
				b0=TRUE;
			}
		}
		else if(c == ')')//����ַ��Ǹ������ţ�closing parenthesis����������������ǰ���������в�������Ȼ���������ӵ�����ַ�����
		{
			if(!szt.IsEmpty())//������ֱ����ӵ�����ַ���
			{
				szD+=szt;
				szD+=' ';
				szt = "";
				b0=FALSE;
			}
			do
			{ 
				if(!stk.Pop(cc)) 
					cc=0;
				if((cc!=0)&&(cc!='(')) 
					szD += cc;
			}while((cc!=0)&&(cc!='('));
	
			b0=FALSE;
		}
		else 
		{
			szErr.Format("����Ĳ�����%c",c);
			return 2;//����������
		}
	}

	if(!szt.IsEmpty())//������,��ӵ�����ַ���
		szD += szt;
	
	bool bret;
	while(stk.GetSize())
	{
		bret=stk.Pop(cc);
		if((bret==true)&&(cc!='(')) 
			szD += cc;
	}
	return 0;
} 

CString exp_GetDbTagName(LPCTSTR s)
{
	CString szt = s;
	if(exp_isstr(szt))//ʵʱ�����
	{
		if(szt.Left(2) == "TV")
		{
			szt = szt.Right(szt.GetLength() - 3);
			szt = szt.Left(szt.GetLength() - 1);
			return szt;
		}
	}
	return CString("");
}

//20090728����tv()����
CExpItem* g_CalBehindParse(LPCTSTR lpszParse,EXP_GetExternVar pfun,void *pParam1,void *pParam2,long &ltags)//���ݺ��ñ��ʽ����
{
	ltags = 0;
	char stmp[10][80];
	CString szexpt;
	
	CExpStack stk;//��������ջ
	CExpItem *pi,*pi1,*pi2;

	CString szs = lpszParse;
	int ns = szs.GetLength();
	int i;
	char c;
	CString szt="";
	
	for(i=0;i<ns;i++)
	{
		c=szs[i];
		if(c==' ')//�ָ���
		{
			if(!szt.IsEmpty())
			{
				pi=new CExpItem;
				if(pi == NULL)
					return NULL;
				if(0 > pi->SetData(szt,pfun,pParam1,pParam2))
				{
					delete pi;
					return NULL;
				}
				//�����ǩ����
				szexpt = exp_GetDbTagName(szt);
				if(!szexpt.IsEmpty())
				{
					int k;
					for(k = 0;k < ltags;k++)
					{
						if(szexpt == stmp[k])
							break;
					}
					if((k == ltags) && (ltags < 10)) //û��
					{
						strncpy(stmp[ltags],szexpt.Left(79),79);
						stmp[ltags][79] = 0;
						ltags++;
					}
				}
				stk.push(pi);
				szt="";
			}
			continue;
		}

		if(
			(c!='+')&&
			(c!='-')&&
			(c!='*')&&
			(c!='/')&&
			(c!='%')&&
			(c!='<')&&
			(c!='>')&&
			(c!='(')&&
			(c!=')')&&
			(c!='&')&&
			(c!='|')&&
			(c!='^')
		 ) //�������ָ�
		{
			szt+=c;
			continue;
		}
		pi=NULL;
		if(!szt.IsEmpty())
		{
			if(c == '(')
			{
				if(_stricmp("tv",szt)==0)
				{
					szt +='(';
					i++;
					int nlk=1;//�����Ÿ���
					int nrk=0;//�����Ÿ���
					while(i<ns)
					{
						szt += szs[i];
						if(szs[i] == ')')
						{
							nrk++;
							if(nrk == nlk)//����ƥ�䣬����
								break;
						}
						i++;
					}
				}
			}

			pi=new CExpItem;
			if(pi == NULL)
				return NULL;
			if(0 > pi->SetData(szt,pfun,pParam1,pParam2))
			{
				delete pi;
				return NULL;
			}
			//�����ǩ����
			szexpt = exp_GetDbTagName(szt);
			if(!szexpt.IsEmpty())
			{
				int k;
				for(k = 0;k < ltags;k++)
				{
					if(szexpt == stmp[k])
						break;
				}
				if((k == ltags) && (ltags < 10)) //û��
				{
					strncpy(stmp[ltags],szexpt.Left(79),79);
					stmp[ltags][79] = 0;
					ltags++;
				}
			}

			stk.push(pi);
			szt="";
		}

		if(c=='&')
		{
			pi2=stk.pop();
			pi1=stk.pop();
			if((pi2 == NULL) || (pi1 == NULL))
			{
				if(pi1!=NULL)
					delete pi1;
				if(pi2!=NULL)
					delete pi2;
				return NULL;
			}
			pi= pi1->BitAnd(pi2);
			delete pi1;
			delete pi2;
			stk.push(pi); // ���
		}
		else if(c=='|')
		{
			pi2=stk.pop();
			pi1=stk.pop();
			if((pi1 == NULL) || (pi2 == NULL))
			{
				if(pi1!=NULL)
					delete pi1;
				if(pi2!=NULL)
					delete pi2;
				return NULL;
			}
			pi= pi1->BitOr(pi2);
			delete pi1;
			delete pi2;
			stk.push(pi); // ���
		}
		else if(c=='+')
		{
			pi2=stk.pop();
			pi1=stk.pop();
			if((pi1 == NULL) || (pi2 == NULL))
			{
				if(pi1!=NULL)
					delete pi1;
				if(pi2!=NULL)
					delete pi2;
				return NULL;
			}
			pi= pi1->Add(pi2);
			delete pi1;
			delete pi2;
			stk.push(pi); // ���
		}
		else if(c=='-')
		{
			pi2=stk.pop();
			pi1=stk.pop();
			if((pi1 == NULL) || (pi2 == NULL))
			{
				if(pi1!=NULL)
					delete pi1;
				if(pi2!=NULL)
					delete pi2;
				return NULL;
			}
			pi= pi1->Dec(pi2);
			delete pi1;
			delete pi2;
			stk.push(pi); // ���
		}
		else if(c=='*')
		{
			pi2=stk.pop();
			pi1=stk.pop();
			if((pi1 == NULL) || (pi2 == NULL))
			{
				if(pi1!=NULL)
					delete pi1;
				if(pi2!=NULL)
					delete pi2;
				return NULL;
			}
			pi= pi1->Mut(pi2);
			delete pi1;
			delete pi2;
			stk.push(pi); // ���
		}
		else if(c=='/')
		{
			pi2=stk.pop();
			pi1=stk.pop();
			if((pi1 == NULL) || (pi2 == NULL))
			{
				if(pi1!=NULL)
					delete pi1;
				if(pi2!=NULL)
					delete pi2;
				return NULL;
			}
			pi= pi1->Div(pi2);
			delete pi1;
			delete pi2;
			stk.push(pi); // ���
		}
		else if(c=='%')
		{
			pi2=stk.pop();
			pi1=stk.pop();
			if((pi1 == NULL) || (pi2 == NULL))
			{
				if(pi1!=NULL)
					delete pi1;
				if(pi2!=NULL)
					delete pi2;
				return NULL;
			}
			pi= pi1->Mod(pi2);
			delete pi1;
			delete pi2;
			stk.push(pi); // ���
		}
		else if(c=='<')
		{
			pi2=stk.pop();
			pi1=stk.pop();
			if((pi1 == NULL) || (pi2 == NULL))
			{
				if(pi1!=NULL)
					delete pi1;
				if(pi2!=NULL)
					delete pi2;
				return NULL;
			}
			pi= pi1->LeftMove(pi2);
			delete pi1;
			delete pi2;
			stk.push(pi); // ���
		}
		else if(c=='>')
		{
			pi2=stk.pop();
			pi1=stk.pop();
			if((pi1 == NULL) || (pi2 == NULL))
			{
				if(pi1!=NULL)
					delete pi1;
				if(pi2!=NULL)
					delete pi2;
				return NULL;
			}
			pi= pi1->RightMove(pi2);
			delete pi1;
			delete pi2;
			stk.push(pi); // ���
		}
		else if(c=='^')//ȡ����
		{
			pi2=stk.pop();
			if(pi2 == NULL)
				return NULL;
			pi1= new CExpItem;
			if(pi1 == NULL)
			{
				delete pi2;
				return NULL;
			}
			pi1->m_ndt = DT_LONG;
			pi1->m_val.lVal = 0;
			pi= pi1->Dec(pi2);
			delete pi1;
			delete pi2;
			stk.push(pi); // ���
		}
	}

	if(stk.Getsize() != 1)
		return NULL;
	
	return stk.pop();
}

//20090728����tv()����
BOOL g_CheckBehindParse(LPCTSTR lpszParse)//���ݺ��ñ��ʽ���� 
{
	
	CExpStack stk;//��������ջ
	CExpItem *pi,*pi1,*pi2;

	CString szs = lpszParse;
	int ns = szs.GetLength();
	int i;
	char c;
	CString szt="";
	
	for(i=0;i<ns;i++)
	{
		c=szs[i];
		if(c==' ')//�ָ���
		{
			if(!szt.IsEmpty())
			{
				pi=new CExpItem;
				stk.push(pi);
				szt="";
			}
			continue;
		}

		if(
			(c!='+')&&
			(c!='-')&&
			(c!='*')&&
			(c!='/')&&
			(c!='%')&&
			(c!='<')&&
			(c!='>')&&
			(c!='(')&&
			(c!=')')&&
			(c!='&')&&
			(c!='|')&&
			(c!='^')
		 ) //�������ָ�
		{
			szt+=c;
			continue;
		}

		pi=NULL;
		if(!szt.IsEmpty())
		{
			if(c == '(')
			{
				if(_stricmp("tv",szt)==0)
				{
					szt +='(';
					i++;
					int nlk=1;//�����Ÿ���
					int nrk=0;//�����Ÿ���
					while(i<ns)
					{
						szt += szs[i];
						if(szs[i] == ')')
						{
							nrk++;
							if(nrk == nlk)//����ƥ�䣬����
								break;
						}
						i++;
					}
				}
			}

			pi=new CExpItem;
			stk.push(pi);
			szt="";
		}

		if((c=='&')||(c=='|')||(c=='+')||(c=='-')||(c=='*')||(c=='/')||(c=='%')||(c=='>')||(c=='<')) //˫������
		{
			pi2=stk.pop();
			pi1=stk.pop();
			if((pi2 == NULL) || (pi1 == NULL))
			{
				if(pi1!=NULL)
					delete pi1;
				if(pi2!=NULL)
					delete pi2;
				return FALSE;
			}
		
			delete pi2;
			stk.push(pi1); // ���
		}

		else if(c=='^')//ȡ����
		{
			pi2=stk.pop();
			if(pi2 == NULL)
				return FALSE;
			stk.push(pi2); // ���
		}
	}

	if(stk.Getsize() != 1)
		return FALSE;
	return TRUE;

}

