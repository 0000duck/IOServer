#ifndef __THREAD_H__
#define __THREAD_H__

TRUNK_NAMESPACE_BEGIN

class CThread;

/// �߳�������
class _GENERAL_CLASS IThreadWorker
{
public:
	friend CThread;

protected:
	/// �߳�����֮ǰ��ʼ���¼�
	virtual BOOL OnInitThread(CThread *pThread) = 0;
	/// �߳��˳��¼�
	virtual void OnShutdownThread(CThread *pThread) = 0;
	/// �߳����к��������OnRunThread����FALSE �߳̽��˳�
	virtual BOOL OnRunThread(CThread *pThread) = 0;
};

/// �߳����װ
class _GENERAL_CLASS CThread
{
public:
	friend IThreadWorker;

	enum {
		tsThreadInit,		// �̳߳�ʼ��״̬����δ����
		tsThreadFree,		// �߳̿���״̬
		tsThreadSuspend,	// �߳���ͣ״̬ 
		tsThreadBusy,		// �߳�æµ״̬
		tsThreadClosing,	// �߳����ڹر�
		tsThreadClose		// �̹߳ر�״̬
	};

public:
	CThread(LPCTSTR lpszName = NULL, IThreadWorker *pThreadWorker = NULL);
	~CThread();

	/// ��ȡ�߳̾��
	HANDLE GetHandle() const { 
		return m_hThread; 
	}
	/// ��ȡ�߳�ID
	UINT GetThreadID() const { 
		return m_nThreadID; 
	}
	/// ��ȡ�߳����ƣ���������˶���߳�ʵ���������ͨ�����ò�ͬ���߳���������������
	LPCTSTR GetThreadName() const {
		return m_szThreadName;
	}
	/// �����߳����ƣ����lpszName==NULL����Ὣ�߳���������Ϊ��
	void SetThreadName(LPCTSTR lpszName) {
		m_szThreadName = lpszName;
	}
	/// ��ȡ�̵߳ĵ�ǰ״̬
	int GetThreadStatus() { return m_nThreadStatus; }
	/// �����̵߳ĵ�ǰ״̬
	void SetThreadStatus(int nThreadStatus) { m_nThreadStatus = nThreadStatus; }
	/*
	*	Priority                        Meaning
	*
	*   THREAD_PRIORITY_ABOVE_NORMAL	Priority 1 point above the priority class. 
	*   THREAD_PRIORITY_BELOW_NORMAL	Priority 1 point below the priority class. 
	*   THREAD_PRIORITY_HIGHEST			Priority 2 points above the priority class. 
	*   THREAD_PRIORITY_IDLE			Base priority of 1 for IDLE_PRIORITY_CLASS, BELOW_NORMAL_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, or HIGH_PRIORITY_CLASS processes, and a base priority of 16 for REALTIME_PRIORITY_CLASS processes. 
	*   THREAD_PRIORITY_LOWEST			Priority 2 points below the priority class. 
	*   THREAD_PRIORITY_NORMAL			Normal priority for the priority class. 
	*   THREAD_PRIORITY_TIME_CRITICAL	Base priority of 15 for IDLE_PRIORITY_CLASS, BELOW_NORMAL_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, or HIGH_PRIORITY_CLASS processes, and a base priority of 31 for REALTIME_PRIORITY_CLASS processes. 
	 */
	/// ��ȡ�߳����ȼ���
	int GetThreadPriority( ) { 
		return ::GetThreadPriority( m_hThread ); 
	}
	/// �����߳����ȼ���
	BOOL SetThreadPriority(int nPriority) { 
		return ::SetThreadPriority( m_hThread, nPriority );
	}
	/// �Ƿ��Զ�ɾ���̶߳���
	BOOL IsAutoDelete() const { 
		return m_bAutoDelete; 
	}
	/// �����Ƿ��Զ�ɾ���̶߳������ΪTRUE�����߳��˳�ʱ���̶߳���CThread���Զ���ɾ��
	void SetAutoDelete(BOOL bAutoDelete) { 
		m_bAutoDelete = bAutoDelete; 
	}
	/// ��ȡ�û�����
	DWORD GetUserData() {
		return m_dwUserData;
	}
	/// �����û�����
	void SetUserData(DWORD dwUserData) {
		m_dwUserData = dwUserData;
	}

public:
	/// �����̲߳�ִ��
	BOOL Start(IThreadWorker *pThreadWorker = NULL, void *pSecurity = NULL, DWORD dwStackSize = NULL);
	/// ��ͣ�߳�ִ��
	BOOL SuspendThread() {
		if (m_hThread != INVALID_HANDLE_VALUE)
			return (::SuspendThread(m_hThread) != 0);
		else
			return FALSE;
	}
	/// �ָ��߳�ִ��
	BOOL ResumeThread() {
		if (m_hThread != INVALID_HANDLE_VALUE)
			return (::ResumeThread(m_hThread) != 0);
		else
			return FALSE;
	}
	/// ֪ͨ�߳��˳������̲߳����������˳����ú���ֻ��������һ���߳��˳����¼��ź�
	void Shutdown();
	/// �ȴ��߳��˳�������ú�������ΪTRUE����ʾ�߳��˳��ɹ��������̻߳�δ�����˳�
	BOOL WaitForShutdown(DWORD dwTimeoutMillis = INFINITE);
	/// �ر��߳̾��������˳�֪ͨ�źţ����WaitForShutdown()��������ΪTRUE������Զ����øú���������Ҫ�ٵ���
	void Close();
	/// ǿ���˳��߳�
	BOOL Terminate(DWORD dwExitCode = 0);


protected:
	/// ��ѯ�˳��߳��¼��ź��Ƿ�����
	inline BOOL IsShutdownning() {
		if (::WaitForSingleObject(m_hShutdownEvent, 0) == WAIT_OBJECT_0)
			return TRUE;
		else
			return FALSE;
	}

private:
	/// �߳����к���
	int Run();

