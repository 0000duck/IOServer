//SortObArray.cpp
//
#include "stdafx.h"
#include "SortObArray.h"

/////////////////////////////////////////////////////////////////////////////
CSortItem::CSortItem()
{
	m_nSortWho = 0;	
	m_bAsc = TRUE;		
}
CSortItem::~CSortItem()
{
}
void CSortItem::SetSortType(int	nSortWho,BOOL	bAsc)//��������ʽ
{
	m_nSortWho = nSortWho;
	m_bAsc = bAsc;
}
int CSortItem::Compare(CSortItem *pOb)//����Ƚ�,bASC������
{
	return 0;//���಻���κ��ж� 
}

/////////////////////////////////////////////////////////////////////////////
CSortObArray::CSortObArray()
{
	m_ob = NULL;
	m_nMaxSize = 0;
	m_nSize = 0;
	m_nGrown = 256;

	m_nSortWho = 0;	
	m_bAsc = TRUE;		
}

CSortObArray::~CSortObArray()
{
	DeleteObjects();//ɾ�����ж���

	if(m_ob)
		free(m_ob); //�ͷſռ�
}

void CSortObArray::SetSortType(int	nSortWho,BOOL	bAsc)//��������ʽ
{
	m_nSortWho = nSortWho;
	m_bAsc = bAsc;
}

int CSortObArray::Comp(const void *p1,const void *p2 )
{
	LPSORTOB pi1 ,pi2;
	pi1 = *((LPSORTOB *)p1);
	pi2 = *((LPSORTOB *)p2);

	return pi1->Compare(pi2);
};


void  CSortObArray::Qsort()//����
{
	if((m_ob == NULL)||(m_nSize == 0))
		return ;
	for(int i=0;i<m_nSize;i++)
		m_ob[i]->SetSortType(m_nSortWho,m_bAsc);
	qsort(m_ob, m_nSize,sizeof(LPSORTOB),Comp);
}

void CSortObArray::SetGrown(int ngrown)	//���������ռ䣬Ĭ��256
{
	m_nGrown = ngrown;
	if(m_nGrown < 16)
		m_nGrown = 16;
	if(m_nGrown > 32 * 1024)
		m_nGrown = 32 * 1024;
}

BOOL	CSortObArray::Grown()//����
{
	LPSORTOB *ob = NULL;
	ob = (LPSORTOB *)malloc((m_nMaxSize + m_nGrown) * sizeof(LPSORTOB));//�����ʼ�ռ�
	
	if(ob == NULL)
		return FALSE;

	m_nMaxSize += m_nGrown;

	if(m_ob == NULL)
	{
		m_ob = ob;
		return TRUE;
	}
	
	int i;
	for(i=0;i<m_nSize;i++)
		ob[i] =  m_ob[i];//����
	
	while(i < m_nMaxSize)//���������
	{
		ob[i] = NULL;
		i++;
	}
	free(m_ob);//����ɵ�
	m_ob = ob;//�����µ�
	return TRUE;
}

void CSortObArray::RemoveAll()//�Ƴ����ж��󣬲�ɾ������
{
	if((m_ob == NULL)||(m_nSize == 0))
		return;
	
	for(int i=0;i< m_nMaxSize;i++)
		m_ob[i] = NULL;

	m_nSize = 0;
}

LPSORTOB	CSortObArray::RemoveAt(int nIndex)//�Ƴ����󣬲�ɾ�������󣬷��ر��Ƴ��Ķ���
{
	if((m_ob == NULL)||(m_nSize == 0))
		return NULL;

	if((nIndex < 0)||(nIndex >= m_nSize))
		return NULL;

	LPSORTOB pob = m_ob[nIndex];
	
	int i;
	for(i=nIndex;i<m_nSize-1;i++)
		m_ob[i] = m_ob[i+1];

	m_ob[i] = NULL;
	m_nSize--;

	return pob;
}

void	CSortObArray::DeleteObjects()//ɾ�����ж���
{
	if((m_ob == NULL)||(m_nSize == 0))
		return;
	for(int i=0;i< m_nSize;i++)
	{
		delete m_ob[i];
		m_ob[i] = 0;
	}
	m_nSize = 0;
}

int		CSortObArray::GetSize()//ȡ��ǰ�������
{
	return m_nSize;
}

LPSORTOB CSortObArray::GetAt(int nIndex)//ȡһ������
{
	if((m_ob == NULL)||(m_nSize == 0))
		return NULL;
	if((nIndex >=0) &&(nIndex <m_nSize))
		return m_ob[nIndex];
	return NULL;
};

BOOL CSortObArray::Add(LPSORTOB pOb)//���
{
	if(m_nSize < m_nMaxSize)
	{
		m_ob[m_nSize] = pOb;
		m_nSize++;
		return TRUE;
	}
	if(!Grown())
		return FALSE;
	m_ob[m_nSize] = pOb;
	m_nSize++;
	return TRUE;
}

/*
���ֲ���,���ض���ָ��,����ǰ�����Ѱ�����,����λ��index
����ֵ��
	>=0:index
	-1:û�ҵ�

  */
int CSortObArray::BinarySearch(CSortItem *ps)
{
	if(m_nSize == 0)
		return -1;

	int nh=0, nt=m_nSize-1;
	int j=-1,ncmp=0;
	while(nh <= nt)
	{
		j = (nh + nt)/2;
		
		ncmp = ps->Compare(m_ob[j]);//->Compare(ps);
		if(ncmp == 0)
			return j;
		if(ncmp < 0)
			nt = j-1;
		else
			nh = j+1;
	}
	if(nh > nt)
		return -1;
	return j;
}