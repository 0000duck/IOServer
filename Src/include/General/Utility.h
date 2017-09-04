#ifndef __UTILITY_H__
#define __UTILITY_H__

#include  <io.h>


TRUNK_NAMESPACE_BEGIN

DWORD TL_GetCRC32(BYTE *szData, DWORD dwSize);

class _GENERAL_CLASS Utility
{
public:
	/// ����CRC32У��ֵ
	static DWORD GetCRC32(BYTE *szData, DWORD dwSize) {
		return TL_GetCRC32(szData, dwSize);
	}

	/// ��ȡ��ǰ����İ汾��
	static DWORD GetModuleVersion(HMODULE hModule = NULL);

	static BOOL GetModuleVersion(CString &szVersion, BOOL bGetBuildInfo = TRUE, HMODULE hModule = NULL);

	/// ��*/?ͨ����ĳ��ַ����ȽϺ���
	static BOOL MatchStr(LPCTSTR ssrc, LPCTSTR sfliter);

	/// ��ȡָ��ģ���·������
	static CString GetModulePath(HMODULE hModule = NULL);

	/// ����Ŀ¼,���𼶽���
	static BOOL CreateDir(LPCTSTR lpszPath);

	/// �жϺ����Ƿ���б��,�������
	static void	MakePath(CString &szPath);

	/// ɾ��Ŀ¼,�ݹ鷽ʽ
	static void DeleteDir(CString strDelDir);
	
	/// ��ȡ���̿ռ�
	static long GetDiskSpace(LPCTSTR lpszDisk);

protected:
	static BOOL GetVersionStringInfo(LPVOID  lpvMem, LPCTSTR lpszKey, LPTSTR lpszReturnValue);
};

TRUNK_NAMESPACE_END

#endif // end of __UTILITY_H__