	/// �̻߳ص�����
	static unsigned int __stdcall ThreadFunction(void *pParam);

	/// �߳̾��
	HANDLE			m_hThread;
	/// �߳�ID
	UINT			m_nThreadID;
	/// �߳�ʵ������
	CString			m_szThreadName;
	/// �Ƿ��Զ�ɾ���߳�ʵ��
	BOOL			m_bAutoDelete;
	/// �߳��˳��¼����
	HANDLE			m_hShutdownEvent;
	/// �̹߳�����ָ��
	IThreadWorker	*m_pThreadWorker;
	/// �̹߳���״̬
	int				m_nThreadStatus;
	/// �û�����
	DWORD			m_dwUserData;

	// No copies do not implement
	CThread(const CThread &rhs);
	CThread &operator=(const CThread &rhs);
};

// �̳߳ع�����
class _GENERAL_CLASS CThreadPool
{
public:
	typedef CList<CThread*, CThread*> CThreadList;

public:
	/// ���캯����dwMaxThreadCount==0��ʾû������߳�������
	CThreadPool(DWORD dwMaxThreadCount = 0);
	~CThreadPool();

public:
	DWORD GetThreadCount() { return (DWORD)m_listThreads.GetCount(); }
	DWORD GetMaxThreadCount() { return m_dwMaxThreadCount; }
	DWORD GetThreadStackSize() { return m_dwStackSize; }
	void SetThreadStackSize(DWORD dwStackSize) { m_dwStackSize = dwStackSize; }

public:
	/// ��ʼ�̳߳�
	void Start(LPCTSTR lpszThreadName, IThreadWorker *pThreadWorker, DWORD dwMaxThreadCount = 0, DWORD dwInitCount = 10);

	/// �¼�һ���߳�
	CThread *AddThread(LPCTSTR lpszThreadName, IThreadWorker *pThreadWorker);

	/// ɾ��һ���̣߳����pThread == NULL������б����ɾ��һ���߳�
	BOOL RemoveThread(CThread *pThread = NULL);

	/// ��ͣ�̳߳�
	BOOL SuspendThreadPool();

	/// �ָ��̳߳�ִ��
	BOOL ResumeThreadPool();

	/// ֪ͨ�߳��˳������̲߳����������˳����ú���ֻ��֪ͨ�߳��߳�
	void Shutdown();

	/// �ȴ��߳��˳�������ú�������ΪTRUE����ʾ�߳��˳��ɹ��������̻߳�δ�����˳�
	BOOL WaitForShutdown(DWORD dwTimeoutMillis = INFINITE);

protected:

private:
	/// �̰߳�ȫ��
	CCriticalSection m_csLock;

	/// ��������߳���
	DWORD m_dwMaxThreadCount;

	/// �̳߳��б�
	CThreadList m_listThreads;

	/// �̶߳�ջ��С
	DWORD m_dwStackSize;
};

TRUNK_NAMESPACE_END

#endif // end of __THREAD_H__