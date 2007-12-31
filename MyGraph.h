#pragma once
#include "cgraph\graphobject.h"
#include <map>

//fred add
#include "xgraph/xgraph10.h"
#define VALCOUNT   1000
#define MAX_SERIES 50
//end of fred 

class CMyGraph :
	public CGraphObject
{
public:
	CMyGraph(void);
	~CMyGraph(void);
	// a curve i is represented with { Segments[i], series[i]//name,Data[i,*]}
	CStringArray Segments;//x轴的坐标点的值
	CStringArray Series;//曲线名字符串数组
	CArray<int *, int *> Data;//y轴的值
	
	void PrepareData(int serie, int segment, bool curve = false);
	void ClearData(void);
	void ShowCurve(CRect & clRect, COLORREF bgcolor, CString title, CString subtitle, CWnd *pParentWnd);
	void ShowBar(CRect & clRect, COLORREF bgcolor, CString title, CString subtitle, CWnd *pParentWnd);
	void ShowPie(CRect & clRect, COLORREF bgcolor, CString title, CString subtitle, CWnd *pParentWnd);

	//the following is for Rating Curve showing with XGraph
	//fred added codes below 20071224
	void AddDataSerie(bool bAddAxes = false);

	CXGraph		m_XGraph;
	//pubb, 07-12-30, change to CArray instead of static array
	CArray<TDataPoint *, TDataPoint *> m_XGraphValues;
	
private:
	int	SegmentCount;
};

#define	ID_CURVE	40000
#define	ID_BAR	40001
#define	ID_PIE	40002