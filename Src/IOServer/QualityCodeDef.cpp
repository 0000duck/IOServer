#include "stdafx.h"
#include "QualityCodeDef.h"

static DBPQUALITYITEM arrDBPQualityList[] = {
	{
		DBP_QUALITY_GOOD, 
		_T("GOOD"), 
		_T("����ֵ")
	},
	{
		DBP_QUALITY_BAD, 
		_T("BAD"), 
		_T("��ֵ����û���ض�ԭ��")
	},
	{
		DBP_QUALITY_BAD_CONFIG_ERROR, 
		_T("CONFIG_ERROR"), 
		_T("���������ô������磬��ǩ�����еķ�����������ɾ����")
	},
	{
		DBP_QUALITY_BAD_NOT_CONNECTED, 
		_T("NOT_CONNECTED"), 
		_T("����Ҫ�����ӵ�����Ŀ�꣬���޷��ɹ����ӡ�������˵���ڸ�ʱ����ϵ�ֵ�����ţ���Ϊ��һ������Դδ�ṩ")
	},
	{
		DBP_QUALITY_BAD_DEVICE_FAIL, 
		_T("DEVICE_FAIL"), 
		_T("��⵽�豸����")
	},
	{
		DBP_QUALITY_BAD_SENSOR_FAIL, 
		_T("SENSOR_FAIL"), 
		_T("��⵽����������")
	},
	{
		DBP_QUALITY_BAD_LAST, 
		_T("BAD_LAST"), 
		_T("�豸���������ͨѶʧ�ܣ����ǣ������Чֵ�ǿ��õġ���ע�⣬ʱ���ǵ������һ����Чֵ��ʱ�꣬���������������ʱ��")
	},
	{
		DBP_QUALITY_BAD_COMM_FAIL, 
		_T("COMM_FAIL"), 
		_T("�豸���������ͨѶʧ�ܣ�����û�������õ���Чֵ")
	},
	{
		DBP_QUALITY_BAD_OUT_OF_SERVICE, 
		_T("OUT_OF_SERVICE"), 
		_T("�ñ�ǩ���鱻����Ϊ�ǻ����ʾ�ñ�ǩ����ĳ��ԭ�򲻱�����������")
	},
	{
		DBP_QUALITY_UNCERTAIN, 
			_T("UNCERTAIN"), 
			_T("ֵ��Ч�Բ�ȷ������û���ض�ԭ��")
	},
	{
		DBP_QUALITY_UNCERTAIN_LAST, 
			_T("UNCERTAIN_LAST"), 
			_T("�Ѿ�ֹͣд������ֵ����ֵӦ����Ϊ����ȥֵ������ע�⣬��״̬��ʾΪ�ڹ涨ʱ���ڱ�ǩδ�����¡���Bad/Last Usable Value��ʾΪ��⵽ͨѶ������")
	},
	{
		DBP_QUALITY_UNCERTAIN_SENSOR, 
			_T("SENSOR_NOT_ACCURATE"), 
			_T("ֵ�������������̷�Χ���ߴ������ڲ����Ϊ������׼ȷ")
	},
	{
		DBP_QUALITY_UNCERTAIN_EU_EXCEEDED, 
			_T("EU_EXCEEDED"), 
			_T("����ֵ�����˸�ֵ����ķ�Χ����ע�⣬��״̬������ζ�Ÿ�ֵ�ǳ��޵�")
	},
	{
		DBP_QUALITY_UNCERTAIN_SUB_NORMAL, 
			_T("SUB_NORMAL"), 
			_T("��ֵ���Զ��Դ��������״̬ΪGOOD������Դ�������ڹ涨ֵ")
	},
	{
		DBP_QUALITY_CALC_OFF, 
			_T("CALC_OFF"), 
			_T("����ֹͣ")
	},
	{
		DBP_QUALITY_LOLO_LIMIT, 
		_T("LOLO_LIMIT"), 
		_T("�͵ͱ�")
	},
	{
		DBP_QUALITY_LO_LIMIT, 
		_T("LO_LIMIT"), 
		_T("�ͱ�")
	},
	{
		DBP_QUALITY_HI_LIMIT, 
		_T("HI_LIMIT"), 
		_T("�߱�")
	},
	{
		DBP_QUALITY_HIHI_LIMIT, 
		_T("HIHI_LIMIT"), 
		_T("�߸߱�")
	},
	{
		DBP_QUALITY_NOT_INIT, 
			_T("NOT_INIT"), 
			_T("�½���ǩ����δ����ʼ��")
	},
	{
		DBP_QUALITY_SHUTDOWN, 
			_T("SHUTDOWN"), 
			_T("�������ر�")
	},
	{
		DBP_QUALITY_UNKOWN, 
		_T("UNKOWN"), 
		_T("δ֪����ֵ")
	}
};

class CDBPQualityMap
{
public:
	CDBPQualityMap() {
		int nCount = sizeof(arrDBPQualityList) / sizeof(DBPQUALITYITEM);
		int nHashSize = ((int)(nCount * 2.4)) / 2 + 1;

		m_mapDBPQuality.InitHashTable( nHashSize );

		for(int i = 0; i < nCount; i++)
		{
			m_mapDBPQuality.SetAt(arrDBPQualityList[i].nQualityCode, &arrDBPQualityList[i]);
		}

		m_pUnkown = &arrDBPQualityList[nCount - 1];
	}
	~CDBPQualityMap() {}

	LPDBPQUALITYITEM FindQualityItem(short nQualityCode) {
		LPDBPQUALITYITEM pItem = NULL;
		m_mapDBPQuality.Lookup(nQualityCode, pItem);

		if (!pItem)
			pItem = m_pUnkown;

		return pItem;
	}
protected:
private:
	LPDBPQUALITYITEM	m_pUnkown;
	CMap<short, short, LPDBPQUALITYITEM, LPDBPQUALITYITEM>	m_mapDBPQuality;
};

static CDBPQualityMap gDBPQualityMap;

LPDBPQUALITYITEM FindQualityItem(short nQualityCode)
{
	return gDBPQualityMap.FindQualityItem(nQualityCode);
}

int GetQualityDefaultCount()
{
	int nCount = sizeof(arrDBPQualityList) / sizeof(DBPQUALITYITEM);
	return nCount;
}

LPDBPQUALITYITEM GetQualityDefaultList()
{
	return arrDBPQualityList;
}
