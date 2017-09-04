#ifndef __ACGBWRUNTIME_MD5_H__
#define __ACGBWRUNTIME_MD5_H__

///////////////////////////////////////////////////////////////////////////
// MD5 �����㷨
#ifndef POINTER
typedef unsigned char *POINTER; 
#endif // POINTER

#ifndef UINT2
typedef unsigned short int UINT2; 
#endif // UINT2

#ifndef UINT4
typedef unsigned long int UINT4; 
#endif // UINT4

typedef struct
{
	UINT4 state[4]; 
	UINT4 count[2]; 
	unsigned char buffer[64]; 
} MD5_CTX; 

void MD5Init(MD5_CTX *); 
void MD5Update(MD5_CTX *, unsigned char *, unsigned int); 
void MD5Final(unsigned char [16], MD5_CTX *); 

// ���ַ�������Ϊ16���ֽڵĶ�������
void MD5_Encode(char *inBuf, unsigned char outBuf[16]);

// ����MD5_Encode�������ɵ�16���ֽڵĶ�������ת����32���ֽڵ��ַ���(�ڸ��ֽ�ת��Ϊ��λ)
void MD5_ToString(unsigned char inBuf[16], char outBuf[33]);

// MD5_ToString() �����Ļ��溯��
void MD5_ToCode(char inBuf[33], unsigned char outBuf[16]);

//MD5���ܱ��32�ֽ�,lpszTxt < 256
BOOL MakeMD5Txt32(LPCTSTR lpszTxt,CString &szMD5);//MD5���ܱ��32�ֽ�,lpszTxt < 256

///////////////////////////////////////////////////////////////////////////

#endif // end of __ACGBWRUNTIME_MD5_H__