// CGeneral.h : CGeneral DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCGeneralApp
// �йش���ʵ�ֵ���Ϣ������� CGeneral.cpp
//

class CCGeneralApp : public CWinApp
{
public:
	CCGeneralApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};