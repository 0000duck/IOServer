// CfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ddclib.h"
#include "CfgDlg.h"
#include "DlgComset.h"
#include "DlgItemAttr.h"
#include "ProgressThread.h"
#include "FileExplorer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCfgDlg dialog

CCfgDlg::CCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCfgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCfgDlg)
	m_szConnectString = _T("");
	//}}AFX_DATA_INIT
	m_pDrvObj = NULL;
}


void CCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCfgDlg)
	DDX_Control(pDX, IDC_STATIC_FRAME, m_ctrlFrame);
	DDX_Text(pDX, IDC_EDIT_COM, m_szConnectString);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCfgDlg, CDialog)
	//{{AFX_MSG_MAP(CCfgDlg)
	ON_BN_CLICKED(IDC_BUTTON_COM, OnButtonCom)
	ON_BN_CLICKED(IDC_BUTTON_COMSET, OnButtonComset)
	ON_BN_CLICKED(IDC_BUTTON_EXAMPLE, OnButtonExample)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, OnButtonExport)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_ADDONE, OnButtonAddone)
	ON_BN_CLICKED(IDC_BUTTON_EDITTAG, OnButtonEdittag)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCfgDlg message handlers

void CCfgDlg::OnSysCommand(UINT nID, LONG lParam)
{
	if (nID == SC_CLOSE)
	{
		if (m_config.IsModified() || m_tagtab.IsModified())   
		{
			int nret = AfxMessageBox(_T("��ǩ���������Ϣ�Ѿ����޸ģ��Ƿ񱣴棿"), 
				MB_YESNOCANCEL | MB_ICONQUESTION);
			switch(nret)
			{
			case IDYES:
				CDialog::OnOK();
				return;
			case IDNO:
				break;
			case IDCANCEL:
				return;
			}
		}   
	}   
	CDialog::OnSysCommand(nID, lParam); 
}

BOOL CCfgDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
	CRect rtc;
	m_ctrlFrame.GetWindowRect(&rtc);
	ScreenToClient(&rtc) ;  
	rtc.DeflateRect(1,1,1,1);
	
	m_winTagInfo.m_pTagtab = &m_tagtab;
	m_winTagInfo.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rtc, this, -1);//��ǩ��
	m_winTagInfo.ReDraw();
	m_szConnectString = m_config.GetConnectString();
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCfgDlg::OnCancel() 
{
	if (m_config.IsModified() || m_tagtab.IsModified())   
	{
		int nret = AfxMessageBox(_T("��ǩ���������Ϣ�Ѿ����޸ģ��Ƿ񱣴棿"), 
			MB_YESNOCANCEL | MB_ICONQUESTION);
		switch(nret)
		{
		case IDYES:
			CDialog::OnOK();
			return;
		case IDNO:
			break;
		case IDCANCEL:
			return;
		}
	}   
	
	CDialog::OnCancel();
}

void CCfgDlg::OnButtonCom() 
{
	CDlgComset dlg;
	dlg.m_pDrvObj = m_pDrvObj;
	dlg.SetConfig(m_config);
	dlg.m_wComTypes = m_wComTypes;
	if (dlg.DoModal() == IDOK)
	{
		CConfigInfo info;
		dlg.GetConfig(info);
		if (m_config != info)
		{
			m_config = info;
			m_config.m_bModified = TRUE;
		}

		m_szConnectString = m_config.GetConnectString();
		UpdateData(FALSE);
	}
}

void CCfgDlg::OnButtonComset() 
{
	OnButtonCom();
}

