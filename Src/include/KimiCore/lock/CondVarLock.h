#ifndef __KIMI_KERNEL_CONDVAR_LOCK_H__
#define __KIMI_KERNEL_CONDVAR_LOCK_H__

_KIMI_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////
// �������� - CCondVarLock
//////////////////////////////////////////////////////////////////////////
class KIMI_CORE_CLASS CCondVarLock
{
public:
	CCondVarLock();
	~CCondVarLock();

	void Signal();
	void Broadcast();
	BOOL Wait(PCRITICAL_SECTION pCSLock, DWORD dwMilliseconds = INFINITE);
	BOOL Wait(CFastLock* pLock, DWORD dwMilliseconds = INFINITE);

private:
	union {
		CONDITION_VARIABLE cond_var_;	// Win7��ϵͳ֧�ֵ���������
		struct {			// ���ϵͳ��֧���������������Լ�ʵ��һ��
			unsigned int waiters_count_;
			CRITICAL_SECTION waiters_count_lock_;
			HANDLE signal_event_;
			HANDLE broadcast_event_;
		} fallback_;
	} cond_var_;

	// No copies do not implement
	CCondVarLock(const CCondVarLock &rhs);
	CCondVarLock &operator=(const CCondVarLock &rhs);
};

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_CONDVAR_LOCK_H__
