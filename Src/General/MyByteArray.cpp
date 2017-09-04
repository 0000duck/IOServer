#include "StdAfx.h"
#include "GeneralAPI.h"
#include "General\MyByteArray.h"

TRUNK_NAMESPACE_BEGIN

CMyByteArray::CMyByteArray()
:	m_pData(NULL),
	m_nSize(0),
	m_nBufSize(0),
	m_nGrown(1024 * 16)
{
}

CMyByteArray::~CMyByteArray()
{
	if(m_pData != NULL)
		free(m_pData);
	m_pData = NULL;
}

BOOL CMyByteArray::PutData(BYTE *pBuf,int nSize)//��������
{
	if((nSize < 1) ||(pBuf == NULL))
		return TRUE;
	if((m_nSize + nSize) > MAX_BYTEARRAYSIZE)
		return FALSE;
	if((m_nBufSize - m_nSize) < nSize){ //�ռ䲻��
		int nsizet = m_nGrown;//��������
		if(m_nGrown < nSize){
			nsizet = (nSize/m_nGrown) * m_nGrown + m_nGrown;
			if(( m_nBufSize + nsizet) > MAX_BYTEARRAYSIZE)
				nsizet = MAX_BYTEARRAYSIZE - m_nBufSize;
		}
		BYTE *p = (BYTE *)malloc(m_nBufSize + nsizet);//�����¿ռ�
		if(p==NULL)
			return FALSE;
		if(m_pData){
			memcpy(p,m_pData,m_nSize);//����
			free(m_pData);	//�ͷžɿռ�
		}
		m_pData = p;
		m_nBufSize += nsizet;
	}
	memcpy(m_pData + m_nSize,pBuf,nSize);//��������
	m_nSize += nSize;
	return TRUE;
}

void CMyByteArray::ClearData()
{
	m_nSize = 0;
	if(m_nBufSize > 64 * 1024) //����64K���ͷ��ڴ�,�Ż��ڴ�ķ�ʽ
	{
		if(m_pData != NULL)
			free(m_pData);
		m_pData = NULL;
		m_nBufSize = 0;	
	}
}

void CMyByteArray::SetGrown(int ngrown)
{
	m_nGrown = ngrown;
	if(m_nGrown < 1024)
		m_nGrown = 1024;
	if(m_nGrown > 1024 * 1024)
		m_nGrown = 1024 * 1024;
}

void CMyByteArray::LeftMove(int n)//����n�ֽ�
{
	if(m_pData == NULL)
		return;
	if(n <= 0)
		return;
	if(n >= m_nSize){
		m_nSize = 0;
		return;
	}
	memmove(m_pData,m_pData + n,m_nSize - n);
	m_nSize -= n; 
}

//���Ƶ���һ���ֽ�����β��.
void CMyByteArray::CopyTo(CMyByteArray *pba)
{
	pba->PutData(GetBuf(),GetDataSize());	
}

CMyByteArray& CMyByteArray::operator = (CMyByteArray& ba) //����=����
{
	ClearData();
	PutData(ba.GetBuf(),ba.GetDataSize());
	return *this;
}

CMyByteArray& CMyByteArray::operator += (CMyByteArray& ba) //����+=����
{
	PutData(ba.GetBuf(),ba.GetDataSize());
	return *this;
}

TRUNK_NAMESPACE_END