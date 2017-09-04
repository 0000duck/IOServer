#include "StdAfx.h"
#include "GeneralAPI.h"
#include "General\Fiber.h"

TPRI_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////
/// CFiber - �˳̷�װ��
//////////////////////////////////////////////////////////////////////////

CFiber::CFiber(void)
{
	m_pFiber = NULL;
}

CFiber::CFiber(IFiberWorker *pFiberWorker, SIZE_T stack/* = 0*/)
{
	pFiberWorker->m_pFiber = this;
	m_pFiber = ::CreateFiber(stack, OnRunFiber, pFiberWorker);
}

CFiber::~CFiber(void)
{
	if (m_pFiber != NULL)
	{
		::DeleteFiber(m_pFiber);
		m_pFiber = NULL;
	}
}

BOOL CFiber::Create(IFiberWorker *pFiberWorker, SIZE_T stack/* = 0*/)
{
	ASSERT(m_pFiber == NULL);
	if (m_pFiber != NULL)
		return FALSE;

	pFiberWorker->m_pFiber = this;
	m_pFiber = ::CreateFiber(stack, OnRunFiber, pFiberWorker);
	return (m_pFiber != NULL);
}

BOOL CFiber::ConvertThreadToFiber()
{
	ASSERT(m_pFiber == NULL);
	if(m_pFiber != NULL)
		return FALSE;

	m_pFiber = ::ConvertThreadToFiber(this);
	return m_pFiber != NULL;
}

void CFiber::Attach(LPVOID p)
{
	ASSERT(m_pFiber == NULL); 
	m_pFiber = p;
}

LPVOID CFiber::Detach()
{
	ASSERT(m_pFiber != NULL);
	LPVOID result = m_pFiber;
	m_pFiber = NULL;

	return result;
}

void CFiber::Run()
{
	ASSERT(m_pFiber != NULL);
	::SwitchToFiber(m_pFiber);
}

void WINAPI CFiber::OnRunFiber(LPVOID pvParam)
{
	IFiberWorker *pFiber = (IFiberWorker*)pvParam;
	pFiber->OnRunFiber();
	pFiber->m_pFiber->m_btStatus = CFiber::DEAD;

	/*���ﲻ��ֱ���˳��˳����к���������ᵼ�������̵߳��˳���
	* ��ȷ�������ǰ�����Ȩ���ظ�scheduler����scheduler��ɾ��
	* ����˳�
	*/
	pFiber->m_pFiber->m_pScheduler->yield();
}

