
// IOServerView.cpp : CIOServerView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "IOServer.h"
#endif

#include "IOServerDoc.h"
#include "IOServerView.h"
#include "DlgLoadDrvTag.h"
#include "DlgQueryTag.h"
#include "DlgImport.h"
#include "DlgTagAttr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIOServerView

IMPLEMENT_DYNCREATE(CIOServerView, CView)

BEGIN_MESSAGE_MAP(CIOServerView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CIOServerView::OnFilePrintPreview)
	ON_COMMAND(ID_GET_DEVICE_TAG, &CIOServerView::OnGetDeviceTag)
	ON_COMMAND(ID_QUERY_TAG, &CIOServerView::OnQueryTag)
	ON_COMMAND(ID_BTN_QUERY, &CIOServerView::OnQueryTag)
	ON_COMMAND(ID_BTN_REFRESH, &CIOServerView::OnReFresh)
	ON_COMMAND(ID_EXPORT_TAG, &CIOServerView::OnExportTag)
	ON_COMMAND(ID_EXPORT_SELECT_TAG, &CIOServerView::OnExportSelectTag)
	ON_COMMAND(ID_IMPORT_TAG, &CIOServerView::OnImportTag)
	ON_COMMAND(ID_DEL_TAG, &CIOServerView::OnDelTag)
	ON_COMMAND(ID_ADD_TAG, &CIOServerView::OnAddTag)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CIOServerView ����/����

CIOServerView::CIOServerView()
{
	// TODO: �ڴ˴���ӹ������
	m_uTimer = 0;
	m_fntText.CreateFont(12, 0, 0, 0, 400, 0, 0, 0, GB2312_CHARSET, 0, 0, PROOF_QUALITY, VARIABLE_PITCH | FF_ROMAN, "����");
}

CIOServerView::~CIOServerView()
{
}

BOOL CIOServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CIOServerView ����

void CIOServerView::OnDraw(CDC* pDC)
{
	CIOServerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CString szq = GetQueryInfo();

	CString szMsg;
	szMsg.Format("����: %s   ���:%d",szq, m_wndTagTable.m_array.GetSize());

	CFont *oldfnt = pDC->SelectObject(&m_fntText);
	pDC->TextOut(140,10,szMsg);
	pDC->SelectObject(oldfnt);
	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
}


// CIOServerView ��ӡ


void CIOServerView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CIOServerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CIOServerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CIOServerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}

void CIOServerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CIOServerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CIOServerView ���

#ifdef _DEBUG
void CIOServerView::AssertValid() const
{
	CView::AssertValid();
}

void CIOServerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CIOServerDoc* CIOServerView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIOServerDoc)));
	return (CIOServerDoc*)m_pDocument;
}
#endif //_DEBUG


// CIOServerView ��Ϣ�������

CString CIOServerView::GetQueryInfo()
{
	CString szQuery = m_szQuery;

	CString sname="",sdbname = "",sdbtagname="",sdes="",stype = "";
	int i,pos=0,nl = szQuery.GetLength();
	if(nl==0)
		return CString("ȫ��");
	char c;
	for(i=0;i<nl;i++)
	{
		c = szQuery[i];
		if(c == '\n')
			pos++;
		else
		{
			if(pos == 0)
				sname+=c;
			else if(pos == 1)
				sdbname+=c;
			else if(pos == 2)
				sdbtagname+=c;
			else if(pos == 3)
				sdes+=c;
			else if(pos == 4)
				stype+=c;
		}
	}
	int ntype = atoi(stype);//zaqxsw19971015

	CString szi = "";
	if(!sname.IsEmpty())
	{
		szi += "��ǩ��=";
		szi += sname;
	}
	if(!sdbname.IsEmpty())
	{
		if(!szi.IsEmpty())
			szi += " AND ";
		szi += "���ݿ�=";
		szi += sdbname;
	}
	if(!sdbtagname.IsEmpty())
	{
		if(!szi.IsEmpty())
			szi += " AND ";
		szi += "���ݿ��ǩ=";
		szi += sdbtagname;
	}
	if(!sdes.IsEmpty())
	{
		if(!szi.IsEmpty())
			szi += " AND ";
		szi += "����=";
		szi += sdes;
	}
	if(ntype > 0)
	{
		if(!szi.IsEmpty())
			szi += " AND ";
		szi += "����=";
		//szi += CTagItem::GetTypeStr(ntype);
	}
	return szi;
}

