//crc32
#ifndef __KIMI_KERNEL_CRC32_H__
#define __KIMI_KERNEL_CRC32_H__

_KIMI_NAMESPACE_BEGIN

/// ����CRC32���ú���������DLL����Utility::GetCRC32()����
DWORD get_crc32(const BYTE *szData, DWORD dwSize);

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_CRC32_H__