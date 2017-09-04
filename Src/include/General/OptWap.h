// OptWap.h
#pragma once

#include "AsmThunkCode.h"

#define OPTDLL_OK			0
#define OPTDLL_ERRHANDLE	1 
#define OPTDLL_ERRPASS		2	//�������
#define OPTDLL_ERRNAME		3	//�޴��˺�
#define OPTDLL_CANCEL		4	//ȡ��
#define OPTDLL_NOPOWER		5	//�޴�Ȩ��
#define OPTDLL_UNREG		6   //δע�� //wzq 20090731 add
#define OPTDLL_MOMERY		7	//�ڴ����

#define LIC_STATUS_FACTOR	36353 // ��Ȩ״̬����

#define LIC_STATUS_FAIL		131		// δע��
#define LIC_STATUS_EXPIRED	163		// ���ð����
#define LIC_STATUS_DEMO		197		// ���ð�
#define LIC_STATUS_REG		239		// ��ʽ��

/// ��Ȩ������������
#define LIC_DT_BOOL			0		// BOOL����
#define LIC_DT_NUMBER		1		// �������ͣ���Ϊint32��int64
#define LIC_DT_FLOAT		2		// ʵ�����ͣ���Ϊfloat��double
#define LIC_DT_STRING		3		// �ַ�������

typedef DWORD	OPTHANDLE;	
class COptApi
{
protected:
	typedef OPTHANDLE (WINAPI *iOPT_Create)();
	typedef  DWORD (WINAPI *iOPT_Destroy)(OPTHANDLE dwHandle); //����
	typedef  DWORD (WINAPI *iOPT_MkDefault)(OPTHANDLE dwHandle); //����Ĭ�Ϲ���Ա
	typedef  DWORD (WINAPI *iOPT_SetOptFile)(OPTHANDLE dwHandle,LPCTSTR lpszFile); //���ò���Ա�ļ�
	typedef  DWORD (WINAPI *iOPT_LoadOptFile)(OPTHANDLE dwHandle,LPCTSTR lpszFile); //�����ļ�
	typedef  DWORD (WINAPI *iOPT_Login)(OPTHANDLE dwHandle,char sName[64],char sGroup[64],char sPass[64],DWORD *pdwPower); //��¼
	typedef  DWORD (WINAPI *iOPT_ModifyPass)(OPTHANDLE dwHandle,LPCTSTR lpszName); //�޸�����
	typedef  DWORD (WINAPI *iOPT_OptMan)(OPTHANDLE dwHandle,LPCTSTR lpszName,LPCTSTR lpszPass); //�����˺�
	typedef  DWORD (WINAPI *iOPT_LoginNoWin)(DWORD dwHandle,const char* suser,const char* spass,DWORD *pdwPower);

	typedef  DWORD (WINAPI *iOPT_CheckLicense)(DWORD dwHandle, const char *lpszProductName);
	typedef  DWORD (WINAPI *iOPT_GetLicenseInfo)(DWORD dwHandle, const char *szProductName, char *szLicInfo, int *nSize);
	typedef  DWORD (WINAPI *iOPT_GetMachineInfo)(char *szMacCode);
	typedef	 DWORD (WINAPI *iOPT_ShowLicDlg)(const char *lpszProductName);
	typedef	 DWORD (WINAPI *iOPT_GetLicProperty)(DWORD dwHandle, const char *lpszProductName, const char *lpszPropertyName, int nDataType, void *pData, int *pnSize);
protected:
	HINSTANCE	m_hinstLib;	//DLL
	DWORD		m_handle;

	iOPT_Create			OPT_Create;
	iOPT_Destroy		OPT_Destroy;
	iOPT_MkDefault		OPT_MkDefault;
	iOPT_SetOptFile		OPT_SetOptFile;
	iOPT_LoadOptFile	OPT_LoadOptFile;
	iOPT_Login			OPT_Login;
	iOPT_ModifyPass		OPT_ModifyPass;
	iOPT_OptMan			OPT_OptMan;
	iOPT_LoginNoWin		OPT_LoginNoWin;

