//crc32.cpp
#include "stdafx.h"
#include "GeneralAPI.h"
#include "General\crc32.h"

TRUNK_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////
// CRC-32 У���㷨

// ���ڲ�������
unsigned long crc32_table[256]; 
BOOL		b_crc32IninTab=FALSE;
unsigned long Reflect(unsigned long ref, char ch)
{	
	int i;
	unsigned long value = 0;

	// bit 0 �� bit 7 ����
	// bit 1 �� bit 6 �������������...
	for(i = 1; i < (ch + 1); i++)
	{
		if (ref & 1)
			value |= 1 << (ch - i);
		ref >>= 1;
	}
	return value;
}
// �����������
void Init_CRC32_Table()
{	
	// �� CRC-32 ����ʽ�㷨�㷺Ӧ���� PKZip��WinZip �� Ethernet ��
	if(b_crc32IninTab)
		return;
	b_crc32IninTab=TRUE;
	unsigned long ulPolynomial = 0x04c11db7;
	int i, j;

	for(i = 0; i <= 0xFF; i++)
	{
		crc32_table[i] = Reflect(i, 8) << 24;
		for (j = 0; j < 8; j++)
			crc32_table[i] = (crc32_table[i] << 1) ^ 
				(crc32_table[i] & (1 << 31) ? ulPolynomial : 0);
		crc32_table[i] = Reflect(crc32_table[i], 32);
	}
}

/*
_GENERAL_API(unsigned long) TL_GetCRC32(unsigned char *szData, unsigned long dwSize) //����CRC32
{
	if(!b_crc32IninTab)
		Init_CRC32_Table();
	unsigned long  crc = 0xffffffff;
	int len;
	unsigned char* buffer = szData;

	len = dwSize;
	while(len--)
		crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ *buffer++];
	return crc^0xffffffff;
}
*/
DWORD TL_GetCRC32(BYTE *szData, DWORD dwSize) //����CRC32
{
	if(!b_crc32IninTab)
		Init_CRC32_Table();
	DWORD  crc = 0xffffffff;
	int len;
	BYTE* buffer = szData;

	len = dwSize;
	while(len--)
		crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ *buffer++];

	return crc^0xffffffff;
}

TRUNK_NAMESPACE_END