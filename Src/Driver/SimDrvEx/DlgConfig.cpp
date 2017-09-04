// DlgConfig.cpp : implementation file
//

#include "stdafx.h"
#include "SimdrvEx.h"
#include "DlgConfig.h"
#include "DlgTagAttr.h"
#include "DlgAcceCfg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConfig dialog


CDlgConfig::CDlgConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgConfig::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgConfig)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bModified = FALSE;
	m_pcfg = NULL;
}


void CDlgConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConfig)
	DDX_Control(pDX, IDC_STATIC_FRM, m_stFrm);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgConfig, CDialog)
	//{{AFX_MSG_MAP(CDlgConfig)
	ON_BN_CLICKED(IDC_BUTTON_DELSEL, OnButtonDelsel)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, OnButtonExport)
	ON_BN_CLICKED(IDC_BUTTON_INPORT, OnButtonInport)
	ON_BN_CLICKED(IDC_BUTTON_EXSAMP, OnButtonExsamp)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WINLIST_LBDBLCLK, OnDblClkWinList)
	ON_BN_CLICKED(IDC_BUTTON_ACCE, &CDlgConfig::OnBnClickedButtonAcce)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgConfig message handlers

BOOL CDlgConfig::OnInitDialog() 
{
	CDialog::OnInitDialog();
	_ASSERT(m_pcfg!=NULL);
	// TODO: Add extra initialization here
	CRect rtc;
	m_stFrm.GetWindowRect(&rtc);
	ScreenToClient(&rtc) ;  
	rtc.DeflateRect(1,1,1,1);

	m_wintag.Create(NULL,NULL, WS_CHILD|WS_VISIBLE, rtc, this, -1);//��ǩ��
	m_wintag.ReDraw();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgConfig::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CDlgConfig::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CDlgConfig::OnButtonDelsel() 
{
	// TODO: Add your control notification handler code here
	if(-1 == m_wintag.m_pdrv->m_tagtab.GetNextSelected(0))
	{
		AfxMessageBox("��ѡ���ǩ!");
		return;
	}
	if(IDYES != AfxMessageBox("ɾ���󲻿ɻָ����Ƿ�Ҫɾ��ѡ��ı�ǩ?",MB_YESNO | MB_ICONQUESTION))
		return;

	if(!m_wintag.m_pdrv->m_tagtab.DelSelect())
		return;
	m_wintag.ReDraw();
	m_bModified = TRUE;
}

void CDlgConfig::OnButtonExport() 
{
	// TODO: Add your control notification handler code here
	
	int n = m_wintag.m_pdrv->m_tagtab.GetSize();
	if(n == 0)
	{
		AfxMessageBox("û�б�ǩ!");
		return;
	}

	/*
	CFileDialog	dlg(FALSE,_T(".csv"),NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("simudrv��ǩ�ļ�(*.csv)|*.csv||"),
		NULL);
	dlg.m_ofn.Flags &= ~OFN_EXPLORER; //���������һ�䣬�������ͷ��˳�
	dlg.m_ofn.lpstrTitle=_T("����SimdrvEx��ǩ�����ļ�"); 
	if(IDOK!=dlg.DoModal())
		return;

	CString szPathName=dlg.GetPathName();
	*/
	
	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, FALSE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("simudrv��ǩ�ļ�(*.csv)|*.csv||"), 
		_T("����SimdrvEx��ǩ�����ļ�"), this))
		return;

	FILE *f = fopen (szPathName, "wt");
	if(f==NULL)
	{
		MessageBox(_T("�ļ�����д��"),_T("EcReport��ʾ"),MB_OK|MB_ICONWARNING);
		return;
	}
	CString szline = "simutags,,,,,,\n";
	fwrite(szline,szline.GetLength(),1,f);

	szline = "��ǩ��,����,��λ,��������,ģ�ⷽʽ,��׼ֵ,����(%)\n";
	fwrite(szline,szline.GetLength(),1,f);

	
	//��ʼд������
	CString szt;
	int i;
	for(i=0;i<n;i++)
	{
		szline = m_wintag.m_pdrv->m_tagtab.GetText(i,0);	szline += ',';
		szline += m_wintag.m_pdrv->m_tagtab.GetText(i,1);	szline += ',';
		szline += m_wintag.m_pdrv->m_tagtab.GetText(i,2);	szline += ',';
		szline += m_wintag.m_pdrv->m_tagtab.GetText(i,3);	szline += ',';
		szline += m_wintag.m_pdrv->m_tagtab.GetText(i,4);	szline += ',';
		szline += m_wintag.m_pdrv->m_tagtab.GetText(i,5);	szline += ',';
		szline += m_wintag.m_pdrv->m_tagtab.GetText(i,6);	szline += '\n';
		fwrite(szline,szline.GetLength(),1,f);
	}
	fclose(f);
	AfxMessageBox("�����ɹ�!");
	
}

