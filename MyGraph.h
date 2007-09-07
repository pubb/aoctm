#pragma once
#include "cgraph\graphobject.h"
#include <map>

class CMyGraph :
	public CGraphObject
{
public:
	CMyGraph(void);
	~CMyGraph(void);

	CStringArray Segments, Series;
	CArray<int *, int *> Data;
	
	void PrepareData(int serie, int segment);
	void ClearData(void);
	void ShowCurve(CRect & clRect, COLORREF bgcolor, CString title, CString subtitle, CWnd *pParentWnd);
	void ShowBar(CRect & clRect, COLORREF bgcolor, CString title, CString subtitle, CWnd *pParentWnd);
	void ShowPie(CRect & clRect, COLORREF bgcolor, CString title, CString subtitle, CWnd *pParentWnd);
};

#define	ID_CURVE	40000
#define	ID_BAR	40001
#define	ID_PIE	40002