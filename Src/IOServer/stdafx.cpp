
// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// IOServer.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"



BOOL IsDigtal(LPCTSTR stxt)
{
	LPCTSTR pc = stxt;
	if(pc == NULL)
		return FALSE;
	if(*pc == 0)
		return FALSE;
	int ndot = 0;
	int nf = 0;
	while(*pc != 0)
	{
		if(!isdigit(*pc))
		{
			if((*pc == '+') || (*pc == '-'))
			{
				nf++;
				if(nf >1)
					return FALSE;
			}
			else if(*pc == '.')
			{
				ndot++;
				if(ndot > 1)
					return FALSE;
			}
			else
				return FALSE;
		}
		pc++;
	}
	return TRUE;
}