void CDlgConfig::OnButtonInport() 
{
	// TODO: Add your control notification handler code here
	/*
	CFileDialog dlg(TRUE, NULL,NULL,OFN_HIDEREADONLY | 	OFN_OVERWRITEPROMPT,"simudrv��ǩ�ļ�(*.csv)|*.csv||",NULL);

	dlg.m_ofn.Flags &= ~OFN_EXPLORER; //���������һ�䣬�������ͷ��˳�
	dlg.m_ofn.lpstrTitle = "����SimdrvEx��ǩ�����ļ�";
	if(IDOK != dlg.DoModal())
		return ;

	CString szPathName = dlg.GetPathName();
	CString szFile = dlg.GetFileName();
	*/
	
	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, TRUE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("simudrv��ǩ�ļ�(*.csv)|*.csv||"), 
		_T("����SimdrvEx��ǩ�����ļ�"), this))
		return;

	FILE* pf=NULL;
	pf = fopen(szPathName,"rt");//�ı�ģʽ��
	if(pf == NULL)
	{
		AfxMessageBox("���ܴ��ļ�!");
		return ;
	}
	//��һ��,��һ��
	int nrow=0;
	int ncol = 0;
	TCHAR c;
	CString sz0="";
	while((c = (TCHAR)fgetc( pf)) != EOF)
	{
		if(c == ',')
			break;
		else if(c == '\n')
		{
			nrow++;//1
			break;
		}
		else
			sz0 += (TCHAR)c;
	}
	if(_stricmp(sz0,"simutags") != 0)		
	{	
		AfxMessageBox("����SimdrvEx��ǩ�����ļ�");
		fclose(pf);
		return;
	}
	//�ƶ�����3��
	while(nrow < 2)
	{
		c = (TCHAR)fgetc( pf);
		if(c == EOF)
			break;
		if(c == '\n')
			nrow++;//2
	}
	//����3�в���
	if(nrow != 2)
	{
		AfxMessageBox("�ļ��޲�����");
		fclose(pf);
		return;
	}
	//��������
	ncol = 0;
	sz0 = "";
	CString sz1="",sz2 = "",sz3="",sz4="",sz5="",sz6="";

	while((c = (TCHAR)fgetc( pf)) != EOF)
	{
		if(c == ',')
		{
			ncol++;
			continue;
		}
		else if(c=='\n')
		{
			ncol = 0;
			nrow++;
			if(sz0.IsEmpty())
			{
				sz0 = "";
				sz1 = "";
				sz2 = "";
				sz3 = "";
				sz4 = "";
				sz5 = "";
				sz6 = "";
				continue;
			}

			AddInPortTag(sz0,sz1,sz2,sz3,sz4,sz5,sz6);
			m_bModified = TRUE; 

			sz0 = "";
			sz1 = "";
			sz2 = "";
			sz3 = "";
			sz4 = "";
			sz5 = "";
			sz6 = "";
			continue;
		}
		if(ncol == 0)
			sz0 += c;
		else if(ncol == 1)
			sz1 += c;
		else if(ncol == 2)
			sz2 += c;
		else if(ncol == 3)
			sz3 += c;
		else if(ncol == 4)
			sz4 += c;
		else if(ncol == 5)
			sz5 += c;
		else if(ncol == 6)
			sz6 += c;
	}
	fclose(pf);
	m_wintag.ReDraw();
}

BOOL CDlgConfig::AddInPortTag(LPCTSTR sz0,LPCTSTR sz1,LPCTSTR sz2,LPCTSTR sz3,LPCTSTR sz4,LPCTSTR sz5,LPCTSTR sz6)
{
	CTagItem tag;
	tag.m_szName = sz0; 
	tag.m_szName = tag.m_szName.Left(79);
	tag.m_szDes = sz1;
	tag.m_szDes = tag.m_szDes.Left(79);
	tag.m_szUnit = sz2;
	tag.m_szUnit = tag.m_szUnit.Left(15);
	
	tag.m_nType = tag.GetTypeVal(sz3);
	tag.m_nSimtype = tag.GetSimTypeVal(sz4);
	tag.m_szBaseVal = sz5;
	
	tag.m_fPercent = (float)atof(sz6);
	tag.m_bSel = FALSE;
	return m_wintag.m_pdrv->m_tagtab.UpdateTag(&tag);
}

