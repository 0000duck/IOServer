//SortObArray.h
#ifndef __SORTOBARRAY_H__
#define __SORTOBARRAY_H__

TRUNK_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
class _GENERAL_CLASS CSortItem	//����������
{
public:
	CSortItem();
	virtual		~CSortItem();
	void		SetSortType(int	nSortWho,BOOL	bAsc);//��������ʽ
	virtual		int Compare(CSortItem *pOb);//����Ƚ�
protected:
	int			m_nSortWho;	//����һ������
	BOOL		m_bAsc;		//�Ƿ�����
};

///////////////////////////////////////////////////////////////////////////////
typedef CSortItem *LPSORTOB;//���Ͷ���
class _GENERAL_CLASS CSortObArray //�������̬����
{
public:
	CSortObArray();
	~CSortObArray();//ɾ������,ɾ���ռ�

public:
	BOOL		Add(LPSORTOB pOb);		//���
	int			GetSize();				//ȡ��ǰ�������
	LPSORTOB	GetAt(int nIndex);		//ȡһ������

	void		RemoveAll();			//����ռ䲢���³�ʼ�� ����ɾ������
	LPSORTOB	RemoveAt(int nIndex);	//�Ƴ����󣬲�ɾ�������󣬷��ر��Ƴ��Ķ���
	void		DeleteObjects();		//ɾ�����ж���
	void		SetGrown(int ngrown);	//���������ռ䣬Ĭ��256
	void		SetSortType(int	nSortWho,BOOL	bAsc);//��������ʽ
	BOOL		IsSortAsc(){return m_bAsc;};

	void		Qsort();		//��������
	int			BinarySearch(CSortItem *ps);//2�ֲ��ң������Ѱ���������,����λ��index, -1��ʾû��
protected:
	LPSORTOB	*m_ob;		//��������
	int			m_nMaxSize;	//�ռ��С
	int			m_nSize;	//��ǰ�������
	int			m_nGrown;	//ÿ�οռ�����

	//������
	int			m_nSortWho;	//����һ������
	BOOL		m_bAsc;		//�Ƿ�����
private:
	static int  Comp(const void *p1,const void *p2);
	BOOL		Grown();//����
};

TRUNK_NAMESPACE_END

#endif // end of __SORTOBARRAY_H__