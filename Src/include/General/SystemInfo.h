// SystemInfo.h: interface for the CSystemInfo class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __SYSTEMINFO_H__
#define __SYSTEMINFO_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

TRUNK_NAMESPACE_BEGIN

// GetProcessMemoryInfo ������ҪWindows NT 4.0����ϵͳ֧�֣�
// �ú����� SDK �� Psapi.h ������������ʱ��Ҫ����Psapi.lib
// ����������Ϊ���ڲ���Ҫ SDK ����±���
#ifndef PROCESS_MEMORY_COUNTERS

#define NO_SDK_SUPPORT	1

typedef struct _PROCESS_MEMORY_COUNTERS {
    DWORD cb;
    DWORD PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
} PROCESS_MEMORY_COUNTERS;
typedef PROCESS_MEMORY_COUNTERS *PPROCESS_MEMORY_COUNTERS;

typedef BOOL (WINAPI *PGetProcessMemoryInfo)(
  HANDLE Process,                          // handle to process
  PPROCESS_MEMORY_COUNTERS ppsmemCounters, // buffer
  DWORD cb                                 // size of buffer
);
#else

#include <psapi.h>
#pragma comment (lib,"psapi.lib")

#endif

//#include <winternl.h>
//#include <ntstatus.h>

#define NT_SUCCESS(Status) (((long)(Status)) >= 0)
#define STATUS_INFO_LENGTH_MISMATCH      ((long)0xC0000004L)

typedef struct _THREAD_INFO
{
	LARGE_INTEGER CreateTime;
	DWORD dwUnknown1;
	DWORD dwStartAddress;
	DWORD StartEIP;
	DWORD dwOwnerPID;
	DWORD dwThreadId;
	DWORD dwCurrentPriority;
	DWORD dwBasePriority;
	DWORD dwContextSwitches;
	DWORD Unknown;
	DWORD WaitReason;
}THREADINFO, *PTHREADINFO;
typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaxLength;
	PWSTR Buffer;
} UNICODE_STRING;
typedef struct _PROCESS_INFO
{
	DWORD dwOffset;
	DWORD dwThreadsCount;
	DWORD dwUnused1[6];
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ProcessName;
	DWORD dwBasePriority;
	DWORD dwProcessID;
	DWORD dwParentProcessId;
	DWORD dwHandleCount;
	DWORD dwUnused3[2];
	DWORD dwVirtualBytesPeak;
	DWORD dwVirtualBytes;
	DWORD dwPageFaults;
	DWORD dwWorkingSetPeak;
	DWORD dwWorkingSet;
	DWORD dwQuotaPeakPagedPoolUsage;
	DWORD dwQuotaPagedPoolUsage;
	DWORD dwQuotaPeakNonPagedPoolUsage;
	DWORD dwQuotaNonPagedPoolUsage;
	DWORD dwPageFileUsage;
	DWORD dwPageFileUsagePeak;
	DWORD dCommitCharge;
	THREADINFO ThreadSysInfo[1];
} PROCESSINFO, *PPROCESSINFO;

