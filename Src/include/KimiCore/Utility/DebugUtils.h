///////////////////////////////////////////////////////////////////
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
//   �����ȹ��о�Ժ���޹�˾
// 
///////////////////////////////////////////////////////////////////
//   ������ʷ:
// 
//   @2010-03-24
//      �����ĵ�
///////////////////////////////////////////////////////////////////
#ifndef __KIMI_KERNEL_DEBUG_UTILS_H__
#define __KIMI_KERNEL_DEBUG_UTILS_H__

_KIMI_NAMESPACE_BEGIN

#ifdef _DEBUG

void __DebugOutput(LPCTSTR lpszFormat, ...);

#define OutputDebug										::__DebugOutput						
#define OutputDebug0(sz)								::__DebugOutput(_T("%s"), _T(sz))
#define OutputDebug1(sz, p1)							::__DebugOutput(_T(sz), p1)
#define OutputDebug2(sz, p1, p2)						::__DebugOutput(_T(sz), p1, p2)						
#define OutputDebug3(sz, p1, p2, p3)					::__DebugOutput(_T(sz), p1, p2, p3)
#define OutputDebug4(sz, p1, p2, p3, p4)				::__DebugOutput(_T(sz), p1, p2, p3, p4)
#define OutputDebug5(sz, p1, p2, p3, p4, p5)			::__DebugOutput(_T(sz), p1, p2, p3, p4, p5)
#define OutputDebug6(sz, p1, p2, p3, p4, p5, p6)		::__DebugOutput(_T(sz), p1, p2, p3, p4, p5, p6)

#else // _DEBUG

inline void __DebugOutput(LPCTSTR lpszFormat, ...) {};

#define OutputDebug              1 ? (void)0 : ::__DebugOutput
#define OutputDebug0(sz)
#define OutputDebug1(sz, p1)
#define OutputDebug2(sz, p1, p2)
#define OutputDebug3(sz, p1, p2, p3)
#define OutputDebug4(sz, p1, p2, p3, p4)
#define OutputDebug5(sz, p1, p2, p3, p4, p5)
#define OutputDebug6(sz, p1, p2, p3, p4, p5, p6)

#endif // _DEBUG

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_DEBUG_UTILS_H__