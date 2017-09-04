#include "StdAfx.h"
#include "IniTool.h"


CIniTool::CIniTool(void)
{
	m_szFile = "";
}
CIniTool::CIniTool(LPCTSTR sFile)
{
	m_szFile = sFile;
}

CIniTool::~CIniTool(void)
{
}

BOOL CIniTool::Write()//д��
{
	FILE *f = fopen (m_szFile, "wt");
	if(f==NULL)
	{
		CString szMsg;
		szMsg.Format("<%s>����д!",m_szFile);
		AfxMessageBox(szMsg,MB_OK|MB_ICONWARNING);
		return FALSE;
	}
	//ע��
	CString sl = ";IOServer V1�����ļ�\n";
	fwrite(sl,sl.GetLength(),1,f);
	sl = ";˫���˿Ƽ�\n\n";
	fwrite(sl,sl.GetLength(),1,f);

	//////////////////////
	sl = "[local]\n";
	fwrite(sl,sl.GetLength(),1,f);

	sl.Format("ProjectName = %s	;������\n",m_szProjectName);
	fwrite(sl,sl.GetLength(),1,f);

	if(m_bAutoRun)
		sl = "AutoRun = yes		;�������Զ���������\n";
	else
		sl = "AutoRun = no		;�������Զ���������\n";
	fwrite(sl,sl.GetLength(),1,f);

	if(m_bSaveLog)
		sl = "savelog = yes		;������־\n";
	else
		sl = "savelog = no		;������־\n";
	fwrite(sl,sl.GetLength(),1,f);

	if(m_bExitPass)
		sl = "exitpassword = yes	;�˳�ʱ������֤\n";
	else
		sl = "exitpassword = no	;�˳�ʱ������֤\n";
	fwrite(sl,sl.GetLength(),1,f);

	if(m_bCashe)
		sl = "cashe = yes		;ʹ�û���\n";
	else
		sl = "cashe = no		;ʹ�û���\n";
	fwrite(sl,sl.GetLength(),1,f);

	if(m_bSkipSameTime)
		sl = "skipsametime = yes	;������ʱ����ڵ�ǰʱ��ֵ�Ŀ���\n";
	else
		sl = "skipsametime = no		;������ʱ����ڵ�ǰʱ��ֵ�Ŀ���\n";
	fwrite(sl,sl.GetLength(),1,f);

	sl.Format("m_nTimeZero = %d		;��ʾʱ������Χ-12��12,Ĭ��Ϊ��8��\n",m_nTimeZero);
	fwrite(sl,sl.GetLength(),1,f);


	sl.Format("upthread = %d		;��Χ1��10\n",m_nupthreads);
	fwrite(sl,sl.GetLength(),1,f);

	sl.Format("loglevel = %d		;��־����0-����1-���桢2-��ʾ��3-����\n",m_nLogLevel);
	fwrite(sl,sl.GetLength(),1,f);

	sl.Format("title = %s		;DDClient������ʾ���⣬������DDClient�Ĺ���\n",m_szTitle);
	fwrite(sl,sl.GetLength(),1,f);//--

	if(m_bWaitTcpAck)
		sl = "wait_tcp_ack = yes	;TCP�Ƿ�����Ӧ����������բ�ǲ���������Ӧ����\n";
	else
		sl = "wait_tcp_ack = no	;TCP�Ƿ�����Ӧ����������բ�ǲ���������Ӧ����\n";
	fwrite(sl,sl.GetLength(),1,f);

	sl.Format("udp_send_delay = %d	;UDP������ʱʱ�䣬������Ϊ0-50����\n\n", m_nUdpSendDelay);
	fwrite(sl,sl.GetLength(),1,f);

	//////////////////////////////
	sl = "[heart]			;˫ǰ����������\n";
	fwrite(sl,sl.GetLength(),1,f);

	sl.Format("heartport = %d	;�����˿�\n",m_wheartport);
	fwrite(sl,sl.GetLength(),1,f);

	m_szHeartIP.TrimLeft();
	m_szHeartIP.TrimRight();
	if(m_szHeartIP.IsEmpty())
		sl = "HeartIP = NULL		;NULL��ʾ��������������дIP��ʾ�����Ǳ��÷���\n";
	else
		sl.Format("HeartIP = %s		;NULL��ʾ����������������дIP��ʾ�����Ǳ��÷���\n",m_szHeartIP);
	fwrite(sl,sl.GetLength(),1,f);
	//
	fclose(f);
	return TRUE;
}