void	CIOServerView::Query()
{
	CString szQuery = m_szQuery;
	CString sname="",sdbname = "",sdbtagname="",sdes="",stype = "";
	int i,pos=0,nl = szQuery.GetLength();
	char c;
	for(i=0;i<nl;i++)
	{
		c = szQuery[i];
		if(c == '\n')
			pos++;
		else
		{
			if(pos == 0)
				sname+=c;
			else if(pos == 1)
				sdbname+=c;
			else if(pos == 2)
				sdbtagname+=c;
			else if(pos == 3)
				sdes+=c;
			else if(pos == 4)
				stype+=c;
		}
	}
	int ntype = atoi(stype);
	if(ntype == 0)
		ntype = -1;

	m_wndTagTable.QueryTag(sname,sdbname,sdbtagname,sdes,ntype);
	m_wndTagTable.RedrawList();
	CRect rt(0,0,1024,29);
	InvalidateRect(&rt);
}

int CIOServerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������

	// TODO: Add your specialized creation code here
	CString szContent;
	szContent.LoadStringA(IDS_QUERY);
	m_btnQuery.Create(szContent,WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,CRect(0,0,0,0),this,ID_BTN_QUERY);
	szContent.LoadStringA(IDS_REFRESH);
	m_btnRefresh.Create(szContent,WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,CRect(0,0,0,0),this,ID_BTN_REFRESH);

	m_btnQuery.SetFont(&m_fntText);
	m_btnRefresh.SetFont(&m_fntText);

	CIOServerDoc* pDoc = GetDocument();

	m_wndTagTable.m_szDecName = pDoc->m_szDecName;
	//m_wndTagTable.m_pvalman = &(theApp.m_pDriverMgr->m_DecValMan);

	CRect rtc(0,0,0,0);	
	m_wndTagTable.Create(NULL,NULL, WS_CHILD|WS_VISIBLE, rtc, this, -1);//��ǩ��
	m_wndTagTable.QueryTag(NULL,NULL,NULL,NULL,-1);//��ѯȫ��
	
	m_wndTagTable.ReDraw();

	SetTimer(1, 1000, NULL);
	return 0;
}


void CIOServerView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	if(m_btnQuery.GetSafeHwnd() != NULL)
		m_btnQuery.MoveWindow(8,5,50,20);
	if(m_btnRefresh.GetSafeHwnd() != NULL)
		m_btnRefresh.MoveWindow(70,5,50,20);

	if(m_wndTagTable.GetSafeHwnd() != NULL)
		m_wndTagTable.MoveWindow(CRect(0,30,cx,cy));
}


void CIOServerView::OnGetDeviceTag()
{
	// TODO: �ڴ���������������

	if(!theApp.CheckPower(0XFFFFFFFF))
		return ;

	CIOServerDoc* pDoc = GetDocument();

	if(DEC_STATUS_RUN == theApp.GetDecStatus(pDoc->m_szDecName))
	{
		AfxMessageBox(IDS_MSG_DEVICESTOP,MB_OK|MB_ICONINFORMATION);
		return ;
	}

	CDlgLoadDrvTag dlg;
	if(dlg.DoModal() != IDOK)
		return;
	if(dlg.m_dwflag == 0)
	{
		AfxMessageBox(IDS_MSG_CANCEL);
		return;
	}

	CDrvTags drvTags;
	int nNew = 0, nUpdate = 0;
	int nCount = theApp.GetDrvTags(dlg.m_dwflag, pDoc->m_szDecName, drvTags, nNew, nUpdate);
	if(nNew || nUpdate)
	{
		Query(); //���²�ѯ
		theApp.Save();//����
	}
	CString szMsg;
	CString szTemp;
	szTemp.LoadString(IDS_MSG_LOADDEV);
	szMsg.Format(szTemp, nCount, nNew, nUpdate);
	AfxMessageBox(szMsg,MB_OK|MB_ICONINFORMATION);
}


