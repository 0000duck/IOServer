#ifndef __FILEEXPLORER_H__
#define __FILEEXPLORER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileExplorer.h : header file
//

/*
 * ����MS��CoInitializeEx (NULL, COINIT_MULTITHREADED)���ú�CFileDialog��OFN_EXPLORER����г�ͻ��
 * �����OPC��������CoUninitialize()ʱ���������޷���ֹ�����򡣲����ļ��Ի����ڻ�ȡShell�ӿ�ʱ��
 * ���ش���״̬�룬������ʹ���ļ��Ի���������ҵĵ���ʱ������ʾ��ǰ���Ե�ʹ���̷���
 *
 * CFileExplorer�಻�������µĶԻ���������˷�װ������ͨ������һ���߳��д�ģ̬�Ի���ķ���
 * ������������ϵ����⡣
 *
 * CFileExplorer���ʹ�÷�����
 *
 *		CString szPathName;
 *		CString szFileName;
 *		if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, TRUE, NULL,NULL, 
 *			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
 *			_T("�������ļ�(*.csv)|*.csv||"), 
 *			_T("�����ǩ�����ļ�"),
 *			this)	// ���봫��pParentWnd�����������޷�������ģ̬����
 *			)
 *			return; // û��ʹ���κ��ļ�
 *
 *		����szFileName����ѡ����ļ�������·������szPathName���ص���ȫ�ļ���������ȫ·������
 *
 * CFileExplorer��Ŀ¼����֧�ֶ�ѡ�ļ�������ͨ���򵥵���չ�Ϳ���֧�֡�
 */

TRUNK_NAMESPACE_BEGIN

/////////////////////////////////////////////////////////////////////////////
// CFileExplorer dialog

class _GENERAL_CLASS CFileExplorer : public CFileDialog
{
	DECLARE_DYNAMIC(CFileExplorer)

public:
	CFileExplorer(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);
	virtual ~CFileExplorer();

	static BOOL ShowFileDialog(
		CString &szFileName, CString &szPathName,
		BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		LPCTSTR lpszTitle = NULL,
		CWnd* pParentWnd = NULL);
protected:
	typedef struct tagFILEDIALOGPARAMETER
	{
		BOOL	bOpenFileDialog;
		DWORD	dwFlags;
		CWnd	*pParentWnd;
		TCHAR	szDefExt[16];
		TCHAR	szFilter[512];
		TCHAR	szFileName[512];
		TCHAR	szTitle[128];
		UINT	nReturnFlag;
		TCHAR	szReturnPathName[512];
		TCHAR	szReturnFileName[512];
	}FILEDIALOGPARAMETER, *LPFILEDIALOGPARAMETER;
	static UINT OnWorkThread(LPVOID lpParameter);

	//{{AFX_MSG(CFileExplorer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

TRUNK_NAMESPACE_END

#endif // end of __FILEEXPLORER_H__
