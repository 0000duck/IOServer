#include "StdAfx.h"
#include "CGeneralAPI.h"
#include "CGeneral\TPRIUGGrid.h"

_KIMI_NAMESPACE__BEGIN

CTPRIUGGrid::CTPRIUGGrid(void)
:	m_pDataSource(NULL)
{
	UGXPThemes::UseThemes(false);
	m_head = NULL;	

	// customize the display
	m_nRedrawDelay = 1000;
}

CTPRIUGGrid::~CTPRIUGGrid(void)
{
	UGXPThemes::CleanUp();
	// clean up ...
	if ( m_head != NULL )
	{
		UGDrwHintVList *pTemp = m_head;
		UGDrwHintVList *pDelTemp = NULL;

		while( pTemp != NULL )
		{
			pDelTemp = pTemp;
			pTemp = pTemp->next;
			delete pDelTemp;
		}
	}
}

BEGIN_MESSAGE_MAP(CTPRIUGGrid, CUGCtrl)
	//{{AFX_MSG_MAP(CTPRIUGGrid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CTPRIUGGrid::HasSelected()
{
	for(int i = 0; i < GetNumberRows(); i++)
	{
		if (IsSelected(0, i))
			return TRUE;
	}

	return FALSE;
}

int CTPRIUGGrid::RefreshGrid( void )
{
	return m_pDataSource->RefreshGrid();
}