void CIOServerView::OnQueryTag()
{
	// TODO: �ڴ���������������
	m_szQuery;//��ѯ�ַ���,name\n �����ǩ�� \n ������ǩ��\n ����\n ����\n

	CString szQuery = m_szQuery;
	CDlgQueryTag dlg;
	CString sname="",sdbname = "",sdbtagname="",sdes="",stype = "";
	int i,pos=0,nl = szQuery.GetLength();
	char c;
	for(i=0;i<nl;i++)
	{
		c = szQuery[i];
		if(c == '\n')
			pos++;
		else
		{
			if(pos == 0)
				sname+=c;
			else if(pos == 1)
				sdbname+=c;
			else if(pos == 2)
				sdbtagname+=c;
			else if(pos == 3)
				sdes+=c;
			else if(pos == 4)
				stype+=c;
		}
	}
	dlg.m_sname = sname;
	dlg.m_spname = sdbname;
	dlg.m_sdname = sdbtagname;
	dlg.m_sdes = sdes;
	dlg.m_ntype = atoi(stype);
	if(dlg.m_ntype < 0)
		dlg.m_ntype = 0;
	if(dlg.DoModal()!= IDOK)
		return;
	int ntype = dlg.m_ntype;
	if(ntype == 0)
		ntype = -1;

	szQuery.Format("%s\n%s\n%s\n%s\n%d\n",dlg.m_sname,dlg.m_spname,dlg.m_sdname,dlg.m_sdes,ntype);
	m_szQuery = szQuery;
	m_wndTagTable.QueryTag(dlg.m_sname,dlg.m_spname,dlg.m_sdes,dlg.m_sdname,ntype);
	m_wndTagTable.ReDraw();//ˢ��
	CRect rt(0,0,1024,29);
	InvalidateRect(&rt);
}




void CIOServerView::OnExportTag()
{
	// TODO: �ڴ���������������
	if(!theApp.CheckPower(1))
		return ;
	CIOServerDoc* pDoc = GetDocument();
	if(theApp.GetDecStatus(pDoc->m_szDecName) == DEC_STATUS_RUN)
	{
		MessageBox("��ֹͣ�豸����!","��ʾ",MB_OK|MB_ICONINFORMATION);
		return ;
	}

	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, FALSE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("��ǩ�ļ�(*.csv)|*.csv||"), 
		_T("������ǩ�ļ�"), this))
		return;

	try
	{
		CCSVFile file(szPathName, CCSVFile::modeWrite);

		int ntags = 0;
		m_wndTagTable.ExportTags(&file,&ntags);
		file.Close();

		CString szMsg;
		szMsg.Format("�������!\n\n%d��ǩ",ntags);
		MessageBox(szMsg,"��ʾ",MB_OK|MB_ICONINFORMATION);
	}
	catch (...)
	{
		MessageBox(_T("�ļ�����д��"),_T("��ʾ"),MB_OK|MB_ICONWARNING);
		return;
	}
}


void CIOServerView::OnExportSelectTag()
{
	// TODO: �ڴ���������������
	if(!theApp.CheckPower(1))
		return ;

	CIOServerDoc* pDoc = GetDocument();
	if(theApp.GetDecStatus(pDoc->m_szDecName) == DEC_STATUS_RUN)
	{
		MessageBox("��ֹͣ�豸����!","��ʾ",MB_OK|MB_ICONINFORMATION);
		return ;
	}
	if(m_wndTagTable.GetNextSelTag(0) == -1)
	{
		MessageBox("û��ѡ���ǩ!","��ʾ",MB_OK|MB_ICONINFORMATION);
		return;
	}

	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, FALSE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("��ǩ�ļ�(*.csv)|*.csv||"), 
		_T("������ǩ"), this))
		return;

	try
	{
		CCSVFile file(szPathName, CCSVFile::modeWrite);

		int ntags = 0;
		m_wndTagTable.ExportSelTags(&file, &ntags);
		file.Close();

		CString szMsg;
		szMsg.Format("���!\n\n%d��ǩ",ntags);
		MessageBox(szMsg,"��ʾ",MB_OK|MB_ICONINFORMATION);	
	}
	catch (...)
	{
		MessageBox(_T("�ļ�����д��"),_T("��ʾ"),MB_OK|MB_ICONWARNING);
		return;
	}
}


