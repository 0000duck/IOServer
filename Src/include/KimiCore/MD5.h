#ifndef __KIMI_KERNEL_MD5_H__
#define __KIMI_KERNEL_MD5_H__

#include <string>

_KIMI_NAMESPACE_BEGIN

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

KIMI_CORE_API(void) MD5Init(MD5_CTX *); 
KIMI_CORE_API(void) MD5Update(MD5_CTX *, unsigned char *, unsigned int); 
KIMI_CORE_API(void) MD5Final(unsigned char [16], MD5_CTX *); 

// ���ַ�������Ϊ16���ֽڵĶ�������
KIMI_CORE_API(void) MD5_Encode(unsigned char *inBuf, unsigned int nsize, unsigned char outBuf[16]);

// ����MD5_Encode�������ɵ�16���ֽڵĶ�������ת����32���ֽڵ��ַ���(�ڸ��ֽ�ת��Ϊ��λ)
KIMI_CORE_API(void) MD5_ToString(unsigned char inBuf[16], char outBuf[33]);

// MD5_ToString() �����Ļ��溯��
KIMI_CORE_API(void) MD5_ToCode(char inBuf[33], unsigned char outBuf[16]);

//MD5���ܱ��32�ֽ�,lpszTxt
//KIMI_CORE_API(BOOL) MakeMD5Txt32(const char *lpszTxt,  string &szMD5);

//MD5���ܱ��32�ֽ�,lpszTxt
//KIMI_CORE_API(BOOL) MakeMD5Txt32W(const wchar_t *lpszTxt, wstring &szMD5);


_KIMI_NAMESPACE_END
///////////////////////////////////////////////////////////////////////////

#endif // end of __KIMI_KERNEL_MD5_H__