// readini.cpp
#include "stdafx.h"
#include "GeneralAPI.h"
#include "General\readini.h"

TRUNK_NAMESPACE_BEGIN

CReadIni::CReadIni()
{
	m_szLine = "";	//��
	m_szBlkName = ""; //����
}
CReadIni::~CReadIni()
{

}

void CReadIni::DoLine()
{
	m_szLine.TrimLeft();
	if(m_szLine.IsEmpty())
		return;
	char c = m_szLine[0];
	if(c == ';')
		return; //ע����
	int i,n=m_szLine.GetLength();
	if(c == '[') //��
	{
		m_szBlkName = "";
		for(i=1;i<n;i++)
		{
			c = m_szLine[i];
			if(c == ']')
				break;
			else 
				m_szBlkName += c;
		}
		if(c != ']')
			m_szBlkName = "";
		return;
	}
	
	for(i=0;i<n;i++) //ȥ������ע��
	{
		if(m_szLine[i] == ';')
		{
			m_szLine = m_szLine.Left(i);
			break;
		}
	}
	//�����ͼ�ֵ
	int npos = 0;
	CString szKeyName = "",szKeyVal = "";
	n=m_szLine.GetLength();
	for(i=0;i<n;i++)
	{
		c = m_szLine[i];
		if(c == '=')
			npos++;
		else
		{
			if(npos == 0)
				szKeyName += c;
			else
				szKeyVal += c;
		}
	}
	szKeyName.TrimLeft();
	szKeyName.TrimRight();
	szKeyVal.TrimLeft();
	szKeyVal.TrimRight();
	if((npos > 0)&&(!m_szBlkName.IsEmpty()))
		OnDoKeyVal(m_szBlkName,szKeyName,szKeyVal); //�����ֵ
}

BOOL CReadIni::ReadIniFile(LPCTSTR lpszFile)
{
	FILE* pf=NULL;
	pf = fopen(lpszFile,"rt");//�ı�ģʽ��
	if(pf == NULL)//���ܴ��ļ�!
		return FALSE;
	char c;
	m_szLine = "";
	while((c = fgetc( pf)) != EOF)
	{
		if(c != '\n')
			m_szLine += c;
		else
		{
			DoLine();
			m_szLine = "";
		}
	}
	if(!m_szLine.IsEmpty())
		DoLine();

	fclose(pf);

	return TRUE;
}


TRUNK_NAMESPACE_END