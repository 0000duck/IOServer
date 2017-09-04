// LogConstDef.h
#pragma once

// ��ǰ��¼�û�
#define LOG_LOGIN_USER	(theApp.GetOperator().IsEmpty()? _T("δ��¼") : theApp.GetOperator())

// ��־��ʽ���ı�
#define LOG_ADD_DEVICE_FORMAT	_T("<%s> ����豸 <%s>")
#define LOG_INSERT_TAG_FORMAT	_T("<%s> ������ǩ�� <%s>")
#define LOG_UPDATE_TAG_FORMAT	_T("<%s> ���±�ǩ�� <%s>")
#define LOG_DELETE_TAG_FORMAT	_T("<%s> ɾ����ǩ�� <%s>")


#define DEFAULT_LOGFILE_SIZE_MAX	2097152 //���LOG�ļ���С2M 

// ��־��Ϣ����
#define LOG_LEVEL_ERROR		0x00	// ���󼶱�
#define LOG_LEVEL_WARNING	0x01	// ���漶��
#define LOG_LEVEL_INFO		0x02	// ��ʾ����
#define LOG_LEVEL_DEBUG		0x03	// Debug����

// ��־��������
#define LOG_OPT_DISPLAY		0x01	// �ڴ�������ʾ��־
#define LOG_OPT_STORAGE		0x02	// д��־����־�ļ�
#define LOG_OPT_ALL			(LOG_OPT_DISPLAY | LOG_OPT_STORAGE)

#define MAX_LOG_MESSAGE_LEN	1024
/// �����������Ŀ�����־��
#define DEFAULT_MAX_LOG_FREE_COUNT	1024