void CDlgConfig::OnButtonExsamp() 
{
	// TODO: Add your control notification handler code here
	/*
	CFileDialog	dlg(FALSE,_T(".csv"),NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("simudrv��ǩ�ļ�(*.csv)|*.csv||"),
		NULL);
	dlg.m_ofn.Flags &= ~OFN_EXPLORER; //���������һ�䣬�������ͷ��˳�
	dlg.m_ofn.lpstrTitle=_T("����SimdrvEx��ǩ�����ļ�-ʾ��"); 
	if(IDOK!=dlg.DoModal())
		return;

	CString szPathName=dlg.GetPathName();
	*/
	
	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, FALSE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("simudrv��ǩ�ļ�(*.csv)|*.csv||"), 
		_T("����SimdrvEx��ǩ����ʾ���ļ�"), this))
		return;

	FILE *f = fopen (szPathName, "wt");
	if(f==NULL)
	{
		MessageBox(_T("�ļ�����д��"),_T("EcReport��ʾ"),MB_OK|MB_ICONWARNING);
		return;
	}
	CString szline = "simutags,,,,,,\n";
	fwrite(szline,szline.GetLength(),1,f);

	szline = "��ǩ��,����,��λ,��������,ģ�ⷽʽ,��׼ֵ,����(%)\n";
	fwrite(szline,szline.GetLength(),1,f);

	szline = "demok101,101��·��·��״̬,,DIGITAL,line,1,50\n";
	fwrite(szline,szline.GetLength(),1,f);
	szline = "demoi32v,ʾ��32λ����,Kw,INT32,line,100,20\n";
	fwrite(szline,szline.GetLength(),1,f);
	
	szline = "demof32v,ʾ��32λ������,Kv,FLOAT32,sin,200,50\n";
	fwrite(szline,szline.GetLength(),1,f);

	szline = "demoi64v,ʾ��64λ����,Kw,INT64,line,1000,20\n";
	fwrite(szline,szline.GetLength(),1,f);

	szline = "demof64v,ʾ��64λ������,Kv,FLOAT64,sin,2000,50\n";
	fwrite(szline,szline.GetLength(),1,f);


	fclose(f);
	AfxMessageBox("����ʾ���ļ��ɹ�!");
	
}

void CDlgConfig::OnButtonAdd() 
{
	CTagItem tag;	
	CDlgTagAttr dlg;
	dlg.m_bAddTag = TRUE;
	
_DO_LOOP:
	if (dlg.DoModal() != IDOK)
		return;
	
	tag.m_szName		= dlg.m_szTagName;
	tag.m_szDes			= dlg.m_szDesc;
	tag.m_szUnit		= dlg.m_szUnit;
	tag.m_nType			= dlg.m_nDataType + 1;
	tag.m_nSimtype		= dlg.m_nSimulateType;
	tag.m_szBaseVal		= dlg.m_szBaseVal;
	tag.m_fPercent		= dlg.m_fPercent;
	
	if (m_wintag.m_pdrv->m_tagtab.AddTag(&tag))
	{
		m_wintag.ReDraw();
		m_bModified = TRUE; 
	}
	else {
		AfxMessageBox("��ǩ���Ѿ�����!");
		goto _DO_LOOP;
	}
}

void CDlgConfig::OnButtonEdit() 
{
	int nSel = m_wintag.GetCurSel();
	if(-1 == nSel)
	{
		AfxMessageBox("��ѡ����Ҫ�༭�ı�ǩ!");
		return;
	}
	CTagItem tag;
	if (!m_wintag.m_pdrv->m_tagtab.GetAt(nSel, &tag))
		return;

	CDlgTagAttr dlg;
	
	dlg.m_szTagName		= tag.m_szName;
	dlg.m_szDesc		= tag.m_szDes;
	dlg.m_szUnit		= tag.m_szUnit;
	dlg.m_nDataType		= tag.m_nType - 1;
	dlg.m_nSimulateType	= tag.m_nSimtype;
	dlg.m_szBaseVal		= tag.m_szBaseVal;
	dlg.m_fPercent		= tag.m_fPercent;

	if (dlg.DoModal() != IDOK)
		return;

	tag.m_szName		= dlg.m_szTagName;
	tag.m_szDes			= dlg.m_szDesc;
	tag.m_szUnit		= dlg.m_szUnit;
	tag.m_nType			= dlg.m_nDataType + 1;
	tag.m_nSimtype		= dlg.m_nSimulateType;
	tag.m_szBaseVal		= dlg.m_szBaseVal;
	tag.m_fPercent		= dlg.m_fPercent;

	m_wintag.m_pdrv->m_tagtab.UpdateTag(&tag);
	m_wintag.ReDraw();
	m_bModified = TRUE; 
}

LRESULT CDlgConfig::OnDblClkWinList(WPARAM wParam, LPARAM lParam)
{
	//int nIndex = (int)lParam;
	//m_wintag.SetCurSel(nIndex);
	OnButtonEdit();

	return 0;
}
//��������
void CDlgConfig::OnBnClickedButtonAcce()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDlgAcceCfg dlg;
	dlg.m_span = m_pcfg->m_span;
	dlg.m_timeincr = m_pcfg->m_timeincr;
	dlg.m_starttime = m_pcfg->m_starttime;
	dlg.m_endtime = m_pcfg->m_endtime;
	dlg.m_bAllowAcce = m_pcfg->m_bAllowAcce;
	
	if(dlg.DoModal() !=IDOK)
		return;

	m_pcfg->m_span = dlg.m_span;
	m_pcfg->m_timeincr = dlg.m_timeincr;
	m_pcfg->m_starttime = dlg.m_starttime;
	m_pcfg->m_endtime = dlg.m_endtime;
	m_pcfg->m_bAllowAcce = dlg.m_bAllowAcce  ;
	m_bModified = TRUE;
	m_pcfg->SetModify(TRUE);//�������ø���
}
