// MyFrmParse.h: interface for the CMyFrmParse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYFRMPARSE_H__BD0EE330_2C82_4E6D_A41E_423C5EFA7F0D__INCLUDED_)
#define AFX_MYFRMPARSE_H__BD0EE330_2C82_4E6D_A41E_423C5EFA7F0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\..\SDK\ddclib.h"
class CMyFrmParse : public CFrmParse  
{
public:
	CMyFrmParse();
	virtual ~CMyFrmParse();

public:
	int	Parse();//�ְ�,����m_nStatus
	static	BOOL CheckCrc32(BYTE *pd);//����16�ֽڱ�ͷУ��

	short SwitchBytes(short wBytes,BOOL bSwitchBytes);
	float SwitchShortToFloat(short wBytes,BOOL bSwitchBytes);
	DWORD SwitchBytes(DWORD wBytes,BOOL bSwitchBytes);
	
	// �ֽ���ת��
	float SwitchBytes(float fltBytes,BOOL bSwitchBytes);
};

#endif // !defined(AFX_MYFRMPARSE_H__BD0EE330_2C82_4E6D_A41E_423C5EFA7F0D__INCLUDED_)
