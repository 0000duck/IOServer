// DBPQualityCodeDef.h
#pragma once

// ���������������
#define DBP_QUALITY_GOOD					0x00	// GOOD
#define DBP_QUALITY_BAD						0x01	// ��ֵ����û���ض�ԭ��
#define DBP_QUALITY_BAD_CONFIG_ERROR		0x02	// ���������ô������磬��ǩ�����еķ�����������ɾ����
#define DBP_QUALITY_BAD_NOT_CONNECTED		0x03	// ����Ҫ�����ӵ�����Ŀ�꣬���޷��ɹ����ӡ�������˵���ڸ�ʱ����ϵ�ֵ�����ţ���Ϊ��һ������Դδ�ṩ
#define DBP_QUALITY_BAD_DEVICE_FAIL			0x04	// ��⵽�豸����
#define DBP_QUALITY_BAD_SENSOR_FAIL			0x05	// ��⵽����������
#define DBP_QUALITY_BAD_LAST				0x06	// �豸���������ͨѶʧ�ܣ����ǣ������Чֵ�ǿ��õġ���ע�⣬ʱ���ǵ������һ����Чֵ��ʱ�꣬���������������ʱ��
#define DBP_QUALITY_BAD_COMM_FAIL			0x07	// �豸���������ͨѶʧ�ܣ�����û�������õ���Чֵ
#define DBP_QUALITY_BAD_OUT_OF_SERVICE		0x08	// �ñ�ǩ���鱻����Ϊ�ǻ����ʾ�ñ�ǩ����ĳ��ԭ�򲻱�����������

#define DBP_QUALITY_UNCERTAIN				0xE0	// ֵ��Ч�Բ�ȷ������û���ض�ԭ��
#define DBP_QUALITY_UNCERTAIN_LAST			0xE1	// �Ѿ�ֹͣд������ֵ����ֵӦ����Ϊ����ȥֵ������ע�⣬��״̬��ʾΪ�ڹ涨ʱ���ڱ�ǩδ�����¡���Bad/Last Usable Value��ʾΪ��⵽ͨѶ������
#define DBP_QUALITY_UNCERTAIN_SENSOR		0xE2	// ֵ�������������̷�Χ���ߴ������ڲ����Ϊ������׼ȷ
#define DBP_QUALITY_UNCERTAIN_EU_EXCEEDED	0xE3	// ����ֵ�����˸�ֵ����ķ�Χ����ע�⣬��״̬������ζ�Ÿ�ֵ�ǳ��޵�
#define DBP_QUALITY_UNCERTAIN_SUB_NORMAL	0xE4	// ��ֵ���Զ��Դ��������״̬ΪGOOD������Դ�������ڹ涨ֵ

#define DBP_QUALITY_CALC_OFF				0xF0	// ����ֹͣ

#define DBP_QUALITY_LOLO_LIMIT				0xF9	// �͵ͱ�
#define DBP_QUALITY_LO_LIMIT				0xFA	// �ͱ�
#define DBP_QUALITY_HI_LIMIT				0xFB	// �߱�
#define DBP_QUALITY_HIHI_LIMIT				0xFC	// �߸߱�
#define DBP_QUALITY_NOT_INIT			0xFD	// �ű���������δ����ʼ��
#define DBP_QUALITY_SHUTDOWN			0xFE	// �������ر�
#define DBP_QUALITY_UNKOWN					0xFF	// δ֪����

typedef struct tagDBPQUALITYITEM
{
	short nQualityCode;
	LPCTSTR lpszQualityName;
	LPCTSTR lpszQualityDescr;
}DBPQUALITYITEM, *LPDBPQUALITYITEM;

LPDBPQUALITYITEM FindQualityItem(short nQualityCode);
int GetQualityDefaultCount();
LPDBPQUALITYITEM GetQualityDefaultList();

