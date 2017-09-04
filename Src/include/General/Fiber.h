#ifndef __TPRI_FIBER_H__
#define __TPRI_FIBER_H__

#include <map>
#include <list>
#include <time.h>

TPRI_NAMESPACE_BEGIN

class CFiberScheduler;
class CFiber;

/// �˳�������
class TPRICOMMON_CLASS IFiberWorker
{
public:
	friend CFiber;
	IFiberWorker() : m_pFiber(NULL) { }
	CFiber *GetFiber() { return m_pFiber; }
	CFiberScheduler *GetScheduler();

public:
	virtual void OnRunFiber() = 0;

private:
	CFiber *m_pFiber;
};

/// �˳̷�װ��
class TPRICOMMON_CLASS CFiber
{
public:
	enum
	{
		NONE,           //�����ó�������
		WAIT4EVENT = 1, //�ȴ�ĳ�¼�������
		WAIT4EVENTTIMEOUT,
		DEAD,           //�˳�������
		ACTIVED,        //�����е�
		SLEEP,
	};
	friend CFiberScheduler;
	friend IFiberWorker;
public:
	CFiber();
	CFiber(IFiberWorker *pFiberWorker, SIZE_T stack = 0);
	virtual ~CFiber(void);

	int GetFiberID() { return m_nFiberID; }
	BYTE GetStatus() { return m_btStatus; }
	int GetWaitEvent() { return m_nWaitEvent; }
	LPVOID GetFiber() { return m_pFiber; }

public:
	BOOL Create(IFiberWorker *pFiberWorker, SIZE_T stack = 0);
	BOOL ConvertThreadToFiber();
	void Attach(LPVOID p);
	LPVOID Detach();
	void Run();
	//�ȴ����¼������ˣ����˳����²��뵽�����ж�����
	void OnEvent(int ev);

protected:
	static void WINAPI OnRunFiber(LPVOID pvParam);

private:
	int m_nFiberID;
	BYTE m_btStatus;
	LPVOID	m_pFiber;
	int m_nWaitEvent;//һ���˳�ֻ�ܵȴ���һ���¼���
	CFiberScheduler *m_pScheduler;
};

#define MAX_FIBER 1024

/// �˳̵�����

class TPRICOMMON_CLASS CFiberScheduler
{
public:
	friend CFiber;
public:
	CFiberScheduler();
	~CFiberScheduler();

public:
	void Init() {
		m_pFiberContext = ::ConvertThreadToFiber(NULL);
	}
	void Destroy();
	int Spawn(IFiberWorker *pFiberWorker, SIZE_T stack = 0);

	//ѡ��һ���˳��Խ��е���
	void Schedule();

	void Sleep(time_t timeout)
	{
		if(timeout > 0)
		{
			m_pFibers[m_nCurrentUID]->m_btStatus = CFiber::SLEEP;
			time_t t = timeout + time(NULL);
			m_sleepList.push_back(std::make_pair(t,m_pFibers[m_nCurrentUID]));
		}

		::SwitchToFiber(m_pFiberContext);

	}

	//��һ���˳���ӵ������ж�����
	void AddToActive(CFiber *ut)
	{
		ut->m_btStatus = CFiber::ACTIVED;
		m_pendingAdd.push_back(ut);
	}

	//��������Ȩ����scheduler
	void yield()
	{
		//������Ȩ����������
		::SwitchToFiber(m_pFiberContext);
	}

	//������ev��,timeout==0����Զ�ȴ�
	int Block(int ev,time_t timeout);
protected:

private:
	std::map<PVOID,CFiber*> m_activeList;//�������б�
	std::list<CFiber*> m_pendingAdd;//�ȴ���ӽ��������б��е��˳�
	std::list<std::pair<time_t,CFiber*> > m_sleepList;//����˯�ߵ��˳̣������ĳ������ȶ���

	PVOID m_pFiberContext; // �����������˳̵�������
	int m_nCount;
	CFiber *m_pFibers[MAX_FIBER];
	int m_nCurrentUID;           //��ǰ�������е��˳̵�uid,==-1��ʾ��scheduler������
	volatile bool m_terminate;       
};

inline CFiberScheduler *IFiberWorker::GetScheduler() { 
	return m_pFiber->m_pScheduler; 
}

#ifdef _FIBER_TEST

class CFiberTest1 : public IFiberWorker
{

public:
	void OnRunFiber()
	{
		for(int i = 0 ; i < 20; ++i)
		{
			TRACE(_T("%d\n"),i);
			TRACE("begin block\n");
			if(0 == GetScheduler()->Block(1, 1))
				TRACE("test wake me up\n");
			else
				TRACE("timeout\n");
			//Scheduler::sleep(1);
		}
		TRACE("die\n");
	}
};

class CFiberTest2 : public IFiberWorker
{
public:
	void OnRunFiber()
	{
		for(int i = 0 ; i < 10; ++i)
		{
			TRACE("%d\n",i);
			if(test1->GetFiber()->GetWaitEvent() == 1)
				test1->GetFiber()->OnEvent(1);
			GetScheduler()->yield();
		}
		TRACE("die\n");
	}
	CFiberTest1 *test1;
};

void FiberTest()
{
	CFiberScheduler fsc;
	fsc.Init();

	CFiberTest1 test1;
	CFiberTest2 test2;
	test2.test1 = &test1;
	fsc.Spawn(&test1,4096);
	fsc.Spawn(&test2,4096);

	fsc.Schedule();
}

#endif

TPRI_NAMESPACE_END

#endif // end of __TPRI_FIBER_H__