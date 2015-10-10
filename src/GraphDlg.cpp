// GraphDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AocTM.h"
#include "GraphDlg.h"

#include <math.h>

// CGraphDlg dialog

IMPLEMENT_DYNAMIC(CGraphDlg, CDialog)

CGraphDlg::CGraphDlg(CWnd* pParent /*=NULL*/)
: CDialog(CGraphDlg::IDD, pParent), m_pPlayers(NULL), m_nCommand(SHOW_RATINGCURVE), m_FirstGameID(0), m_LastGameID(0)
{
	if(::IsWindow(m_Graph.m_hWnd))
		m_Graph.DestroyWindow();
	m_pXGraphWndHandle = NULL;//fred
	first = true;
}

CGraphDlg::~CGraphDlg()
{
	//pubb, 07-09-09, restore the global
	theApp.Players.Update(false);
}

void CGraphDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGraphDlg, CDialog)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZING()
	ON_BN_CLICKED(IDC_BUTTON_ZOOM, &CGraphDlg::OnBnClickedButtonZoom)
	ON_BN_CLICKED(IDC_BUTTON_MARK, &CGraphDlg::OnBnClickedButtonMark)
	ON_BN_CLICKED(IDC_BUTTON_Measure, &CGraphDlg::OnBnClickedButtonMeasure)
	ON_BN_CLICKED(IDC_BUTTON_Select, &CGraphDlg::OnBnClickedButtonSelect)
	ON_BN_CLICKED(IDC_BUTTON_Reset, &CGraphDlg::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_Clear, &CGraphDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_ClearMeasure, &CGraphDlg::OnBnClickedButtonClearmeasure)
END_MESSAGE_MAP()


// CGraphDlg message handlers

int CGraphDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_pPlayers)
		return 0;
	return 0;
}

void CGraphDlg::ShowTechStatBar(void)
{
	//prepare data
	m_Graph.PrepareData(6, (int)m_pPlayers->GetCount());
	m_Graph.Series[0] = _T("Min Feud Time");
	m_Graph.Series[1] = _T("Avg Feud Time");
	m_Graph.Series[2] = _T("Min Cstl Time");
	m_Graph.Series[3] = _T("Avg Cstl Time");
	m_Graph.Series[4] = _T("Min Impl Time");
	m_Graph.Series[5] = _T("Avg Impl Time");

	int playcount, count;
	CTimeSpan avg, min;
	CPlayer * player;

//	m_pPlayers->Update();

	for(int i = 0; i < m_pPlayers->GetCount(); i++)
	{
		player = m_pPlayers->GetAt(i);
		m_Graph.Segments[i] = player->NickNames[0];

		playcount = player->PlayCount;

		for(int j = 0; j < 3 ; j++)
		{
			switch(j)
			{
			case 0:
				min = player->MinFeudTime;
				count = player->FeudCount;
				if(count)
					avg = CTimeSpan(player->TotalFeudTime.GetTotalSeconds() / count);
				else
					avg = CTimeSpan(0);
				break;
			case 1:
				min = player->MinCstlTime;
				count = player->CstlCount;
				if(count)
					avg = CTimeSpan(player->TotalCstlTime.GetTotalSeconds() / count);
				else
					avg = CTimeSpan(0);
				break;
			case 2:
				min = player->MinImplTime;
				count = player->ImplCount;
				if(count)
					avg = CTimeSpan(player->TotalImplTime.GetTotalSeconds() / count);
				else
					avg = CTimeSpan(0);
				break;
			}
			if(min == MAX_TIMESPAN)
				min = 0;
			m_Graph.Data[2 * j][i] = (int)min.GetTotalSeconds();
			m_Graph.Data[2 * j + 1][i] = (int)avg.GetTotalSeconds();
		}
	}

	CRect clRect;
	GetClientRect(clRect);
	//fred
	HideUserControl();

	m_Graph.ShowBar(clRect, RGB(255, 255, 255), _T("Tech Statics"), _T(""), this);
}

