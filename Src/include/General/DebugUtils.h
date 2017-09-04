///////////////////////////////////////////////////////////////////
//   DbProxyĿ�����������
//
//   �õ�Ԫ�ṩ��ʽ����OutputDebugString�������������ʹ��DbgView
//   ���Թ��߲鿴���������Ϣ
//
//       Ϊ���ܹ������ڴ�й¶�ı�����Ϣ����Ҫ����Ŀ�м������´��룺
//    1. ��StdAfx.h�ļ��м���
//        #define _crtdbg_map_alloc
//        #include "stdlib.h"
//        #include "crtdbg.h"
//    2. ��CWinApp������ExitInstance()���������������º�����
//        _CrtDumpMemoryLeaks()
//
//   ���ߣ�����
//   tech
// 
///////////////////////////////////////////////////////////////////
//   ������ʷ:
// 
//   @2010-03-24
//      �����ĵ�
///////////////////////////////////////////////////////////////////
#ifndef __DEBUG_UTILS_H__
#define __DEBUG_UTILS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>

TRUNK_NAMESPACE_BEGIN

#ifdef _DEBUG

static CCriticalSection __debug_lock;

inline void __DebugOutput(LPCTSTR lpszFormat, ...)
{
	static TCHAR buf[1024];
	
	__debug_lock.Lock();
	va_list args;
	va_start(args, lpszFormat);
#if _MSC_VER > 1200
	vsprintf_s(buf, 1024, lpszFormat, args);
#else
	_vstprintf(buf, lpszFormat, args);
#endif
	va_end(args);
	// ���뻻�з����Է�����VC��Output����ʾ
	int nlen = (int)_tcslen(buf);
	buf[nlen] = _T('\n');
	buf[nlen + 1] = _T('\0');
	
	OutputDebugString(buf);
	__debug_lock.Unlock();
}
#define OutputDebug										::__DebugOutput						
#define OutputDebug0(sz)								::__DebugOutput(_T("%s"), _T(sz))
#define OutputDebug1(sz, p1)							::__DebugOutput(_T(sz), p1)
#define OutputDebug2(sz, p1, p2)						::__DebugOutput(_T(sz), p1, p2)						
#define OutputDebug3(sz, p1, p2, p3)					::__DebugOutput(_T(sz), p1, p2, p3)
#define OutputDebug4(sz, p1, p2, p3, p4)				::__DebugOutput(_T(sz), p1, p2, p3, p4)
#define OutputDebug5(sz, p1, p2, p3, p4, p5)			::__DebugOutput(_T(sz), p1, p2, p3, p4, p5)
#define OutputDebug6(sz, p1, p2, p3, p4, p5, p6)		::__DebugOutput(_T(sz), p1, p2, p3, p4, p5, p6)

#else // _DEBUG

inline void __DebugOutput(LPCTSTR, ...) { }
#define OutputDebug              1 ? (void)0 : ::__DebugOutput
#define OutputDebug0(sz)
#define OutputDebug1(sz, p1)
#define OutputDebug2(sz, p1, p2)
#define OutputDebug3(sz, p1, p2, p3)
#define OutputDebug4(sz, p1, p2, p3, p4)
#define OutputDebug5(sz, p1, p2, p3, p4, p5)
#define OutputDebug6(sz, p1, p2, p3, p4, p5, p6)

#endif // _DEBUG

TRUNK_NAMESPACE_END

#endif // end of __DEBUG_UTILS_H__