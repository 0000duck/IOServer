// MyFrmParse.h: interface for the CMyFrmParse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYFRMPARSE_H__1AD52466_5898_4762_B5F7_DA665B9B6484__INCLUDED_)
#define AFX_MYFRMPARSE_H__1AD52466_5898_4762_B5F7_DA665B9B6484__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\..\SDK\ddclib.h"

#pragma pack()
//*************************************DIFINE****************************************
#define TCP_BUFFER_SIZE		 1024*8  // 

enum GET_BYTE{
	GET_BYTE_HIGH ,	       // ¸ßÎ»
	GET_BYTE_LOW  ,          // µÍÎ»
};

class CMyFrmParse : public CFrmParse  
{
public:
	CMyFrmParse();
	virtual ~CMyFrmParse();
	static BYTE        CalaCS(BYTE *pData, int iSize);
	static BYTE        GetByte(BYTE btData, char cType);
	static short   SwitchBytes(short wBytes,BOOL bSwitchBytes);
	static float    SwitchBytes(float fltBytes,BOOL bSwitchBytes);
	static DWORD   SwitchBytes(DWORD wBytes,BOOL bSwitchBytes);
	static DWORD   SwitchBytes(BYTE *pBytes,BOOL bSwitchBytes);
};

#endif // !defined(AFX_MYFRMPARSE_H__1AD52466_5898_4762_B5F7_DA665B9B6484__INCLUDED_)
