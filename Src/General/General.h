// General.h : General DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CGeneralApp
// �йش���ʵ�ֵ���Ϣ������� General.cpp
//

class CGeneralApp : public CWinApp
{
public:
	CGeneralApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};