void CGraphDlg::ShowRatingCurve(void)
{
	//pubb, 14-06-22, fix repaint bug
	if (!first) goto draw;

	if(m_LastGameID < m_pPlayers->m_pRecgameDB->GetCount() - 1)
		m_LastGameID++;
	if(m_FirstGameID > 0)
		m_FirstGameID--;

	if(m_LastGameID < m_FirstGameID)
		return;

//fred, replace CGraph with XGraph for curve
#if 0
	CTime t;

	for(segment = 0, t = m_FirstGame; segment < m_Graph.Segments.GetCount(); t += CTimeSpan(0, step, 0, 0), segment++)
	{
		m_Graph.Segments[segment] = t.Format(_T("%m.%d"));
		//XXX, pubb, 07-09-09, not a good way m_LastGame change the global variable
		theApp.Players.GetRatings(t);
		//m_pPlayers->GetRatings(t);
		for(serie = 0; serie < m_pPlayers->GetCount(); serie++)
		{
			//pubb, 07-09-18, use local DB
			int rating = theApp.Players[theApp.Players.GetFirstSamePlayer(m_pPlayers->GetAt(serie)->NickNames[0])]->Rating;	//XXX, in order to get the correct Ratings from the main db
			//int rating = m_pPlayers->GetAt(serie)->Rating;
			m_Graph.Data[serie][segment] = rating -  /* XXX, for more clear display */1000;
		}
	}
#endif

	int serie = m_pPlayers->GetCount(), segment = m_LastGameID - m_FirstGameID + 1;
	m_Graph.PrepareData(serie, segment, true);

	int i, j;

	for(i = 0 ; i < serie; i++)
		m_Graph.Series[i] = m_pPlayers->GetAt(i)->NickNames[0];

	theApp.Players.GetRatings(m_pPlayers->m_pRecgameDB->GetAt(m_FirstGameID)->RecordTime - CTimeSpan(0, 0, 5, 0));	//XXX, use global database. not a good way. but no way except for it now as i know
	for ( j = 0; j < segment; j++)
	{
		theApp.Players.Add(m_pPlayers->m_pRecgameDB->GetAt(m_FirstGameID + j));
		for(i = 0; i < serie; i++)
		{
			int index = theApp.Players.GetFirstSamePlayer(m_pPlayers->GetAt(i)->NickNames[0]);
			m_Graph.m_XGraphValues[i][j].fXVal = COleDateTime(m_pPlayers->m_pRecgameDB->GetAt(m_FirstGameID + j)->RecordTime.GetTime());
			m_Graph.m_XGraphValues[i][j].fYVal = theApp.Players[index]->Rating;
		}
	}
	theApp.Players.Update(false);	//restore global database
	first = false;	//only prepare once

draw:
	CRect clRect;
	//GetClientRect(clRect);
	//fred add test codes for xgraph
	
	CWnd * pTmp = (CWnd *)GetDlgItem(IDC_Curve_XGraph);	
	m_pXGraphWndHandle = pTmp;//fred 20080110
	pTmp->GetWindowRect(clRect);
	ScreenToClient(clRect);
	//pTmp = (CWnd *)GetDlgItem(IDC_STATIC_Info);
	//pTmp->SetWindowTextW(m_Graph.m_cInfo);
	m_Graph.ShowCurve(clRect, RGB(255, 255, 255), _T("Rating Curves"), _T(""), this);
}

void CGraphDlg::ShowUsedCivs(void)
{
	//XXX, pubb, 07-09-09, only deal with the first selected player for now
	//prepare data
	extern CString civ_name[];
	float winpercentage = 0;
	CString str;

	m_Graph.PrepareData(1, CIVS-1);	//one serie, 24 civs for segments
	theApp.Players.Update(false);	//to generate Civs[]
	CPlayer * player = m_pPlayers->GetAt(0);
	for(int i = 0; i < CIVS-1; i++)
	{
		str.Format(_T("%9s : %.2f%%"), civ_name[i + 1], 100.0 * player->Civs[i+1][1] / player->Civs[i+1][0]);	
		m_Graph.Segments[i] = str;
		m_Graph.Data[0][i] = player->Civs[i + 1][0];
	}

	CRect clRect;
	GetClientRect(clRect);
	//fred
	HideUserControl();

	//pubb, 14-09-26, show wincount/playcount for each civ.
	str.Format(_T("%s : %d / %d"), player->NickNames[0], player->WinCount, player->PlayCount);
	m_Graph.ShowPie(clRect, RGB(255, 255, 255), _T("Used Civs"), str, this);
}

void CGraphDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	//fred changed 20080103
	switch(m_nCommand)
	{
	case SHOW_RATINGCURVE:
		ShowRatingCurve();//fred
		break;
	case SHOW_TECHSTAT:
		ShowTechStatBar();
		break;
	case SHOW_USEDCIVS:
		ShowUsedCivs();
		break;
	}

}

void CGraphDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);

	// TODO: 在此处添加消息处理程序代码
}

// Hide user control for curve view
int CGraphDlg::HideUserControl(void)
{
	//fred add
	CWnd * pTmp = (CWnd *)GetDlgItem(IDC_Curve_XGraph);
	pTmp->ShowWindow(SW_HIDE);
	(CWnd *)GetDlgItem(IDC_BUTTON_ZOOM)->ShowWindow(SW_HIDE);
	(CWnd *)GetDlgItem(IDC_BUTTON_Measure)->ShowWindow(SW_HIDE);
	(CWnd *)GetDlgItem(IDC_BUTTON_MARK)->ShowWindow(SW_HIDE);
	(CWnd *)GetDlgItem(IDC_STATIC_Info)->ShowWindow(SW_HIDE);
	(CWnd *)GetDlgItem(IDC_BUTTON_Select)->ShowWindow(SW_HIDE);
	(CWnd *)GetDlgItem(IDC_BUTTON_Clear)->ShowWindow(SW_HIDE);
	(CWnd *)GetDlgItem(IDC_BUTTON_ClearMeasure)->ShowWindow(SW_HIDE);
	(CWnd *)GetDlgItem(IDC_BUTTON_Reset)->ShowWindow(SW_HIDE);
	//end of fred added IDC_BUTTON_Select
	return 0;
}

void CGraphDlg::OnBnClickedButtonZoom()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Graph.m_XGraph.Zoom();
	//m_Graph.m_cInfo = _T("Use the mouse to define an area to zoom in.");
	
	m_pXGraphWndHandle->UpdateData(FALSE);
	//OnPaint();
}

void CGraphDlg::OnBnClickedButtonMark()
{
	// TODO: 在此添加控件通知处理程序代码
	/*
	if( m_Graph.m_bShowMarker )
		m_Graph.m_bShowMarker = FALSE;
	else
		m_Graph.m_bShowMarker = TRUE;
	OnPaint();
	*/
	m_Graph.m_XGraph.Cursor();
	//m_cInfo = _T("Use the mouse to move the cursor left and right. The second chart will be synchronised. Use the left mousebutton to insert a data annotation"); 
	m_pXGraphWndHandle->UpdateData(FALSE);

	//OnPaint();
}

void CGraphDlg::OnBnClickedButtonMeasure()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Graph.m_XGraph.Measure();
	CPoint point;
	point.x = 0;
	point.y = 0;
	m_Graph.m_XGraph.SetCursor(point);
	//m_Graph.m_cInfo = _T("Use the mouse to measure the distance between two points."); 
	m_pXGraphWndHandle->UpdateData(FALSE);
}

void CGraphDlg::OnBnClickedButtonSelect()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Graph.m_XGraph.NoOp();

	//m_cInfo = _T("Use the mouse to select objects like axes, curves, ..."); 
	m_pXGraphWndHandle->UpdateData(FALSE);
	//OnPaint();
}

void CGraphDlg::OnBnClickedButtonReset()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Graph.m_XGraph.ResetZoom();
	m_pXGraphWndHandle->UpdateData(FALSE);
}

void CGraphDlg::OnBnClickedButtonClear()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Graph.m_XGraph.NoOp();
	m_Graph.m_XGraph.DeleteMeasures();
	m_Graph.m_XGraph.ResetAll();//
	//m_Graph.m_XGraph.
	OnPaint();
	m_pXGraphWndHandle->UpdateData(FALSE);
}

void CGraphDlg::OnBnClickedButtonClearmeasure()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Graph.m_XGraph.NoOp();
	m_Graph.m_XGraph.DeleteMeasures();
	m_pXGraphWndHandle->UpdateData(FALSE);
}

BOOL CGraphDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString str;
	switch(m_nCommand)
	{
	case SHOW_RATINGCURVE:
		str = _T("Show Rating Curves");
		break;
	case SHOW_TECHSTAT:
		str = _T("Show Technique Statistic");
		break;
	case SHOW_USEDCIVS:
		str = _T("Show Used Civilization Distribution");
		break;
	default:
		str = _T("???");
	}

	SetWindowText(str);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
