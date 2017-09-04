#ifndef __KIMI_KERNEL_IOCP_THREAD_POOL_H__
#define __KIMI_KERNEL_IOCP_THREAD_POOL_H__

// ʹ��IOCPʵ�ֵ��̳߳أ�Ч��Զ������ͨ���̳߳�

_KIMI_NAMESPACE_BEGIN

#define SHUTDOWN_FLAG	((DWORD_PTR)-1)

class CIOCPThreadPool;
class CProcessorThread;

/// �̳߳�������
class KIMI_CORE_CLASS CThreadRequest
{
public:
	CThreadRequest();
	virtual ~CThreadRequest();

	CIOCPThreadPool* GetThreadPool() {
		return m_pThreadPool;
	}
	void SetThreadPool(CIOCPThreadPool* pThreadPool) {
		m_pThreadPool = pThreadPool;
	}

	CProcessorThread* GetThread() {
		return m_pThread;
	}
	void SetThread(CProcessorThread* pThread) {
		m_pThread = pThread;
	}
	CThreadRequest* GetNext() {
		return m_pNext;
	}
	void SetNext(CThreadRequest* pThreadRequest) {
		m_pNext = pThreadRequest;
	}
	CThreadRequest* GetPrev() {
		return m_pPrev;
	}
	void SetPrev(CThreadRequest* pThreadRequest) {
		m_pPrev = pThreadRequest;
	}

public:
	// �̳߳ع����������̳����������д�Լ����̴߳���
	virtual void Execute() = 0;

protected:
private:
	CIOCPThreadPool*	m_pThreadPool;
	CProcessorThread*	m_pThread;
	CThreadRequest*		m_pNext;
	CThreadRequest*		m_pPrev;
};

/// �̳߳صĹ����߳�
class CProcessorThread : public CThread
{
public:
	CProcessorThread(CIOCPThreadPool* pThreadPool);
	virtual ~CProcessorThread();

	CIOCPThreadPool* GetThreadPool() {
		return m_pThreadPool;
	}
	void SetThreadPool(CIOCPThreadPool* pThreadPool) {
		m_pThreadPool = pThreadPool;
	}

public:
	/// �߳�����֮ǰ��ʼ���¼�
	BOOL OnInitThread(CThread *pThread);
	/// �߳��˳��¼�
	void OnShutdownThread(CThread *pThread);
	/// �߳����к��������OnRunThread����FALSE �߳̽��˳�
	BOOL OnRunThread(CThread *pThread);

protected:
private:
	CIOCPThreadPool*	m_pThreadPool;
};

/// ����IOCP���̳߳���
class KIMI_CORE_CLASS CIOCPThreadPool
{
public:
	friend CProcessorThread;

public:
	CIOCPThreadPool(int nNumberOfThreads = 0);
	virtual ~CIOCPThreadPool();

	HANDLE GetHandle() {
		return m_hHandle;
	}
	BOOL IsShutdowned() {
		return m_bShutdowned;
	}
	long GetPendingRequest() {
		return m_nPendingRequest;
	}
	int GetMaxFreeRequest() {
		return m_nMaxFreeRequest;
	}
	void SetMaxFreeRequest(int nMaxFreeRequest) {
		m_nMaxFreeRequest = nMaxFreeRequest;
	}
	int GetFreeRequest() {
		return m_nFreeRequest;
	}

	CThreadRequest* AllocThreadRequest();

	virtual BOOL AddRequest(CThreadRequest* pThreadRequest);
	BOOL Startup();
	void Shutdown();

protected:
	void RemoveRequest(CThreadRequest* pThreadRequest);
	void ReleaseThreadRequest(CThreadRequest* pThreadRequest);

	// ����CThreadRequest�ĺ���
	virtual CThreadRequest* CreateThreadRequest() = 0;

	// �߳������¼�
	virtual void OnThreadStart(CProcessorThread* pThread) = 0;

	// �߳̽����¼�
	virtual void OnThreadExit(CProcessorThread* pThread) = 0;
private:
	HANDLE					m_hHandle;
	int						m_nNumberOfThreads;
	int						m_nNumberOfWorkThreads;
	CProcessorThread**		m_pWorkThreads;
	HANDLE*					m_pThreadHandles;
	CThreadRequest*			m_pRequestHead;
	CThreadRequest*			m_pRequestTail;
	volatile long			m_nPendingRequest;
	BOOL					m_bShutdowned;
	CRITICAL_SECTION		m_csLock;

private:
	int						m_nMaxFreeRequest;
	int						m_nFreeRequest;
	CThreadRequest*			m_pFreeRequest;
	CRITICAL_SECTION		m_csFreeRequest;
};

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_IOCP_THREAD_POOL_H__