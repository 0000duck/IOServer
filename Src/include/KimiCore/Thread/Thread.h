#ifndef __KIMI_KERNEL_THREAD_H__
#define __KIMI_KERNEL_THREAD_H__

_KIMI_NAMESPACE_BEGIN

/// �߳����װ
class KIMI_CORE_CLASS CThread
{
public:

	enum {
		tsThreadInit,		// �̳߳�ʼ��״̬����δ����
		tsThreadFree,		// �߳̿���״̬
		tsThreadSuspend,	// �߳���ͣ״̬ 
		tsThreadBusy,		// �߳�æµ״̬
		tsThreadClosing,	// �߳����ڹر�
		tsThreadClose		// �̹߳ر�״̬
	};

public:
	CThread(LPCTSTR lpszName = NULL);
	virtual ~CThread();

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
		if (m_szThreadName)
			free(m_szThreadName);
		m_szThreadName = _tcsdup( (lpszName == NULL) ? _T("") : lpszName );
	}
	/// ��ȡ�̵߳ĵ�ǰ״̬
	int GetThreadStatus() { return m_nThreadStatus; }
	/// �����̵߳ĵ�ǰ״̬
	void SetThreadStatus(int nThreadStatus) { m_nThreadStatus = nThreadStatus; }

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
	/// �߳�����֮ǰ��ʼ���¼�
	virtual BOOL OnInitThread(CThread *pThread) {return TRUE;};
	/// �߳��˳��¼�
	virtual void OnShutdownThread(CThread *pThread){return;} ;
	/// �߳����к��������OnRunThread����FALSE �߳̽��˳�
	virtual BOOL OnRunThread(CThread *pThread) {return TRUE;};
	/// �����̲߳�ִ��
	BOOL Start( void *pSecurity = NULL, DWORD dwStackSize = NULL);
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
	TCHAR*			m_szThreadName;
	/// �Ƿ��Զ�ɾ���߳�ʵ��
	BOOL			m_bAutoDelete;
	/// �߳��˳��¼����
	HANDLE			m_hShutdownEvent;
	/// �̹߳�����ָ��
	//IThreadWorker	*m_pThreadWorker;
	/// �̹߳���״̬
	int				m_nThreadStatus;
	/// �û�����
	DWORD			m_dwUserData;
	
	// No copies do not implement
	CThread(const CThread &rhs);
	CThread &operator=(const CThread &rhs);
};

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_THREAD_H__