typedef struct _SYSTEM_PERFORMANCE_INFORMATION
{
	LARGE_INTEGER	IdleTime;								// CPU����ʱ��;
	LARGE_INTEGER	ReadTransferCount;						// I/O��������Ŀ;
	LARGE_INTEGER	WriteTransferCount;						// I/Oд������Ŀ;
	LARGE_INTEGER	OtherTransferCount;						// I/O����������Ŀ;
	DWORD			ReadOperationCount;						// I/O��������Ŀ;
	DWORD			WriteOperationCount;					// I/Oд������Ŀ;
	DWORD			OtherOperationCount;					// I/O��������������Ŀ;
	DWORD			AvailablePages;							// �ɻ�õ�ҳ��Ŀ;
	DWORD			TotalCommittedPages;					// �ܹ��ύҳ��Ŀ;
	DWORD			TotalCommitLimit;						// ���ύҳ��Ŀ;
	DWORD			PeakCommitment;							// ҳ�ύ��ֵ;
	DWORD			PageFaults;								// ҳ������Ŀ;
	DWORD			WriteCopyFaults;						// Copy-On-Write������Ŀ;
	DWORD			TransitionFaults;						// ��ҳ������Ŀ;
	DWORD			Reserved1;
	DWORD			DemandZeroFaults;						// ����0������;
	DWORD			PagesRead;								// ��ҳ��Ŀ;
	DWORD			PageReadIos;							// ��ҳI/O������;
	DWORD			Reserved2[2];
	DWORD			PagefilePagesWritten;					// ��дҳ�ļ�ҳ��;
	DWORD			PagefilePageWriteIos;					// ��дҳ�ļ�������;
	DWORD			MappedFilePagesWritten;					// ��дӳ���ļ�ҳ��;
	DWORD			MappedFileWriteIos;						// ��дӳ���ļ�������;
	DWORD			PagedPoolUsage;							// ��ҳ��ʹ��;
	DWORD			NonPagedPoolUsage;						// �Ƿ�ҳ��ʹ��;
	DWORD			PagedPoolAllocs;						// ��ҳ�ط������;
	DWORD			PagedPoolFrees;							// ��ҳ���ͷ����;
	DWORD			NonPagedPoolAllocs;						// �Ƿ�ҳ�ط������;
	DWORD			NonPagedPoolFress;						// �Ƿ�ҳ���ͷ����;
	DWORD			TotalFreeSystemPtes;					// ϵͳҳ�����ͷ�����;
	DWORD			SystemCodePage;							// ����ϵͳ����ҳ��;
	DWORD			TotalSystemDriverPages;					// �ɷ�ҳ��������ҳ��;
	DWORD			TotalSystemCodePages;					// ����ϵͳ����ҳ����;
	DWORD			SmallNonPagedLookasideListAllocateHits;	// С�Ƿ�ҳ�����б�������;
	DWORD			SmallPagedLookasideListAllocateHits;	// С��ҳ�����б�������;
	DWORD			Reserved3;
	DWORD			MmSystemCachePage;						// ϵͳ����ҳ��;
	DWORD			PagedPoolPage;							// ��ҳ��ҳ��;
	DWORD			SystemDriverPage;						// �ɷ�ҳ����ҳ��;
	DWORD			FastReadNoWait;							// �첽���ٶ���Ŀ;
	DWORD			FastReadWait;							// ͬ�����ٶ���Ŀ;
	DWORD			FastReadResourceMiss;					// ���ٶ���Դ��ͻ��;
	DWORD			FastReadNotPossible;					// ���ٶ�ʧ����;
	DWORD			FastMdlReadNoWait;						// �첽MDL���ٶ���Ŀ;
	DWORD			FastMdlReadWait;						// ͬ��MDL���ٶ���Ŀ;
	DWORD			FastMdlReadResourceMiss;				// MDL����Դ��ͻ��;
	DWORD			FastMdlReadNotPossible;					// MDL��ʧ����;
	DWORD			MapDataNoWait;							// �첽ӳ�����ݴ���;
	DWORD			MapDataWait;							// ͬ��ӳ�����ݴ���;
	DWORD			MapDataNoWaitMiss;						// �첽ӳ�����ݳ�ͻ����;
	DWORD			MapDataWaitMiss;						// ͬ��ӳ�����ݳ�ͻ����;
	DWORD			PinMappedDataCount;						// ǣ��ӳ��������Ŀ;
	DWORD			PinReadNoWait;							// ǣ���첽����Ŀ;
	DWORD			PinReadWait;							// ǣ��ͬ������Ŀ;
	DWORD			PinReadNoWaitMiss;						// ǣ���첽����ͻ��Ŀ;
	DWORD			PinReadWaitMiss;						// ǣ��ͬ������ͻ��Ŀ;
	DWORD			CopyReadNoWait;							// �첽����������;
	DWORD			CopyReadWait;							// ͬ������������;
	DWORD			CopyReadNoWaitMiss;						// �첽���������ϴ���;
	DWORD			CopyReadWaitMiss;						// ͬ�����������ϴ���;
	DWORD			MdlReadNoWait;							// �첽MDL������;
	DWORD			MdlReadWait;							// ͬ��MDL������;
	DWORD			MdlReadNoWaitMiss;						// �첽MDL�����ϴ���;
	DWORD			MdlReadWaitMiss;						// ͬ��MDL�����ϴ���;
	DWORD			ReadAheadIos;							// ��ǰ��������Ŀ;
	DWORD			LazyWriteIos;							// LAZYд������Ŀ;
	DWORD			LazyWritePages;							// LAZYдҳ�ļ���Ŀ;
	DWORD			DataFlushes;							// ����ˢ�´���;
	DWORD			DataPages;								// ����ˢ��ҳ��;
	DWORD			ContextSwitches;						// �����л���Ŀ;
	DWORD			FirstLevelTbFills;						// ��һ�㻺����������;
	DWORD			SecondLevelTbFills;						// �ڶ��㻺����������;
	DWORD			SystemCall;								// ϵͳ���ô���;
}SYSTEM_PERFORMANCE_INFORMATION,*PSYSTEM_PERFORMANCE_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemBasicInformation = 0,
	SystemPerformanceInformation = 2,
	SystemTimeOfDayInformation = 3,
	SystemProcessInformation = 5,
	SystemProcessorPerformanceInformation = 8,
	SystemInterruptInformation = 23,
	SystemExceptionInformation = 33,
	SystemRegistryQuotaInformation = 37,
	SystemLookasideInformation = 45
} SYSTEM_INFORMATION_CLASS;

typedef long ( __stdcall *PNtQuerySystemInformation )(IN     SYSTEM_INFORMATION_CLASS,
													  IN OUT PVOID,
													  IN     ULONG,
													  OUT    PULONG OPTIONAL);

class _GENERAL_CLASS CSystemInfo  
{
public:
	CSystemInfo();
	virtual ~CSystemInfo();

	void UpdateMemoryCounter();
	BOOL UpdateProcessInfo(DWORD dwPID);

public:
	PROCESS_MEMORY_COUNTERS		m_pmc;
	PROCESSINFO					m_processinfo;

	CString &GetHostName() {
		return m_szHostName;
	}
	CStringArray &GetHostIpAddress() {
		return m_arrHostIpAddress;
	}
	double GetCPUUsage() { return m_dblCPUUsage; }

	static CString CommaFormat(double dblVal, int nDecimal = -1); // nDecimal==-1��ʾ������С����
	static CString CommaFormat(long lVal);
	static CString CommaFormat(DWORD dwVal);
	static CString CommaFormat(LPCTSTR lpszStr);
	static CString FormatMemory(DWORD dwMemory);
	static CString FormatMemoryKB(DWORD dwMemory);

protected:
	double	m_dblCPUUsage;
	void GetHostIpAndName();

private:
#ifdef NO_SDK_SUPPORT
	HINSTANCE	m_hPsapiLibHandle;
	PGetProcessMemoryInfo	GetProcessMemoryInfo;
	PNtQuerySystemInformation NtQuerySystemInformation;
#endif
	CString			m_szHostName;
	CStringArray	m_arrHostIpAddress;
};

TRUNK_NAMESPACE_END

#endif // __SYSTEMINFO_H__
