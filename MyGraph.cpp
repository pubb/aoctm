#include "StdAfx.h"
#include "MyGraph.h"

CMyGraph::CMyGraph(void)
{
	//pubb, 07-12-30, for what use?
	//fred
	//m_bDrawCurveWithXGraph = FALSE;
	m_bShowMarker = false;
	//m_cInfo = _T("Select Buttons to control the chart !");
}

CMyGraph::~CMyGraph(void)
{
	ClearData();
}

void	CMyGraph::ShowCurve(CRect & clRect, COLORREF bgcolor, CString title, CString subtitle, CWnd *pParentWnd)
{
	//pubb, 07-12-26, fred replaced CGraph with XGraph
	if (!::IsWindow(m_XGraph.m_hWnd))
	{
		//pubb, 07-12-26
		m_XGraph.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, clRect, pParentWnd, ID_CURVE, NULL );
//		m_XGraph.Create(_T("XGraph"),_T("Player Rating Curve"), WS_CHILD  | WS_VISIBLE, clRect, pParentWnd, 1001 );
		m_XGraph.SetData(NULL,0,0,0,false);
		m_XGraph.SetCursorFlags(XGC_LEGEND | XGC_VERT | XGC_ADJUSTSMOOTH);
		
		//pubb, 08-01-15, dont do force
		// Force cursor to snap to the first curve
		//m_XGraph.ForceSnap(0);
		
		//m_XGraph.SetInteraction(false);
		//m_XGraph.EnableWindow(false);
	}
	AddDataSerie();
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

void	CMyGraph::PrepareData(int serie, int segment, bool curve)
{
	ClearData();

	Series.SetSize(serie);
	SegmentCount = segment;

	if(curve)
	{
		m_XGraphValues.SetSize(serie);
	}
	else
	{
		Segments.SetSize(segment);
		Data.SetSize(serie);
	}
	for(int i = 0; i < serie; i++)
	{
		if(curve)
			m_XGraphValues[i] = new TDataPoint[segment];
		else
			Data[i] = new int[segment];
	}
}

void	CMyGraph::ClearData(void)
{
	int i;
	for(i = 0; i < Data.GetCount(); i++)
		delete [] Data[i];
	for(i = 0; i < m_XGraphValues.GetCount(); i++)		
		delete [] m_XGraphValues[i];

	m_XGraphValues.RemoveAll();
	Data.RemoveAll();
	Segments.RemoveAll();
	Series.RemoveAll();
}

