#include "StdAfx.h"
#include "MyGraph.h"

CMyGraph::CMyGraph(void)
{
}

CMyGraph::~CMyGraph(void)
{
}

void	CMyGraph::ShowCurve(CRect & clRect, COLORREF bgcolor, CString title, CString subtitle, CWnd *pParentWnd)
{
/* pubb, 07-09-06
 * CGraphObject is not very suitable for our Curve show.
 * it has fixed segments for X axe. that means each serie has the same points for X, they are different in Y only.
 */
	// Create CGraphObject10 ( 2DLine graph )
	Create( NULL, NULL, NULL, clRect, pParentWnd, ID_CURVE, NULL );
	// Create graph and set graph parameters
	CreateGraph( GT_2DLINE );
	SetGraphBackgroundColor( bgcolor );
	SetGraphTitle(title);
	SetGraphSubtitle(subtitle);

	int serie, segment;
	for(segment = 0; segment < Segments.GetCount(); segment++)
	{
		Add2DLineGraphSegment(Segments[segment]);
	}

	for(serie = 0; serie < Series.GetCount(); serie++)
	{
		Add2DLineGraphSeries(Series[serie], RGB(rand() % 255, rand() % 255, rand() % 255));
		for(segment = 0; segment < Segments.GetCount(); segment++)
		{
			Set2DLineGraphValue(segment + 1, serie + 1, Data[serie][segment]);
		}
	}
	// Set graph animation
	SetGraphAnimation( FALSE, AT_LINE_DRAW_ALL );
}

void	CMyGraph::ShowBar(CRect & clRect, COLORREF bgcolor, CString title, CString subtitle, CWnd *pParentWnd)
{
	Create( NULL, NULL, NULL, clRect, pParentWnd, ID_BAR, NULL );
	// Create graph and set graph parameters
	CreateGraph( GT_2DBAR );
	SetGraphBackgroundColor( bgcolor );
	SetGraphTitle(title);
	SetGraphSubtitle(subtitle);
	int serie, segment;
	// Add graph segments
	for(segment = 0; segment < Segments.GetCount(); segment++)
	{
		Add2DBarGraphSegment(Segments[segment]);
	}

	for(serie = 0; serie < Series.GetCount(); serie++)
	{
		Add2DBarGraphSeries(Series[serie], RGB(rand() % 255, rand() % 255, rand() % 255));
		for(segment = 0; segment < Segments.GetCount(); segment++)
		{
			Set2DBarGraphValue(segment + 1, serie + 1, Data[serie][segment]);
		}
	}
	// Set graph animation
	SetGraphAnimation( FALSE, AT_BAR_DRAW_ALL );
}

void	CMyGraph::ShowPie(CRect & clRect, COLORREF bgcolor, CString title, CString subtitle, CWnd *pParentWnd)
{
	Create( NULL, NULL, NULL, clRect, pParentWnd, ID_PIE, NULL );
	// Create graph and set graph parameters
	CreateGraph( GT_2DPIE );
	SetGraphBackgroundColor(bgcolor);
	SetGraphTitle(title);
	SetGraphSubtitle(subtitle);
	
	//to change to the percent
	int count = 0;
	for(int i = 0; i < Segments.GetCount(); i++)
	{
		count += Data[0][i];
	}

	// Add graph segments
	int segment;
	for(segment = 0; segment < Segments.GetCount(); segment++)
	{
		Add2DPieGraphSegment(Data[0][segment] * 100.0 / count, RGB(rand() % 255, rand() % 255, rand() % 255), Segments[segment]);
	}
	// Set graph animation
	SetGraphAnimation( FALSE, AT_PIE_DRAW );
}

void	CMyGraph::PrepareData(int serie, int segment)
{
	for(int i = 0; i < serie; i++)
		Data.Add(new int[segment]);
	Segments.SetSize(segment);
	Series.SetSize(serie);
}

void	CMyGraph::ClearData(void)
{
	for(int i = 0; i < Data.GetCount(); i++)
		delete [] Data[i];
	Data.RemoveAll();
	Segments.RemoveAll();
	Series.RemoveAll();
}