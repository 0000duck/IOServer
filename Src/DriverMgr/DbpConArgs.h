#pragma once
#include "Define.h"


class CDbpConArgs
{
public:
	CDbpConArgs(void);
	~CDbpConArgs(void);

	TDBPARG m_args[MAX_DBP_CONARGS]; //�ռ�
	int     m_nsize; //��������

	CString m_szfile;

	BOOL LoadConArgs();//�������ò���
	BOOL SaveConArgs();//�������ò���
};

