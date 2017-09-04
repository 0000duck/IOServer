#if !defined _OPTAPI_H_I
#define _OPTAPI_H_I

#define OPT_OK			0
#define OPT_ERRHANDLE	1 
#define OPT_ERRPASS		2	//�������
#define OPT_ERRNAME		3	//�޴��˺�
#define OPT_CANCEL		4	//ȡ��
#define OPT_NOPOWER		5	//�޴�Ȩ��

typedef DWORD	OPTHANDLE;	
extern "C"{

OPTHANDLE __stdcall OPT_Create(); //����
DWORD __stdcall OPT_Destroy(OPTHANDLE dwHandle); //����
DWORD __stdcall OPT_MkDefault(OPTHANDLE dwHandle); //����Ĭ�Ϲ���Ա
DWORD __stdcall OPT_SetOptFile(OPTHANDLE dwHandle,LPCTSTR lpszFile); //���ò���Ա�ļ�
DWORD __stdcall OPT_LoadOptFile(OPTHANDLE dwHandle,LPCTSTR lpszFile); //�����ļ�
DWORD __stdcall OPT_Login(OPTHANDLE dwHandle,char sName[64],char sGroup[64],char sPass[64],DWORD *pdwPower,BOOL bLic = FALSE); //��¼
DWORD __stdcall OPT_ModifyPass(OPTHANDLE dwHandle,LPCTSTR lpszName); //�޸�����
DWORD __stdcall OPT_OptMan(OPTHANDLE dwHandle,LPCTSTR lpszName,LPCTSTR lpszPass); //�����˺�
DWORD __stdcall OPT_LoginNoWin(DWORD dwHandle,const char* suser,const char* spass,DWORD *pdwPower);
}
#endif //_OPTAPI_H_I