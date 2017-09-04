#pragma once

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