void CCfgDlg::OnButtonExample() 
{
	/*
	CFileDialog	dlg(FALSE,_T(".csv"),NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("CSV(���ŷָ�)�ļ�(*.csv)|*.csv||"),
		NULL);
	dlg.m_ofn.lpstrTitle=_T("���������豸�����ļ�"); 
	if(IDOK!=dlg.DoModal())
		return;
	
	CString szPathName=dlg.GetPathName();
	*/
	
	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, FALSE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("CSV(���ŷָ�)�ļ�(*.csv)|*.csv||"), 
		_T("���������豸�����ļ�"), this))
		return;
	
	FILE *f = fopen (szPathName, "wt");
	if(f==NULL)
	{
		AfxMessageBox(_T("�ļ�����д��"), MB_OK | MB_ICONWARNING);
		return;
	}
	
	CWaitCursor cursor;
	//CProgressThread *pThread = CProgressThread::StartThread();
	//pThread->SetRange(0, 100);
	//pThread->SetText(_T("���ڵ�����ǩ�����ļ�..."));
	
	int i;
	// ���ɱ�ͷ
	CString szline = CTagItem::GetCSVFormatName();
	szline += _T(",,,,,");
	for(i = 0; i < CTagItem::GetCustomFieldCount(); i++)
		szline += _T(",");
	szline += _T("\n");
	
	fwrite(szline,szline.GetLength(),1,f);
	
	// ��������
	szline = "ID,��ǩ��,����,��λ,����";
	for(i = 0; i < CTagItem::GetCustomFieldCount(); i++)
	{	
		CCustomField &field = CTagItem::GetCustomField(i);
		szline += _T(",") + field.m_szDisplayName;
	}
	szline += _T(",��ע\n");
	fwrite(szline, szline.GetLength(), 1, f);
		
	// ��һ��ʾ����ǩ
	szline = _T("1,TESTPOINT1,���Ե�1,c,float32");
	for(i = 0; i < CTagItem::GetCustomFieldCount(); i++)
	{	
		CCustomField &field = CTagItem::GetCustomField(i);
		szline += _T(",");
		szline += field.DefaultValueAsString();
	}
	szline += _T(",��ע1\n");
	fwrite(szline, szline.GetLength(), 1, f);
	
	// �ڶ���ʾ����ǩ
	szline = _T("2,TESTPOINT2,���Ե�2,c,float32");
	for(i = 0; i < CTagItem::GetCustomFieldCount(); i++)
	{	
		CCustomField &field = CTagItem::GetCustomField(i);
		szline += _T(",");
		szline += field.DefaultValueAsString();
	}
	szline += _T(",��ע2\n");
	fwrite(szline, szline.GetLength(), 1, f);
	
	// ������ʾ����ǩ
	szline = _T("3,TESTPOINT3,���Ե�3,c,int32");
	for(i = 0; i < CTagItem::GetCustomFieldCount(); i++)
	{	
		CCustomField &field = CTagItem::GetCustomField(i);
		szline += _T(",");
		szline += field.DefaultValueAsString();
	}
	szline += _T(",��ע3\n");
	fwrite(szline, szline.GetLength(), 1, f);
	
	// ���ĸ�ʾ����ǩ
	szline = _T("4,TESTPOINT4,���Ե�4,c,int32");
	for(i = 0; i < CTagItem::GetCustomFieldCount(); i++)
	{	
		CCustomField &field = CTagItem::GetCustomField(i);
		szline += _T(",");
		szline += field.DefaultValueAsString();
	}
	szline += _T(",��ע4\n");
	fwrite(szline, szline.GetLength(), 1, f);
	
	// �����ʾ����ǩ
	szline = _T("5,TESTPOINT5,���Ե�5,c,digital");
	for(i = 0; i < CTagItem::GetCustomFieldCount(); i++)
	{	
		CCustomField &field = CTagItem::GetCustomField(i);
		szline += _T(",");
		szline += field.DefaultValueAsString();
	}
	szline += _T(",��ע5\n");
	fwrite(szline, szline.GetLength(), 1, f);
	
	// ������ʾ����ǩ
	szline = _T("6,TESTPOINT6,���Ե�6,c,string");
	for(i = 0; i < CTagItem::GetCustomFieldCount(); i++)
	{	
		CCustomField &field = CTagItem::GetCustomField(i);
		szline += _T(",");
		szline += field.DefaultValueAsString();
	}
	szline += _T(",��ע6\n");
	fwrite(szline, szline.GetLength(), 1, f);

	fclose(f);

	AfxMessageBox("�����ɹ���������Excel�򿪲��༭��������ļ�!",MB_OK|MB_ICONINFORMATION);
}

