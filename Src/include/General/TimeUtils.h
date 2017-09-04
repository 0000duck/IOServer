///////////////////////////////////////////////////////////////////
//   DbProxyĿ�����������
//
//   �õ�Ԫ�ṩ����ʵʱ���ϵ���ݿ��е�ʱ�������װ
//
//   ���ߣ�����
//   ���ڣ�2010-03-23
//   tech
// 
///////////////////////////////////////////////////////////////////
//   ������ʷ:
// 
//   @2010-03-23
//      �����ĵ�����ԭ��ʱ�������ش���ϲ�����
///////////////////////////////////////////////////////////////////

#ifndef __TIME_UTILS_H__
#define __TIME_UTILS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

TRUNK_NAMESPACE_BEGIN

class _GENERAL_CLASS CTimeCount   
{   
public:   
	   CTimeCount(){};   
	   ~CTimeCount(){};   
	   void	start()
	   {
		   QueryPerformanceFrequency(&litmp);   
		   dfFreq   =   (double)litmp.QuadPart;//   ��ü�������ʱ��Ƶ��   
		   QueryPerformanceCounter(&litmp);   
		   QPart1   =   litmp.QuadPart;//   ��ó�ʼֵ   
	   };     //��ʼ��ʱ   
	   void	end()
	   {
		   QueryPerformanceCounter(&litmp);   
		   QPart2   =   litmp.QuadPart;//�����ֵֹ   
		   dfMinus   =   (double)(QPart2-QPart1);   
		   dfTim   =   dfMinus   /   dfFreq;//   ��ö�Ӧ��ʱ��ֵ����λΪ�� 
	   };         //ֹͣ��ʱ   
	   
	   double time_milli(){
		   return dfTim * 1000.0;
	   };       //�Ժ��뾫���������ʱ��   
	   
private:   
	LARGE_INTEGER   litmp;     
    LONGLONG   QPart1,QPart2;   
    double   dfMinus,   dfFreq,   dfTim;     
};

class _GENERAL_CLASS CGmtTime//����ʱ��
{
public:
	CGmtTime(CTime &ct)
	{
		m_ctime = ct;
	};
	CGmtTime(){
		m_ctime = CTime::GetCurrentTime();
	};
	~CGmtTime(){};
public:
	CTime m_ctime;
public:
	int GetGmtYear()
	{
		struct tm stm;
		m_ctime.GetGmtTm(&stm);
		return stm.tm_year + 1900;
	};
	int GetGmtMon()
	{
		struct tm stm;
		m_ctime.GetGmtTm(&stm);
		return stm.tm_mon + 1;
	};
	int GetGmtDay()
	{
		struct tm stm;
		m_ctime.GetGmtTm(&stm);
		return stm.tm_mday;
	};
	
};

TRUNK_NAMESPACE_END

#endif // end of __TIME_UTILS_H__