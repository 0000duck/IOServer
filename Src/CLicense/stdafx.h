// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

//#define _CRT_SECURE_NO_WARNINGS

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>

#include <atlbase.h>
#include <comutil.h>
#if _MSC_VER > 1200
#include <comdef.h>
#endif

#include <string>

#define _KIMI_CORE_STATICLINK
#include "kimiCore.h"
DEFINE_KIMI_CORE_NAMESPACE;

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
