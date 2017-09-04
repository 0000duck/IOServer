#ifndef __KIMI_KERNEL_UTILITY_H__
#define __KIMI_KERNEL_UTILITY_H__

#include <io.h>
#include <string>
#include <tchar.h>

_KIMI_NAMESPACE_BEGIN

class KIMI_CORE_CLASS Utility
{
public:
	//////////////////////////////////////////////////////////////////////////
	/// ϵͳ���
	//////////////////////////////////////////////////////////////////////////

	/// ��ǰϵͳ�Ƿ�ΪNTƽ̨
	static BOOL IsWindowNT(void);

	/// ��ǰϵͳ�汾��
	static string GetWindowVersion(void);

	static string FormatErrorMessage(DWORD dwErrorCode);


	//////////////////////////////////////////////////////////////////////////
	/// �ַ������
	//////////////////////////////////////////////////////////////////////////
	/// ��ʽ���ļ���С���Զ�ѡ��KB/GB/TB��λ
	static string FormatFileSize(__int64 nFileSize);

	/// ��KBΪ��λ��ʽ���ļ���СΪ
	static string FormatFileSizeKB(__int64 nFileSize);

	//////////////////////////////////////////////////////////////////////////
	/// ������Ŀ¼���
	//////////////////////////////////////////////////////////////////////////

	/// ��ȡģ��ľ���·��������MFCӦ�ã�hModule = AfxGetInstanceHandle()ʱ�ɻ�ȡ��ǰ���г����·��
	static string GetModulePath(HMODULE hModule/* = NULL*/);

	/// ָ��Ŀ¼�Ƿ����
	static BOOL DirectoryExists(const char *lpszPath);
	static BOOL DirectoryExists(const wchar_t *lpszPath);

	/// ����Ŀ¼�����𼶽���
	static BOOL CreateDirectory(const char *lpszPath);
	static BOOL CreateDirectory(const wchar_t *lpszPath);

	/// ɾ��Ŀ¼���ݹ鷽ʽ
	static BOOL DeleteDirectory(const char *lpszPath);
	static BOOL DeleteDirectory(const wchar_t *lpszPath);

	/// ��ȡ���̿��ÿռ�
	static ULONGLONG GetDiskFreeSpace(const char *lpszDisk);
	static ULONGLONG GetDiskFreeSpace(const wchar_t *lpszDisk);

	//////////////////////////////////////////////////////////////////////////
	/// �ļ��汾���
	//////////////////////////////////////////////////////////////////////////

	/// ��ȡָ���ļ��İ汾��
	static BOOL GetFileVersion(const char *lpszFileName, WORD &wProductVersionMS1, WORD &wProductVersionMS2, WORD &wProductVersionLS1, WORD &wProductVersionLS2);
	static BOOL GetFileVersion(const wchar_t *lpszFileName, WORD &wProductVersionMS1, WORD &wProductVersionMS2, WORD &wProductVersionLS1, WORD &wProductVersionLS2);
	
	/// ��ȡ�ļ��İ汾�ַ���
	static BOOL GetFileVersion(const char *lpszFileName, std::string &strVersion, BOOL bGetBuildInfo = TRUE);
	static BOOL GetFileVersion(const wchar_t *lpszFileName, std::wstring &strVersion, BOOL bGetBuildInfo = TRUE);

	//////////////////////////////////////////////////////////////////////////
	/// ����У�鼰HASHֵ���
	//////////////////////////////////////////////////////////////////////////
	/// ����CRC32У��ֵ
	static DWORD GetCRC32(const BYTE *szData, DWORD dwSize);
	/// ����CRC64У��ֵ
	static ULONGLONG GetCRC64(ULONGLONG ullCRC64, const unsigned char *pBuffer, ULONGLONG ullBufferSize);

	/// ������������MD5ֵ��ANSI�汾
	static string GetMD5(const unsigned char* lpBuffer, unsigned int nTextLen);
	/// ������������MD5ֵ��Unicode�汾
	static wstring GetMD5W(const unsigned char* lpBuffer, unsigned int nTextLen);
	/// ����ָ���ַ�����MD5ֵ��ANSI�汾
	static string GetMD5(const char* lpszText);
	/// ����ָ���ַ�����MD5ֵ��Unicode�汾
	static wstring GetMD5W(const wchar_t* lpszText);

	/// ������������SHA1ֵ��ANSI�汾
	static string GetSHA1(const unsigned char* lpBuffer, unsigned int nTextLen);
	/// ������������SHA1ֵ��Unicode�汾
	static wstring GetSHA1W(const unsigned char* lpBuffer, unsigned int nTextLen);
	/// �����ַ�����SHA1ֵ��ANSI�汾
	static string GetSHA1(const char* lpszText);
	/// �����ַ�����SHA1ֵ��Unicode�汾
	static wstring GetSHA1W(const wchar_t* lpszText);

protected:
	static BOOL GetVersionStringInfo(LPVOID lpvMem, const char *lpszKey, char *lpszReturnValue, DWORD *pdwReturnValueSize);
	static BOOL GetVersionStringInfo(LPVOID lpvMem, const wchar_t *lpszKey, wchar_t *lpszReturnValue, DWORD *pdwReturnValueSize);
};

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_UTILITY_H__