/*
Ϊ�˰�ȫ��ʹ��TPRICommon�⣬�����StdAfx.h��VC_EXTRALEAN���������������µĺ궨�壺
 
// ������뽫λ������ָ��ƽ̨֮ǰ��ƽ̨��ΪĿ�꣬���޸����ж��塣
// �йز�ͬƽ̨��Ӧֵ��������Ϣ����ο� MSDN��
#ifndef WINVER				// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define WINVER 0x0501		// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif

#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define _WIN32_WINNT 0x0501	// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif						

#ifndef _WIN32_WINDOWS		// ����ʹ���ض��� Windows 98 ����߰汾�Ĺ��ܡ�
#define _WIN32_WINDOWS 0x0410 // ����ֵ����Ϊ�ʵ���ֵ����ָ���� Windows Me ����߰汾��ΪĿ�ꡣ
#endif

#ifndef _WIN32_IE			// ����ʹ���ض��� IE 6.0 ����߰汾�Ĺ��ܡ�
#define _WIN32_IE 0x0600	// ����ֵ����Ϊ��Ӧ��ֵ���������� IE �������汾��
#endif

*/

#ifndef __KIMI_COMMON_API_H__
#define __KIMI_COMMON_API_H__

// �Ƿ�ʹ�����ƿռ�
#define TPRI_USE_NAMESPACE

#ifdef TPRICOMMON_EXPORTS
#define TPRICOMMON_API(x) __declspec(dllexport) x
#define TPRICOMMON_CLASS __declspec(dllexport)
#else
#define TPRICOMMON_API(x) __declspec(dllimport) x
#define TPRICOMMON_CLASS __declspec(dllimport)

#ifndef _LINK_TO_UG_IN_EXTDLL
#define _LINK_TO_UG_IN_EXTDLL	// for Ultimate Grid
#endif

#endif

// ���ƿռ�궨��
#ifdef TPRI_USE_NAMESPACE
	#define _KIMI_NAMESPACE__BEGIN namespace KIMI { namespace COMMON {
	#define _KIMI_NAMESPACE__END }}
	#define DEFINE_TPRICOMMON_NAMESPACE using namespace KIMI::COMMON
#else
	#define _KIMI_NAMESPACE__BEGIN
	#define _KIMI_NAMESPACE__END
	#define DEFINE_TPRICOMMON_NAMESPACE
#endif

#pragma warning(disable : 4819) // warning C4819: ���ļ����������ڵ�ǰ����ҳ(936)�б�ʾ���ַ����뽫���ļ�����Ϊ Unicode ��ʽ�Է�ֹ���ݶ�ʧ

#include "CGeneral\Ultimate Grid\UGCtrl.h"

#include "CGeneral\Ultimate Grid\UGCTafnt.h"
#include "CGeneral\Ultimate Grid\UGCTAutoSize.h"
#include "CGeneral\Ultimate Grid\UGCTbutn.h"
#include "CGeneral\Ultimate Grid\UGCTDropGrid.h"
#include "CGeneral\Ultimate Grid\UGCTdtp.h"
#include "CGeneral\Ultimate Grid\ugctelps.h"
#include "CGeneral\Ultimate Grid\UGCTExpand.h"
#include "CGeneral\Ultimate Grid\UGCTLabeled.h"
#include "CGeneral\Ultimate Grid\UGCTMail.h"
#include "CGeneral\Ultimate Grid\UGCTMailSort.h"
#include "CGeneral\Ultimate Grid\UGCTMarquee.h"
#include "CGeneral\Ultimate Grid\UGCTmfnt.h"
#include "CGeneral\Ultimate Grid\UGCTNote.h"
#include "CGeneral\Ultimate Grid\ugctnotewnd.h"
#include "CGeneral\Ultimate Grid\UGCTOutlookHeader.h"
#include "CGeneral\Ultimate Grid\UGCTpie.h"
#include "CGeneral\Ultimate Grid\UGCTpro1.h"
#include "CGeneral\Ultimate Grid\UGCTprog.h"
#include "CGeneral\Ultimate Grid\UGCTRado.h"
#include "CGeneral\Ultimate Grid\UGCTsarw.h"
#include "CGeneral\Ultimate Grid\UGCTsldr.h"
#include "CGeneral\Ultimate Grid\UGCTSpin.h"
#include "CGeneral\Ultimate Grid\UTEdit.h"
#include "CGeneral\Ultimate Grid\UTMaskedEdit.h"

#ifndef TPRICOMMON_EXPORTS
///{{ �������ģ����ͷ�ļ�

#ifndef __KIMI_NOCASEMAP_H__
#include "CGeneral\NoCaseMap.h"
#endif

///}}
#endif

///{{ �������һЩ��Ҫ��ͷ�ļ�

#ifndef __KIMI_SYSTEMTRAY_H__
#include "CGeneral\SystemTray.h"
#endif

#ifndef __KIMI_SORTOBARRAY_H__
#include "CGeneral\SortObArray.h"
#endif
#ifndef __KIMI_MEMDC_H__
#include "CGeneral\MemDC.h"
#endif

#ifndef __KIMI_WINLIST_H__
#include "CGeneral\WinList.h"
#endif

#ifndef __KIMI_TPRIUGDATASOURCE_H__
#include "CGeneral\TPRIUGDataSource.h"
#endif

#ifndef __KIMI_TPRIUGDGRID_H__
#include "CGeneral\TPRIUGGrid.h"
#endif

#ifndef __SAFELOCK_H__
#include "CGeneral\safelock.h"
#endif
///}}

///{{ ��������һЩ���õļ����inline����
_KIMI_NAMESPACE__BEGIN

_KIMI_NAMESPACE__END
///}}

// ���Ӷ�̬�������������DEBUGģʽ��������TPRICommonD.lib��Releaseģʽ������TPRICommon.lib
#ifndef TPRICOMMON_EXPORTS
	#ifdef _DEBUG
		#pragma comment( lib, "CGenerald.lib")
	#else
		#pragma comment( lib, "CGeneral.lib")
	#endif
#endif

#endif // end of __KIMI_COMMON_API_H__