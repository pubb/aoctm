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
		ShowPlayCountBar();
		break;
	case SHOW_USEDCIVS:
		ShowUsedCivs();
		break;
	}

	return 0;
}

void CGraphDlg::ShowPlayCountBar(void)
{
	//prepare data
	m_Graph.PrepareData(2, (int)m_pPlayers->GetCount());
	m_Graph.Series[0] = _T("Play Count");
	m_Graph.Series[1] = _T("Win Count");
	for(int i = 0; i < m_pPlayers->GetCount(); i++)
	{
		CPlayer * player = m_pPlayers->GetAt(i);
		m_Graph.Segments[i] = player->NickNames[0];
		m_Graph.Data[0][i] = player->PlayCount;
		m_Graph.Data[1][i] = player->WinCount;
	}

	CRect clRect;
	GetClientRect(clRect);
	m_Graph.ShowBar(clRect, RGB(255, 255, 255), _T("Play Count"), _T(""), this);
	
	m_Graph.ClearData();
}

#define	RATINGCURVE_SEG_NUM	15
void CGraphDlg::ShowRatingCurve(void)
{
	if(m_LastGame < m_FirstGame)
		return;
	
	int	seg_num = RATINGCURVE_SEG_NUM;
	int	step = (int)(((m_LastGame - m_FirstGame).GetTotalHours() + /* 1 more day */24)/ (RATINGCURVE_SEG_NUM - 1));
	if(step == 0)
	{
		seg_num = (int)(m_LastGame - m_FirstGame).GetTotalHours();
		step = 1;
	}

	m_Graph.PrepareData((int)m_pPlayers->GetCount(), seg_num);
	
	int serie, segment;
	for(serie = 0; serie < m_pPlayers->GetCount(); serie++)
	{
		m_Graph.Series[serie] = m_pPlayers->GetAt(serie)->NickNames[0];
	}

	CTime t;

	for(segment = 0, t = m_FirstGame; segment < m_Graph.Segments.GetCount(); t += CTimeSpan(0, step, 0, 0), segment++)
	{
		m_Graph.Segments[segment] = t.Format(_T("%m.%d"));
		//XXX, pubb, 07-09-09, not a good way m_LastGame change the global variable
		theApp.Players.Update(t);
		for(serie = 0; serie < m_pPlayers->GetCount(); serie++)
		{
			int rating = theApp.Players[theApp.Players.GetFirstSamePlayer(m_pPlayers->GetAt(serie)->NickNames[0])]->Rating;	//XXX, in order to get the correct Ratings from the main db
			m_Graph.Data[serie][segment] = rating -  /* XXX, for more clear display */1000;
		}
	}

	CRect clRect;
	GetClientRect(clRect);
	m_Graph.ShowCurve(clRect, RGB(255, 255, 255), _T("Rating Curves"), _T(""), this);
	
	m_Graph.ClearData();
}

void CGraphDlg::ShowUsedCivs(void)
{
	//XXX, pubb, 07-09-09, only deal with the first selected player for now
	//prepare data
	extern CString civ_name[19];
	m_Graph.PrepareData(1, 18);	//one serie, 19 civs for segments
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
	
	m_Graph.ClearData();
}