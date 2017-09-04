// Operator.h : main header file for the OPERATOR DLL
//

#if !defined(AFX_OPERATOR_H__41D5B80E_4E4C_446E_BE3C_4DF64A49D88B__INCLUDED_)
#define AFX_OPERATOR_H__41D5B80E_4E4C_446E_BE3C_4DF64A49D88B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include <afxtempl.h>		//ģ��֧��
#include "MD5.h"
#include "reglic.h"

//---------------------------------------------------------------------------
class COperatorItem
{
public:
	COperatorItem();
	~COperatorItem();
public:
	CString m_szName;
	CString m_szGroup;//������
	DWORD   m_dwPower;
	BOOL	m_bEnable;		//�Ƿ�����
	CString m_szMD5Pass;	//MD5���ܵ�����
	CString m_szNote;
	CString m_szRes;		//����
public:
	void	Serialize(CArchive& ar);//
};
#define MAX_POWERS		32 //���Ȩ����
class CPowerItem //Ȩ��
{
public:
	CPowerItem();
	~CPowerItem();
public:
	int		m_nID;			// 0-31
	BOOL	m_bEnable;		//�Ƿ�����
	CString m_szPower;	//Ȩ��˵��
	CString	m_szRes;
public:
	void	Serialize(CArchive& ar);//
};

class CGroupItem	//Ȩ����
{
public:
	CGroupItem();
	~CGroupItem();
public:
	DWORD   m_dwPowers;
	BOOL	m_bEnable;		//�Ƿ�����
	CString m_szName;
	CString m_szRes;//����
public:
	void	Serialize(CArchive& ar);//
};

#define OPT_OK			0
#define OPT_ERRHANDLE	1 
#define OPT_ERRPASS		2	//�������
#define OPT_ERRNAME		3	//�޴��˺�
#define OPT_CANCEL		4	//ȡ��
#define OPT_NOPOWER		5	//�޴�Ȩ��
#define OPT_UNREG       6   //δע�� //wzq 20090731 add
#define OPT_MOMERY		7	//�ڴ����

class COperator
{
public:
	COperator();
	~COperator();
public:
	void	Serialize(CArchive& ar);//
	int		AddOpt(LPCTSTR lpszName,LPCTSTR lpszPass,LPCTSTR lpszGroup,BOOL bEnable);
	int		DelOpt(LPCTSTR lpszName);
	int		ModifyOpt(LPCTSTR lpszName,LPCTSTR lpszGroup,BOOL bEnable);

	int		AddPower(CPowerItem *pi); //����0��ʾ�ɹ� ,1�Ѵ��ڣ�2�ռ䲻��
	int		AddGroup(CGroupItem *pg);	// 0:�ɹ���1���Ѵ���
	int		ModifyPass(LPCTSTR lpszOpt,LPCTSTR lpszPass,LPCTSTR lpszNewPass);// 0:ok,1:�˺Ų����ڣ�2���������
	int		GetPowers();//ȡȨ�޸���
	DWORD   GetGroupPower(LPCTSTR lpszgp); //ȡ��Ȩ��
	int     Login(LPCTSTR lpszOpt,LPCTSTR lpszPass,DWORD *dwPower,CString &szGroupName);//����0��ʾ�ɹ���1������,2�������

	void	SetOptFile(LPCTSTR lpszFile);
	void	Save();
	void	Load();
	void	Group2ComboBox(CComboBox *pcb);
	void	Opt2ListCtrl(CListCtrl *pList);
	COperatorItem*	FindOpt(LPCTSTR lpszOpt);
	
	void	MakeDefaultOpt();//����Ĭ�ϵ�admin/admin �˺�
	DWORD	CheckLicense(LPCTSTR lpszProductName, BOOL bShowBox = FALSE);
	CString GetLicInfo(LPCTSTR lpszProductName);
	DWORD	GetLicProperty(LPCTSTR lpszProductName, LPCTSTR lpszPropertyName, int nDataType, void *pData, int *pnSize);
protected:
	CPowerItem *m_pPowers[MAX_POWERS];								//32��Ȩ��
	CMap <CString,LPCTSTR, CGroupItem*, CGroupItem*> m_GpMap;		//Ȩ�����
	CMap <CString,LPCTSTR, COperatorItem*, COperatorItem*> m_OpMap; //����Ա��
	CString m_szOpFile;//�ļ�ȫ·��

	CString m_szRes;
	BOOL	m_bModify;

	CRegLic m_RegLic; //wzq 20090802 add

protected:
	void	ClearGroup();
	void	ClearOpt();
	void	DefautGroup();
	
};

//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// COperatorApp
// See Operator.cpp for the implementation of this class
//

class COperatorApp : public CWinApp
{
public:
	COperatorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COperatorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(COperatorApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPERATOR_H__41D5B80E_4E4C_446E_BE3C_4DF64A49D88B__INCLUDED_)
