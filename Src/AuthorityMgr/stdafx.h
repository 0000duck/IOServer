// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
#endif

#include <afx.h>
#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <iostream>
// Windows ͷ�ļ�:
#include <windows.h>
//
//typedef OPTHANDLE (WINAPI *iOPT_Create)();
//typedef  DWORD (WINAPI *iOPT_Destroy)(OPTHANDLE dwHandle); //����
//typedef  DWORD (WINAPI *iOPT_MkDefault)(OPTHANDLE dwHandle); //����Ĭ�Ϲ���Ա
//typedef  DWORD (WINAPI *iOPT_SetOptFile)(OPTHANDLE dwHandle,LPCTSTR lpszFile); //���ò���Ա�ļ�
//typedef  DWORD (WINAPI *iOPT_LoadOptFile)(OPTHANDLE dwHandle,LPCTSTR lpszFile); //�����ļ�
//typedef  DWORD (WINAPI *iOPT_Login)(OPTHANDLE dwHandle,char sName[64],char sGroup[64],char sPass[64],DWORD *pdwPower); //��¼
//typedef  DWORD (WINAPI *iOPT_ModifyPass)(OPTHANDLE dwHandle,LPCTSTR lpszName); //�޸�����
//typedef  DWORD (WINAPI *iOPT_OptMan)(OPTHANDLE dwHandle,LPCTSTR lpszName,LPCTSTR lpszPass); //�����˺�
//typedef  DWORD (WINAPI *iOPT_LoginNoWin)(DWORD dwHandle,const char* suser,const char* spass,DWORD *pdwPower);
//
//typedef  DWORD (WINAPI *iOPT_CheckLicense)(DWORD dwHandle, const char *lpszProductName);
//typedef  DWORD (WINAPI *iOPT_GetLicenseInfo)(DWORD dwHandle, const char *szProductName, char *szLicInfo, int *nSize);
//typedef  DWORD (WINAPI *iOPT_GetMachineInfo)(char *szMacCode);
//typedef	 DWORD (WINAPI *iOPT_ShowLicDlg)(const char *lpszProductName);
//typedef	 DWORD (WINAPI *iOPT_GetLicProperty)(DWORD dwHandle, const char *lpszProductName, const char *lpszPropertyName, int nDataType, void *pData, int *pnSize);


// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
