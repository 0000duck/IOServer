// LogModule.h : LogModule DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CLogModuleApp
// �йش���ʵ�ֵ���Ϣ������� LogModule.cpp
//

class CLogModuleApp : public CWinApp
{
public:
	CLogModuleApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
