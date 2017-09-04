// WinTagInfo.h: interface for the CWinTagInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINTAGINFO_H__50A4EBA6_5F5E_4D93_9371_A6479675C59F__INCLUDED_)
#define AFX_WINTAGINFO_H__50A4EBA6_5F5E_4D93_9371_A6479675C59F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWinTagInfo : public CWinList  
{
public:
	CWinTagInfo();
	virtual ~CWinTagInfo();
	
public:
	CTagDefTab	*m_pTagtab;	//��ǩ��

public:
	int		GetNextSelected(int nstart);//����ѡ��
	void	ReDraw();
	void	RedrawList();
	
protected:
	virtual CString GetCellText(int nrow,int ncol);//ȡ��ʾ�ַ���
	virtual CString GetHeadText(int ncol);//ȡ��ʾ�ַ���
	virtual void	GetRowColorS(int nrow,int ncol,COLORREF &clrTxt,COLORREF &clrBk);//ȡ����ɫ
	virtual void	OnClickHeadCol(int ncol);//�������ͷ
	
	virtual void	OnCurSelChange(int nrow);
	virtual void	OnSelChange(int nrowstart,int nrowend);
	virtual void	OnCtrlSelChange(int nrow);//CTRL��סʱ��ѡ��
};

#endif // !defined(AFX_WINTAGINFO_H__50A4EBA6_5F5E_4D93_9371_A6479675C59F__INCLUDED_)
