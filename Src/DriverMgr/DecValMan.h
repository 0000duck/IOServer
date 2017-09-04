#pragma once

#include "DrvItem.h"
#include "TagVal.h"
#include "DecVal.h"
#include "SOEBUF.h"


#include "StreamMessageQueue.h"

class CDecValMan
{
public:
	CDecValMan();
	~CDecValMan();
public:
	void AddDec(CDrvItem* pdec); //����豸
	//ɾ���豸
	void DelDecVal(long lid);
	CString GetDecName(long lid);
	void OnDecDataOK(long ldecid,T_DRREC recs[],int nsize); //����豸ֵ�������ڻص�������ʹ�ã�����
	void OnSoeOk(long ldecid,LPSOE psoe); //SOE�¼��ص���������

	DWORD OnWriteHistoryCB(long ldecid,T_DRREC recs[],int nsize);
	DWORD OnStreamMessageCB(long ldecid, const char* lpszTagName, DWORD	dwCommandID, BYTE* pBuffer,	int	nSize);

	//������ʾ��
	BOOL GetTagVal(long ldecid,LPCTSTR lpszName,CVarVal &val);//ȡ��ʾֵ
	BOOL GetTagViewTime(long ldecid,LPCTSTR lpszName,long *pltime);
	BOOL GetTagViewQa(long ldecid,LPCTSTR lpszName,long *pnqa);
	BOOL GetTagViewVal(long ldecid,LPCTSTR lpszName,CString &szVal);

	//������
	int  GetSaveRecs(CTagVal* lprecs,int nsize);//ȡ��������(��������)
	BOOL GetSaveVarVal(CTagVal* lprecs);//ȡ��������(string blob)
	BOOL GetSaveSoe(CSoe *psoe); //ȡSOE�¼����ڷ���
	CMyByteArray* GetSaveStreamMessage() {
		CSafeLock lock(&m_Lock);
		return m_smsgQueue.Get();
	}
	void FreeStreamMessage(CMyByteArray* pMessagePacket) {
		m_smsgQueue.Free(pMessagePacket);
	}

private:
	void DelDec(long lid);//ɾ���豸
protected:
	CCriticalSection m_Lock; //�ٽ�����

	CMap <long,long,CDecVal*,CDecVal*> m_mapdec;//�豸map,��IDΪ����
	CArray <CDecVal*,CDecVal*> m_arraydec;		//�豸����
	int m_ndecpos; //ȡ�������ݵ��豸λ��,������������
	int m_ndecposvar; //ȡ�������ݵ��豸λ��,string blob

	CSOEBUF	m_soeQueue;//SOE����
	CStreamMessageQueue m_smsgQueue; // ����Ϣ����
};