void CTPRIUGGrid::RedrawScrollBar()
{
	CScrollBar *pBar = GetScrollBarCtrl(SB_HORZ);
	if (pBar)
		pBar->Invalidate();

	pBar = GetScrollBarCtrl(SB_VERT);
	if (pBar)
		pBar->Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
//	OnSetup
//		This function is called just after the grid window 
//		is created or attached to a dialog item.
//		CCalcTaskGrid can be used to initially setup the grid
void CTPRIUGGrid::OnSetup()
{
	m_fntText.CreateFont(12, 0, 0, 0, FW_REGULAR, 0, 0, 0, GB2312_CHARSET, 0, 0, 
		PROOF_QUALITY, VARIABLE_PITCH | FF_ROMAN, _T("����"));
	SetDefFont(&m_fntText);

	SetVScrollMode( UG_SCROLLTRACKING );	// ��ֱ����������ģʽ
	SetHScrollMode( UG_SCROLLTRACKING );	// ˮƽ����������ģʽ
	SetDoubleBufferMode(TRUE);				// ˫����ģʽ
	SetHighlightRow(TRUE);					// �и���ѡ��
	SetMultiSelectMode(TRUE);				// ��ѡ
	//SetCurrentCellMode( 2 );
	
	// �����ͷ���
	SetColWidth(-1, 20);
	
	int index = AddDataSource(m_pDataSource);
	SetDefDataSource(index);
	SetGridUsingDataSource(index);
	
	// ��������
	m_iArrowIndex = AddCellType( &m_sortArrow );
	SetCurrentCellMode( 2 );
	//UseHints( TRUE );
	
	m_iSortCol = 0;
	m_bSortedAscending = TRUE;
	QuickSetCellType( m_iSortCol, -1, m_iArrowIndex );
	QuickSetCellTypeEx( m_iSortCol, -1, UGCT_SORTARROWUP );

	int i;	
	// ��ʼ����ͷ����ʾ�ı��Ϳ������
	for(i = 0 ; i < GetNumberCols() ; i++ )
	{
		SetColWidth(i, m_pDataSource->GetHeadSize(i));
	}

	m_pDataSource->SortBy(m_iSortCol, UG_SORT_ASCENDING);
}

void CTPRIUGGrid::UpdateSort()
{
	m_pDataSource->SortBy(m_iSortCol, m_bSortedAscending?UG_SORT_ASCENDING:UG_SORT_DESCENDING);
}

// ��������д�С
int CTPRIUGGrid::OnCanSizeCol(int col)
{
	return TRUE;
}

// ����������и߶�
int CTPRIUGGrid::OnCanSizeRow(long row)
{
	return FALSE;
}

// �����������ͷ�ĸ߶ȴ�С
int CTPRIUGGrid::OnCanSizeTopHdg()
{
	return FALSE;
}

// �����������ߵ�һ��ͷ�Ĵ�С
int CTPRIUGGrid::OnCanSizeSideHdg()
{
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//	OnGetCell
//		This message is sent everytime the grid needs to
//		draw a cell in the grid. At this point the cell
//		class has been filled with the information to be
//		used to draw the cell. The information can now be
//		changed before it is used for drawing
//
//	Warning:
//		This notification is called for each cell that needs to be painted
//		Placing complicated calculations here will slowdown the refresh speed.
//	Params:
//		col, row	- coordinates of cell currently drawing
//		cell		- pointer to the cell object that is being drawn
//	Return:
//		<none>
void CTPRIUGGrid::OnGetCell(int col,long row,CUGCell *cell)
{
	/*
	if ( col >= 0 && row == -1 )
	{	// set default values to the top heading
		cell->SetNumberDecimals( 0 );
		cell->SetNumber( col );
	}
	else if ( row >= 0 && col == -1 )
	{	// set default values to the side heading (row numbers)
		cell->SetNumberDecimals( 0 );
		cell->SetNumber( row );
	}
	else if ( col >= 0 && row >= 0 )
	{
		cell->SetNumberDecimals( 0 );
		cell->SetNumber( m_data.GetValue( col, row ));
		cell->SetAlignment( UG_ALIGNRIGHT );
	}
	*/
}

/////////////////////////////////////////////////////////////////////////////
//	RedrawCell_WCache
//		This version of RedrawCell will use redraw batching functionality
int CTPRIUGGrid::RedrawCell_WCache( int col, long row )
{
	if ( m_nRedrawDelay > 0 )
	{
		AddToCache( col, row );
	}
	else
	{	// pass the redraw message without delay for this you can use:
		// 1. PaintDrawHintsNow function, which is a better way for frequent updates
		CRect rect;
		GetCellRect(col,row,&rect);
		m_CUGGrid->m_drawHint.AddHint( col, row, col, row );
		
		if( GetCurrentRow() != row && GetCurrentCol() != col )
			TempDisableFocusRect();
		
		m_CUGGrid->PaintDrawHintsNow(&rect);
		
		// 2. or standard RedrawCell
		// RedrawCell( col, row );
	}
	
	return UG_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//	Cached drawing
//		Purpose of this function is to create a list of redraw requests.
//		This list will then be passed to the grid for painting.
void CTPRIUGGrid::AddToCache( int col, long row )
{
	// m_CUGGrid->m_drawHint.AddHint( col, row );
	// do not call AddHint directly, instead create a queue that will
	// temporarly hold coordinates of all cells marked for redraw
	// and then make calls to AddHint when ready to redraw
	
	UGDrwHintVList *pTemp = m_head;
	UGDrwHintVList *pNewItem = new UGDrwHintVList;
	
	// populate the structure
	pNewItem->Col = col;
	pNewItem->Row = row;
	pNewItem->next = NULL;
	
	// add new element to the list
	if ( m_head == NULL )
	{
		m_head = pNewItem;
	}
	else
	{
		while ( pTemp->next != NULL )
		{
			pTemp = pTemp->next;
		}
		pTemp->next = pNewItem;
	}
}

/////////////////////////////////////////////////////////////////////////////
//	DrawCached
//		This function will draw all cashed redraw requests in one sweep
void CTPRIUGGrid::DrawCached()
{
	UGDrwHintVList *pTemp = m_head;
	UGDrwHintVList *pDelTemp = NULL;
	
	// prevent new items from being added to the draw list
	m_head = NULL;
	
	// copy all of the cached draw hints, and
	// clear them as we go
	while( pTemp != NULL )
	{
		RECT rect;
		GetCellRect(pTemp->Col,pTemp->Row,&rect);
		m_CUGGrid->m_drawHint.AddHint(pTemp->Col,pTemp->Row,pTemp->Col,pTemp->Row);
		
		if(GetCurrentRow() != pTemp->Row || GetCurrentCol() != pTemp->Col)
			TempDisableFocusRect();
		
		m_CUGGrid->PaintDrawHintsNow(&rect);
		
		// delete cached draw hint
		pDelTemp = pTemp;
		pTemp = pTemp->next;
		delete pDelTemp;
	}
}

void CTPRIUGGrid::OnTH_LClicked( int iCol, long lRow, int iUpdn, RECT* pRect, POINT* pPoint, BOOL bProcessed )
{
	UNREFERENCED_PARAMETER(lRow);
	UNREFERENCED_PARAMETER(pRect);
	UNREFERENCED_PARAMETER(pPoint);
	UNREFERENCED_PARAMETER(bProcessed);
	
	if( iUpdn == 0)
		return;

	if (!CanSorted(iCol, lRow))
		return;
	
	QuickSetCellType( m_iSortCol, -1, 0 );
	
	if( iCol == m_iSortCol )
	{
		if( m_bSortedAscending )
			m_bSortedAscending = FALSE;
		else
			m_bSortedAscending = TRUE;
	}
	else
	{
		m_iSortCol = iCol;
		m_bSortedAscending = TRUE;
	}
	
	if( m_bSortedAscending )
	{
		m_pDataSource->SortBy( iCol, UG_SORT_ASCENDING );
		QuickSetCellType( m_iSortCol, -1, m_iArrowIndex );
		QuickSetCellTypeEx( m_iSortCol, -1, UGCT_SORTARROWUP );
	}
	else
	{
		m_pDataSource->SortBy( iCol, UG_SORT_DESCENDING );
		QuickSetCellType( m_iSortCol, -1, m_iArrowIndex );
		QuickSetCellTypeEx( m_iSortCol, -1,  UGCT_SORTARROWDOWN);
	}
	
	RedrawAll();
}

int CTPRIUGGrid::OnSortEvaluate( CUGCell* pCell1, CUGCell* pCell2, int iFlags )
{
	if( iFlags & UG_SORT_DESCENDING )
	{
		CUGCell *ptr = pCell1;
		pCell1 = pCell2;
		pCell2 = ptr;
	}
	
	int retVal;
	
	if( CString(pCell1->GetText()) == "" )
		retVal = 1;
	else if( CString(pCell2->GetText()) == "" )
		retVal = -1;
	else
		retVal = _tcscmp( pCell1->GetText( ), pCell2->GetText());
	
	return retVal;
}

void CTPRIUGGrid::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	CWnd *pWnd = GetParent();
	if (pWnd && pWnd->GetSafeHwnd())
		pWnd->SendMessage(CWM_GRID_DBCLICKED, (WPARAM)col, (LPARAM)row);
}

_KIMI_NAMESPACE__END