void CMyGraph::AddDataSerie(bool bAddAxes)
{
	// setup data
	// number of players to be shown:  pPlayers->GetCount();
	// Data of every players:
	// x_axis mark: time of saved games
	// x_axis Span: dateSpan
	// total x_axis hours : (int)(((m_LastGame - m_FirstGame).GetTotalHours();
	// curve mark string: pPlayers->GetAt(serie)->NickNames[0];;
	//  points : int rating = theApp.Players[theApp.Players.GetFirstSamePlayer(pPlayers->GetAt(serie)->NickNames[0])]->Rating;

	//cureve related vars define
	//int iCurveNum = pPlayers->GetCount();
	int iCurveNum = Series.GetCount();

	/* pubb, 07-12-26
	COleDateTime tBeginTime;
	GetBeginTime(tBeginTime);
	COleDateTime tEndTime;
	GetEndTime(tEndTime);

	COleDateTimeSpan dateSpan(2, 1, 0, 0);// span is an hour
	*/
	/* pubb, 07-12-30
	//total game count of a player
	int iTotalGameCount;
	*/

	int i=0;
	int j = 0;

	//pubb, 07-12-30, moved to GraphDlg.ShowCurve()
#if 0
	// watch data format
	tXGameTime = aDT;//tBeginTime; 
	for ( i = 0; i <  iCurveNum; i++)
	{
		iTotalGameCount = 1000; //change here to players 's game counts
		tXGameTime = COleDateTime::GetCurrentTime();
		for ( j=0; j< iTotalGameCount; j++)
		{
			//TODO
			//iYPlayerScore = (int)(1300 + (( sin(j/ 180.0 * 3.141) * 10))+cos(j / 2.0) + (j / 20.0)+i*100);
			iYPlayerScore = 1500 * i + j;
			m_XGraphValues[i][j].fXVal = tXGameTime;
//			m_XGraphValues[i][j].fXVal = tXGameTime;//COleDateTime::GetCurrentTime () +  COleDateTimeSpan(0, 0, 0, 1 * j);// 
			m_XGraphValues[i][j].fYVal = iYPlayerScore;
			tXGameTime = tXGameTime + COleDateTimeSpan(0,i+1,0,0);
		}
		
	}
#endif

	// link series to graph
	for ( i = 0; i <  iCurveNum; i++)
	{
		m_XGraph.SetData (m_XGraphValues[i], SegmentCount, i, 0, 0);//or i,1,0
	
		// setup visual aspects
		m_XGraph.GetCurve (i).SetLabel(Series[i]);//set player name here
		
		//part 2, style
		m_XGraph.GetCurve (i).SetShowMarker(m_bShowMarker);// mark points in curve fred

		//m_XGraph.GetCurve (0).SetColor(RGB(0,0,0));
		//m_XGraph.GetCurve (3).SetColor(RGB(255,0,0));
		m_XGraph.GetCurve (i).SetFillBeneath(false);
		m_XGraph.GetCurve (i).SetFillColor(RGB(255,0,0));
		m_XGraph.GetCurve (i).SetColor(BASECOLORTABLE[i]);//RGB(255-i*2,255-i*2,255-i*2));
		m_XGraph.GetCurve (i).SetLineSize(3); // we can set it here , fred
	}

	//m_XGraph.GetYAxis (0).SetAxisType(CXGraphAxis::atLog);//org is 1
	m_XGraph.GetXAxis (0).SetDisplayFmt(_T("%Y-%m-%d"));//year,month,day, //"%H:%M:%S"// Hour, Minitus, Seconds));
	m_XGraph.GetXAxis (0).SetDateTime(true);
 
	m_XGraph.GetXAxis (0).SetShowGrid(false);
	m_XGraph.GetYAxis (0).SetShowGrid(false);

	//x-axis
	m_XGraph.GetXAxis (0).SetLabel(_T("Date"));
	m_XGraph.GetXAxis (0).SetAxisMarker(0, 90.0, RGB(150, 150, 255), PS_DASH);
	m_XGraph.GetXAxis (0).SetAxisMarker(1, 180.0, RGB(150, 150, 255));
	m_XGraph.GetXAxis (0).SetAxisMarker(2, 270.0, RGB(150, 150, 255));
	m_XGraph.GetXAxis (0).SetAxisMarker(3, 360.0, RGB(150, 150, 255));
		
	//y-axis
	m_XGraph.GetYAxis (0).SetLabel(_T("Score"));
	//m_XGraph.GetXAxis (0).m_fMin = 1000;
	//m_XGraph.GetXAxis (0).m_fCurMin = 1000;
//	m_XGraph.GetYAxis (0).SetAxisMarker(0, 90.0, RGB(150, 150, 255));
//	m_XGraph.GetYAxis (0).SetAxisMarker(1, 100.0, RGB(150, 150, 255));
//	m_XGraph.GetYAxis (0).SetAxisMarker(2, 110.0, RGB(150, 150, 255));
//	m_XGraph.GetYAxis (0).SetAxisMarker(3, 120.0, RGB(150, 150, 255));

//	m_XGraph.GetYAxis (0).GetAxisMarker(2).szLabel = _T("Max. Score"); // we can set here
		
	//m_XGraph.GetCurve (0).SetFillBeneath(true, 0);
	//m_XGraph.GetCurve (0).SetFillColor(RGB(150 ,255 - (i * 100),150 + (i * 50)));
	//m_XGraph.GetCurve (0).SetFillTransparent(true);

		
	m_XGraph.GetXAxis (0).SetShowMarker(false);
	m_XGraph.GetYAxis (0).SetShowMarker(false);
	//m_XGraph.GetXAxis (0).SetColorRange(0, 90 , 200, RGB(200,200, 255), RGB(100,255,200),_T("Range 1"), HS_DIAGCROSS);
	//m_XGraph.GetXAxis (0).SetColorRange(1, 490 , 600, RGB(200,255, 100), RGB(100,255,200),_T("Range 2"), HS_DIAGCROSS);

	m_XGraph.SetShowLegend (true);
	m_XGraph.SetInnerColor (RGB(255,255,255));
}//fred