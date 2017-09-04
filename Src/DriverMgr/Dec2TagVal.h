#pragma once

class CDecVal;
class CDec2TagVal
{
public:
	CDec2TagVal(LPCTSTR lpszName){
		m_szname = lpszName;
		m_tab.SetSize(0,8);
		m_pNext = NULL;
	};
	~CDec2TagVal()
	{
		m_tab.RemoveAll();
	};

	CDec2TagVal* GetNext() {
		return m_pNext;
	}

	void AddItem(CDec2TagVal* pTagVal);
	void OnDrvTagDataOk(CDecVal* pdecval, LPDRREC pdv); //��������OK
public:
	CString						m_szname; //������ǩ��
	CArray<CTagVal*,CTagVal*>	m_tab;
	CVarVal						m_val;//�����ϴε�ֵ�����ڹ���

protected:
	CDec2TagVal*				m_pNext;
	BOOL IsDataEqual(TVVAL *pval1, TVVAL *pval2);
	/*
	static		DWORD WINAPI GetExternVar( 
					CExpItem *pi,//in/out
					void *lpParam1,//���� CDecVal* ���ڲ����豸�ڱ���
					void *lpParam2//���� LPDRREC* ����ȡcx,px
					);//���㹫ʽ���ڻ�ȡ�ⲿ�����õĻص�����
	*/
	friend class			CDecVal;
};