void CCfgDlg::OnButtonAdd() 
{
	/*
	CFileDialog dlg(TRUE, NULL,NULL,OFN_HIDEREADONLY | 	OFN_OVERWRITEPROMPT,
		"�������ļ�(*.csv)|*.csv||",NULL);

	dlg.m_ofn.lpstrTitle = "�����������ļ�";
	if(IDOK != dlg.DoModal())
		return ;	

	CString szPathName = dlg.GetPathName();
	CString szFileName = dlg.GetFileName();
	*/
	
	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, TRUE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("CSV(���ŷָ�)�ļ�(*.csv)|*.csv||"), 
		_T("�����ǩ�����ļ�"), this))
		return;

	FILE* pf=NULL;
	pf = fopen(szPathName, "rt");//�ı�ģʽ��
	if (pf == NULL)
	{
		AfxMessageBox("���ܴ��ļ�!");
		return ;
	}

	CWaitCursor cursor;
	//CProgressThread *pThread = CProgressThread::StartThread();
	//pThread->SetRange(0, 100);
	//pThread->SetText(_T("���ڵ����ǩ..."));
	//CString szInfo1;
	//szInfo1.Format("���ڶ����ǩ......");
	// �����ļ���С
	//fseek(pf, 0, SEEK_END);
	//long fsize = ftell(pf);
	//fseek(pf, 0, SEEK_SET);

	//��һ��,��һ��
	TCHAR c;
	int nrow = 0;
	int ncol = 0;
	CString sz0="";
	while((c = fgetc( pf)) != EOF)
	{
		if(c == ',')
			break;
		else if(c == '\n')
		{
			nrow++;//1
			break;
		}
		else
			sz0 += c;
	}
	if(_stricmp(sz0, CTagItem::GetCSVFormatName()) != 0)		
	{	
		//pThread->Close();
		AfxMessageBox("����Ҫ����������ʽ�ļ�");
		fclose(pf);
		return;
	}
	//�ƶ�����3��
	while(nrow < 2)
	{
		c = fgetc( pf);
		if(c == EOF)
			break;
		if(c == '\n')
			nrow++;//2
	}
	//����3�в���
	if(nrow != 2)
	{
		//pThread->Close();
		AfxMessageBox("�ļ��޲�����");
		fclose(pf);
		return;
	}
	//��������
	int nUpdate = 0,nInsert = 0;

	CTagItem TagItem;

	int i;
	ncol = 0;
	CStringArray arrFields;
	arrFields.SetSize(32);
	int nDataType = 6;//float
	while((c = fgetc( pf)) != EOF)
	{
		if(c == ',')
		{
			ncol++;
			continue;
		}
		else if(c=='\n')
		{
			int nColCount = ncol + 1;
			ncol = 0;
			nrow++;
			if (nColCount < FIELD_LAST + CTagItem::GetCustomFieldCount() + 1)
			{
				for(i = 0; i < arrFields.GetSize(); i++)
					arrFields[i] = _T("");
				continue;
			}
			for(i = 0; i < nColCount; i++)
			{
				arrFields[i].TrimLeft();
				arrFields[i].TrimRight();
			}
			
			if(arrFields[1].IsEmpty())
			{
				for(i = 0; i < arrFields.GetSize(); i++)
					arrFields[i] = _T("");
				continue;
			}
			TagItem.m_szID		= arrFields[0];
			TagItem.m_szName	= arrFields[1];
			TagItem.m_szDes		= arrFields[2];
			TagItem.m_szUnit	= arrFields[3];
			TagItem.SetTypeVal(arrFields[4]);
			for(i = 0; i < CTagItem::GetCustomFieldCount(); i++)
			{
				TagItem.SetCustomField(i, arrFields[FIELD_LAST + i]);
			}
			TagItem.m_szNote	= arrFields[FIELD_LAST + CTagItem::GetCustomFieldCount()];

			CTagItem *pTemp = m_tagtab.GetTag(TagItem.m_szName);
			if (pTemp)
			{
				TagItem.CopyTo(pTemp);
				m_tagtab.SetModified(TRUE);

				nUpdate++;
			}
			else {
				nInsert++;
				m_tagtab.AddTag(&TagItem);
			}

			// ͨ�������ļ��Ķ�λ�������㴦�����
			//pThread->SetPos((int)(ftell(pf) / (double)fsize * 100));
			
			for(i = 0; i < arrFields.GetSize(); i++)
				arrFields[i] = _T("");
			continue;
		}
		arrFields[ncol] += c;
	}
	fclose(pf);
	//pThread->Close();

	//m_tagtab.Qsort();
	m_winTagInfo.ReDraw();
	//cursor.Restore();
	
	CString szInfo;
	szInfo.Format("������ǩ%d�������±�ǩ%d����", nInsert, nUpdate);
	AfxMessageBox(szInfo);
}

