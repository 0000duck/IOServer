#include "StdAfx.h"
#include "StreamMessageQueue.h"
//#include "dacls.h"

CStreamMessageQueue::CStreamMessageQueue()
{
	m_nHead = 0;
	m_nTail = 0;
	memset(m_pQueue, 0, STREAM_MESSAGE_QUEUE_SIZE * sizeof(CMyByteArray*)); //��ʼ��

	memset(m_pFree, 0, STREAM_MESSAGE_FREE_SIZE * sizeof(CMyByteArray*));
	m_nFreeHead = 0;
	m_nFreeTail = 0;
}

CStreamMessageQueue::~CStreamMessageQueue()
{
	for (int i = 0; i < STREAM_MESSAGE_QUEUE_SIZE; i++)
	{
		if (m_pQueue[i])
			delete m_pQueue[i];
	}

	CMyByteArray* p = Malloc();
	while(p)
	{
		delete p;
		p = Malloc();
	}
}

CMyByteArray* CStreamMessageQueue::Malloc()
{
	CMyByteArray* pMessagePacket = NULL;

	CSingleLock lock(&m_csLock, TRUE);

	if (m_nFreeHead == m_nFreeTail) // ��
	{
		pMessagePacket = new CMyByteArray();
	}
	else {
		// ��ͷ��
		pMessagePacket = m_pFree[m_nFreeHead];
		m_pFree[m_nFreeHead] = NULL;
		m_nFreeHead = (m_nFreeHead + 1) % STREAM_MESSAGE_FREE_SIZE;
	}

	return pMessagePacket;
}

CMyByteArray* CStreamMessageQueue::Malloc(const char* lpszDeviceName, const char* lpszTagName, DWORD dwCommandID, BYTE* pBuffer, int nSize)
{
	CMyByteArray* pMessagePacket = Malloc();

	if (!pMessagePacket)
		return NULL;

	int nPacketSize = 16 + 80 + sizeof(DWORD) + nSize + 16;

	if (nPacketSize > MAX_FRM_LEN)
	{
		Free(pMessagePacket);
		return NULL;
	}

	pMessagePacket->ClearData();

	BYTE ucbuf[32];
	DWORD *pdw = (DWORD *)ucbuf;
	pdw[0] = FRM_SYN_WORD;
	pdw[1] = nPacketSize;
	ucbuf[8] = WDDS_OD_STREAM_MESSAGE;
	ucbuf[9] = 0;
	ucbuf[10] = 0;
	ucbuf[11] = 0;
	pdw[3] = Utility::GetCRC32(ucbuf,12);

	// ����ͷ
	pMessagePacket->PutData(ucbuf,16);

	// �豸ʵ����
	char szBuffer[80] = {0};
	int nLen = lpszDeviceName ? strlen(lpszDeviceName) : 0;
	if (nLen > 0)
		memcpy(szBuffer, lpszDeviceName, min(15, nLen));
	pMessagePacket->PutData(szBuffer, 16);

	// ��ǩ��
	memset(szBuffer, 0, 80);
	nLen = lpszTagName ? strlen(lpszTagName) : 0;
	if (nLen > 0)
		memcpy(szBuffer, lpszTagName, min(79, nLen));
	pMessagePacket->PutData(szBuffer, 80);

	// ��Ϣָ��ID
	pMessagePacket->PutData(&dwCommandID, sizeof(DWORD));

	// ����Ϣ���ݿ�
	pMessagePacket->PutData(pBuffer, nSize);

	return pMessagePacket;
}

void CStreamMessageQueue::Free(CMyByteArray* pMessagePacket)
{
	CSingleLock lock(&m_csLock, TRUE);

	if ( ((m_nFreeTail + 1) % STREAM_MESSAGE_FREE_SIZE) == m_nFreeHead)
	{
		// ����
		delete pMessagePacket;
		return;
	}

	// ����β��
	m_pFree[m_nFreeTail] = pMessagePacket; // ��ֵ
	m_nFreeTail = (m_nFreeTail + 1) % STREAM_MESSAGE_FREE_SIZE;
}

BOOL CStreamMessageQueue::Add(CMyByteArray* pMessagePacket)
{
	if ( ((m_nTail + 1) % STREAM_MESSAGE_QUEUE_SIZE) == m_nHead)
		return FALSE; // ��

	// ����β��
	m_pQueue[m_nTail] = pMessagePacket; // ��ֵ
	m_nTail = (m_nTail + 1) % STREAM_MESSAGE_QUEUE_SIZE;

	return TRUE;
}

CMyByteArray* CStreamMessageQueue::Get()
{
	if (m_nHead == m_nTail) //��
		return NULL;
	
	// ��ͷ��
	CMyByteArray* pMessagePacket = m_pQueue[m_nHead];
	m_pQueue[m_nHead] = NULL;
	m_nHead = (m_nHead + 1) % STREAM_MESSAGE_QUEUE_SIZE;

	return pMessagePacket;
}