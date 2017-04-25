#pragma once

#include "InPlaceEdit.h"
#include "MIPSSetup.h"

// CEditableListCtrl

class CEditableListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CEditableListCtrl)

public:
	CEditableListCtrl();
	virtual ~CEditableListCtrl();
	int GetRowFromPoint(CPoint &point, int *col) const;
	CEdit* EditSubLabel(int nItem, int nCol);

	void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	void OnLButtonDown(UINT nFlags, CPoint point);

protected:
	DECLARE_MESSAGE_MAP()
};


