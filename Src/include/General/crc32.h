//crc32
#ifndef __CRC32_H__
#define __CRC32_H__

TRUNK_NAMESPACE_BEGIN

//_GENERAL_API(unsigned long) TL_GetCRC32(unsigned char *szData, unsigned long dwSize); //����CRC32

/// ����CRC32���ú�����չ��DLL����CUtility::GetCRC32()����
DWORD TL_GetCRC32(BYTE *szData, DWORD dwSize);

TRUNK_NAMESPACE_END

#endif // end of __CRC32_H__