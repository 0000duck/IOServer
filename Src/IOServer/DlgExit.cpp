// DlgExit.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "IOServer.h"
#include "DlgExit.h"
#include "afxdialogex.h"


// CDlgExit �Ի���

IMPLEMENT_DYNAMIC(CDlgExit, CDialogEx)

CDlgExit::CDlgExit(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgExit::IDD, pParent)
{

}

CDlgExit::~CDlgExit()
{
}

void CDlgExit::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgExit, CDialogEx)
END_MESSAGE_MAP()


// CDlgExit ��Ϣ�������


BOOL CDlgExit::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
{
	// TODO: �ڴ����ר�ô����/����û���
	BOOL bRet = CDialogEx::Create(CDlgExit::IDD, pParentWnd);
	return bRet;
	//return CDialogEx::Create(lpszTemplateName, pParentWnd);
}


BOOL CDlgExit::CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam)
{
	// TODO: �ڴ����ר�ô����/����û���

	return CDialogEx::CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, lpParam);
}
