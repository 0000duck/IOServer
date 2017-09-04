#ifndef __KIMI_KERNEL_LOCK_GUARD_H__
#define __KIMI_KERNEL_LOCK_GUARD_H__

_KIMI_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////
// ��ջ���������Զ�����/������ģ��
//////////////////////////////////////////////////////////////////////////
template <class T>
class CLockGuard
{
public:
	explicit CLockGuard(T* pLock) : m_pLock(pLock) { m_pLock->Lock(); }
	~CLockGuard() { m_pLock->Unlock(); }

private:
	T* m_pLock;

	// No copies do not implement
	CLockGuard(const CLockGuard &rhs);
	CLockGuard &operator=(const CLockGuard &rhs);
};

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_LOCK_GUARD_H__