void CCfgDlg::OnButtonExport() 
{
	int nsize = m_tagtab.GetSize();
	if (nsize == 0)
	{
		AfxMessageBox("û�пɵ����ı�ǩ�㣡");
		return;
	}
	/*
	CFileDialog	dlg(FALSE,_T(".csv"),NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("CSV(���ŷָ�)�ļ�(*.csv)|*.csv||"),
		NULL);
	dlg.m_ofn.lpstrTitle=_T("������ǩ�����ļ�"); 
	if(IDOK!=dlg.DoModal())
		return;	
	
	CString szPathFile=dlg.GetPathName();
	*/
	
	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, FALSE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("CSV(���ŷָ�)�ļ�(*.csv)|*.csv||"), 
		_T("������ǩ�����ļ�"), this))
		return;
	
	FILE *f = fopen (szPathName, "wt");
	if(f==NULL)
	{
		AfxMessageBox(_T("�ļ�����д��"), MB_OK | MB_ICONWARNING);
		return;
	}
	
	CWaitCursor cursor;
	//CProgressThread *pThread = CProgressThread::StartThread();
	//pThread->SetRange(0, 100);
	//pThread->SetText(_T("���ڵ�����ǩ�����ļ�..."));
	
	int i;
	// ���ɱ�ͷ
	CString szline = CTagItem::GetCSVFormatName();
	szline += _T(",,,,,");
	for(i = 0; i < CTagItem::GetCustomFieldCount(); i++)
		szline += _T(",");
	szline += _T("\n");

	fwrite(szline,szline.GetLength(),1,f);
	
	// ��������
	szline = "ID,��ǩ��,����,��λ,����";
	for(i = 0; i < CTagItem::GetCustomFieldCount(); i++)
	{	
		CCustomField &field = CTagItem::GetCustomField(i);
		szline += _T(",") + field.m_szDisplayName;
	}
	szline += _T(",��ע\n");
	fwrite(szline, szline.GetLength(), 1, f);
	
	int nNum = 0;
	CTagItem *pItem;
	CString szErr;
	CSortObArray &tagList = m_tagtab.GetSortObArray();
	for(i = 0; i < tagList.GetSize(); i++)
	{
		pItem = (CTagItem *)tagList.GetAt(i);
		
		if (pItem == NULL)
			continue;
		
		if (pItem->m_nError ==0)
			szErr = "";
		else if (pItem->m_nError == 1)
			szErr = "�޴˱�ǩ";
		else
			szErr = "���ʹ���";
		
		szline.Format("%s,%s,%s,%s,%s", pItem->m_szID, pItem->m_szName, pItem->m_szDes,
			pItem->m_szUnit, pItem->GetTypeStr());
		for(int j = 0; j < CTagItem::GetCustomFieldCount(); j++)
		{	
			CCustomField &field = CTagItem::GetCustomField(j);
			szline += _T(",");
			szline += pItem->GetText(j + FIELD_LAST);
		}
		szline += _T(",");
		szline += pItem->m_szNote;
		if (!szErr.IsEmpty())
		{
			szline += _T(" - (");
			szline += szErr;	// ������Ϣ��ʾ�����
			szline += _T(")");
		}
		szline += _T("\n");
		fwrite(szline,szline.GetLength(),1,f);
		nNum++;
		//pThread->SetPos((int)(nNum / (double)nsize * 100));
	}
	fclose(f);
	
	//pThread->Close();
	
	szline.Format("�ɹ�����%d����ǩ��¼!", nNum);
	MessageBox(szline, _T("��ʾ"), MB_OK | MB_ICONINFORMATION);
}

