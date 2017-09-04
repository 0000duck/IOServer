#include "stdafx.h"
#include "KimiCore.h"
#include "Thread\IOCPThreadPool.h"


_KIMI_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////
/// CThreadRequest - �̳߳�������
//////////////////////////////////////////////////////////////////////////

CThreadRequest::CThreadRequest()
	: m_pThreadPool(NULL)
	, m_pThread(NULL)
	, m_pNext(NULL)
	, m_pPrev(NULL)
{
}

CThreadRequest::~CThreadRequest()
{

}

//////////////////////////////////////////////////////////////////////////
/// CProcessorThread - �̳߳صĹ����߳�
//////////////////////////////////////////////////////////////////////////

CProcessorThread::CProcessorThread(CIOCPThreadPool* pThreadPool)
	: m_pThreadPool(pThreadPool)
{
}

CProcessorThread::~CProcessorThread()
{

}

BOOL CProcessorThread::OnInitThread(CThread *pThread)
{
	m_pThreadPool->OnThreadStart(this);
	return TRUE;
}

void CProcessorThread::OnShutdownThread(CThread *pThread)
{
	m_pThreadPool->OnThreadExit(this);
}

BOOL CProcessorThread::OnRunThread(CThread *pThread)
{
	DWORD dwBytes = 0;
	ULONG_PTR dwCompKey = 0;
	CThreadRequest* pThreadRequest = NULL;

	if (!::GetQueuedCompletionStatus(m_pThreadPool->GetHandle(), &dwBytes, &dwCompKey, (LPOVERLAPPED*)&pThreadRequest, INFINITE))
		return FALSE;

	// ������Ч�����󣬺���
	if (dwCompKey != (ULONG_PTR)m_pThreadPool)
		return TRUE;

	// �յ��߳��˳���־������ѭ��
	if ((ULONG_PTR)pThreadRequest == SHUTDOWN_FLAG)
		return FALSE;

	if (pThreadRequest != NULL)
	{
		pThreadRequest->SetThreadPool(m_pThreadPool);
		pThreadRequest->SetThread(this);
		pThreadRequest->Execute();

		m_pThreadPool->RemoveRequest(pThreadRequest);
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
/// CIOCPThreadPool - ����IOCP���̳߳���
//////////////////////////////////////////////////////////////////////////

CIOCPThreadPool::CIOCPThreadPool(int nNumberOfThreads/* = 0*/)
	: m_nNumberOfThreads(nNumberOfThreads)
	, m_nNumberOfWorkThreads(0)
	, m_hHandle(0)
	, m_nPendingRequest(0)
	, m_bShutdowned(TRUE)
	, m_pWorkThreads(NULL)
	, m_pThreadHandles(NULL)
	, m_pRequestHead(NULL)
	, m_pRequestTail(NULL)
	, m_nMaxFreeRequest(0)
	, m_nFreeRequest(0)
	, m_pFreeRequest(NULL)
{
	//::InitializeCriticalSection(&m_crit);
	::InitializeCriticalSectionAndSpinCount(&m_csLock, 0x80000400);
	::InitializeCriticalSectionAndSpinCount(&m_csFreeRequest, 0x80000400);
}

CIOCPThreadPool::~CIOCPThreadPool()
{
	Shutdown();
	::DeleteCriticalSection(&m_csLock);
	::DeleteCriticalSection(&m_csFreeRequest);
}

CThreadRequest* CIOCPThreadPool::AllocThreadRequest()
{
	CThreadRequest* pThreadRequest = NULL;

	::EnterCriticalSection(&m_csFreeRequest);

	if (m_pFreeRequest)
	{
		pThreadRequest = m_pFreeRequest;
		m_pFreeRequest = m_pFreeRequest->GetNext();
		m_nFreeRequest--;
	}
	else {
		pThreadRequest = CreateThreadRequest();
	}

	::LeaveCriticalSection(&m_csFreeRequest);

	pThreadRequest->SetNext(NULL);
	pThreadRequest->SetPrev(NULL);

	return pThreadRequest;
}

void CIOCPThreadPool::ReleaseThreadRequest(CThreadRequest* pThreadRequest)
{
	::EnterCriticalSection(&m_csFreeRequest);

	if (m_nFreeRequest >= m_nMaxFreeRequest)
	{
		delete pThreadRequest;
	}
	else {
		pThreadRequest->SetThreadPool(NULL);
		pThreadRequest->SetThread(NULL);
		pThreadRequest->SetNext( m_pFreeRequest );
		m_pFreeRequest = pThreadRequest;
		m_nFreeRequest++;
	}

	::LeaveCriticalSection(&m_csFreeRequest);
}

void CIOCPThreadPool::RemoveRequest(CThreadRequest* pThreadRequest)
{
	// �������������ʹ��˫�������ɼ���ɾ�������ʱ�仨��
	::EnterCriticalSection(&m_csLock);

	CThreadRequest* pNext = pThreadRequest->GetNext();
	CThreadRequest* pPrev = pThreadRequest->GetPrev();

	if (pNext)
	{
		if (pPrev)
		{
			// �м��ĳ���ڵ�
			pPrev->SetNext(pNext);
			pNext->SetPrev(pPrev);
		}
		else {
			// ���׸��ڵ�
			pNext->SetPrev(NULL);
			m_pRequestHead = pNext;
		}
	}
	else {
		if (pPrev)
		{
			// �����һ���ڵ�
			pPrev->SetNext(NULL);
			m_pRequestTail = pPrev;
		}
		else {
			// �ǵ�ǰ˫�������Ψһ�ڵ�
			m_pRequestHead = NULL;
			m_pRequestTail = NULL;
		}
	}

	::LeaveCriticalSection(&m_csLock);

	InterlockedDecrement(&m_nPendingRequest);

	// �����������ŵ������б������
	ReleaseThreadRequest(pThreadRequest);
}

BOOL CIOCPThreadPool::AddRequest(CThreadRequest* pThreadRequest)
{
	if (m_hHandle == 0)
		return FALSE;

	::EnterCriticalSection(&m_csLock);
	if (m_pRequestHead)
	{
		pThreadRequest->SetNext(m_pRequestHead);
		pThreadRequest->SetPrev(NULL);
		m_pRequestHead->SetPrev(pThreadRequest);

		m_pRequestHead = pThreadRequest;
	}
	else {
		pThreadRequest->SetNext(NULL);
		pThreadRequest->SetPrev(NULL);
		m_pRequestHead = pThreadRequest;
		m_pRequestTail = pThreadRequest;
	}

	::LeaveCriticalSection(&m_csLock);


	BOOL bResult = ::PostQueuedCompletionStatus(m_hHandle, 0, (ULONG_PTR)this, (LPOVERLAPPED)pThreadRequest);
	if (bResult)		
		InterlockedIncrement(&m_nPendingRequest);

	return bResult;
}

BOOL CIOCPThreadPool::Startup()
{
	if (m_hHandle != 0)
		return FALSE;

	int nNumberOfWorkThreads = 0;
	if (m_nNumberOfThreads == 0)
	{
		SYSTEM_INFO sysinfo;
		::GetSystemInfo(&sysinfo);

		nNumberOfWorkThreads = sysinfo.dwNumberOfProcessors * 2;
	}
	else {
		nNumberOfWorkThreads = min(m_nNumberOfThreads, 64); // WaitForMultipleObjects()����ͬʱ�ȴ�64������
	}

	// ������ɶ˿�
	// NumberOfConcurrentThreads = 0 ��ʾÿ��CPU����һ�������߳�
	m_hHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	if (m_hHandle == INVALID_HANDLE_VALUE)
	{
		// IocpThreadPool����IOCP����ʧ��
		return FALSE;
	}

	m_pWorkThreads = new CProcessorThread*[nNumberOfWorkThreads];
	memset(m_pWorkThreads, 0, sizeof(CProcessorThread*) * nNumberOfWorkThreads);

	m_pThreadHandles = new HANDLE[nNumberOfWorkThreads];
	memset(m_pThreadHandles, 0, sizeof(HANDLE*) * nNumberOfWorkThreads);

	m_nNumberOfWorkThreads = 0;
	// �������й����߳�
	for (int i = 0; i < nNumberOfWorkThreads; i++)
	{
		CProcessorThread *pThread = new CProcessorThread(this);
		if (pThread)
		{
			m_pWorkThreads[m_nNumberOfWorkThreads] = pThread;
			m_pThreadHandles[m_nNumberOfWorkThreads] = pThread->GetHandle();
			m_nNumberOfWorkThreads++;

			pThread->Start();
		}
	}

	m_bShutdowned = FALSE;

	return TRUE;
}

void CIOCPThreadPool::Shutdown()
{
	if (m_hHandle == 0)
		return;

	m_bShutdowned = TRUE;

	// �����й����̷߳����˳�����
	for (int i = 0; i < m_nNumberOfWorkThreads; i++)
	{
		::PostQueuedCompletionStatus(m_hHandle, 0, (ULONG_PTR)this, (LPOVERLAPPED)SHUTDOWN_FLAG);
		::SleepEx(10, TRUE);
	}

	// �ȴ������߳̽���
	::WaitForMultipleObjects(m_nNumberOfWorkThreads, m_pThreadHandles, TRUE, INFINITE);

	// �ͷ��̶߳���
	for (int i = 0; i < m_nNumberOfWorkThreads; i++)
	{
		delete m_pWorkThreads[i];
	}
	delete[] m_pWorkThreads;
	m_pWorkThreads = NULL;

	delete[] m_pThreadHandles;
	m_pThreadHandles = NULL;

	// �ر���ɶ˿ھ��
	::CloseHandle(m_hHandle);
	m_hHandle = 0;

	// �ͷ��̳߳ص�����
	::EnterCriticalSection(&m_csLock);
	CThreadRequest* pThreadRequest = m_pRequestHead;
	CThreadRequest* pNext = NULL;
	while(pThreadRequest)
	{
		pNext = pThreadRequest->GetNext();
		delete pThreadRequest;
		pThreadRequest = pNext;
	}
	m_pRequestHead = NULL;
	m_pRequestTail = NULL;
	::LeaveCriticalSection(&m_csLock);

	::EnterCriticalSection(&m_csFreeRequest);
	pThreadRequest = m_pFreeRequest;
	while(pThreadRequest)
	{
		pNext = pThreadRequest->GetNext();
		delete pThreadRequest;
		pThreadRequest = pNext;
	}
	m_pFreeRequest = NULL;
	m_nFreeRequest = 0;
	::LeaveCriticalSection(&m_csFreeRequest);
}

_KIMI_NAMESPACE_END