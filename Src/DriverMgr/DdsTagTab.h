#pragma once

//���һ��map��������ڴ洢����DDS��������,��load��saveʱ������������
class CDdsTagTab //DDS�ñ�ǩ�����ڴ���DDS��ǩ
{
protected:
	class CDDSTI
	{
	public:
		CDDSTI()
		{
			m_szname = "";
			m_szdes = "";
			m_szunit = "";
			m_wtype = DT_VOID;
			m_wcomptye = CASHE_COMP_NONE;
			m_szcompval = "";
		};
		CString m_szname;   //����
		CString m_szdes;	//����
		CString m_szunit;	//��λ
		CString m_szcompval;//ѹ������
		WORD	m_wtype;	//��������
		WORD    m_wcomptye;	 //ѹ����ʽ
	};
	class CMKDDSVTAG //��֯�ɱ䳤DDS��ǩ����
	{
	public:
		CMKDDSVTAG(){};
		~CMKDDSVTAG(){};
		char m_sbuf[512];
		int m_nsize;
		BOOL MakeCell(CDDSTI* ptag)
		{
			LPCTSTR pt;
			char *pc = (char*)m_sbuf;
			char *ph = pc;

			*((DWORD*)(pc + 2)) = ptag->m_wtype;//type
			*((WORD*)(pc + 4)) = ptag->m_wcomptye;//ѹ������

			pc += 6;
			pt = (LPCTSTR)ptag->m_szname;
			if(pt)
				while(*pt != 0)
					*pc++ = *pt++;
			*pc++ = 0x0A;

			pt = (LPCTSTR)ptag->m_szdes;
			if(pt)
				while(*pt != 0)
					*pc++ = *pt++;
			*pc++ = 0x0A;

			pt = (LPCTSTR)ptag->m_szunit;
			if(pt)
				while(*pt != 0)
					*pc++ = *pt++;
			*pc++ = 0x0A;

			pt = (LPCTSTR)ptag->m_szcompval;
			if(pt)
				while(*pt != 0)
					*pc++ = *pt++;
			*pc++ = 0x0A;

			m_nsize = (int)(pc - ph);//���峤��
			*((WORD*)ph) = ((WORD)m_nsize) | 0x7E00;//���峤��

			return TRUE;
		}
	};
public:
	CDdsTagTab();
	~CDdsTagTab();
public:
	void ReMap(LPVOID pEcdproject); //���½���map,������Ͷ��빤��ʱ���ã����ں�ECD�����еı�ǩ��ͬ��
	BOOL GetSendData(CMyByteArray &uca,int &nindex); //ȡ���ͱ�ǩ���ݶ�
protected:
	CCriticalSection m_Lock;

	//CMap<CString,LPCTSTR,CDDSTI*,CDDSTI*> m_map; //��ǩ��
	CNoCaseMap<CDDSTI*,CDDSTI*> m_map; //��ǩ��
	CArray<CDDSTI*,CDDSTI*> m_array;			 //���飬������ʱ�õģ��������ɾ��

	void Delmap();
	void Delarray();
	BOOL IsExistInMap(LPCTSTR lpszname);
	void ReMakeArray();
};
