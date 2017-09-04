///////////////////////////////////////////////////////////////////
//
//   �õ�Ԫ�ṩ�߾���ʱ��������࣬���ھ�ȷ��������ִ��ʱ��
//
//   ���ߣ�����
//   ���ڣ�2010-03-23
//   �����ȹ��о�Ժ���޹�˾
// 
///////////////////////////////////////////////////////////////////
//   ������ʷ:
// 
//   @2010-03-23
//      �����ĵ�����ԭ��ʱ�������ش���ϲ�����
///////////////////////////////////////////////////////////////////

#ifndef __KIMI_KERNEL_TIME_COUNTER_H__
#define __KIMI_KERNEL_TIME_COUNTER_H__

class CTimeCounter   
{   
public:   
	CTimeCounter(BOOL bAutoStart = FALSE) 
		: m_llStartCounter(0)
	{
		LARGE_INTEGER liTmp;

		// ��ü�������ʱ��Ƶ�� 
		::QueryPerformanceFrequency(&liTmp);
		m_llFrequency = liTmp.QuadPart;

		if (bAutoStart)
			Start();
	};   
	~CTimeCounter() {}

	// ��ʼ��ʱ
	void Start()
	{
		LARGE_INTEGER liTmp;

		::QueryPerformanceCounter(&liTmp);   
		m_llStartCounter = liTmp.QuadPart; 
	}

	// ������ʱ���������Ժ���Ϊ��λ�ĺ�ʱֵ
	double Stop()
	{
		LARGE_INTEGER liTmp;
		LONGLONG llStopCounter;

		::QueryPerformanceCounter(&liTmp);   
		llStopCounter = liTmp.QuadPart;

		m_dblMillisecond = (double)(llStopCounter - m_llStartCounter) / m_llFrequency * 1000.0;

		return m_dblMillisecond;
	}

	double GetMillisecond(){
		return m_dblMillisecond;
	}
	double GetSecond() {
		return (m_dblMillisecond / 1000.0);
	}

private:
	LONGLONG m_llFrequency;		// ��������ʱ��Ƶ��
	LONGLONG m_llStartCounter;	// ��ʼʱ�Ӽ�����ֵ

	double m_dblMillisecond;
};

#endif // end of __KIMI_KERNEL_TIME_COUNTER_H__