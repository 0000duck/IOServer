/*
 * DrvVersion.h �������ð汾���ƺ궨��
 *
 *  �������������������ͱ�־�Ͱ汾�ű�־��
 */
#pragma once

// �������ͱ�־ �� DWORD���ͣ���ʹ��CString��Ϊ�������ͱ�־������Ϊ����ṩ���Ϸ���
//                 ���л�����ʱ������CString���Ϳ��ܻ�����쳣����DWORD����һ�㲻���׳����쳣
#define DRVCFG_FLAG_OPCCLIENT			0x00000001		// OPC����
#define DRVCFG_FLAG_DBPROXY				0x00000002		// Ŀ�����������
#define DRVCFG_FLAG_ODBC				0x00000003		// ODBC����
#define DRVCFG_FLAG_MODBUS				0x00000004		// Modbus����

#define DRVCFG_FLAG_USER				0x0000FFFF		// ʹ�ÿ��������Ӹ�ֵ���壬���Ҿ�����Ҫ�ظ�
														// �û���������ʱ�������ȶ��壺
														// #define DRVCFG_FLAG_MYDRV	(DRVCFG_FLAG_USER + 100)
														// Ȼ��Ϳ���ʹ�ø��������ͱ�־��	

// �汾�ű�־ �� WORD���ͣ����Դ�0��ʼ��ÿ����һ�ΰ汾���汾��ֵ��һ��Ҳ�����Լ����壬
//               ����WORD���ֽ�Ϊ���汾�ţ����ַ�Ϊ���汾��
#define DRVCFG_VERSION_USER				0x0000			// �汾�Ż�ֵ