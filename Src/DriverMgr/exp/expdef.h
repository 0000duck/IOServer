#pragma once

#define DT_LONG		1
#define DT_LLONG    2
#define DT_FLOAT	3
#define DT_DOUBLE	4

template <class T,int SIZE> class CArrayStackTemp
{
public:
	CArrayStackTemp (){top= -1;}; //ȱʡ���캯��,����һ���ն�ջ
	~CArrayStackTemp (){};//��������
	bool Push(T element); //��ջ
	bool Pop(T& element);//��ջ
	int GetSize(){return top+1;};//ȡԪ�ظ���
private:
	T Buffer[SIZE];
	int top;
};
template <class T, int SIZE> bool CArrayStackTemp<T, SIZE>:: Push (T element)
{
	top++;
	if (top>SIZE-1) {top--; return false;}//��
	Buffer[top]=element;
	return true;
}
template <class T, int SIZE> bool CArrayStackTemp<T, SIZE>:: Pop (T& element)
{
	if (top==-1) return false;//��
	element=Buffer[top];
	top--;
	return true;
}

class CExpItem;

// ��ȡ�ⲿ��������
typedef DWORD (WINAPI *EXP_GetExternVar)(
					CExpItem *pi,//in/out
					void *lpParam1,//����1
					void *lpParam2//����2
					);//����ֵ0��ʾ�ɹ�

//������
class CExpItem
{
public:
	CString m_szid;//Ϊ�ձ�ʾֵ����Ϊ�ձ�ʾ������
	
	int m_ndt;	//DT_CHAR
	union VAL
	{
		long	lVal;
		float	fltVal;
		__int64 llVal;
		double	dblVal;
	}m_val;

	CExpItem()
	{
		m_szid = "";
		m_ndt = DT_LONG;
		m_val.lVal = 0;
	};
	~CExpItem(){};

	BOOL ChangeType(int ndt);//�ı���������

	//{{�����
	CExpItem *Add(CExpItem *ps);// +
	CExpItem *Dec(CExpItem *ps);// -
	CExpItem *Mut(CExpItem *ps);// *
	CExpItem *Div(CExpItem *ps);// /
	CExpItem *Mod(CExpItem *ps);// %
	CExpItem *LeftMove(CExpItem *ps);// <<
	CExpItem *RightMove(CExpItem *ps);// >>
	CExpItem *BitAnd(CExpItem *ps);// &
	CExpItem *BitOr(CExpItem *ps);// |
	//}}

	//{{��ֵ
	void SetData(long val){m_ndt = DT_LONG;m_val.lVal = val;};
	void SetData(float val){m_ndt = DT_FLOAT;m_val.fltVal = val;};
	void SetData(__int64 val){m_ndt = DT_LLONG;m_val.llVal = val;};
	void SetData(double val){m_ndt = DT_DOUBLE;m_val.dblVal = val;};

	
	int SetData(LPCTSTR lpszVal,EXP_GetExternVar pfun,void *pParam1,void *pParam2);//�ַ������ƣ�ת��Ϊ��ֵ
	//}}

	BOOL GetVal(CString &szr);//ȡֵ
};

class CExpStack //��ջ
{
public:
	int nsize;//�ռ��С
	int ntop;//��λ�ã���ʼλ��-1;
	DWORD dwbuf[1024];

	CExpStack()
	{
		nsize = 1024;
		ntop = -1;
	};
	~CExpStack()
	{
		CExpItem *pi;
		while(ntop >=0)
		{
			pi = (CExpItem*)dwbuf[ntop];
			delete pi;
			ntop--;
		}
	};
	int Getsize()
	{
		return ntop+1;
	};
	BOOL push(CExpItem *pi)
	{
		if(ntop >= nsize - 1)
			return FALSE;
		ntop++;
		dwbuf[ntop] = DWORD(pi);
		return TRUE;
	};

	CExpItem* pop()
	{
		if(ntop == -1)
			return NULL;
		CExpItem *pi;
		pi = (CExpItem*)dwbuf[ntop];
		ntop--;
		return pi;
	};
};

int			g_Mid2Behind(LPCTSTR lpszS,CString &szD,CString &szErr);//����->���� ret = 0 ok
CExpItem*	g_CalBehindParse(LPCTSTR lpszParse,EXP_GetExternVar pfun,void *pParam1,void *pParam2,long &ltags);//���ݺ��ñ��ʽ����,���ؽ��

