// DlgConfig.cpp : implementation file
//

#include "stdafx.h"
#include "odbcdrv.h"
#include "DlgConfig.h"

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
	m_bUp = FALSE;
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
	ON_BN_CLICKED(IDC_BUTTON_EXSAMP, OnButtonExsamp)
	ON_BN_CLICKED(IDC_BUTTON_INPORT, OnButtonInport)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnButtonUp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgConfig message handlers

BOOL CDlgConfig::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
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

void CDlgConfig::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CDlgConfig::OnCancel() 
{
	// TODO: Add extra cleanup here
	//return;
	CDialog::OnCancel();
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
	dlg.m_ofn.lpstrTitle=_T("����odbcdrv��ǩ�����ļ�"); 
	if(IDOK!=dlg.DoModal())
		return;
	CString szPathName=dlg.GetPathName();
	*/
	
	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, FALSE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("odbcdrv��ǩ���ļ�(*.csv)|*.csv||"), 
		_T("����odbcdrv��ǩ���ļ�"), this))
		return;

	try
	{
		CCSVFile csvfile(szPathName, CCSVFile::modeWrite);
		CStringArray arrData;
		arrData.SetSize(5);
		arrData[0] = _T("odbctags");
		arrData[1] = _T("");
		arrData[2] = _T("");
		arrData[3] = _T("");
		arrData[4] = _T("");
		csvfile.WriteData(arrData);
		
		arrData[0] = _T("��ǩ��");
		arrData[1] = _T("��ǩ����");
		arrData[2] = _T("����");
		arrData[3] = _T("��λ");
		arrData[4] = _T("��������");
		csvfile.WriteData(arrData);
		
		int i;
		for(i = 0; i < n; i++)
		{
			arrData[0] = m_wintag.m_pdrv->m_tagtab.GetText(i, 0);
			arrData[1] = m_wintag.m_pdrv->m_tagtab.GetText(i, 1);
			arrData[2] = m_wintag.m_pdrv->m_tagtab.GetText(i, 2);
			arrData[3] = m_wintag.m_pdrv->m_tagtab.GetText(i, 3);
			arrData[4] = m_wintag.m_pdrv->m_tagtab.GetText(i, 4);
			
			csvfile.WriteData(arrData);
		}
		
		AfxMessageBox("odbcdrv��ǩ�����ļ������ɹ�!", MB_OK | MB_ICONINFORMATION);
	}
	catch (CFileException* e)
	{
		e->Delete();
		AfxMessageBox("odbcdrv��ǩ�����ļ�����ʧ�ܣ�\n��д�ļ�ʧ�ܡ�!", MB_OK | MB_ICONERROR);
	}
	catch (CException *e)
	{
		e->Delete();
		AfxMessageBox("odbcdrv��ǩ�����ļ�����ʧ��!", MB_OK | MB_ICONERROR);
	}

}

BOOL CDlgConfig::AddInPortTag(LPCTSTR szName, LPCTSTR szAliasName, LPCTSTR szDesc, LPCTSTR szUnit, LPCTSTR szDataType)
{
	CTagItem tag;
	tag.m_szName = szName; 
	tag.m_szName = tag.m_szName.Left(79);
	tag.m_szAliasName = szAliasName;
	tag.m_szAliasName = tag.m_szAliasName.Left(79);
	tag.m_szDes = szDesc;
	tag.m_szDes = tag.m_szDes.Left(79);
	tag.m_szUnit = szUnit;
	tag.m_szUnit = tag.m_szUnit.Left(15);
	
	tag.m_nType = tag.GetTypeVal(szDataType);

	tag.m_bSel = TRUE;
	return m_wintag.m_pdrv->m_tagtab.AddTag(&tag);
}

void CDlgConfig::OnButtonExsamp() 
{
	// TODO: Add your control notification handler code here
	/*
	CFileDialog	dlg(FALSE,_T(".csv"),NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("odbcdrv��ǩ�ļ�(*.csv)|*.csv||"),
		NULL);
	dlg.m_ofn.Flags &= ~OFN_EXPLORER; //���������һ�䣬�������ͷ��˳�
	dlg.m_ofn.lpstrTitle=_T("����odbcdrv��ǩ�����ļ�-ʾ��"); 
	if(IDOK!=dlg.DoModal())
		return;

	CString szPathName=dlg.GetPathName();
	*/
	
	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, FALSE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("DDC��ϵ���ݿ��ǩ���ļ�(*.csv)|*.csv||"), 
		_T("����DDC��ϵ���ݿ��ǩ����ʾ���ļ�"), this))
		return;

	FILE *f = fopen (szPathName, "wt");
	if(f==NULL)
	{
		MessageBox(_T("�ļ�����д��"),_T("��ʾ"),MB_OK|MB_ICONWARNING);
		return;
	}
	CString szline = "odbctags,,,,,,,\n";
	fwrite(szline,szline.GetLength(),1,f);

	szline = "��ǩ��,��ǩ����,����,��λ,��������\n";
	fwrite(szline,szline.GetLength(),1,f);

	szline = "demok101,demok101,101��·��·��״̬,,DIGITAL\n";
	fwrite(szline,szline.GetLength(),1,f);
	szline = "demoi32v,demoi32v,demoi32vʾ��32λ����,Kw,INT32\n";
	fwrite(szline,szline.GetLength(),1,f);
	
	szline = "demof32v,demof32v,ʾ��32λ������,Kv,FLOAT32\n";
	fwrite(szline,szline.GetLength(),1,f);

	szline = "demoi64v,demoi64v,ʾ��64λ����,Kw,INT64\n";
	fwrite(szline,szline.GetLength(),1,f);

	szline = "demof64v,demof64v,ʾ��64λ������,Kv,FLOAT64\n";
	fwrite(szline,szline.GetLength(),1,f);


	fclose(f);
	AfxMessageBox("����ʾ���ļ��ɹ�!");
}

