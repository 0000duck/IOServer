// MyFrmParse.cpp: implementation of the CMyFrmParse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyFrmParse.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyFrmParse::CMyFrmParse()
{

}

CMyFrmParse::~CMyFrmParse()
{

}

/*
分包处理，根据实际情况改动分包算法

  返回值：
	0:没找到报头标志
	1:找到报头标志,长度不够无法验证报头
	2:报头校验完成,数据长度不够
	3:报文完成,
  */
//////////////////////////////////////////////////////////////////////////

BYTE    CMyFrmParse::GetByte(BYTE btData, char cType)
{
	BYTE btRet = 0;
	if (cType == GET_BYTE_HIGH)
	{
		btRet = btData &0xF0;
		btRet = btRet>>4;
	}
	else
	{
		btRet = btData &0x0F;
	}
	return   btRet;
}

BYTE  CMyFrmParse::CalaCS(BYTE *pData, int iSize)
{
	BYTE byCS = 0;
	if (pData == NULL || iSize <= 0)
	{       return byCS;      }

	for (int i = 0 ; i < iSize; i++ )
	{
		byCS += pData[i];
	}

	return byCS;
}

//////////////////////////////////////////////////////////////////////////

// 字节序转换
short CMyFrmParse::SwitchBytes(short wBytes,BOOL bSwitchBytes)
{
	if (bSwitchBytes)
	{
		BYTE btVal[2];
		BYTE *p = (BYTE *)&wBytes;
		btVal[0] = p[1];
		btVal[1] = p[0];

		return *((WORD*)btVal);
	}
	else {
		return wBytes;
	}
}

DWORD CMyFrmParse::SwitchBytes(DWORD wBytes,BOOL bSwitchBytes)
{
	if (bSwitchBytes)
	{
		BYTE btVal[4];
		BYTE *p = (BYTE *)&wBytes;
		btVal[0] = p[3];
		btVal[1] = p[2];
		btVal[2] = p[1];
		btVal[3] = p[0];
		return  *((DWORD*)btVal);
	}
	else {
		return wBytes;
	}
}

// 字节序转换
float CMyFrmParse::SwitchBytes(float fltBytes,BOOL bSwitchBytes)
{

	if (bSwitchBytes)
	{
		BYTE btVal[4];
		BYTE *p = (BYTE *)&fltBytes;
		btVal[0] = p[3];
		btVal[1] = p[2];
		btVal[2] = p[1];
		btVal[3] = p[0];
		return *((float*)btVal);
	}
	else {
		return fltBytes;
	}
}

DWORD CMyFrmParse::SwitchBytes(BYTE * pBytes,BOOL bSwitchBytes)
{
	if ( pBytes == NULL )
	{
		return 0;
	}

	BYTE btVal[4];

	if (bSwitchBytes)
	{
		btVal[0] = pBytes[0];
		btVal[1] = pBytes[1];
		btVal[2] = pBytes[2];
		btVal[3] = pBytes[3];
	}
	else {
		btVal[0] = pBytes[3];
		btVal[1] = pBytes[2];
		btVal[2] = pBytes[1];
		btVal[3] = pBytes[0];
	}

	return *((DWORD*)btVal);
}
