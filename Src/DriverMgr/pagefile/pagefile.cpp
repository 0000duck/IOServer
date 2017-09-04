//pagefile.cpp ҳ���ļ� 

#include "stdafx.h"
#include "pagefile.h"
#include <io.h>

//CLogger *GetLogger();
////////////////////////////////////////////////////
CPfRecVal::CPfRecVal()
{
	m_szdes = "";//����
	m_ltime = 0;//����ʱ��
	m_ntype = 0; //����
	m_nszie = 0; //�����ֽ���
	m_lbufsize =0 ;
	m_pdata = NULL;

	m_snqa = 0;  //����
	m_wres = 0;		//����
	m_dwres = 0;		//����

}
CPfRecVal::~CPfRecVal()
{
	if(m_pdata)
		free(m_pdata);
}

void	CPfRecVal::ResetBufSize(long lsize) //���ô�С
{
	if(m_pdata)
		free(m_pdata);
	m_nszie = 0;
	m_lbufsize = 0;
	if(lsize <= 0)
		return ;
	m_lbufsize = (lsize/8) * 8 + 8;
	m_pdata = malloc(m_lbufsize);
	if(m_pdata == NULL)
		m_lbufsize = 0;
}

BOOL	CPfRecVal::SetData(void* pdata,long lsize)//��������
{

	if(m_pdata)
		free(m_pdata);
	m_lbufsize = 0;
	m_nszie = 0;
	if((pdata == NULL) || (lsize <= 0))
		return TRUE;
	m_lbufsize = (lsize/8) * 8 + 8;
	m_pdata = malloc(m_lbufsize);
	if(m_pdata == NULL)
	{
		m_lbufsize = 0;
		return FALSE;
	}
	memcpy(m_pdata,pdata,lsize);
	m_nszie = lsize;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
CPageFile::CPageFile()
{
	m_lgrownsize = 16 * 1024 * 1024;//Ĭ��16M����
	m_bOpen = FALSE;
	m_szFile = "";

	memset(&m_head,0,sizeof(PFHEAD));
	m_head.dwfalg = 0x81018202;
	m_head.lpages = 0;
	m_head.lindexs = 0;
	m_head.lgrownsize = m_lgrownsize;

	m_indexs = (LPPFINDEX)malloc(PF_MAXINDEXS * sizeof(PFINDEX));
	m_pages = (long*)malloc( PF_MAXPAGES * sizeof(long));

	if(m_indexs)
		memset(m_indexs,0,PF_MAXINDEXS * sizeof(PFINDEX));
	if(m_pages)
		memset(m_pages,0,PF_MAXPAGES * sizeof(long));
}


CPageFile::~CPageFile()
{
	if(m_indexs)
		free(m_indexs);
	if(m_pages)
		free(m_pages);

	m_indexs = NULL;
	m_pages = NULL;

	m_bNeedFlush = FALSE;
}

void CPageFile::Clear()	//��������Ϳ���
{
	m_szFile = "";
	memset(&m_head,0,sizeof(PFHEAD));
	//�������
	if(m_indexs)
		memset(m_indexs,0,PF_MAXINDEXS * sizeof(PFINDEX));

	//���ҳ����
	if(m_pages)
		memset(m_pages,0,PF_MAXPAGES * sizeof(long));
}

//void g_AddLog(long ltime,LPCTSTR lpszTxt);
void		CPageFile::EmptyFile()//��գ���ȡ������ļ�β
{
	if(m_file.GetLength() > 16 * 1024 * 1024)
	{
		Clear();
		//д���ļ�ͷ
		m_head.dwfalg = 0x81018202;
		m_head.lgrownsize = m_lgrownsize;//�ļ�����
		m_file.SetLength(16 * 1024 * 1024);//16M ��ʼ�ļ���С
		m_head.lpages = (16 * 1024 * 1024 - PF_DATAPOS)/PF_PAGESIZE;//��ʼҳ����
		m_file.SeekToBegin();
		m_file.Write(&m_head,sizeof(PFHEAD));
		m_file.Write(m_indexs,PF_MAXINDEXS * sizeof(PFINDEX));
		m_file.Write(m_pages,PF_MAXPAGES * sizeof(long));
		m_file.Flush();
		TRACE("EmptyPageFile\n");
		GetLogger()->AddInfo("��ǰ�ɱ����ͻ���cashe�������!");
	}
}

void	CPageFile::CloseAndDelFile()//�رպ�ɾ���ļ�
{
	CString szFile = m_szFile;
	Close();
	if(!szFile.IsEmpty())
	{
		::DeleteFile(szFile);

		GetLogger()->AddInfoFmt("<%s>ҳ���ļ��������!",szFile);
	}
}

int	CPageFile::Close()					//�ر�
{
	Clear();
	if(!m_bOpen)
		return PFR_OK;
	try
	{
		m_file.Close();
	}
	catch ( CFileException *ex ) 
	{
	   ex->Delete();
	}
	m_bOpen = FALSE;
	return PFR_OK;
}

void CPageFile::TraceIndex()
{
	int i;
	for(i=0;i<PF_MAXINDEXS;i++)
	{
		if(m_indexs[i].ltime)
			TRACE("index[%d].ltime = %d,sp = %d\n",i,m_indexs[i].ltime ,m_indexs[i].lpageno);
	}
}

void CPageFile::TracePageMark()
{
	int i;
	for(i=0;i<m_head.lpages;i++)
	{
		if(m_pages[i])
			TRACE("m_pagse[%d] = %d\n",i,m_pages[i]);
	}
}

/*
	��ҳ���ļ�,����Ѿ������ȹرա�
	������
		lpszFile �ļ���
	����ֵ��
		�ɹ�:PFR_OK��PFR_MEMERR��PFR_NOOPEN��PFR_ERR  

  */
int	CPageFile::Open(LPCTSTR lpszFile)		//��
{
	if((m_indexs == NULL)||(m_pages == NULL))
		return PFR_MEMERR;
	
	Close();
	//�����ļ�ͷ
	UINT uread = 0;
	try
	{
		if(!m_file.Open(lpszFile,CFile::modeReadWrite |  CFile::shareExclusive))
			return PFR_NOOPEN;

		uread = m_file.Read(&m_head,sizeof(PFHEAD));
		if(m_head.dwfalg != 0x81018202)
		{
			m_file.Close();
			return PFR_NOTPF;
		}
		//��������
		uread = m_file.Read(m_indexs,PF_MAXINDEXS * sizeof(PFINDEX));
		if(uread != PF_MAXINDEXS * sizeof(PFINDEX))
		{
			m_file.Close();
			return PFR_NOTPF;
		}
//		TraceIndex(); //mydebug
		uread = m_file.Read(m_pages,PF_MAXPAGES * sizeof(long));
		if(uread != PF_MAXPAGES * sizeof(long))
		{
			m_file.Close();
			return PFR_PAGEMARKERR;
		}
//		TracePageMark();//mydebug
		m_szFile = lpszFile;
		
		if(m_head.lgrownsize < (1024 * 1024))
			m_head.lgrownsize= 1024 * 1024;
		if(m_head.lgrownsize > 512 * 1024 * 1024)
			m_head.lgrownsize = 512 * 1024 * 1024;

		m_lgrownsize = m_head.lgrownsize;
		
		m_bOpen = TRUE;
		return PFR_OK;
	}
	catch ( CFileException *ex ) 
	{
	   ex->Delete();
	}
	m_file.Close();
	return PFR_ERR;
}

/*
	����ҳ���ļ�
	������
		lpszFile ҳ���ļ���
	����ֵ��
		PFR_OK��PFR_FILEEXIST��PFR_ERR
  */
int	CPageFile::Create(LPCTSTR lpszFile)	//����,��ʼ����16M
{
	//����ļ��Ƿ����
	if(0 == _access(lpszFile,0))
		return PFR_FILEEXIST;//�ļ�����
	Close();
	try
	{
		if(!m_file.Open(lpszFile,CFile::modeReadWrite | CFile::shareExclusive | CFile::modeCreate))
			return PFR_ERR;//����ʧ��

		//д���ļ�ͷ
		m_head.dwfalg = 0x81018202;
		m_head.lgrownsize = m_lgrownsize;//�ļ�����
		m_file.SetLength(16 * 1024 * 1024);//16M ��ʼ�ļ���С
		m_head.lpages = (16 * 1024 * 1024 - PF_DATAPOS)/PF_PAGESIZE;//��ʼҳ����
		m_file.SeekToBegin();
		m_file.Write(&m_head,sizeof(PFHEAD));
		m_file.Write(m_indexs,PF_MAXINDEXS * sizeof(PFINDEX));
		m_file.Write(m_pages,PF_MAXPAGES * sizeof(long));
		m_file.Flush();
		m_bOpen = TRUE;
		m_szFile = lpszFile;
		return PFR_OK;
	}
	catch ( CFileException *ex ) 
	{
	   ex->Delete();
	}
	return PFR_ERR;//����ʧ��
}

void CPageFile::Flush()
{
	if(!m_bOpen)
		return ;//û�д�
	if(!m_bNeedFlush)
		return;
	try
	{
		m_file.Flush();
		m_bNeedFlush = FALSE;
	}
	catch ( CFileException *ex ) 
	{
	   ex->Delete();
	}
}

long CPageFile::GetEmptyPages()//ȡ���е�ҳ����
{
	int i,n=0;
	for(i=0;i<m_head.lpages;i++)
	{
		if(m_pages[i] == 0)
			n++;
	}
	return n;
}

long CPageFile::GetOneEmptyIndex()//ȡһ����������λ��
{
	for(int i=0;i<PF_MAXINDEXS;i++)
	{
		if(m_indexs[i].ltime == 0)
			return i;
	}
	return -1;
}

int	CPageFile::GetNextEmptyPage(int nstartpos)
{
	int i;
	for(i=nstartpos;i<m_head.lpages;i++)
	{
		if(m_pages[i] == 0)
			return i;
	}
	//�����´��̿ռ�
	if(m_head.lpages < PF_MAXPAGES)
	{
		long lnewpages = m_head.lpages + m_lgrownsize/PF_PAGESIZE;
		if(lnewpages > PF_MAXPAGES)
			lnewpages = PF_MAXPAGES;
		try
		{
			m_file.SetLength(lnewpages * PF_PAGESIZE + PF_DATAPOS);
			m_head.lpages = lnewpages;
			m_file.SeekToBegin();
			m_file.Write(&m_head,sizeof(PFHEAD));
		}
		catch ( CFileException *ex ) 
		{
		   ex->Delete();
		   return -1;
		}
		while(i<m_head.lpages)
		{
			if(m_pages[i] == 0)
				return i;
			i++;
		}
	}
	return -1;
}

int	CPageFile::GetNextEmptyIndex(int nstart)//ȡ��һ�����õ�������
{
	int	i;
	for(i=nstart;i<PF_MAXINDEXS;i++)
	{
		if(m_indexs[i].ltime == 0)
			return i;
	}
	return -1;
}

int	CPageFile::PF_GetPages(LPPFINDEX pf)//ȡһ��������ռҳ����
{
	int n=0;
	long lno = pf->lpageno;
	while(lno != PF_PAGEEND)
	{
		n++;
		lno = m_pages[lno];
	}
	return n;
}

/*
	���ҳ�棬������ӵ�ҳ���,-1��ʾ����
  */
int	CPageFile::PF_AddPage(PF_HANDLE h)//���һ��ҳ��
{
	LPPFINDEX pf = m_indexs + h;
	long lpno = pf->lpageno;
	while(m_pages[lpno] != PF_PAGEEND)
		lpno = m_pages[lpno];

	long lnewp = GetNextEmptyPage(0);
	if(lnewp == -1)
		return -1;
	m_pages[lpno] = lnewp;
	m_pages[lnewp] = PF_PAGEEND;
	try
	{
		m_file.Seek(PF_PAGEMARKPOS + lpno * sizeof(long),CFile::begin);
		m_file.Write(&(m_pages[lpno]), sizeof(long));//дǰһ��

		m_file.Seek(PF_PAGEMARKPOS + lnewp * sizeof(long),CFile::begin);
		m_file.Write(&(m_pages[lnewp]), sizeof(long));//д���һ��
	}
	catch ( CFileException *ex ) 
	{
	   ex->Delete();
	   //�ָ�
	   m_pages[lpno] = PF_PAGEEND;
	   m_pages[lnewp] = 0;

	   return -1;
	}
	return lnewp;
}

/*
	��λҳ���ļ�offset��ҳ���
	����ҳ��ţ�-1��ʾʧ��
  */
int	CPageFile::PF_GetPageNo(LPPFINDEX pindex,long offset)
{
	long lpages = offset/PF_PAGESIZE;
	int n=0;
	long lpageno = pindex->lpageno;
	while(lpageno != PF_PAGEEND)
	{
		if(n == lpages)
			return lpageno;
		lpageno = m_pages[lpageno];
		n++;
	}
	return -1;
}

int	CPageFile::PF_Delete(PF_HANDLE h)//ɾ��
{
	LPPFINDEX pf = m_indexs + h;

	//�����ռҳ��
	long lmk=0;
	int npr=-1,npno = pf->lpageno;
	try
	{
		while(npno != PF_PAGEEND)
		{
			npr = npno;
			npno = m_pages[npno];
			m_pages[npr] = 0; //���Ϊ����

			m_file.Seek(PF_PAGEMARKPOS + npr * sizeof(long),CFile::begin);
			m_file.Write(&lmk, sizeof(long)); //ͬ��������
		}
		m_indexs[h].ltime = 0;//ɾ������
		if(h == m_head.lindexs-1)
		{
			m_head.lindexs--;
			m_file.SeekToBegin();
			m_file.Write(&m_head,sizeof(PFHEAD));
		}
		m_file.Seek(PF_INDEXPOS + h * sizeof(PFINDEX),CFile::begin);
		m_file.Write(&(m_indexs[h]),sizeof(PFINDEX)); //ͬ��������
		m_file.Flush();
		m_bNeedFlush = FALSE;
	}
	catch( CFileException *ex ) 
	{
	   ex->Delete();
	   return PFR_ERR;
	}
	
	return PFR_OK;
}

/*
	д�ļ�
	h ҳ���ļ����
	offset λ��: -1��ʾ���ļ�β����
	pbuf: ����
	nsize:�����ֽ���

	����ֵ:
	PFR_OK;
	PFR_ERR;
  */
int	CPageFile::PF_Write(PF_HANDLE h,long offset,const void *pbuf,long nsize,BOOL bFlush)//д
{
	if(nsize <= 0)
		return PFR_OK;

	LPPFINDEX pf = m_indexs + h;
	long loff = offset;
	if(loff == -1)
		loff = pf->lsize;

	int npno = PF_GetPageNo(pf,loff);
	if(npno == -1)
	{
		if((pf->lsize % PF_PAGESIZE) == 0) //�ų���ҳ�������
		{
			npno = PF_AddPage(h);
			if(npno == -1)
				return PFR_ERR;
		}
		else
		{
			return PFR_ERR;
		}
	}
	long lt = PF_PAGESIZE - (pf->lsize % PF_PAGESIZE);//��ǰҳʣ��ռ�
	long lw = 0;
	if(lt > nsize)
		lt = nsize;
	char *pc = (char *)pbuf;
	long lseek = PF_DATAPOS + npno * PF_PAGESIZE + (pf->lsize % PF_PAGESIZE);
	try
	{
		//д��һҳ
		m_file.Seek(lseek,CFile::begin);
		m_file.Write(pc,lt);
		lw += lt;
		
		//дʣ�µ�ҳ
		while(lw < nsize)
		{
			pc += lt;
			lt = PF_PAGESIZE;
			if((nsize - lw) < PF_PAGESIZE)
				lt = nsize - lw;
			npno = m_pages[npno];
			if(npno == PF_PAGEEND) //ҳ�治��
			{
				npno = PF_AddPage(h); //���ҳ��
				if( npno == -1)
					return PFR_NOPAGE;//�޿ռ�
			}
			lseek = PF_DATAPOS + npno * PF_PAGESIZE;
			m_file.Seek(lseek,CFile::begin);
			m_file.Write(pc,lt);
			lw += lt;
		}
		
		if(loff + lw > pf->lsize)
		{
			pf->lsize = loff + lw; //����ҳ���ļ���С
			//д������
			m_file.Seek(PF_INDEXPOS + h * sizeof(PFINDEX),CFile::begin);
			m_file.Write(pf,sizeof(PFINDEX));
		}
		if(bFlush)
		{
			m_file.Flush();
			m_bNeedFlush = FALSE;
		}
		else
			m_bNeedFlush = TRUE;
	}
	catch( CFileException *ex ) 
	{
	   ex->Delete();
	   return PFR_ERR;
	}
	
	return PFR_OK;
}

/*
	��
  */
int	CPageFile::PF_Read(PF_HANDLE h,long offset,void *pbuf,long nsize,long &nreadsize)//��
{
	LPPFINDEX pf = m_indexs + h;
	int npno = PF_GetPageNo(pf,offset);
	char *pc = (char *)pbuf;
	long lr = 0,lt;
	if(npno == -1)
		return PFR_ERR;
	try
	{
		m_file.Seek(PF_DATAPOS + npno * PF_PAGESIZE + (offset % PF_PAGESIZE),CFile::begin);

		lt = PF_PAGESIZE - (offset % PF_PAGESIZE);

		if(lt > nsize)
			lt = nsize;
		if((offset + lt) > pf->lsize)
			lt = pf->lsize - offset;
		
		m_file.Read(pc,lt);
		lr += lt;
		npno = m_pages[npno];
		while((lr < pf->lsize)&&(lr < nsize) && ( npno != PF_PAGEEND))
		{
			pc += lt;
			lt = PF_PAGESIZE;
			
			if(lt > (pf->lsize - offset - lr)) 
				lt = pf->lsize - offset - lr; //�������ļ���С
			
			if(lt > (nsize - lr))
				lt = nsize - lr;		//�������ռ��С
			m_file.Seek(PF_DATAPOS + npno * PF_PAGESIZE,CFile::begin);
			m_file.Read(pc,lt);
			lr += lt;
			npno = m_pages[npno];
		}
		nreadsize = lr; 
	}
	catch( CFileException *ex ) 
	{
	   ex->Delete();
	   return PFR_ERR;
	}
	return PFR_OK;
}


PF_HANDLE	CPageFile::PF_CreateRecDir(CPfRecVal* prec)//����ҳ���ļ������������ţ�-1��ʾ����
{
	int ni = GetNextEmptyIndex(0);
	if(ni == -1)
		return -2;
	int i,npr,npi = -1;
	long initpages = prec->GetDataSize() / PF_PAGESIZE;
	if(prec->GetDataSize() % PF_PAGESIZE)
		initpages++;
	if(initpages == 0)
		initpages++; //����һ��ҳ��

	//��д����
	m_indexs[ni].ltype = prec->m_ntype;
	m_indexs[ni].ltime = prec->m_ltime;
	strcpy(m_indexs[ni].sdes,prec->m_szdes.Left(PGFDESLEN - 1));

	m_indexs[ni].lsize = 0;
	m_indexs[ni].lpageno = PF_PAGEEND;
	m_indexs[ni].snqa = prec->m_snqa;
	m_indexs[ni].wres = prec->m_wres;
	m_indexs[ni].dwres = prec->m_dwres;

	for(i=0;i<initpages;i++)
	{
		npr = npi;
		npi = GetNextEmptyPage(npi+1);
		if(npi == -1)
		{
			m_indexs[ni].ltime = 0;
			return PF_ERR_HANDLE;
		}
		if(i==0)
			m_indexs[ni].lpageno = npi;
		if(npr != -1)
			m_pages[npr] = npi;

		m_pages[npi] = PF_PAGEEND;
	}
	
	//д�����
	try
	{
		//д������
		if(ni >= m_head.lindexs)
		{
			m_head.lindexs++;
			m_file.SeekToBegin();
			m_file.Write(&m_head,sizeof(PFHEAD));
		}
		m_file.Seek(PF_INDEXPOS + ni * sizeof(PFINDEX),CFile::begin);
		m_file.Write(&(m_indexs[ni]),sizeof(PFINDEX)); //д������

		//ҳ���Ǵ�m_indexs[ni].lpageno �� npi
		if(npi >= m_indexs[ni].lpageno)
		{
			m_file.Seek(PF_PAGEMARKPOS + m_indexs[ni].lpageno * sizeof(long),CFile::begin);
			m_file.Write(&(m_pages[m_indexs[ni].lpageno]),(npi - m_indexs[ni].lpageno + 1) * sizeof(long));
		}
		return ni;
	}
	catch ( CFileException *ex ) 
	{
	   ex->Delete();
	}
	m_indexs[ni].ltime = 0;//�������
	return PF_ERR_HANDLE;
}

int			CPageFile::WriteRec(CPfRecVal* prec)
{
	if(!m_bOpen)
		return PFR_NOOPEN;//û�д�
	PF_HANDLE h = PF_CreateRecDir(prec);
	
	// ���û�пռ䣬�򷵻�-2�����������ж�ʧ����ɳ������
	//if(h == -1)
	{
		if(h == -1)
			return PFR_ERR;
		else if(h == -2)
			return PFR_NOPAGE;//�޿ռ�
	}
	
	long  lsize = prec->GetDataSize();
	void *pdata = prec->GetBuf();
	return PF_Write(h,0,pdata,lsize,TRUE);//д
	
}
int			CPageFile::GetLastRec(PF_HANDLE &h,CPfRecVal* prec)
{
	int	i;
	h = -1;
	for(i = (m_head.lindexs - 1); i>= 0; i--) //�Ż���
	//for(i = (PF_MAXINDEXS - 1); i >= 0;i--)
	{
		if(m_indexs[i].ltime != 0)
		{
			h = i;
			break;
		}
	}
	if(h == -1)
		return PFR_NOINDEX;
	int nsize = m_indexs[i].lsize;
	
	prec->ResetBufSize(nsize); //
	
	//Ŀ¼��Ϣ
	prec->m_ltime = m_indexs[i].ltime;
	prec->m_ntype = m_indexs[i].ltype;
	prec->m_snqa = m_indexs[i].snqa;
	prec->m_szdes = m_indexs[i].sdes;

	long ldatasize = 0;
	void *pdata = prec->GetBuf();
	int nbufsize = prec->GetBufSize();

	int	nret = PF_Read(h,0,pdata,nbufsize,ldatasize);//��
	if(nret == PFR_OK)
	{
		prec->m_nszie = ldatasize;
		return nret;
	}

	//PF_Delete(h); //ɾ��
	h = -1;
	return PFR_ERR;

}

int			CPageFile::GetFirstRec(PF_HANDLE &h,CPfRecVal* prec)
{
	int	i;
	h = -1;
	for(i = 0; i < m_head.lindexs; i++) //�Ż���
		//for(i = (PF_MAXINDEXS - 1); i >= 0;i--)
	{
		if(m_indexs[i].ltime != 0)
		{
			h = i;
			break;
		}
	}
	if(h == -1)
		return PFR_NOINDEX;
	int nsize = m_indexs[i].lsize;

	prec->ResetBufSize(nsize); //

	//Ŀ¼��Ϣ
	prec->m_ltime = m_indexs[i].ltime;
	prec->m_ntype = m_indexs[i].ltype;
	prec->m_snqa = m_indexs[i].snqa;
	prec->m_szdes = m_indexs[i].sdes;

	long ldatasize = 0;
	void *pdata = prec->GetBuf();
	int nbufsize = prec->GetBufSize();

	int	nret = PF_Read(h,0,pdata,nbufsize,ldatasize);//��
	if(nret == PFR_OK)
	{
		prec->m_nszie = ldatasize;
		return nret;
	}

	//PF_Delete(h); //ɾ��
	h = -1;
	return PFR_ERR;

}