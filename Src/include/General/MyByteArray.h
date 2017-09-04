#ifndef __MYBYTEARRAY_H__
#define __MYBYTEARRAY_H__

TRUNK_NAMESPACE_BEGIN

/*------------------------------------------------------------------------------------

  �ֽڶ�̬����CMyByteArray�����ڱ������ݱ��Ļ����������

  */
#define MAX_BYTEARRAYSIZE	(1024 * 4096 + 256)	// ����ֽڶ�̬�����С4M

class _GENERAL_CLASS CMyByteArray //�ֽڶ�̬���飬���ڱ������ݱ��Ļ����������
{
public:
	CMyByteArray();
	~CMyByteArray();

	BOOL PutData(BYTE *pBuf,int nSize); //��������
	BOOL PutData(void *pBuf,int nSize)//��������
	{
		return PutData((BYTE*)pBuf,nSize);
	};
	BOOL PutStr(LPCTSTR lpszData,int nSize){
		return PutData((BYTE*)lpszData,nSize);
	};

	BYTE *GetBuf(){return m_pData;};
	BYTE *GetBuf(int &nsize){
		nsize = m_nSize;
		if(m_pData == NULL)
			nsize = 0;
		return m_pData;
	};

	int GetDataSize(){return m_nSize;};
	void ClearData();
	void SetGrown(int ngrown);
	void LeftMove(int n); //����n�ֽ�
	void CopyTo(CMyByteArray *pba); //���Ƶ���һ���ֽ�����β��.
	CMyByteArray& operator = (CMyByteArray& ba); //����=����
	CMyByteArray& operator += (CMyByteArray& ba); //����+=����
	
protected:
	BYTE *m_pData;		//���ݶ�
	int  m_nSize;		//���ݴ�С
	int  m_nBufSize;	//�ռ��С
	int	 m_nGrown;		//��������
};

TRUNK_NAMESPACE_END

#endif // end of __MYBYTEARRAY_H__