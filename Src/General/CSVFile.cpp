#include "StdAfx.h"
#include "GeneralAPI.h"
#include "General\CSVFile.h"

TRUNK_NAMESPACE_BEGIN

CCSVFile::CCSVFile(LPCTSTR lpszFilename, Mode mode)
  : CStdioFile(lpszFilename, (mode == modeRead) ?
    CFile::modeRead|CFile::shareDenyWrite|CFile::typeText   
    :
    CFile::modeWrite|CFile::shareDenyWrite|CFile::modeCreate|CFile::typeText)
{
#ifdef _DEBUG
	m_nMode = mode;
#endif
}

CCSVFile::~CCSVFile(void)
{
}

/*
 * ���ַ���ת��ΪCSV����
 *
 * CSV�������
 * 1. ����ַ����а����ж��š�˫���š��س����з������ַ�������ʹ��˫���Ű������ַ�����
 * 2. ����ַ�������˫���ţ���������˫���ű�ʾһ��˫���ţ�
 */
CString CCSVFile::EncodeCSVString(LPCTSTR lpszStr)
{
	CString szt = _T("");

	if (!lpszStr || *lpszStr == _T('\0'))
		return szt;
	
	static TCHAR chQuote = _T('"');
	
	LPCTSTR ptr = lpszStr;
	TCHAR ch;
	BOOL bComma = FALSE;
	BOOL bQuote = FALSE;
	while(ch = *ptr++)
	{
		if (ch == chQuote) {
			bQuote = TRUE;
			szt += ch; // �ַ����е�һ��˫����������˫���Ŵ���
		}
		else if ((ch == _T(',')) || (ch == _T('\n')) || (ch == _T('\r')))
			bComma = TRUE;

		szt += ch;
	}

	if (bComma || bQuote)
	{
		szt = chQuote + szt + chQuote;
	}

	return szt;
}

CString CCSVFile::EncodeCSVString(CStringArray &arr)
{
	CString szt = _T("");

	if (arr.GetSize() == 0)
		return szt;

	for(int i = 0; i < arr.GetSize(); i++)
	{
		if (i > 0)
			szt.Insert(szt.GetLength(), _T(","));
		
		szt.Insert(szt.GetLength(), EncodeCSVString(arr[i]));
	}

	return szt;
}

// ��CSV����ת��Ϊԭ�ַ���
BOOL CCSVFile::DecodeCSVString(LPCTSTR lpszStr, CStringArray &arr)
{
	if (!lpszStr || *lpszStr == _T('\0'))
		return FALSE;

	LPCTSTR p = lpszStr;
	int nValue = 0;
	
	// Parse values in this line
	
	while (*p != '\0')
	{
		CString s;  // String to hold this value
		
		if (*p == '"')
		{
			p++;
			while (*p != '\0')
			{
				if (*p == '"')
				{
					p++;
					if (*p != '"')
					{
						p++;
						break;
					}
				}
				//s.AppendChar(*p++);
				s.Insert(s.GetLength(), (TCHAR)*p++);
			}
		}
		else
		{
			while (*p != '\0' && *p != ',')
			{
				//s.AppendChar(*p++);
				s.Insert(s.GetLength(), (TCHAR)*p++);
			}
			if (*p != '\0')
				p++;
		}
		
		if (nValue < arr.GetSize())
			arr[nValue] = s;
		else
			arr.Add(s);
		nValue++;
	}
	
	if (arr.GetSize() > nValue)
		arr.RemoveAt(nValue, arr.GetSize() - nValue);
	
	return TRUE;
}

BOOL CCSVFile::ReadLine(CString& rString)
{
	char ch;
	int nComma = 0;
	BOOL bOK = FALSE;
	while((ch = fgetc( m_pStream)) != EOF)
	{
		bOK = TRUE;
		if (ch == '"')
			nComma++;

		if (nComma % 2 == 0)
		{
			if (ch == '\r')
				continue;
			if (ch == '\n')
				break;
		}
		rString.Insert(rString.GetLength(), ch);
	}

	return bOK;
}

BOOL CCSVFile::ReadData(CStringArray &arr)
{
	// Verify correct mode in debug build
	ASSERT(m_nMode == modeRead);
	
	// Read next line
	CString sLine;
	if (!ReadLine(sLine))
		return FALSE;

	return DecodeCSVString(sLine, arr);


	/*
	LPCTSTR p = sLine;
	int nValue = 0;

	// Parse values in this line

	while (*p != '\0')
	{
		CString s;  // String to hold this value

		if (*p == '"')
		{
			p++;
			while (*p != '\0')
			{
				if (*p == '"')
				{
					p++;
					if (*p != '"')
					{
						p++;
						break;
					}
				}
				//s.AppendChar(*p++);
				s.Insert(s.GetLength(), (TCHAR)*p++);
			}
		}
		else
		{
			while (*p != '\0' && *p != ',')
			{
				//s.AppendChar(*p++);
				s.Insert(s.GetLength(), (TCHAR)*p++);
			}
			if (*p != '\0')
				p++;
		}
		
		if (nValue < arr.GetSize())
			arr[nValue] = s;
		else
			arr.Add(s);
		nValue++;
	}
	
	if (arr.GetSize() > nValue)
		arr.RemoveAt(nValue, arr.GetSize() - nValue);

	return TRUE;
	*/
}

void CCSVFile::WriteData(CStringArray &arr)
{
	ASSERT(m_nMode == modeWrite);
	
	CString szLine = EncodeCSVString(arr);
	WriteString(szLine);
	WriteString(_T("\n"));
	/*
	static TCHAR chQuote = '"';
	static TCHAR chComma = ',';

	ASSERT(m_nMode == modeWrite);

	for (int i = 0; i < arr.GetSize(); i++)
	{
		if (i > 0)
			WriteString(_T(","));
		
		bool bComma = (arr[i].Find(chComma) != -1);
		bool bQuote = (arr[i].Find(chQuote) != -1);
		if (bComma || bQuote)
		{
			Write(&chQuote, sizeof(TCHAR));
			if (bQuote)
			{
				for (int j = 0; j < arr[i].GetLength(); i++)
				{
					if (arr[i][j] == chQuote)
						Write(&chQuote, sizeof(TCHAR));
					TCHAR ch = arr[i][j];
					Write(&ch, sizeof(TCHAR));
				}
			}
			else
			{
				WriteString(arr[i]);
			}
			Write(&chQuote, sizeof(TCHAR));
		}
		else
		{
			WriteString(arr[i]);
		}
	}
	
	  WriteString(_T("\n"));
	*/
}

TRUNK_NAMESPACE_END