/*
Ϊ�˰�ȫ��ʹ��GENERAL�⣬�����StdAfx.h��VC_EXTRALEAN���������������µĺ궨�壺
 
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

#ifndef __GENERAL_API_H__
#define __GENERAL_API_H__

// �Ƿ�ʹ�����ƿռ�
#define TRUNK_USE_NAMESPACE

#ifdef _GENERAL_EXPORTS
#define _GENERAL_API(x) __declspec(dllexport) x
#define _GENERAL_CLASS __declspec(dllexport)
#else
#define _GENERAL_API(x) __declspec(dllimport) x
#define _GENERAL_CLASS __declspec(dllimport)

#ifndef _LINK_TO_UG_IN_EXTDLL
#define _LINK_TO_UG_IN_EXTDLL	// for Ultimate Grid
#endif

#endif

// ���ƿռ�궨��
#ifdef TRUNK_USE_NAMESPACE
	#define TRUNK_NAMESPACE_BEGIN namespace TRUNK{
	#define TRUNK_NAMESPACE_END }
	#define DEFINE_TRUNK_NAMESPACE using namespace TRUNK
#else
	#define TRUNK_NAMESPACE_BEGIN
	#define TRUNK_NAMESPACE_END
	#define DEFINE_TRUNK_NAMESPACE
#endif

#pragma warning(disable : 4819) // warning C4819: ���ļ����������ڵ�ǰ����ҳ(936)�б�ʾ���ַ����뽫���ļ�����Ϊ Unicode ��ʽ�Է�ֹ���ݶ�ʧ

#include "General\Ultimate Grid\UGCtrl.h"

#include "General\Ultimate Grid\UGCTafnt.h"
#include "General\Ultimate Grid\UGCTAutoSize.h"
#include "General\Ultimate Grid\UGCTbutn.h"
#include "General\Ultimate Grid\UGCTDropGrid.h"
#include "General\Ultimate Grid\UGCTdtp.h"
#include "General\Ultimate Grid\ugctelps.h"
#include "General\Ultimate Grid\UGCTExpand.h"
#include "General\Ultimate Grid\UGCTLabeled.h"
#include "General\Ultimate Grid\UGCTMail.h"
#include "General\Ultimate Grid\UGCTMailSort.h"
#include "General\Ultimate Grid\UGCTMarquee.h"
#include "General\Ultimate Grid\UGCTmfnt.h"
#include "General\Ultimate Grid\UGCTNote.h"
#include "General\Ultimate Grid\ugctnotewnd.h"
#include "General\Ultimate Grid\UGCTOutlookHeader.h"
#include "General\Ultimate Grid\UGCTpie.h"
#include "General\Ultimate Grid\UGCTpro1.h"
#include "General\Ultimate Grid\UGCTprog.h"
#include "General\Ultimate Grid\UGCTRado.h"
#include "General\Ultimate Grid\UGCTsarw.h"
#include "General\Ultimate Grid\UGCTsldr.h"
#include "General\Ultimate Grid\UGCTSpin.h"
#include "General\Ultimate Grid\UTEdit.h"
#include "General\Ultimate Grid\UTMaskedEdit.h"

#ifndef _GENERAL_EXPORTS
///{{ �������ģ����ͷ�ļ�

#ifndef _NOCASEMAP_H__
#include "General\NoCaseMap.h"
#endif

#ifndef __SINGLETON_H__
#include "General\DesignPatterns\Singleton.h"
#endif

///}}
#endif

///{{ �������һЩ��Ҫ��ͷ�ļ�

//���ʹ����Xtreme ToolkitPro�ؼ���������__NOT_USE_CRT_HELPER������ֹCRT��������ͷ�ļ�
#ifndef __NOT_USE_CRT_HELPER

#ifndef __CRT_API_HELPER_H__
#include "General\CrtApiHelper.h"
#endif

#endif

//#ifndef __THEME_UTIL_H__
//#include "General\ThemeUtil.h"
//#endif

#ifndef __DEBUG_UTILS_H__
#include "General\DebugUtils.h"
#endif

#ifndef __TIME_UTILS_H__
#include "General\TimeUtils.h"
#endif

#ifndef __HASHTABLE_UTILS_H__
#include "General\HashTableUtils.h"
#endif

#ifndef __UTILITY_H__
#include "General\Utility.h"
#endif

#ifndef __MD5_H__
#include "General\md5.h"
#endif

#ifndef __SAFELOCK_H__
#include "General\safelock.h"
#endif

#ifndef __REGEX_H__
#include "General\regex.h"
#endif

#ifndef __READINI_H__
#include "General\readini.h"
#endif

#ifndef __SYSTEMTRAY_H__
#include "General\SystemTray.h"
#endif

#ifndef __SYSTEMINFO_H__
#include "General\SystemInfo.h"
#endif

#ifndef __CSVFILE_H__
#include "General\CSVFile.h"
#endif

#ifndef __FILEEXPLORER_H__
#include "General\FileExplorer.h"
#endif


#ifndef __SORTOBARRAY_H__
#include "General\SortObArray.h"
#endif

#ifndef __SIZECBAR_H__
#include "General\Sizecbar.h"
#endif

#ifndef __TRUECOLOR_TOOLBAR_H__
#include "General\TrueColorToolbar.h"
#endif

#ifndef __LISTBOXEX_H__
#include "General\ListBoxEx.h"
#endif

#ifndef __MDITABS_H__
#include "General\MDITabs.h"
#endif

#ifndef __TABSHEET_H__
#include "General\TabSheet.h"
#endif

#ifndef __SAFESTRINGLIST_H__
#include "General\SafeStringList.h"
#endif

#ifndef __THREAD_H__
#include "General\Thread.h"
#endif

#ifndef __THREAD_TLS_H__
#include "General\ThreadTLS.h"
#endif

#ifndef __MEMDC_H__
#include "General\MemDC.h"
#endif

#ifndef __WINLIST_H__
#include "General\WinList.h"
#endif

#ifndef __LOG_WINLIST_H__
#include "General\LogWinList.h"
#endif

#ifndef __MYBYTEARRAY_H__
#include "General\MyByteArray.h"
#endif

#ifndef __CCLIPBOARD_H__
#include "General\Clipboard.h"
#endif

#ifndef __COLORLISTBOX_H__
#include "General\ColorListBox.h"
#endif

#ifndef __HYPERLINK_H__
#include "General\HyperLink.h"
#endif

#ifndef __RESIZESHEET_H__
#include "General\ResizeSheet.h"
#endif

#ifndef __TPRIUGDATASOURCE_H__
#include "General\TPRIUGDataSource.h"
#endif

#ifndef __TPRIUGDGRID_H__
#include "General\TPRIUGGrid.h"
#endif

#ifndef __NPIPE_H__
#include "General\NamedPipe.h"
#endif

#ifndef __RWCRITICALSECTION_H__
#include "General\RWCriticalSection.h"
#endif

#ifndef __DRAW_HTML_H__
#include "General\DrawHTML.h"
#endif

#ifndef __HTML_LIST_CTRL_H__
#include "General\HTMLListCtrl.h"
#endif

#ifndef __EDITNUMBER_H__
#include "General\EditNumber.h"
#endif
//
//#ifndef __CUSTOM_TAB_CTRL_H__
//#include "General\CustomTabCtrl.h"
//#endif
//
#ifndef __LISTOPTIONSCTRL_H__
#include "General\ListOptionsCtrl.h"
#endif

#ifndef __PARAMETER_STRING_PARSER_H__
#include "General\ParameterStringParser.h"
#endif
///}}

///{{ ��������һЩ���õļ����inline����
TRUNK_NAMESPACE_BEGIN

TRUNK_NAMESPACE_END
///}}

// ���Ӷ�̬�������������DEBUGģʽ��������GeneralD.lib��Releaseģʽ������GeneralD.lib
#ifndef _GENERAL_EXPORTS
	#ifdef _DEBUG
		#pragma comment( lib, "GeneralD.lib")
	#else
		#pragma comment( lib, "General.lib")
	#endif
#endif

#endif // end of __GENERAL_API_H__