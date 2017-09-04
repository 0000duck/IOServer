//pagefile.h ҳ���ļ�
#pragma once
/*
	ҳ���ļ�,string blob soe ����ר�ð�
  */
//����ֵ����
#define PFR_OK				0	//�ɹ�	
#define PFR_ERR				-1	//ʧ��
#define PFR_NOOPEN			-2	//ҳ���ļ�û�д�
#define PFR_NOPAGE			-3	//ҳ����
#define PFR_NOINDEX			-4	//û��ID
#define PFR_INDEXFULL		-5	//������
#define PFR_MEMERR			-6	//�ڴ����
#define	PFR_NOTPF			-7	//����tonmaҳ���ļ�
#define PFR_INDEXERR		-8	//��������
#define PFR_PAGEMARKERR		-9	//ҳ���Ǵ���
#define PFR_FILEEXIST		-10	//�ļ�����
#define PFR_CREATEINDEXERR  -11	//������������
#define PFR_MARKERR			-12	//ҳ���Ǵ���

//�̶��������� ҳ��ߴ�1K�汾
// Ŀ¼���ռ�8M-1024��ҳ���ǿռ�8M
// ���ݿռ䣬2000M
// ���ҳ���ļ��ռ� 2000M + 16M  = 2016M  < 2048M,���ᳬ���ļ�ϵͳ��С
#define PF_PAGESIZE			1024	//ҳ���С
#define PF_MAXINDEXS		65528	//�����������,Ŀ¼��,ռ�ÿռ�8M - 1024
#define PF_MAXPAGES			2048000	//������,ÿҳ��1K,���ɱ��2000M,ռ�ñ�ǿռ�8192000�ֽ�
#define PF_INDEXPOS			1024	//����λ��
#define PF_PAGEMARKPOS		8388608	//ҳ���λ��   8M(8 * 1024 * 1024)
#define PF_DATAPOS			16777216//�ļ�����λ�� 16M(16 * 1024 * 1024) + 

#define PF_PAGEEND			(-1)	//�ļ��������־


//ע��ҳ����ÿ�����ռ4�ֽڣ���

//��д����
typedef struct			//���ļ�ͷ
{
	DWORD	dwfalg;		//�ļ���ʶ  0x81018202
	long	lpages;		//��ǰҳ����
	long	lindexs;	//��ǰ�ļ�����
	long	lgrownsize;	//�ļ�ÿ�����Ӵ�С��Ĭ��16M
	char	cres[1024 - 16];
}PFHEAD,*LPPFHEAD;      //sizeof() = 1024

#define PGFDESLEN	104  //ҳ���ļ�Ŀ¼�����ռ��С
typedef struct		//�����ṹ,Ҳ��Ŀ¼�ṹ
{
	long	ltime;		//����ʱ��,=0��ʾ��Ŀ¼δ�ã����д��¼ʱ��ֵһ������Ϊ0
	long	lpageno;	//��ʼҳ���
	long	lsize;		//�ļ���С
	long	ltype;		//���� 0 string ,1 blob,2 soe 
	short	snqa;		//����
	WORD	wres;		//����
	DWORD	dwres;		//����
	char	sdes[104];	//�ļ���������,���ø�ʽ����ʽ

}PFINDEX,*LPPFINDEX; //sizeof() = 128

#define PFILE_VOID		0
#define PFILE_STRING	1
#define PFILE_BLOB		2
#define PFILE_SOE		3 
class CPageFile;
class CPfRecVal //��¼ֵ
{
public:
	CPfRecVal();
	~CPfRecVal();
	CString m_szdes;//����
	long	m_ltime;//����ʱ��
	long	m_ntype; //����,PFILE_STRING��
	short	m_snqa;  //����
	WORD	m_wres;		//����
	DWORD	m_dwres;		//����
protected:
	long	m_nszie; //�����ֽ���
	long	m_lbufsize;
	void*	m_pdata;
	friend class CPageFile;
public:
	long	GetDataSize(){return m_nszie;};
	long	GetBufSize(){return m_lbufsize;};
	void	ResetBufSize(long lsize); //���ô�С
	void*	GetBuf(){
		return m_pdata;
	};

	BOOL	SetData(void* pdata,long lsize);//��������
};

typedef int		PF_HANDLE;		//ҳ���ļ����

#define PF_ERR_HANDLE	(-1);	//��Ч���

class CPageFile //ҳ���ļ���
{
public:
	CPageFile();
	~CPageFile();
protected:
	PFHEAD		m_head;		//�ļ�ͷ
	LPPFINDEX	m_indexs;	//����
	long*		m_pages;	//ҳ����

	CFile		m_file;		//���ļ�
	CString		m_szFile;	//�ļ���

	BOOL		m_bOpen;	//�Ƿ��
	long		m_lgrownsize;	//�����ߴ�

	BOOL		m_bNeedFlush;
protected:
	void		Clear();	//��������Ϳ���
	long		GetEmptyPages();//ȡ���е�ҳ����
	long		GetOneEmptyIndex();//ȡһ����������λ��
	
	int			GetNextEmptyIndex(int nstart);//ȡ��һ�����õ�������
	int			GetNextEmptyPage(int nstartpos);

	//{{��ҳ���ļ���������
	int			PF_GetPages(LPPFINDEX pf);//ȡһ��ҳ���ļ���ռҳ����
	int			PF_AddPage(PF_HANDLE h);//���һ��ҳ��,�����¼ӵ�ҳ���
	
	int			PF_GetPageNo(LPPFINDEX pf,long offset);//ȡ��ҳ���ļ�������λ��Ϊoffset��ҳ���,����-1��ʾ����
	int			PF_Write(PF_HANDLE h,long offset,const void *pbuf,long nsize,BOOL bFlush = TRUE);//д
	int			PF_Read(PF_HANDLE h,long offset,void *pbuf,long nsize,long &nreadsize);//��
	int			PF_Delete(PF_HANDLE h);//ɾ��
	//}}

	//{{debug
	void		TraceIndex();
	void		TracePageMark();
	//}}

	//������¼���ļ�
	PF_HANDLE	PF_CreateRecDir(CPfRecVal* prec);//����ҳ���ļ������������ţ�-1��ʾ����
public:
	BOOL		IsOpen(){return m_bOpen;};
	void		SetGrownSize(long lmsize) //�����ļ�������С����λM
	{
		long lt = lmsize;
		if(lt < 1)
			lt = 1;
		if(lt > 500 )
			lt = 500;
		m_lgrownsize = lt * 1024 * 1024;

	};//�ļ�������С
	int			Open(LPCTSTR lpszFile);		//��,û���򴴽�
	int			Close();					//�ر�
	int			Create(LPCTSTR lpszFile);	//����
	void		Flush();

public: //������
	int			WriteRec(CPfRecVal* prec);
	int			GetLastRec(PF_HANDLE &h,CPfRecVal* prec); //h���ڶ����ͳɹ���ɾ����¼��
	int			GetFirstRec(PF_HANDLE &h,CPfRecVal* prec); //h���ڶ����ͳɹ���ɾ����¼��
	int			DeletePf(PF_HANDLE h){return PF_Delete(h);};//ɾ����¼
	void		EmptyFile();//��գ���ȡ������ļ�β
	void		CloseAndDelFile();//�رպ�ɾ���ļ�
};