void CCfgDlg::OnButtonDel() 
{
	if(IDYES != AfxMessageBox("ɾ���󲻿ɻָ����Ƿ�Ҫɾ��ѡ��ı�ǩ?",MB_YESNO | MB_ICONQUESTION))
		return;
	
	int nDel = m_tagtab.DelSelect();
	m_winTagInfo.ReDraw();
	CString szt;
	szt.Format(_T("�ܹ�ɾ��%d�������"), nDel);
	AfxMessageBox(szt, MB_OK | MB_ICONINFORMATION);
}

void CCfgDlg::OnButtonAddone() 
{	
	CDlgItemAttr dlg;
	dlg.m_bAdd = TRUE;
	CString szError;
	
_loop_show_dlg:
	if(dlg.DoModal()==IDOK)
	{
		
		if(dlg.m_bAdd)
		{
			if(dlg.m_szTagname.IsEmpty())
			{
				AfxMessageBox("��ǩ������Ϊ��!", MB_OK | MB_ICONERROR);
				return;
			}
			CTagItem tagItem;
			tagItem.m_szID	=	dlg.m_szID;
			tagItem.m_szName = dlg.m_szTagname;
			tagItem.m_szDes = dlg.m_szDesc;
			tagItem.m_szUnit = dlg.m_szUnit;
			tagItem.m_szNote = dlg.m_szNote;
			tagItem.m_wType = dlg.GetDataType();
			tagItem.m_bSel = FALSE;
			
			if (m_tagtab.GetTag(dlg.m_szTagname))
			{
				
				szError.Format("%s<%s>����!","����:������", dlg.m_szTagname);//����:������,����
				AfxMessageBox(szError, MB_OK | MB_ICONERROR);
				goto _loop_show_dlg;
			}
			m_tagtab.AddTag(&tagItem);
			//m_tagtab.Qsort();
			m_winTagInfo.ReDraw();
		}
	}	
}

void CCfgDlg::OnButtonEdittag() 
{
	int nSel = m_winTagInfo.GetCurSel();
	
	if(nSel == -1)
	{
		AfxMessageBox("��ѡ��һ��������!", MB_OK | MB_ICONWARNING);
		return;
	}
	CTagItem *pTagItem = m_tagtab.GetTag(nSel);
	if (!pTagItem)
	{
		AfxMessageBox("��ѡ��һ��������!", MB_OK | MB_ICONWARNING);
		return;
	}
	
	CString szError;
	CDlgItemAttr dlg;
	dlg.m_bAdd = FALSE;
	dlg.m_szID		= pTagItem->m_szID;
	dlg.m_szOldName   = pTagItem->m_szName;
	dlg.m_szTagname   = pTagItem->m_szName;
	dlg.m_szDesc      = pTagItem->m_szDes;
	dlg.m_szUnit      = pTagItem->m_szUnit;
	dlg.SetDataType(pTagItem->m_wType);
	dlg.m_szNote      = pTagItem->m_szNote;
	
_loop_show_dlg:
	if(dlg.DoModal()==IDOK)
	{
		if ((dlg.m_szTagname != dlg.m_szOldName) && m_tagtab.GetTag(dlg.m_szTagname))
		{
			szError.Format(_T("��ǩ����%s���Ѿ����ڣ�"), dlg.m_szTagname);
			AfxMessageBox(szError, MB_OK | MB_ICONERROR);
			goto _loop_show_dlg;
		}
		
		pTagItem->m_szID	= dlg.m_szID;
		pTagItem->m_szName	= dlg.m_szTagname;
		pTagItem->m_szDes	= dlg.m_szDesc;
		pTagItem->m_szUnit	= dlg.m_szUnit;
		pTagItem->m_szNote	= dlg.m_szNote;
		pTagItem->m_wType	= dlg.GetDataType();
		pTagItem->m_bSel = FALSE;
		
		m_tagtab.ModifyItem(pTagItem, dlg.m_szOldName);
		//m_tagtab.Qsort();
		m_winTagInfo.ReDraw();
	}	
}
