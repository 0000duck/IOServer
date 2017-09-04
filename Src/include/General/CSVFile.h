///////////////////////////////////////////////////////////////////
//   DbProxyĿ�����������
//
//   CSV��ʽ�ļ���д��
//
//   ���ߣ�����
//   ���ڣ�2010-03-23
//   tech
// 
///////////////////////////////////////////////////////////////////
//   ������ʷ:
// 
//   @2010-03-31
//      �����ĵ�
///////////////////////////////////////////////////////////////////

#ifndef __CSVFILE_H__
#define __CSVFILE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afx.h"

TRUNK_NAMESPACE_BEGIN

class _GENERAL_CLASS CCSVFile : public CStdioFile
{
public:
	enum Mode { modeRead, modeWrite };
	CCSVFile(LPCTSTR lpszFilename, Mode mode = modeRead);
	virtual ~CCSVFile(void);

	BOOL ReadData(CStringArray &arr);
	void WriteData(CStringArray &arr);

#ifdef _DEBUG
  Mode m_nMode;
#endif

	static CString EncodeCSVString(LPCTSTR lpszStr);	// ���ַ���ת��ΪCSV����
	static CString EncodeCSVString(CStringArray &arr);	// ���ַ���ת��ΪCSV����
	static BOOL DecodeCSVString(LPCTSTR lpszStr, CStringArray &arr);	// ��CSV����ת��Ϊԭ�ַ���

protected:
	BOOL ReadLine(CString& rString);
};

TRUNK_NAMESPACE_END

#endif // end of __CSVFILE_H__