void CIOServerView::OnImportTag()
{
	// TODO: �ڴ���������������
	if(!theApp.CheckPower(0XFFFFFFFF))
		return ;

	CIOServerDoc* pDoc = GetDocument();

	if(theApp.GetDecStatus(pDoc->m_szDecName) == DEC_STATUS_RUN)
	{
		MessageBox("��ֹͣ�豸����!","��ʾ",MB_OK|MB_ICONINFORMATION);
		return ;
	}

	CString szPathName;
	CString szFileName;
	if (!CFileExplorer::ShowFileDialog(szFileName, szPathName, TRUE, _T(".csv"),
		NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("��ǩ�ļ�(*.csv)|*.csv||"), 
		_T("�����ǩ�����ļ�"), this))
		return;

	CDlgImport dlg;
	if(dlg.DoModal() != IDOK)
		return;
	if(dlg.m_dwflag == 0) //û�в���
	{
		MessageBox("����0�������0��","��ʾ",MB_OK|MB_ICONINFORMATION);
		return;
	}

	try
	{
		CCSVFile csvfile(szPathName, CCSVFile::modeRead);

		int nUpdate = 0, nAdd=0, nAll = 0;
		if(!theApp.ImportTags(pDoc->m_szDecName, csvfile,nUpdate,nAdd,nAll,dlg.m_dwflag))
		{
			csvfile.Close();
			AfxMessageBox("�ļ���ʽ����!");
			return;
		}
		csvfile.Close();

		Query(); //���²�ѯ
		theApp.Save();//���湤��

		CString szMsg;
		szMsg.Format("�������!\n\n����%d�������%d��",nUpdate,nAll);
		MessageBox(szMsg,"��ʾ",MB_OK|MB_ICONINFORMATION);
	}
	catch (...)
	{
		AfxMessageBox("���ܴ��ļ�!");
		return ;
	}	
}


void CIOServerView::OnDelTag()
{
	// TODO: �ڴ���������������
	if(!theApp.CheckPower(0XFFFFFFFF))
		return ;

	CIOServerDoc* pDoc = GetDocument();

	if(theApp.GetDecStatus(pDoc->m_szDecName) == DEC_STATUS_RUN)
	{
		MessageBox("��ֹͣ�豸����!","��ʾ",MB_OK|MB_ICONINFORMATION);
		return ;
	}

	if(m_wndTagTable.GetNextSelTag(0) == -1)
	{
		MessageBox("û��ѡ���ǩ!","��ʾ",MB_OK|MB_ICONINFORMATION);
		return;
	}
	if(IDYES != MessageBox("ɾ���󲻿ɻָ����Ƿ�Ҫɾ��ѡ��ı�ǩ?","��ʾ",MB_YESNOCANCEL|MB_ICONINFORMATION))
		return;
	int ntags = DelSelTags(pDoc->m_szDecName); 

	Query(); //���²�ѯ

	theApp.Save();//���湤��

	CString szMsg;
	szMsg.Format("ɾ��ѡ��ı�ǩ���!\n\n%d��ǩ",ntags);
	MessageBox(szMsg,"��ʾ",MB_OK|MB_ICONINFORMATION);
}