//�ȴ����¼������ˣ����˳����²��뵽�����ж�����
void CFiber::OnEvent(int ev)
{
	if(ev == m_nWaitEvent)
	{
		m_btStatus = CFiber::ACTIVED;
		m_pScheduler->AddToActive(this);
		m_nWaitEvent = 0;

		//��sleeplist��ɾ��
		std::list<std::pair<time_t,CFiber*> >::iterator it = m_pScheduler->m_sleepList.begin();
		std::list<std::pair<time_t,CFiber*> >::iterator end = m_pScheduler->m_sleepList.end();
		for( ; it != end; ++it )
		{
			if(it->second == this)
			{
				it = m_pScheduler->m_sleepList.erase(it);
				break;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
/// CFiberScheduler - �˳̵�����
//////////////////////////////////////////////////////////////////////////
CFiberScheduler::CFiberScheduler() 
:	m_nCurrentUID(-1),
	m_nCount(0),
	m_terminate(FALSE)
{
	memset(m_pFibers, 0, sizeof(CFiber *) * MAX_FIBER);
}

CFiberScheduler::~CFiberScheduler()
{

}

void CFiberScheduler::Destroy()
{
	for(int i = 0; i < MAX_FIBER; ++i)
	{
		if (m_pFibers[i])
		{
			::DeleteFiber(m_pFibers[i]->GetFiber());
			delete m_pFibers[i];
		}
	}
}

int CFiberScheduler::Spawn(IFiberWorker *pFiberWorker, SIZE_T stack/* = 0*/)
{
	if(m_nCount >= MAX_FIBER)
		return -1;

	//�մ������˳̲����ڿ�����״̬
	CFiber *pFiber = new CFiber(pFiberWorker, stack);
	pFiber->m_pScheduler = this;
	for(int i= 0; i < MAX_FIBER; ++i)
	{
		if(NULL == m_pFibers[i])
		{
			pFiber->m_nFiberID = i;
			m_pFibers[i] = pFiber;
			break;
		}
	}
	AddToActive(pFiber);
	++m_nCount;
	return pFiber->m_nFiberID;
}

int CFiberScheduler::Block(int ev, time_t timeout)
{
	m_pFibers[m_nCurrentUID]->m_nWaitEvent = ev;
	if(timeout > 0)
	{
		m_pFibers[m_nCurrentUID]->m_btStatus = CFiber::WAIT4EVENTTIMEOUT;
		time_t t = timeout + time(NULL);
		m_sleepList.push_back(std::make_pair(t,m_pFibers[m_nCurrentUID]));
	}
	else
		m_pFibers[m_nCurrentUID]->m_btStatus = CFiber::WAIT4EVENT;

	::SwitchToFiber(m_pFiberContext);

	if(m_pFibers[m_nCurrentUID]->m_nWaitEvent == 0)
	{
		//�ȴ����¼�����
		return 0;
	}
	else
		return -1;//��ʱ��

}

void CFiberScheduler::Schedule()
{
	while(!m_terminate && m_nCount > 0)
	{
		std::list<std::map<PVOID, CFiber*>::iterator> deletes;

		std::map<PVOID, CFiber*>::iterator it = m_activeList.begin();
		std::map<PVOID, CFiber*>::iterator end = m_activeList.end();
		for( ; it != end; ++it)
		{
			m_nCurrentUID = it->second->m_nFiberID;
			//::SwitchToFiber(it->second->GetFiber());
			it->second->Run();
			m_nCurrentUID = -1;
			if(it->second->m_btStatus == CFiber::DEAD || it->second->m_btStatus == CFiber::SLEEP || it->second->m_btStatus == CFiber::WAIT4EVENT
				|| it->second->m_btStatus == CFiber::WAIT4EVENTTIMEOUT)
			{
				deletes.push_back(it);
			}
		}

		{
			std::list<std::map<PVOID,CFiber*>::iterator>::iterator it = deletes.begin();
			std::list<std::map<PVOID,CFiber*>::iterator>::iterator end = deletes.end();
			for( ; it != end; ++it)
			{
				CFiber *pFiber = (*it)->second;
				if(pFiber->m_btStatus == CFiber::DEAD)
				{
					m_pFibers[pFiber->m_nFiberID] = NULL;
					delete pFiber;
					--m_nCount;
				}
				m_activeList.erase(*it);
			}
		}
		//�����еȴ���ӵ�m_activeList�е��˳̶���ӽ�ȥ
		{
			while(!m_pendingAdd.empty())
			{
				CFiber *tmp = m_pendingAdd.back();
				m_pendingAdd.pop_back();
				m_activeList.insert(std::make_pair(tmp->GetFiber(),tmp));
			}

		}

		//������û��timeout���˳�
		{
			time_t now = time(NULL);
			std::list<std::pair<time_t,CFiber*> >::iterator it = m_sleepList.begin();
			for( ; it != m_sleepList.end(); )
			{
				time_t t = it->first;
				if(it->first <= now)
				{
					it->second->m_btStatus = CFiber::ACTIVED; 
					m_activeList.insert(std::make_pair(it->second->GetFiber(),it->second));
					it = m_sleepList.erase(it);
				}
				else
					++it;
			}
		}

	}

	Destroy();
	::ConvertFiberToThread();
}

TPRI_NAMESPACE_END