	iOPT_CheckLicense	OPT_CheckLicense;
	iOPT_GetLicenseInfo	OPT_GetLicenseInfo;
	iOPT_GetMachineInfo	OPT_GetMachineInfo;
	iOPT_ShowLicDlg		OPT_ShowLicDlg;
	iOPT_GetLicProperty	OPT_GetLicProperty;
public:
	COptApi()
	{
		m_szOpt = "";
		m_szGroup = "";
		m_szPass = "";
		m_dwPower = 0;

		m_hinstLib = NULL;
		m_handle = NULL;

		OPT_Create = NULL;
		OPT_Destroy = NULL;
		OPT_MkDefault = NULL;
		OPT_SetOptFile = NULL;
		OPT_LoadOptFile = NULL;
		OPT_Login = NULL;
		OPT_ModifyPass = NULL;
		OPT_OptMan = NULL;
		OPT_LoginNoWin = NULL;

		OPT_CheckLicense = NULL;
		OPT_GetLicenseInfo = NULL;
		OPT_GetMachineInfo = NULL;
		OPT_ShowLicDlg = NULL;
		OPT_GetLicProperty = NULL;
	};
	~COptApi()
	{
		if(	m_handle != 0)
			OPT_Destroy(m_handle);
		if(m_hinstLib)
			::FreeLibrary(m_hinstLib);
	};
	BOOL	LoadDrv(LPCTSTR lpszDllFile,CString &szErr)//��������
	{
		if(m_hinstLib != NULL)
			return TRUE;
		m_hinstLib = LoadLibrary(lpszDllFile); 
		if(m_hinstLib == NULL)
		{
			szErr.Format("Err:can't find driver<%s>",lpszDllFile);
			TRACE("%s\n",szErr);
			return FALSE;//û�ҵ���������
		}

		OPT_Create			= (iOPT_Create)::GetProcAddress(m_hinstLib,"OPT_Create");
		OPT_Destroy			= (iOPT_Destroy)::GetProcAddress(m_hinstLib,"OPT_Destroy");
		OPT_MkDefault		= (iOPT_MkDefault)::GetProcAddress(m_hinstLib,"OPT_MkDefault");
		OPT_SetOptFile		= (iOPT_SetOptFile)::GetProcAddress(m_hinstLib,"OPT_SetOptFile");
		OPT_LoadOptFile		= (iOPT_LoadOptFile)::GetProcAddress(m_hinstLib,"OPT_LoadOptFile");
		OPT_Login			= (iOPT_Login)::GetProcAddress(m_hinstLib,"OPT_Login");
		OPT_ModifyPass		= (iOPT_ModifyPass)::GetProcAddress(m_hinstLib,"OPT_ModifyPass");
		OPT_OptMan			= (iOPT_OptMan)::GetProcAddress(m_hinstLib,"OPT_OptMan");
		OPT_LoginNoWin		= (iOPT_LoginNoWin)::GetProcAddress(m_hinstLib,"OPT_LoginNoWin");

		OPT_CheckLicense	= (iOPT_CheckLicense)::GetProcAddress(m_hinstLib,"OPT_CheckLicense");
		OPT_GetLicenseInfo	= (iOPT_GetLicenseInfo)::GetProcAddress(m_hinstLib,"OPT_GetLicenseInfo");
		OPT_GetMachineInfo	= (iOPT_GetMachineInfo)::GetProcAddress(m_hinstLib,"OPT_GetMachineInfo");
		OPT_ShowLicDlg		= (iOPT_ShowLicDlg)::GetProcAddress(m_hinstLib,"OPT_ShowLicDlg");
		OPT_GetLicProperty	= (iOPT_GetLicProperty)::GetProcAddress(m_hinstLib,"OPT_GetLicProperty");

		//Ӧ����ص�
		if(OPT_Create)
			m_handle = OPT_Create();
		return TRUE;
	};

	DWORD MakeDefault()
	{
		if(!m_hinstLib)
			return OPTDLL_ERRHANDLE;

		return OPT_MkDefault(m_handle);
	};
	DWORD SetOptFile(LPCTSTR lpszFile)
	{
		if(!m_hinstLib)
			return OPTDLL_ERRHANDLE;
		return OPT_SetOptFile(m_handle,lpszFile);
	};
	DWORD LoadOptFile(LPCTSTR lpszFile)
	{
		if(!m_hinstLib)
			return OPTDLL_ERRHANDLE;
		return OPT_LoadOptFile(m_handle,lpszFile);
	};
	
