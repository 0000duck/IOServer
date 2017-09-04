#ifndef __KIMI_TPRIUGDATASOURCE_H__
#define __KIMI_TPRIUGDATASOURCE_H__

//#include "Ultimate Grid\UGCtrl.h"

_KIMI_NAMESPACE__BEGIN

class TPRICOMMON_CLASS CTPRIUGDataSource : public CUGDataSource
{
public:
	CTPRIUGDataSource(void);
	virtual ~CTPRIUGDataSource(void);

	CTPRIUGDataSource(const CTPRIUGDataSource& );
	CTPRIUGDataSource operator=(const CTPRIUGDataSource&);

	CSortObArray m_array;

	void RemoveHead() {
		m_arrHeadSize.RemoveAll();
		m_arrHeadText.RemoveAll();
	}
	void AddHead(int nSize, LPCTSTR lpszText) {
		m_arrHeadSize.Add(nSize);
		m_arrHeadText.Add(lpszText);
	}
	int GetHeadSize(int nIndex) {
		return m_arrHeadSize[nIndex];
	}
	LPCTSTR GetHeadText(int nIndex) {
		return m_arrHeadText[nIndex];
	}

public:	
	int RefreshGrid( void );
	/////////////////////////////////////////////////////////////////////
	// CUGDataSource ���غ��� - ��Ϊ����һ��ֻ��������Դ������ֻ��Ҫ���ؼ��������ľͿ�����
	virtual long GetNumRows();
	virtual int GetNumCols();
	virtual int	GetCell(int col,long row,CUGCell *cell);
	virtual int FindNext(CString *string,int *col,long *row,int flags);
	virtual int SortBy(int col,int flags);

protected:
	CArray<int, int>	m_arrHeadSize;
	CStringArray		m_arrHeadText;
};

bool operator==(const CTPRIUGDataSource&, const CTPRIUGDataSource&);
bool operator!=(const CTPRIUGDataSource&, const CTPRIUGDataSource&);

_KIMI_NAMESPACE__END

#endif // end of __KIMI_TPRIUGDATASOURCE_H__