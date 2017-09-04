#ifndef __THREAD_TLS_H__
#define __THREAD_TLS_H__

TRUNK_NAMESPACE_BEGIN

/// ��ʼ���߳�TLS�洢��
_GENERAL_API(void) TlsInitThreadSlodData();

/// �ͷ��߳�TLS�洢��
_GENERAL_API(void) TlsFreeThreadSlodData();

/// �������ݵ��߳�TLS�洢��
_GENERAL_API(void) TlsSetThreadSlodData(LPVOID lpszData);

/// ���߳�TLS�洢���ж�ȡ����
_GENERAL_API(LPVOID) TlsGetThreadSlodData();

TRUNK_NAMESPACE_END

#endif // end of __THREAD_TLS_H__