#pragma once

//////////////////////////////////////////////////////////////////////////
/// CTPRIPropertyGridProperty - TPRI���Ի����࣬��Ҫ����m_szPropertyName�������ڴ����������ԭʼ��
//////////////////////////////////////////////////////////////////////////
class CTPRIPropertyGridProperty : public CMFCPropertyGridProperty
{
public:
	CTPRIPropertyGridProperty(const CString& strGroupName, DWORD_PTR dwData = 0, BOOL bIsValueList = FALSE) 
		: CMFCPropertyGridProperty(strGroupName, dwData, bIsValueList) {}

	// Simple property
	CTPRIPropertyGridProperty(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL, LPCTSTR lpszValidChars = NULL)
		: CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars) {}

public:
	LPCTSTR GetPropertyName() const {
		return m_szPropertyName;
	}
	void setPropertyName(LPCTSTR lpszPropertyName) {
		m_szPropertyName = lpszPropertyName;
	}

private:
	CString m_szPropertyName;
};

//////////////////////////////////////////////////////////////////////////
/// CTPRIPropertyGridDateProperty - �������Ա༭��
//////////////////////////////////////////////////////////////////////////
class CTPRIPropertyGridDateProperty : public CTPRIPropertyGridProperty
{
public:
	CTPRIPropertyGridDateProperty(const CString& strName, long lTime, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);

	virtual BOOL OnUpdateValue();
	virtual CString FormatProperty();

protected:
	virtual CWnd* CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat);
	virtual BOOL OnEdit(LPPOINT /*lptClick*/);
};

//////////////////////////////////////////////////////////////////////////
/// CTPRIPropertyGridMultipleLineTextProperty - �����ı����Ա༭��
//////////////////////////////////////////////////////////////////////////
class CTPRIPropertyGridMultipleLineTextProperty : public CTPRIPropertyGridProperty
{
public:
	CTPRIPropertyGridMultipleLineTextProperty(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);

	int GetEditHeight() const {
		return m_nEditHeight;
	}
	void SetEditHeight(int nEditHeight) {
		m_nEditHeight = nEditHeight;
	}
protected:
	virtual CWnd* CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat);

private:
	int m_nEditHeight;
};

//////////////////////////////////////////////////////////////////////////
/// CTPRIPropertyGridMachineCodeProperty - ������༭��
//////////////////////////////////////////////////////////////////////////
class CTPRIPropertyGridMachineCodeProperty : public CTPRIPropertyGridMultipleLineTextProperty
{
public:
	CTPRIPropertyGridMachineCodeProperty(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);

	virtual BOOL OnUpdateValue();
};