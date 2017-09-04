#include "stdafx.h"
#include "ExpressionEngine.h"

/// �ɱ�ǩ������ǩ�ĵ�ǰֵ
mu::value_type Expre_GetTagValue(unsigned long dwUserData, const mu::char_type *lpszTagName) 
{
	CExpressionEngine *pEngine = (CExpressionEngine*)dwUserData;
	if (NULL == pEngine)
		return 0.0;
	if (pEngine->m_pdecval == NULL)
		return 0.0;

	CTagVal *pv = pEngine->m_pdecval->GetTagVal(lpszTagName);
	if (pv) {
		//TRACE(_T("%s = %f\n"), lpszTagName, pv->m_val.GetDouble());
		if (pEngine->m_lTimeMax < pv->m_val.ltime)
			pEngine->m_lTimeMax = pv->m_val.ltime;

		if (abs(pEngine->m_nQualityMax) < abs(pv->m_val.snqa))
			pEngine->m_nQualityMax = pv->m_val.snqa;

		return pv->m_val.GetDouble();
	}
	else
		return 0.0;
}

/// �����ʽ��Ч��ʱ������ȡ��ǰ���ʽ��ʹ������Щ��ǩ
mu::value_type Check_GetTagValue(unsigned long dwUserData, const mu::char_type *lpszTagName) 
{
	CExpressionEngine *pEngine = (CExpressionEngine*)dwUserData;

	CExpressionEngine::AddTag(lpszTagName);
	return 0.0;
}

/// ��ֹ���㣬���a==true������ֹ���㣬���򷵻�b
mu::value_type Expre_CalcOff(mu::value_type a, mu::value_type b)
{
	long bCheck = (long)a;

	if (a != 0)
		throw mu::Parser::exception_type( mu::ecCALC_OFF );
	else
		return b;
}

mu::value_type Check_CalcOff(mu::value_type a, mu::value_type b)
{
	return b;
}

/// ��λ�������������C�е� &
mu::value_type Expre_BitAnd(mu::value_type a, mu::value_type b)
{
	return (long)a & (long)b;
}

/// ��λ�������������C�е� |
mu::value_type Expre_BitOr(mu::value_type a, mu::value_type b)
{
	return (long)a | (long)b;
}

/// ��λ��������������C�е� ^
mu::value_type Expre_BitXor(mu::value_type a, mu::value_type b)
{
	return (long)a ^ (long)b;
}

/// ��λ�������������C�е� ~
mu::value_type Expre_BitNot(mu::value_type a)
{
	return ~(long)a;
}

/// ��λ���������������C�е� <<
mu::value_type Expre_Shll(mu::value_type a, mu::value_type b)
{
	return (long)a << (long)b;
}

/// ��λ���������������C�е� >>
mu::value_type Expre_Shlr(mu::value_type a, mu::value_type b)
{
	return (long)a >> (long)b;
}

/// ��̬��Ա����
CStringArray CExpressionEngine::m_tags;

//////////////////////////////////////////////////////////////////////////
// CExpressionEngine
//////////////////////////////////////////////////////////////////////////

CExpressionEngine::CExpressionEngine(void)
{
	m_pdecval = NULL;
	m_dblTagValue = 0.0;
}

CExpressionEngine::CExpressionEngine(BOOL bCalcMode)
{
	m_pdecval = NULL;
	m_dblTagValue = 0.0;

	Init( bCalcMode );
}

CExpressionEngine::~CExpressionEngine(void)
{
}

