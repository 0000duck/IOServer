// DlgPIServerItem.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PI_F701Drv.h"
#include "DlgPIServerItem.h"
#include <piapix.h>

// CDlgPIServerItem �Ի���

IMPLEMENT_DYNAMIC(CDlgPIServerItem, CDialog)

CDlgPIServerItem::CDlgPIServerItem(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPIServerItem::IDD, pParent)
	, m_nConnTimeout(10)
	, m_nDATimeout(60)
	, m_szIP(_T("127.0.0.1"))
	, m_szPort(_T("5450"))
	, m_szPassword(_T(""))
	, m_szUserName(_T("piadmin"))
{

}

CDlgPIServerItem::~CDlgPIServerItem()
{
}

void CDlgPIServerItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CONNTIMEOUT, m_nConnTimeout);
	DDV_MinMaxInt(pDX, m_nConnTimeout, 0, 6000);
	DDX_Text(pDX, IDC_EDIT_DATIMEOUT, m_nDATimeout);
	DDV_MinMaxInt(pDX, m_nDATimeout, 0, 6000);
	DDX_Text(pDX, IDC_EDIT_SERVERNAME, m_szIP);
	DDX_Text(pDX, IDC_EDIT_PORTNUM, m_szPort);
	DDX_Text(pDX, IDC_EDIT_PASSWARD, m_szPassword);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_szUserName);
}


BEGIN_MESSAGE_MAP(CDlgPIServerItem, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CDlgPIServerItem::OnBnClickedButtonTest)
END_MESSAGE_MAP()


// CDlgPIServerItem ��Ϣ�������

void CDlgPIServerItem::OnBnClickedButtonTest()
{
	UpdateData();

#ifdef USE_STATIC_SAFELOCK
	CPiDBDrv::m_csLock.Enter();
#endif

	CWaitCursor wc;
	BOOL bConnected = FALSE;
	PIINT32	nret = ::piut_setservernode(m_szIP);
	//if(0 == nret)
	{
		LONG valid;
		nret = ::piut_login( m_szUserName, m_szPassword, &valid );	
		if(0 == nret)
			bConnected = TRUE;
	}
	wc.Restore();
#ifdef USE_STATIC_SAFELOCK
	CPiDBDrv::m_csLock.Leave();
#endif

	// ���ͷ����ӣ�������ܻ���ɵ�ǰ���е����������ж�

	if (bConnected)
	{
		AfxMessageBox(_T("PI���ݿ����ӳɹ���"), MB_OK | MB_ICONINFORMATION);
	}
	else {
		AfxMessageBox(_T("PI���ݿ�����ʧ�ܣ�"), MB_OK | MB_ICONERROR);
	}
}

void CDlgPIServerItem::OnOK()
{
	UpdateData();

	m_szIP.Trim();
	if (m_szIP.IsEmpty())
	{
		GetDlgItem(IDC_EDIT_SERVERNAME)->SetFocus();
		AfxMessageBox(_T("����дPI��������IP��ַ��"), MB_OK | MB_ICONERROR);
		return;
	}

	m_szPort.Trim();
	if (m_szPort.IsEmpty())
	{
		GetDlgItem(IDC_EDIT_PORTNUM)->SetFocus();
		AfxMessageBox(_T("����дPI�������Ķ˿ںţ�"), MB_OK | MB_ICONERROR);
		return;
	}

	if (atol(m_szPort) <= 0)
	{
		GetDlgItem(IDC_EDIT_PORTNUM)->SetFocus();
		AfxMessageBox(_T("����д��Ч��PI�������˿ںţ�Ĭ��Ϊ5450��"), MB_OK | MB_ICONERROR);
		return;
	}

	CDialog::OnOK();
}
