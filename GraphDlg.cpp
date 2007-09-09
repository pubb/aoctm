// GraphDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AocTM.h"
#include "GraphDlg.h"

#include <math.h>

// CGraphDlg dialog

IMPLEMENT_DYNAMIC(CGraphDlg, CDialog)

CGraphDlg::CGraphDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGraphDlg::IDD, pParent), m_pPlayers(NULL)
{
	if(::IsWindow(m_Graph.m_hWnd))
		m_Graph.DestroyWindow();
}

CGraphDlg::~CGraphDlg()
{
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

	CArray<CPlayer *, CPlayer *> players;
	for(int i = 0; i < m_pPlayers->GetCount(); i++)
	{
		players.Add(m_pPlayers->GetAt(i));
	}
	
	ShowRatingCurve(players, CTime(2007,1,1,0,0,0));

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

void CGraphDlg::ShowRatingCurve(CArray<CPlayer *, CPlayer *> & players, CTime from, CTime to)
{
	if(to < from)
		return;
	
	m_Graph.PrepareData((int)players.GetCount(), (int)(to - from).GetDays());
	
	int serie, segment;
	for(serie = 0; serie < players.GetCount(); serie++)
	{
		m_Graph.Series[serie] = players[serie]->NickNames[0];
	}

	CTime t;
	for(segment = 0, t = from; t < to && segment < m_Graph.Segments.GetCount(); t += CTimeSpan(1, 0, 0, 0), segment++)
	{
		if(t.GetDayOfWeek() == 6)	//every Friday
			m_Graph.Segments[segment] = t.Format(_T("%m.%d"));
		else
			m_Graph.Segments[segment] = _T("");
		m_pPlayers->Update(CTime(0), t);
		for(serie = 0; serie < players.GetCount(); serie++)
		{
			m_Graph.Data[serie][segment] = players[serie]->Rating -  /* XXX */1000;
		}
	}

	CRect clRect;
	GetClientRect(clRect);
	m_Graph.ShowCurve(clRect, RGB(255, 255, 255), _T("Rating Curves"), _T(""), this);
	
	m_Graph.ClearData();
}