void CExpressionEngine::Init(BOOL bCalcMode/* = TRUE*/)
{
	m_parser.m_dwUserData = (DWORD)this;

	if (bCalcMode) {
#ifdef _DEBUG
		m_parser.EnableOptimizer(false); // ���Ż��������������ֽ���
#endif

		m_parser.DefineFun(_T("TV"), Expre_GetTagValue, false);
		m_parser.DefineFun(_T("tv"), Expre_GetTagValue, false);

		m_parser.DefineFun(_T("CalcOff"), Expre_CalcOff, false);
	}
	else { // ���㹫ʽ���ģʽ
		m_parser.EnableOptimizer(false);

		m_parser.DefineFun(_T("TV"), Check_GetTagValue, false);
		m_parser.DefineFun(_T("tv"), Check_GetTagValue, false);

		m_parser.DefineFun(_T("CalcOff"), Check_CalcOff, false);
	}

	// �Զ��尴λ�����
	m_parser.DefineOprt(_T("&"), Expre_BitAnd, mu::prLOGIC);
	m_parser.DefineOprt(_T("|"), Expre_BitOr, mu::prLOGIC);
	m_parser.DefineOprt(_T("bxor"), Expre_BitXor, mu::prLOGIC);
	m_parser.DefineInfixOprt(_T("~"), Expre_BitNot, mu::prPOW);
	m_parser.DefineOprt(_T("<<"), Expre_Shll, mu::prMUL_DIV + 1);
	m_parser.DefineOprt(_T(">>"), Expre_Shlr, mu::prMUL_DIV + 1);

	// ע���ǩ�ĵ�ǰֵ
	m_parser.DefineVar(_T("this"), &m_dblTagValue);
}


void CExpressionEngine::SetExpr(LPCTSTR lpszExpr)
{
	m_parser.SetExpr(lpszExpr);
}

void CExpressionEngine::SetThisTag(double dblValue, long lTimeMax, short nQualityMax)
{
	m_dblTagValue = dblValue;
	m_lTimeMax = lTimeMax;
	m_nQualityMax = nQualityMax;
}

BOOL CExpressionEngine::Eval(double &dblResult)
{
	try
	{
		dblResult = m_parser.Eval();
		return TRUE;
	}
	catch (mu::Parser::exception_type &e)
	{
		e;
		/*
		mu::console() << _T("\nError:\n");
		mu::console() << _T("------\n");
		mu::console() << _T("Message:     ")   << e.GetMsg()   << _T("\n");
		mu::console() << _T("Expression:  \"") << e.GetExpr()  << _T("\"\n");
		mu::console() << _T("Token:       \"") << e.GetToken()    << _T("\"\n");
		mu::console() << _T("Position:    ")   << (int)e.GetPos() << _T("\n");
		mu::console() << _T("Errc:        ")   << std::dec << e.GetCode() << _T("\n");
		*/
		dblResult = 0.0;

		//if (e.GetCode() == mu::ecCALC_OFF)
		//{
			// ֹͣ����
		//}

		return FALSE;
	}
	catch (...)
	{
		dblResult = 0.0;

		return FALSE;
	}
}

BOOL CExpressionEngine::Check(LPCTSTR lpszTagName)
{
	try
	{
		m_parser.Eval();
		mu::varmap_type map = m_parser.GetUsedVar();
		for(mu::varmap_type::iterator itr = map.begin(); itr != map.end(); itr++)
		{
			// �Ƿ�ʹ�õ�ǰ��ǩ
			if (strcmp("this", itr->first.c_str()) == 0)
				AddTag( lpszTagName );
			//TRACE("Item: %s found, content: %f\n", itr->first.c_str(), itr->second);
		}
		return TRUE;
	}
	catch (mu::Parser::exception_type &e)
	{
		e;
		/*
		mu::console() << _T("\nError:\n");
		mu::console() << _T("------\n");
		mu::console() << _T("Message:     ")   << e.GetMsg()   << _T("\n");
		mu::console() << _T("Expression:  \"") << e.GetExpr()  << _T("\"\n");
		mu::console() << _T("Token:       \"") << e.GetToken()    << _T("\"\n");
		mu::console() << _T("Position:    ")   << (int)e.GetPos() << _T("\n");
		mu::console() << _T("Errc:        ")   << std::dec << e.GetCode() << _T("\n");
		*/

		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
}