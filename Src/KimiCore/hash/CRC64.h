//crc64
#ifndef __KIMI_KERNEL_CRC64_H__
#define __KIMI_KERNEL_CRC64_H__

_KIMI_NAMESPACE_BEGIN

/// ����CRC64���ú���������DLL����Utility::GetCRC64()����
ULONGLONG get_crc64(ULONGLONG crc, const unsigned char *s, ULONGLONG l);

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_CRC64_H__