void CDlgConfig::OnButtonInport() 
{
	/*
	CFileDialog dlg(TRUE, "", "", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "odbcdrv��ǩ�ļ�(*.csv)|*.csv|", NULL);
	
	dlg.m_ofn.Flags &= ~OFN_EXPLORER; //���������һ�䣬�������ͷ��˳�
	dlg.m_ofn.lpstrTitle = "����odbcdrv��ǩ�����ļ�";
	if(IDOK != dlg.DoModal())
	return ;
	CString szPathName = dlg.GetPathName();
	*/
	
	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, TRUE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("DDC��ϵ���ݿ��ǩ���ļ�(*.csv)|*.csv||"), 
		_T("����DDC��ϵ���ݿ��ǩ�����ļ�"), this))
		return;

	CCSVFile csvfile(szPathName, CCSVFile::modeRead);
	BOOL bOK = TRUE;
	CStringArray arrData;
	int nLine = 0;
	CString szt;
	int nInsert = 0, nUpdate = 0;

	while(bOK) {
		bOK = csvfile.ReadData(arrData);
		if (bOK) {
			nLine++;

			if (1 == nLine) {
				szt = arrData[0];
				szt.TrimLeft();
				szt.TrimRight();
				if (szt.CompareNoCase(_T("odbctags")) != 0)
				{
					AfxMessageBox(_T("�ⲻ��DDC��ϵ���ݿ��ǩ�ļ���"), MB_OK | MB_ICONERROR);
					return;
				}
				continue;
			}
			else if (2 == nLine) {
				/* �ֶ���������ѡ�򵥴�������Ҫ��֤����ֶ����Ǳ�׼˳���ʽ�ģ�
				 *
				 *  odbctags,,,,,
				 *  ��ǩ��,��ǩ����,����,��λ,����
				 *  1000,TEMPAI00001,������001,,float32
				 */
			}
			else {
				if (arrData.GetSize() < 5)
					continue;
				
				CTagItem pi;
				CString szt;

				szt = arrData[0];szt.TrimLeft(); szt.TrimRight();
				pi.m_szName = szt.Left(79);				
				szt = arrData[1];szt.TrimLeft(); szt.TrimRight();
				pi.m_szAliasName = szt.Left(79);
				szt = arrData[2];szt.TrimLeft(); szt.TrimRight();
				pi.m_szDes = szt.Left(79);
				szt = arrData[3];szt.TrimLeft(); szt.TrimRight();
				pi.m_szUnit = szt.Left(15);		
				szt = arrData[4];szt.TrimLeft(); szt.TrimRight();		
				pi.m_nType = CTagItem::GetTypeVal(szt);

				CTagItem *pItem = m_wintag.m_pdrv->m_tagtab.GetTag(pi.m_szName);
				if (pItem)
				{
					if ((pi.m_szAliasName != pItem->m_szAliasName) ||
						(pi.m_szDes != pItem->m_szDes) ||
						(pi.m_szUnit != pItem->m_szUnit) ||
						(pi.m_nType != pItem->m_nType))
					{
						pi.CopyTo(pItem);
						pItem->m_bSel = TRUE;
						nUpdate++;
					}
				}
				else {
					AddInPortTag(pi.m_szName, pi.m_szAliasName, pi.m_szDes, pi.m_szUnit, szt);
					nInsert++;
				}
			}
		}
	}

	if ((nUpdate > 0) || (nInsert > 0)) 
	{
		m_bModified = TRUE;
		m_wintag.ReDraw();
	}

	szt.Format("������ǩ%d�������±�ǩ%d����", nInsert, nUpdate);
	AfxMessageBox(szt);
}

void CDlgConfig::OnButtonUp() 
{
	// TODO: Add your control notification handler code here
	m_bUp = TRUE;
	CDialog::OnOK();
}
