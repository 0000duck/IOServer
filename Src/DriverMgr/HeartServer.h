#pragma once
#include "asyntcp/asyntcp.h"
//��������
class CHeartServer : public CAsynTcpServer
{
public:
	CHeartServer();
	virtual ~CHeartServer();

public:
	int GetConnectNum(){
		return this->m_conpool.GetConnectNum();
	};
protected:

	//��Ҫ���ص�
protected:
	virtual CAsynThread* CreateThread();
	virtual BOOL	OnLogin(SOCKET s,LPCTSTR szip,CString &szUser,CString &szActor); //Accept��ĵ�¼����
};

