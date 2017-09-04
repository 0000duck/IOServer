#include "StdAfx.h"
#include "Dec2TagVal.h"

void CDec2TagVal::AddItem(CDec2TagVal* pTagVal)
{
	if (m_pNext)
		pTagVal->m_pNext = m_pNext;

	m_pNext = pTagVal;
}

BOOL CDec2TagVal::IsDataEqual(TVVAL *pval1, TVVAL *pval2)
{
	if (pval1->ltime != pval2->ltime)
		return FALSE;

	if (pval1->snqa != pval2->snqa)
		return FALSE;

	if (pval1->vt != pval2->vt)
		return FALSE;

	switch(pval1->vt)
	{
	case DT_STRING:
		if (pval1->lvlen != pval2->lvlen)
			return FALSE;

		if (!pval1->lvlen || !pval2->lvlen)
			return FALSE;

		if (strcmp(pval1->pstr, pval2->pstr) != 0)
			return FALSE;
		break;
	case DT_BLOB:
		if (pval1->lvlen != pval2->lvlen)
			return FALSE;

		if (!pval1->lvlen || !pval2->lvlen)
			return FALSE;

		if (memcmp(pval1->pblob, pval2->pblob, pval1->lvlen) != 0)
			return FALSE;
		break;
	default:
		if (pval1->llval != pval2->llval)
			return FALSE;
		break;
	}

	return TRUE;
}

void CDec2TagVal::OnDrvTagDataOk(CDecVal* pdecval,LPDRREC pdv)//��������OK
{
	// ������ʱ����ڵ�ǰʱ��ֵ�Ŀ���
	//if (theApp.m_Config.m_bSkipSameTime && (m_val.ltime >= pdv->val.ltime))
	//	return;
	//}}

#if 0
	if ((strcmp(m_szname, "REAL_MW") == 0) && ((m_val.fval < 100.0) || (m_val.fval > 400.0)))
	{
		GetLogger()->AddInfoFmt(_T("DDClient�յ���ǩ<%s>��ֵ = %s, ʱ�� = %s"), m_szname, m_val.GetValStr(), m_val.GetTimeStr());
	}
#endif

	int i,n = m_tab.GetSize();
	if (n == 0)
		return;

	if (IsDataEqual(&m_val, &pdv->val)&& n==1)
		return;

	long lupdatetime = (long)::time(NULL);//����ʱ��
	CVarVal vv(DT_VOID);
	CTagVal* ptv;
	double dblVal = 0;

	CVarVal vCurValue = *((CVarVal*)&pdv->val);

	// û�б��ʽ��ֱ�Ӽ򻯴���
	if (n == 1)
	{
		// �������һ��д��ֵ�������´ι����Ƿ�Ϊ�ظ�ֵ
		m_val.SetVoid();
		m_val = vCurValue;

		ptv = m_tab.GetAt(0);//ȡһ��

		CSpinLockGuard lock(&ptv->m_lock);
		ptv->m_val = vCurValue;
		ptv->m_nUpdate = 1;
		ptv->m_lsendtime = lupdatetime;//����ʱ��

		pdecval->m_bvardataok = TRUE;

		return;
	}

	// ���ڴ����ʽ�ı�ǩ����ʹ�����´������⴦��

	BOOL bCurrentTagCalc = FALSE;

	// �ӵ�2����ʼ���ת��
	for (i = 1; i < n; i++)
	{
		ptv = m_tab.GetAt(i);//ȡһ��

		// ��ǰ��ǩ�Ƿ��Ѿ��������
		if (ptv == m_tab.GetAt(0))
			bCurrentTagCalc = TRUE;

		if (ptv->m_pEngine == NULL)
		{
			ptv->m_pEngine = new CExpressionEngine(TRUE);
			if (ptv->m_pEngine) {
				ptv->m_pEngine->m_pdecval = pdecval;
				ptv->m_pEngine->SetExpr(ptv->m_szExpcp);
			}
			//return;
		}
		if (ptv->m_pEngine)
		{
			// ���õ�ǰֵ
			if (ptv == m_tab.GetAt(0)) // ���㵱ǰ��ǩ������ʹ�ñ�ǩ����ֵ��Ӧ���������ϴ�ֵ
			{
				ptv->m_pEngine->SetThisTag( vCurValue.GetDouble(), vCurValue.ltime, vCurValue.snqa);
			}
			else 
			{
				ptv->m_pEngine->SetThisTag( ptv->m_val.GetDouble(),
					(ptv->m_val.ltime < vCurValue.ltime) ? vCurValue.ltime : ptv->m_val.ltime,
					(abs(ptv->m_val.snqa) < abs(vCurValue.snqa)) ? vCurValue.snqa : ptv->m_val.snqa	);
			}

			if (ptv->m_pEngine->Eval(dblVal))
			{
				if (ptv == m_tab.GetAt(0))
				{
					// �������һ��д��ֵ�������´ι����Ƿ�Ϊ�ظ�ֵ
					m_val.SetVoid();
					m_val = vCurValue;
				}

				vv.ResetType(DT_VOID);
				if(ptv->m_nType == DT_DIGITAL || ptv->m_nType == DT_INT32)
					vv = (long)dblVal;
				else if(ptv->m_nType == DT_INT64)
					vv = (__int64)dblVal;
				else if(ptv->m_nType == DT_FLOAT32)
					vv = (float)dblVal;
				else if(ptv->m_nType == DT_FLOAT64)	
					vv = dblVal;

				vv.ltime = ptv->m_pEngine->m_lTimeMax;
				vv.snqa  = ptv->m_pEngine->m_nQualityMax;
				vv.snerr = pdv->val.snerr;

				CSpinLockGuard lock(&ptv->m_lock);
				ptv->m_val = vv;
				ptv->m_nUpdate = 1;
				ptv->m_lsendtime = lupdatetime;//����ʱ��
			}
			else 
			{
				// ������Calc Off
			}
		}
	}

	// �����ǰ��ǩδ������㣬����Ҫ���ø�ֵ
	if (!bCurrentTagCalc)
	{
		// �������һ��д��ֵ�������´ι����Ƿ�Ϊ�ظ�ֵ
		m_val.SetVoid();
		m_val = vCurValue;

		ptv = m_tab.GetAt(0);//ȡһ��

		/*
		CString strExp(ptv->m_szExpcp),strName(m_szname); 
		strExp.MakeLower();strName.MakeLower();

		int iPos = strExp.Find("this"); int iPos2 = strExp.Find(strName);

		if (n>1 && iPos < 0 && iPos2 < 0)
		{
			continue;
		}
		*/

		CSpinLockGuard lock(&ptv->m_lock);
		ptv->m_val = vCurValue;
		ptv->m_nUpdate = 1;
		ptv->m_lsendtime = lupdatetime;//����ʱ��

		pdecval->m_bvardataok = TRUE;
	}
}


