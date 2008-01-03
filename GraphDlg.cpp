// GraphDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AocTM.h"
#include "GraphDlg.h"

#include <math.h>

// CGraphDlg dialog

IMPLEMENT_DYNAMIC(CGraphDlg, CDialog)

CGraphDlg::CGraphDlg(CWnd* pParent /*=NULL*/)
: CDialog(CGraphDlg::IDD, pParent), m_pPlayers(NULL), m_nCommand(SHOW_RATINGCURVE), m_FirstGame(FIRSTGAME_TIME), m_LastGame(CTime::GetCurrentTime())
{
	if(::IsWindow(m_Graph.m_hWnd))
		m_Graph.DestroyWindow();
}

CGraphDlg::~CGraphDlg()
{
	//pubb, 07-09-09, restore the global
	theApp.Players.Update();
}

void CGraphDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGraphDlg, CDialog)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CGraphDlg message handlers

int CGraphDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_pPlayers)
		return 0;

	switch(m_nCommand)
	{
	case SHOW_RATINGCURVE:
		ShowRatingCurve();
		break;
	case SHOW_PLAYCOUNT:
		ShowTechStatBar();
		break;
	case SHOW_USEDCIVS:
		ShowUsedCivs();
		break;
	}

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
	m_Graph.ShowBar(clRect, RGB(255, 255, 255), _T("Tech Statics"), _T(""), this);
}

void CGraphDlg::ShowRatingCurve(void)
{
	if(m_LastGame < m_FirstGame)
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

	int serie = m_pPlayers->GetCount(), segment = m_pPlayers->m_pRecgameDB->GetCount();
	m_Graph.PrepareData(serie, segment, true);

	int i, j;

	for(i = 0 ; i < serie; i++)
		m_Graph.Series[i] = m_pPlayers->GetAt(i)->NickNames[0];

	theApp.Players.GetRatings(m_FirstGame - CTimeSpan(0, 0, 5, 0));	//XXX, use global database. not a good way. but no way except for it now as i know
	for ( j = 0; j < segment; j++)
	{
		theApp.Players.Add(m_pPlayers->m_pRecgameDB->GetAt(j));
		for(i = 0; i < serie; i++)
		{
			int index = theApp.Players.GetFirstSamePlayer(m_pPlayers->GetAt(i)->NickNames[0]);
			m_Graph.m_XGraphValues[i][j].fXVal = COleDateTime(m_pPlayers->m_pRecgameDB->GetAt(j)->RecordTime.GetTime());
			m_Graph.m_XGraphValues[i][j].fYVal = theApp.Players[index]->Rating;
		}
	}
	theApp.Players.Update();	//restore global database

	CRect clRect;
	GetClientRect(clRect);
	m_Graph.ShowCurve(clRect, RGB(255, 255, 255), _T("Rating Curves"), _T(""), this);
}

void CGraphDlg::ShowUsedCivs(void)
{
	//XXX, pubb, 07-09-09, only deal with the first selected player for now
	//prepare data
	extern CString civ_name[19];
	m_Graph.PrepareData(1, 18);	//one serie, 18 civs for segments
	theApp.Players.Update();	//to generate Civs[]
	CPlayer * player = m_pPlayers->GetAt(0);
	for(int i = 0; i < 18; i++)
	{
		m_Graph.Segments[i] = civ_name[i + 1];
		m_Graph.Data[0][i] = player->Civs[i + 1];
	}

	CRect clRect;
	GetClientRect(clRect);
	m_Graph.ShowPie(clRect, RGB(255, 255, 255), _T("Used Civs"), player->NickNames[0], this);
}