	DWORD Login() //��¼
	{
		if(!m_hinstLib)
			return OPTDLL_ERRHANDLE;
		DWORD dwPower = 0;
		char sGroup[64];
		char sName[64];
		char sPass[64];
		sGroup[0] = 0;
		DWORD dwret = OPT_Login(m_handle,sName,sGroup,sPass,&dwPower);
		
		if(dwret == 0)
		{
			m_szGroup = sGroup;
			m_szOpt = sName;
			m_szPass = sPass;
			m_dwPower = dwPower;
		}
		return dwret;
	};

	DWORD LoginNoWin(const char* suser,const char* spass,DWORD *pdwPower ) //��¼
	{
		if(!m_hinstLib)
			return OPTDLL_ERRHANDLE;
		return OPT_LoginNoWin(m_handle,suser,spass,pdwPower);
	};

	DWORD ModifyPass(LPCTSTR lpszName) //�޸�����
	{
		if(!m_hinstLib)
			return OPTDLL_ERRHANDLE;
		return OPT_ModifyPass(m_handle,lpszName);
	};

	DWORD OptMan(LPCTSTR lpszName,LPCTSTR lpszPass) //�����˺�
	{
		if(!m_hinstLib)
			return OPTDLL_ERRHANDLE;
		if(m_szOpt.IsEmpty())
		{
			AfxMessageBox("���¼!");
			return 0;
		}
		return OPT_OptMan(m_handle,lpszName,lpszPass);
	};
	void	Logout()
	{
		m_szOpt = "";
		m_szGroup = "";
		m_szPass = "";
		m_dwPower = 0;
	};
	CString GetLoginMsg()
	{
		CString szt = "";
		if(m_szOpt.IsEmpty())
			szt = "��¼:��";
		else
		{
			szt = "��¼:";
			szt += m_szOpt;
			szt += "(";
			szt += m_szGroup;
			szt += ")";
		}
		return szt;
	};
	DWORD CheckLicense() {
		__THUNK_CODE_0

		if(!m_hinstLib)
			return OPTDLL_ERRHANDLE;

		__THUNK_CODE_1

		DWORD dwret = OPT_CheckLicense(m_handle, m_szProductName);
		__THUNK_CODE_2

		DWORD dwret2 = Random2LicStatus(dwret);

		__THUNK_CODE_3

		return dwret2;
	}
	DWORD GetLicenseInfo(CString &szLicInfo) {	
		int nlen = 1024 * 4;
		char *pbuf = new char[nlen];

		szLicInfo.Empty();

		DWORD dwret = OPT_GetLicenseInfo(m_handle, m_szProductName, pbuf, &nlen);
		if (dwret == OPTDLL_OK)
			szLicInfo = pbuf;

		delete[] pbuf;
		return dwret;
	}
	CString GetMachineInfo() {		
		if(!m_hinstLib)
			return _T("");

		char szMacCode[80] = { 0 };
		if (OPT_GetMachineInfo(szMacCode) == OPTDLL_OK)
			return CString(szMacCode);
		else
			return _T("");
	}
	DWORD ShowLicDlg() {		
		if(!m_hinstLib)
			return OPTDLL_ERRHANDLE;

		return OPT_ShowLicDlg(m_szProductName);
	}
	BOOL PropertyAsBool(LPCTSTR lpszPropertyName, BOOL bDefault = FALSE)
	{
		__THUNK_CODE_0
		BOOL bval = bDefault;
		__THUNK_CODE_1
		int nsize = sizeof(BOOL);
		__THUNK_CODE_2

		if (GetLicProperty(lpszPropertyName, LIC_DT_BOOL, &bval, &nsize) == OPTDLL_OK)
		{
			__THUNK_CODE_3
			return bval;
		}
		else {
			__THUNK_CODE_4
			return bDefault;
		}
	}
	long PropertyAsLong(LPCTSTR lpszPropertyName, long lDefault = 0)
	{
		__THUNK_CODE_5
		long lval = lDefault;
		__THUNK_CODE_6
		int nsize = sizeof(long);
		__THUNK_CODE_7

		if (GetLicProperty(lpszPropertyName, LIC_DT_NUMBER, &lval, &nsize) == OPTDLL_OK)
		{
			__THUNK_CODE_8
			return lval;
		}
		else {
			__THUNK_CODE_9
			return lDefault;
		}
	}
	__int64 PropertyAsInt64(LPCTSTR lpszPropertyName, __int64 nDefault = FALSE)
	{
		__THUNK_CODE_0
		__int64 nval = nDefault;
		__THUNK_CODE_1
		int nsize = sizeof(__int64);
		__THUNK_CODE_2

		if (GetLicProperty(lpszPropertyName, LIC_DT_NUMBER, &nval, &nsize) == OPTDLL_OK)
		{
			__THUNK_CODE_3
			return nval;
		}
		else {
			__THUNK_CODE_4
			return nDefault;
		}
	}
	float PropertyAsFloat(LPCTSTR lpszPropertyName, float fDefault = 0.0)
	{
		__THUNK_CODE_5
		float fval = fDefault;
		__THUNK_CODE_6
		int nsize = sizeof(float);
		__THUNK_CODE_7

		if (GetLicProperty(lpszPropertyName, LIC_DT_FLOAT, &fval, &nsize) == OPTDLL_OK)
		{
			__THUNK_CODE_8
			return fval;
		}
		else {
			__THUNK_CODE_9
			return fDefault;
		}
	}
	double PropertyAsDouble(LPCTSTR lpszPropertyName, double dblDefault = 0.0)
	{
		__THUNK_CODE_0
		double dblval = dblDefault;
		__THUNK_CODE_1

		int nsize = sizeof(float);

		__THUNK_CODE_2
		if (GetLicProperty(lpszPropertyName, LIC_DT_FLOAT, &dblval, &nsize) == OPTDLL_OK)
		{
			__THUNK_CODE_3
			return dblval;
		}
		else {
			__THUNK_CODE_4
			return dblDefault;
		}
	}
	CString PropertyAsString(LPCTSTR lpszPropertyName, LPCTSTR lpszDefault = NULL)
	{
		__THUNK_CODE_0
		CString szval = lpszDefault ? lpszDefault : _T("");
		__THUNK_CODE_1
		int nsize = 0;
		__THUNK_CODE_2
		// �Ȳ�ѯ��Ҫ���ڴ泤��
		if (GetLicProperty(lpszPropertyName, LIC_DT_STRING, NULL, &nsize) == OPTDLL_MOMERY)
		{
			__THUNK_CODE_3
			// �ַ���̫��
			if (nsize > 1024)
				return szval;

			__THUNK_CODE_4

			LPCTSTR lpszStr = szval.GetBuffer(nsize);

			__THUNK_CODE_5

			if (GetLicProperty(lpszPropertyName, LIC_DT_STRING, (void*)lpszStr, &nsize) != OPTDLL_OK)
			{
				__THUNK_CODE_6
				szval = lpszDefault ? lpszDefault : _T("");
			}

			__THUNK_CODE_7

			szval.ReleaseBuffer();

			__THUNK_CODE_8
		}

		__THUNK_CODE_9

		return szval;
	}
public:
	CString m_szOpt;
	CString m_szGroup;
	CString m_szPass;
	DWORD	m_dwPower;
	CString m_szProductName;

protected:

	DWORD Random2LicStatus(DWORD dwRandom)
	{
		__THUNK_CODE_9

		DWORD dwret = dwRandom % LIC_STATUS_FACTOR;

		__THUNK_CODE_8

		return dwret;
	}

	DWORD GetLicProperty(const char *lpszPropertyName, int nDataType, void *pData, int *pnSize)
	{
		__THUNK_CODE_5
		if(!m_hinstLib)
		{
			__THUNK_CODE_6
			return OPTDLL_ERRHANDLE;
		}

		__THUNK_CODE_7
		DWORD dwret = OPT_GetLicProperty(m_handle, m_szProductName, lpszPropertyName, nDataType, pData, pnSize);
		__THUNK_CODE_8
			
		return dwret;
	}
};