int	CIOServerView::DelSelTags(CString szDecName)//ɾ��ѡ��ı�ǩ
{
	int i = 0,n=0;
	CTagItem *pi;

	// ��ֹ��־�������
	//theApp.m_runlog.SetMaxLimit(FALSE);

	while(i<m_wndTagTable.m_array.GetSize())
	{
		pi = (CTagItem*)m_wndTagTable.m_array.GetAt(i);
		if(!pi->m_bSel)
			i++;
		else
		{
			theApp.DelTag(szDecName, pi->m_szName);
			m_wndTagTable.m_array.RemoveAt(i);

			GetLogger()->AddWarningFmt(LOG_OPT_STORAGE, LOG_DELETE_TAG_FORMAT, LOG_LOGIN_USER, pi->m_szName);

			delete pi;

			n++;
		}
	}
	//theApp.m_runlog.SetMaxLimit(TRUE);
	return n;
}
void CIOServerView::OnAddTag()
{
	// TODO: �ڴ���������������
	if(!theApp.CheckPower(0XFFFFFFFF))
		return ;

	CIOServerDoc* pDoc = GetDocument();

	if(theApp.GetDecStatus(pDoc->m_szDecName) == DEC_STATUS_RUN)
	{
		MessageBox("��ֹͣ�豸����!","��ʾ",MB_OK|MB_ICONINFORMATION);
		return ;
	}

	CDlgTagAttr dlg;
	//dlg.m_pdrv = pdrv;
	dlg.m_szDrvName = m_wndTagTable.m_szDecName;
	dlg.m_bAdd = TRUE;
lpredo:
	if(dlg.DoModal() != IDOK)
		return;

	if(!dlg.m_szexp.IsEmpty())
	{
		/*
		CString szbh="",szErr;
		if(0 != g_Mid2Behind(dlg.m_szexp,szbh,szErr))//����->���� ret=0 ok
		{
			AfxMessageBox("ת����ʽ�﷨����!");
			goto lpredo;
		}
		if(!g_CheckBehindParse(szbh))
		{
			AfxMessageBox("ת����ʽ�﷨����!");
			goto lpredo;
		}
		*/
		//CExpressionEngine::RemoveAll();
		//CExpressionEngine engine(FALSE); // ����һ�����ڼ����ʽ�Ƿ���ȷ�ļ�������
		//engine.SetExpr(dlg.m_szexp);
		//if (!engine.Check(dlg.m_sztagname))
		//{
		//	AfxMessageBox("ת����ʽ�﷨�������������ת����ʽ�Ƿ���ȷ��");
		//	goto lpredo;
		//}

		//if (CExpressionEngine::GetTagCount() == 0)
		//{
		//	if (AfxMessageBox(_T("ת����ʽ��δ�����в������ı�ǩ������ת����ʽ�����⣬���������ת����ʽ�Ƿ���ȷ��"), MB_YESNO) == IDNO)
		//		goto lpredo;
		//}
	}
	//�����ǩ��,���ɹ��򱨴���ת��
	CTagItem tag;
	tag.m_szName = dlg.m_sztagname;
	tag.m_sznamedb=dlg.m_sztagdbname;
	tag.m_szdes=dlg.m_szdes;
	tag.m_szunit = dlg.m_szunit;
	tag.m_sznamedrv=dlg.m_sztagdrvname;
	tag.m_ntype = dlg.m_ndatatype + 1;
	tag.m_nCompType=dlg.m_ncomptype;
	tag.m_szCompVal=dlg.m_szcompval;
	tag.m_szexpcp=dlg.m_szexp;

	if(theApp.AddTag2Drv(pDoc->m_szDecName, tag, dlg.m_bAdd))
	{
		// ��ֹ��־�������
		//theApp.m_runlog.SetMaxLimit(FALSE);
		//theApp.m_logger.AddWarningFmt(LOG_OPT_STORAGE, LOG_INSERT_TAG_FORMAT, LOG_LOGIN_USER, tag.m_szname);
		//theApp.m_runlog.SetMaxLimit(TRUE);

		theApp.Save();
		Query(); //���²�ѯ
	}
	else
	{
		AfxMessageBox(IDS_TAG_EXIST);
		goto lpredo;
	}
}


void CIOServerView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (1 == nIDEvent)
	{
		m_wndTagTable.Invalidate();
		return;
	}
	CView::OnTimer(nIDEvent);
}

void CIOServerView::OnReFresh()
{
	Query();
}